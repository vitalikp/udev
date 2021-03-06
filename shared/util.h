/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#pragma once

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

#include <alloca.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sched.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/resource.h>
#include <stddef.h>
#include <unistd.h>
#include <locale.h>
#include <mntent.h>
#include <sys/socket.h>

#if SIZEOF_PID_T == 4
#  define PID_FMT "%" PRIu32
#elif SIZEOF_PID_T == 2
#  define PID_FMT "%" PRIu16
#else
#  error Unknown pid_t size
#endif

#if SIZEOF_UID_T == 4
#  define UID_FMT "%" PRIu32
#elif SIZEOF_UID_T == 2
#  define UID_FMT "%" PRIu16
#else
#  error Unknown uid_t size
#endif

#if SIZEOF_GID_T == 4
#  define GID_FMT "%" PRIu32
#elif SIZEOF_GID_T == 2
#  define GID_FMT "%" PRIu16
#else
#  error Unknown gid_t size
#endif

#if SIZEOF_TIME_T == 8
#  define PRI_TIME PRIu64
#elif SIZEOF_TIME_T == 4
#  define PRI_TIME PRIu32
#else
#  error Unknown time_t size
#endif

#if SIZEOF_RLIM_T == 8
#  define RLIM_FMT "%" PRIu64
#elif SIZEOF_RLIM_T == 4
#  define RLIM_FMT "%" PRIu32
#else
#  error Unknown rlim_t size
#endif

#include "macro.h"
#include "missing.h"
#include "time-util.h"

/* What is interpreted as whitespace? */
#define WHITESPACE " \t\n\r"
#define NEWLINE    "\n\r"
#define QUOTES     "\"\'"
#define COMMENTS   "#;"
#define GLOB_CHARS "*?["

#define FORMAT_BYTES_MAX 8

#define ANSI_HIGHLIGHT_ON "\x1B[1;39m"
#define ANSI_RED_ON "\x1B[31m"
#define ANSI_HIGHLIGHT_RED_ON "\x1B[1;31m"
#define ANSI_GREEN_ON "\x1B[32m"
#define ANSI_HIGHLIGHT_GREEN_ON "\x1B[1;32m"
#define ANSI_HIGHLIGHT_YELLOW_ON "\x1B[1;33m"
#define ANSI_HIGHLIGHT_BLUE_ON "\x1B[1;34m"
#define ANSI_HIGHLIGHT_OFF "\x1B[0m"
#define ANSI_ERASE_TO_END_OF_LINE "\x1B[K"

size_t page_size(void);
#define PAGE_ALIGN(l) ALIGN_TO((l), page_size())

#define streq(a,b) (strcmp((a),(b)) == 0)
#define strneq(a, b, n) (strncmp((a), (b), (n)) == 0)
#define strcaseeq(a,b) (strcasecmp((a),(b)) == 0)
#define strncaseeq(a, b, n) (strncasecmp((a), (b), (n)) == 0)

bool streq_ptr(const char *a, const char *b) _pure_;

#define new(t, n) ((t*) malloc_multiply(sizeof(t), (n)))

#define new0(t, n) ((t*) calloc((n), sizeof(t)))

#define newa(t, n) ((t*) alloca(sizeof(t)*(n)))

#define newdup(t, p, n) ((t*) memdup_multiply(p, sizeof(t), (n)))

#define malloc0(n) (calloc((n), 1))

static inline const char* yes_no(bool b) {
        return b ? "yes" : "no";
}

static inline const char* true_false(bool b) {
        return b ? "true" : "false";
}

static inline const char* strempty(const char *s) {
        return s ? s : "";
}

static inline const char* strnull(const char *s) {
        return s ? s : "(null)";
}

static inline const char *strna(const char *s) {
        return s ? s : "n/a";
}

static inline bool isempty(const char *p) {
        return !p || !p[0];
}

static inline const char *startswith(const char *s, const char *prefix) {
        if (strncmp(s, prefix, strlen(prefix)) == 0)
                return s + strlen(prefix);
        return NULL;
}

