/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/xattr.h>
#ifdef HAVE_SELINUX
#include <selinux/selinux.h>
#include <selinux/label.h>
#endif

#include "label.h"
#include "strv.h"
#include "util.h"
#include "path-util.h"

#ifdef HAVE_SELINUX
static struct selabel_handle *label_hnd = NULL;
#endif

int label_init() {
        int r = 0;

#ifdef HAVE_SELINUX
        usec_t before_timestamp, after_timestamp;
        struct mallinfo before_mallinfo, after_mallinfo;

        if (!is_selinux_enabled())
                return 0;

        if (label_hnd)
                return 0;

        before_mallinfo = mallinfo();
        before_timestamp = now(CLOCK_MONOTONIC);

        label_hnd = selabel_open(SELABEL_CTX_FILE, NULL, 0);

        if (!label_hnd) {
                log_full(security_getenforce() == 1 ? LOG_ERR : LOG_DEBUG,
                         "Failed to initialize SELinux context: %m");
                r = security_getenforce() == 1 ? -errno : 0;
        } else  {
                char timespan[FORMAT_TIMESPAN_MAX];
                int l;

                after_timestamp = now(CLOCK_MONOTONIC);
                after_mallinfo = mallinfo();

                l = after_mallinfo.uordblks > before_mallinfo.uordblks ? after_mallinfo.uordblks - before_mallinfo.uordblks : 0;

                log_debug("Successfully loaded SELinux database in %s, size on heap is %iK.",
                          format_timespan(timespan, sizeof(timespan), after_timestamp - before_timestamp, 0),
                          (l+1023)/1024);
        }
#endif

        return r;
}

static int label_fix_selinux(const char *path, bool ignore_enoent, bool ignore_erofs) {
        int r = 0;

#ifdef HAVE_SELINUX
        struct stat st;
        security_context_t fcon;

        if (!label_hnd)
                return 0;

        r = lstat(path, &st);
        if (r == 0) {
                r = selabel_lookup_raw(label_hnd, &fcon, path, st.st_mode);

                /* If there's no label to set, then exit without warning */
                if (r < 0 && errno == ENOENT)
                        return 0;

                if (r == 0) {
                        r = lsetfilecon(path, fcon);
                        freecon(fcon);

                        /* If the FS doesn't support labels, then exit without warning */
                        if (r < 0 && errno == ENOTSUP)
                                return 0;
                }
        }

        if (r < 0) {
                /* Ignore ENOENT in some cases */
                if (ignore_enoent && errno == ENOENT)
                        return 0;

                if (ignore_erofs && errno == EROFS)
                        return 0;

                log_full(security_getenforce() == 1 ? LOG_ERR : LOG_DEBUG,
                         "Unable to fix label of %s: %m", path);
                r = security_getenforce() == 1 ? -errno : 0;
        }
#endif

        return r;
}

int label_fix(const char *path, bool ignore_enoent, bool ignore_erofs) {
        int r = 0;

#ifdef HAVE_SELINUX
        if (is_selinux_enabled()) {
                r = label_fix_selinux(path, ignore_enoent, ignore_erofs);
                if (r < 0)
                        return r;
        }
#endif

        return r;
}

void label_finish(void) {

#ifdef HAVE_SELINUX
        if (!is_selinux_enabled())
                return;

        if (label_hnd)
                selabel_close(label_hnd);
#endif
}

int label_context_set(const char *path, mode_t mode) {
        int r = 0;

#ifdef HAVE_SELINUX
        security_context_t filecon = NULL;

        if (!is_selinux_enabled() || !label_hnd)
                return 0;

        r = selabel_lookup_raw(label_hnd, &filecon, path, mode);
        if (r < 0 && errno != ENOENT)
                r = -errno;
        else if (r == 0) {
                r = setfscreatecon(filecon);
                if (r < 0) {
                        log_error("Failed to set SELinux file context on %s: %m", path);
                        r = -errno;
                }

                freecon(filecon);
        }

        if (r < 0 && security_getenforce() == 0)
                r = 0;
#endif

        return r;
}

void label_context_clear(void) {

#ifdef HAVE_SELINUX
        PROTECT_ERRNO;

        if (!is_selinux_enabled())
                return;

        setfscreatecon(NULL);
#endif
}

static int label_mkdir_selinux(const char *path, mode_t mode) {
        int r = 0;

#ifdef HAVE_SELINUX
        /* Creates a directory and labels it according to the SELinux policy */
        security_context_t fcon = NULL;

        if (!label_hnd)
                return 0;

        if (path_is_absolute(path))
                r = selabel_lookup_raw(label_hnd, &fcon, path, S_IFDIR);
        else {
                _cleanup_free_ char *newpath;

                newpath = path_make_absolute_cwd(path);
                if (!newpath)
                        return -ENOMEM;

                r = selabel_lookup_raw(label_hnd, &fcon, newpath, S_IFDIR);
        }

        if (r == 0)
                r = setfscreatecon(fcon);

        if (r < 0 && errno != ENOENT) {
                log_error("Failed to set security context %s for %s: %m", fcon, path);

                if (security_getenforce() == 1) {
                        r = -errno;
                        goto finish;
                }
        }

        r = mkdir(path, mode);
        if (r < 0)
                r = -errno;

finish:
        setfscreatecon(NULL);
        freecon(fcon);
#endif

        return r;
}

int label_mkdir(const char *path, mode_t mode) {
        int r;

#ifdef HAVE_SELINUX
        if (is_selinux_enabled()) {
                r = label_mkdir_selinux(path, mode);
                if (r < 0)
                        return r;
        }
#endif

        r = mkdir(path, mode);
        if (r < 0 && errno != EEXIST)
                return -errno;

        return 0;
}

int label_apply(const char *path, const char *label) {
        int r = 0;

#ifdef HAVE_SELINUX
        if (!is_selinux_enabled())
                return 0;

        r = setfilecon(path, (char *)label);
#endif
        return r;
}
