/***
  This file is part of systemd.

  Copyright 2003-2004 Greg Kroah-Hartman <greg@kroah.com>
  Copyright 2004-2012 Kay Sievers <kay@vrfy.org>

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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <grp.h>
#include <sched.h>
#include <dirent.h>
#include <sys/mount.h>
#include <sys/signalfd.h>

#include "missing.h"
#include "udev.h"


static int check_dir(const char *path)
{
	struct stat st = {};
	DIR *dir;
	struct dirent *entry;
	int res;

	if (stat(path, &st) < 0)
		return -1;

	if (!S_ISDIR(st.st_mode))
		return -1;

	dir = opendir(path);
	if (!dir)
		return -1;

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type != DT_DIR)
			break;

		if (entry->d_name[0] != '.')
			break;

		if (entry->d_name[1] == '\0')
			continue;

		if (entry->d_name[1] != '.')
			break;

		if (entry->d_name[2] != '\0')
			break;
	}

	if (closedir(dir) < 0)
		return -1;

	if (!entry)
		return 0;

	return 1;
}

static int check_dirs(void)
{
	int res;

	res = check_dir("sys");
	if (res < 0)
	{
		fprintf(stderr, "'sys' path not exist in current directory.\n");
		return -1;
	}

	if (!res)
	{
		fprintf(stderr, "'sys' path is empty, unpack 'sys.tar.xz' archive to run this test.\n");
		return -1;
	}

	res = check_dir("test");
	if (res < 0)
	{
		fprintf(stderr, "'test' path not exist in current directory, create empty directory with name test.\n");
		return -1;
	}

	if (res > 0)
	{
		fprintf(stderr, "'test' path is not empty.\n");
		return -1;
	}

	return 0;
}

static int fake_filesystems(void) {
        static const struct fakefs {
                const char *src;
                const char *target;
                const char *error;
        } fakefss[] = {
                { "sys", "/sys",                          "test"  },
                { "dev", "/dev",                          "test"  },
                { "run", "/run",                          "test"  },
                { "run", UDEVSYSCONFDIR "/rules.d",       "empty" },
                { "run", "/usr/lib/udev/rules.d",         "empty" },
        };
        unsigned int i;
        int err;

        err = unshare(CLONE_NEWNS);
        if (err < 0) {
                err = -errno;
                fprintf(stderr, "failed to call unshare(): %m\n");
                goto out;
        }

        if (mount(NULL, "/", NULL, MS_PRIVATE|MS_REC, NULL) < 0) {
                err = -errno;
                fprintf(stderr, "failed to mount / as private: %m\n");
                goto out;
        }

        for (i = 0; i < ELEMENTSOF(fakefss); i++) {
                err = mount(fakefss[i].src, fakefss[i].target, NULL, MS_BIND, NULL);
                if (err < 0) {
                        err = -errno;
                        fprintf(stderr, "failed to mount %s %s %m", fakefss[i].error, fakefss[i].target);
                        return err;
                }
        }
out:
        return err;
}

int main(int argc, char *argv[]) {
        struct udev *udev = NULL;
        struct udev_event *event = NULL;
        struct udev_device *dev = NULL;
        struct udev_rules *rules = NULL;
        char syspath[UTIL_PATH_SIZE];
        const char *devpath;
        const char *action;
        sigset_t mask, sigmask_orig;

        log_set_max_level(LOG_DEBUG);

        action = argv[1];
        if (!action) {
                log_error("action missing");
                return EXIT_FAILURE;
        }

        devpath = argv[2];
        if (!devpath) {
                log_error("devpath missing");
                return EXIT_FAILURE;
        }

        if (check_dirs() < 0)
                return EXIT_FAILURE;

        if (fake_filesystems() < 0)
                return EXIT_FAILURE;

        udev = udev_new();
        if (!udev)
                return EXIT_FAILURE;

        log_debug("version %s", VERSION);

        sigprocmask(SIG_SETMASK, NULL, &sigmask_orig);

        rules = udev_rules_new(udev, 1);

        strscpyl(syspath, sizeof(syspath), "/sys", devpath, NULL);
        dev = udev_device_new_from_syspath(udev, syspath);
        if (!dev) {
                log_debug("unknown device '%s'", devpath);
                goto out;
        }

        udev_device_set_action(dev, action);
        event = udev_event_new(dev);
        if (!event) {
                log_oom();
                goto out;
        }

        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, &sigmask_orig);
        event->fd_signal = signalfd(-1, &mask, SFD_NONBLOCK|SFD_CLOEXEC);
        if (event->fd_signal < 0) {
                fprintf(stderr, "error creating signalfd\n");
                goto out;
        }

        /* do what devtmpfs usually provides us */
        if (udev_device_get_devnode(dev) != NULL) {
                mode_t mode = 0600;

                if (str_eq(udev_device_get_subsystem(dev), "block"))
                        mode |= S_IFBLK;
                else
                        mode |= S_IFCHR;

                if (!str_eq(action, "remove")) {
                        path_create(udev_device_get_devnode(dev), 0755);
                        mknod(udev_device_get_devnode(dev), mode, udev_device_get_devnum(dev));
                } else {
                        unlink(udev_device_get_devnode(dev));
                        path_remove(udev_device_get_devnode(dev));
                }
        }

        udev_event_execute_rules(event, USEC_PER_SEC, rules, &sigmask_orig);
        udev_event_execute_run(event, USEC_PER_SEC, NULL);
out:
        if (event != NULL && event->fd_signal >= 0)
                close(event->fd_signal);
        udev_event_unref(event);
        udev_device_unref(dev);
        udev_rules_unref(rules);
        udev_unref(udev);

        return EXIT_SUCCESS;
}