static inline const char *startswith_no_case(const char *s, const char *prefix) {
        if (strncasecmp(s, prefix, strlen(prefix)) == 0)
                return s + strlen(prefix);
        return NULL;
}

char *endswith(const char *s, const char *postfix) _pure_;

int close_nointr(int fd);
int safe_close(int fd);

void close_many(const int fds[], unsigned n_fd);

int parse_size(const char *t, off_t base, off_t *size);

int parse_boolean(const char *v) _pure_;

int safe_atou(const char *s, unsigned *ret_u);
int safe_atoi(const char *s, int *ret_i);

const char* split(const char **state, size_t *l, const char *separator, bool quoted);

#define FOREACH_WORD(word, length, s, state)                            \
        _FOREACH_WORD(word, length, s, WHITESPACE, false, state)

#define FOREACH_WORD_SEPARATOR(word, length, s, separator, state)       \
        _FOREACH_WORD(word, length, s, separator, false, state)

#define FOREACH_WORD_QUOTED(word, length, s, state)                     \
        _FOREACH_WORD(word, length, s, WHITESPACE, true, state)

#define _FOREACH_WORD(word, length, s, separator, quoted, state)        \
        for ((state) = (s), (word) = split(&(state), &(length), (separator), (quoted)); (word); (word) = split(&(state), &(length), (separator), (quoted)))

char *strappend(const char *s, const char *suffix);
char *strnappend(const char *s, const char *suffix, size_t length);

char *strstrip(char *s);
char *delete_chars(char *s, const char *bad);
char *truncate_nl(char *s);

char hexchar(int x) _const_;
int unhexchar(char c) _const_;
char octchar(int x) _const_;
int unoctchar(char c) _const_;
char decchar(int x) _const_;
int undecchar(char c) _const_;

char *cescape(const char *s);
char *cunescape(const char *s);
char *cunescape_length(const char *s, size_t length);
char *cunescape_length_with_prefix(const char *s, size_t length, const char *prefix);

char *xescape(const char *s, const char *bad);

char *ascii_strlower(char *path);

bool dirent_is_file(const struct dirent *de) _pure_;
bool dirent_is_file_with_suffix(const struct dirent *de, const char *suffix) _pure_;

bool ignore_file(const char *filename) _pure_;

bool chars_intersect(const char *a, const char *b) _pure_;

int dev_urandom(void *p, size_t n);
void random_bytes(void *p, size_t n);

static inline uint64_t random_u64(void) {
        uint64_t u;
        random_bytes(&u, sizeof(u));
        return u;
}

static inline uint32_t random_u32(void) {
        uint32_t u;
        random_bytes(&u, sizeof(u));
        return u;
}

