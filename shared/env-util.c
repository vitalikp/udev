/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2012 Lennart Poettering

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

#include <limits.h>
#include <sys/param.h>
#include <unistd.h>

#include "strv.h"
#include "utf8.h"
#include "util.h"
#include "env-util.h"
#include "def.h"

#define VALID_CHARS_ENV_NAME                    \
        DIGITS LETTERS                          \
        "_"

#ifndef ARG_MAX
#define ARG_MAX ((size_t) sysconf(_SC_ARG_MAX))
#endif

static int env_append(char **r, char ***k, char **a) {
        assert(r);
        assert(k);

        if (!a)
                return 0;

        /* Add the entries of a to *k unless they already exist in *r
         * in which case they are overridden instead. This assumes
         * there is enough space in the r array. */

        for (; *a; a++) {
                char **j;
                size_t n;

                n = strcspn(*a, "=");

                if ((*a)[n] == '=')
                        n++;

                for (j = r; j < *k; j++)
                        if (strneq(*j, *a, n))
                                break;

                if (j >= *k)
                        (*k)++;
                else
                        free(*j);

                *j = strdup(*a);
                if (!*j)
                        return -ENOMEM;
        }

        return 0;
}

char **strv_env_merge(unsigned n_lists, ...) {
        size_t n = 0;
        char **l, **k, **r;
        va_list ap;
        unsigned i;

        /* Merges an arbitrary number of environment sets */

        va_start(ap, n_lists);
        for (i = 0; i < n_lists; i++) {
                l = va_arg(ap, char**);
                n += strv_length(l);
        }
        va_end(ap);

        r = new(char*, n+1);
        if (!r)
                return NULL;

        k = r;

        va_start(ap, n_lists);
        for (i = 0; i < n_lists; i++) {
                l = va_arg(ap, char**);
                if (env_append(r, &k, l) < 0)
                        goto fail;
        }
        va_end(ap);

        *k = NULL;

        return r;

fail:
        va_end(ap);
        strv_free(r);

        return NULL;
}

_pure_ static bool env_match(const char *t, const char *pattern) {
        assert(t);
        assert(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        if (streq(t, pattern))
                return true;

        if (!strchr(pattern, '=')) {
                size_t l = strlen(pattern);

                return strneq(t, pattern, l) && t[l] == '=';
        }

        return false;
}

char **strv_env_delete(char **x, unsigned n_lists, ...) {
        size_t n, i = 0;
        char **k, **r;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        r = new(char*, n+1);
        if (!r)
                return NULL;

        STRV_FOREACH(k, x) {
                unsigned v;

                va_start(ap, n_lists);
                for (v = 0; v < n_lists; v++) {
                        char **l, **j;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                if (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                r[i] = strdup(*k);
                if (!r[i]) {
                        strv_free(r);
                        return NULL;
                }

                i++;
                continue;

        skip:
                va_end(ap);
        }

        r[i] = NULL;

        assert(i <= n);

        return r;
}

char **strv_env_unset(char **l, const char *p) {

        char **f, **t;

        if (!l)
                return NULL;

        assert(p);

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        for (f = t = l; *f; f++) {

                if (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        return l;
}

char **strv_env_unset_many(char **l, ...) {

        char **f, **t;

        if (!l)
                return NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        for (f = t = l; *f; f++) {
                bool found = false;
                const char *p;
                va_list ap;

                va_start(ap, l);

                while ((p = va_arg(ap, const char*))) {
                        if (env_match(*f, p)) {
                                found = true;
                                break;
                        }
                }

                va_end(ap);

                if (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        return l;
}

char **strv_env_set(char **x, const char *p) {

        char **k, **r;
        char* m[2] = { (char*) p, NULL };

        /* Overrides the env var setting of p, returns a new copy */

        r = new(char*, strv_length(x)+2);
        if (!r)
                return NULL;

        k = r;
        if (env_append(r, &k, x) < 0)
                goto fail;

        if (env_append(r, &k, m) < 0)
                goto fail;

        *k = NULL;

        return r;

fail:
        strv_free(r);
        return NULL;
}

char *strv_env_get_n(char **l, const char *name, size_t k) {
        char **i;

        assert(name);

        if (k <= 0)
                return NULL;

        STRV_FOREACH(i, l)
                if (strneq(*i, name, k) &&
                    (*i)[k] == '=')
                        return *i + k + 1;

        return NULL;
}

char *strv_env_get(char **l, const char *name) {
        assert(name);

        return strv_env_get_n(l, name, strlen(name));
}