/* For basic lookup tables with strictly enumerated entries */
#define __DEFINE_STRING_TABLE_LOOKUP(name,type,scope)                   \
        scope const char *name##_to_string(type i) {                    \
                if (i < 0 || i >= (type) ELEMENTSOF(name##_table))      \
                        return NULL;                                    \
                return name##_table[i];                                 \
        }                                                               \
        scope type name##_from_string(const char *s) {                  \
                type i;                                                 \
                if (!s)                                                 \
                        return (type) -1;                               \
                for (i = 0; i < (type)ELEMENTSOF(name##_table); i++)    \
                        if (name##_table[i] &&                          \
                            streq(name##_table[i], s))                  \
                                return i;                               \
                return (type) -1;                                       \
        }                                                               \
        struct __useless_struct_to_allow_trailing_semicolon__

#define DEFINE_STRING_TABLE_LOOKUP(name,type) __DEFINE_STRING_TABLE_LOOKUP(name,type,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name,type) __DEFINE_STRING_TABLE_LOOKUP(name,type,static)

/* For string conversions where numbers are also acceptable */
#define DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name,type,max)         \
        int name##_to_string_alloc(type i, char **str) {                \
                char *s;                                                \
                int r;                                                  \
                if (i < 0 || i > max)                                   \
                        return -ERANGE;                                 \
                if (i < (type) ELEMENTSOF(name##_table)) {              \
                        s = strdup(name##_table[i]);                    \
                        if (!s)                                         \
                                return log_oom();                       \
                } else {                                                \
                        r = asprintf(&s, "%u", i);                      \
                        if (r < 0)                                      \
                                return log_oom();                       \
                }                                                       \
                *str = s;                                               \
                return 0;                                               \
        }                                                               \
        type name##_from_string(const char *s) {                        \
                type i;                                                 \
                unsigned u = 0;                                         \
                assert(s);                                              \
                for (i = 0; i < (type)ELEMENTSOF(name##_table); i++)    \
                        if (name##_table[i] &&                          \
                            streq(name##_table[i], s))                  \
                                return i;                               \
                if (safe_atou(s, &u) >= 0 && u <= max)                  \
                        return (type) u;                                \
                return (type) -1;                                       \
        }                                                               \
        struct __useless_struct_to_allow_trailing_semicolon__

int fd_nonblock(int fd, bool nonblock);
int fd_cloexec(int fd, bool cloexec);

int reset_terminal_fd(int fd, bool switch_to_text);
int reset_terminal(const char *name);

int open_terminal(const char *name, int mode);

int flush_fd(int fd);

int ignore_signals(int sig, ...);
int default_signals(int sig, ...);
int sigaction_many(const struct sigaction *sa, ...);

int fopen_temporary(const char *path, FILE **_f, char **_temp_path);

ssize_t loop_read(int fd, void *buf, size_t nbytes, bool do_poll);
ssize_t loop_write(int fd, const void *buf, size_t nbytes, bool do_poll);

bool is_device_path(const char *path);

int getttyname_malloc(int fd, char **r);

int pipe_eof(int fd);

int fd_columns(int fd);
unsigned columns(void);
int fd_lines(int fd);
unsigned lines(void);
void columns_lines_cache_reset(int _unused_ signum);

bool on_tty(void);

static inline const char *ansi_highlight(void) {
        return on_tty() ? ANSI_HIGHLIGHT_ON : "";
}

static inline const char *ansi_highlight_red(void) {
        return on_tty() ? ANSI_HIGHLIGHT_RED_ON : "";
}

static inline const char *ansi_highlight_green(void) {
        return on_tty() ? ANSI_HIGHLIGHT_GREEN_ON : "";
}

static inline const char *ansi_highlight_yellow(void) {
        return on_tty() ? ANSI_HIGHLIGHT_YELLOW_ON : "";
}

static inline const char *ansi_highlight_blue(void) {
        return on_tty() ? ANSI_HIGHLIGHT_BLUE_ON : "";
}

static inline const char *ansi_highlight_off(void) {
        return on_tty() ? ANSI_HIGHLIGHT_OFF : "";
}

int files_same(const char *filea, const char *fileb);

int running_in_chroot(void);

char *unquote(const char *s, const char *quotes);
char *normalize_env_assignment(const char *s);

bool null_or_empty(struct stat *st) _pure_;
int null_or_empty_path(const char *fn);
int null_or_empty_fd(int fd);

char *fstab_node_to_udev_node(const char *p);

bool nulstr_contains(const char*nulstr, const char *needle);

char* strshorten(char *s, size_t l);

int in_gid(gid_t gid);

char* uid_to_name(uid_t uid);
char* gid_to_name(gid_t gid);

char *strjoin(const char *x, ...) _sentinel_;

bool is_main_thread(void);

#define NULSTR_FOREACH(i, l)                                    \
        for ((i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#define NULSTR_FOREACH_PAIR(i, j, l)                             \
        for ((i) = (l), (j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

int log_level_to_string_alloc(int i, char **s);
int log_level_from_string(const char *s);

extern int saved_argc;
extern char **saved_argv;

int fd_wait_for_event(int fd, int event, usec_t timeout);

void* memdup(const void *p, size_t l) _alloc_(2);

int fd_inc_sndbuf(int fd, size_t n);

static inline void freep(void *p) {
        free(*(void**) p);
}

#define DEFINE_TRIVIAL_CLEANUP_FUNC(type, func)                 \
        static inline void func##p(type *p) {                   \
                if (*p)                                         \
                        func(*p);                               \
        }                                                       \
        struct __useless_struct_to_allow_trailing_semicolon__

static inline void closep(int *fd) {
        safe_close(*fd);
}

static inline void umaskp(mode_t *u) {
        umask(*u);
}

DEFINE_TRIVIAL_CLEANUP_FUNC(FILE*, fclose);
DEFINE_TRIVIAL_CLEANUP_FUNC(FILE*, pclose);
DEFINE_TRIVIAL_CLEANUP_FUNC(DIR*, closedir);
DEFINE_TRIVIAL_CLEANUP_FUNC(FILE*, endmntent);

#define _cleanup_free_ _cleanup_(freep)
#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_umask_ _cleanup_(umaskp)
#define _cleanup_globfree_ _cleanup_(globfree)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_endmntent_ _cleanup_(endmntentp)

_malloc_  _alloc_(1, 2) static inline void *malloc_multiply(size_t a, size_t b) {
        if (_unlikely_(b == 0 || a > ((size_t) -1) / b))
                return NULL;

        return malloc(a * b);
}

_alloc_(2, 3) static inline void *memdup_multiply(const void *p, size_t a, size_t b) {
        if (_unlikely_(b == 0 || a > ((size_t) -1) / b))
                return NULL;

        return memdup(p, a * b);
}

/**
 * Check if a string contains any glob patterns.
 */
_pure_ static inline bool string_is_glob(const char *p) {
        return !!strpbrk(p, GLOB_CHARS);
}

void *xbsearch_r(const void *key, const void *base, size_t nmemb, size_t size,
                 int (*compar) (const void *, const void *, void *),
                 void *arg);

#define FOREACH_LINE(line, f, on_error)                         \
        for (;;)                                                \
                if (!fgets(line, sizeof(line), f)) {            \
                        if (ferror(f)) {                        \
                                on_error;                       \
                        }                                       \
                        break;                                  \
                } else

char *strextend(char **x, ...) _sentinel_;
char *strrep(const char *s, unsigned n);

void* greedy_realloc(void **p, size_t *allocated, size_t need, size_t size);
void* greedy_realloc0(void **p, size_t *allocated, size_t need, size_t size);
#define GREEDY_REALLOC(array, allocated, need)                          \
        greedy_realloc((void**) &(array), &(allocated), (need), sizeof((array)[0]))

#define GREEDY_REALLOC0(array, allocated, need)                         \
        greedy_realloc0((void**) &(array), &(allocated), (need), sizeof((array)[0]))

static inline void _reset_errno_(int *saved_errno) {
        errno = *saved_errno;
}

#define PROTECT_ERRNO _cleanup_(_reset_errno_) __attribute__((unused)) int _saved_errno_ = errno

int unlink_noerrno(const char *path);

#define alloca0(n)                                      \
        ({                                              \
                char *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca(_len_);                  \
                (void *) memset(_new_, 0, _len_);       \
        })

#define strappenda(a, ...)                                       \
        ({                                                       \
                int _len = strlen(a);                            \
                unsigned _i;                                     \
                char *_d_, *_p_;                                 \
                const char *_appendees_[] = { __VA_ARGS__ };     \
                for (_i = 0; _i < ELEMENTSOF(_appendees_); _i++) \
                        _len += strlen(_appendees_[_i]);         \
                _d_ = alloca(_len + 1);                          \
                _p_ = stpcpy(_d_, a);                            \
                for (_i = 0; _i < ELEMENTSOF(_appendees_); _i++) \
                        _p_ = stpcpy(_p_, _appendees_[_i]);      \
                _d_;                                             \
        })

/**
 * Normal qsort requires base to be nonnull. Here were require
 * that only if nmemb > 0.
 */
static inline void qsort_safe(void *base, size_t nmemb, size_t size,
                              int (*compar)(const void *, const void *)) {
        if (nmemb) {
                assert(base);
                qsort(base, nmemb, size, compar);
        }
}

int getpeercred(int fd, struct ucred *ucred);

int mkostemp_safe(char *pattern, int flags);
