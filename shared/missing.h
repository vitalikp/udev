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

/* Missing glibc definitions to access certain kernel APIs */

#include <sys/resource.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/oom.h>
#include <linux/input.h>
#include <linux/if_link.h>
#include <linux/loop.h>
#include <linux/if_link.h>

#ifdef HAVE_AUDIT
#include <libaudit.h>
#endif

#include "macro.h"

#ifdef ARCH_MIPS
#include <asm/sgidefs.h>
#endif

#ifndef RLIMIT_RTTIME
#define RLIMIT_RTTIME 15
#endif

/* If RLIMIT_RTTIME is not defined, then we cannot use RLIMIT_NLIMITS as is */
#define _RLIMIT_MAX (RLIMIT_RTTIME+1 > RLIMIT_NLIMITS ? RLIMIT_RTTIME+1 : RLIMIT_NLIMITS)

#ifndef TIOCVHANGUP
#define TIOCVHANGUP 0x5437
#endif

#if !HAVE_DECL_PIVOT_ROOT
static inline int pivot_root(const char *new_root, const char *put_old) {
        return syscall(SYS_pivot_root, new_root, put_old);
}
#endif

#ifdef __x86_64__
#  ifndef __NR_fanotify_init
#    define __NR_fanotify_init 300
#  endif
#  ifndef __NR_fanotify_mark
#    define __NR_fanotify_mark 301
#  endif
#elif defined _MIPS_SIM
#  if _MIPS_SIM == _MIPS_SIM_ABI32
#    ifndef __NR_fanotify_init
#      define __NR_fanotify_init 4336
#    endif
#    ifndef __NR_fanotify_mark
#      define __NR_fanotify_mark 4337
#    endif
#  elif _MIPS_SIM == _MIPS_SIM_NABI32
#    ifndef __NR_fanotify_init
#      define __NR_fanotify_init 6300
#    endif
#    ifndef __NR_fanotify_mark
#      define __NR_fanotify_mark 6301
#    endif
#  elif _MIPS_SIM == _MIPS_SIM_ABI64
#    ifndef __NR_fanotify_init
#      define __NR_fanotify_init 5295
#    endif
#    ifndef __NR_fanotify_mark
#      define __NR_fanotify_mark 5296
#    endif
#  endif
#else
#  ifndef __NR_fanotify_init
#    define __NR_fanotify_init 338
#  endif
#  ifndef __NR_fanotify_mark
#    define __NR_fanotify_mark 339
#  endif
#endif

#ifndef HAVE_FANOTIFY_INIT
static inline int fanotify_init(unsigned int flags, unsigned int event_f_flags) {
        return syscall(__NR_fanotify_init, flags, event_f_flags);
}
#endif

#ifndef HAVE_FANOTIFY_MARK
static inline int fanotify_mark(int fanotify_fd, unsigned int flags, uint64_t mask,
                                int dfd, const char *pathname) {
#if defined _MIPS_SIM && _MIPS_SIM == _MIPS_SIM_ABI32 || defined __powerpc__ && !defined __powerpc64__ \
    || defined __arm__ && !defined __aarch64__
        union {
                uint64_t _64;
                uint32_t _32[2];
        } _mask;
        _mask._64 = mask;

        return syscall(__NR_fanotify_mark, fanotify_fd, flags,
                       _mask._32[0], _mask._32[1], dfd, pathname);
#else
        return syscall(__NR_fanotify_mark, fanotify_fd, flags, mask, dfd, pathname);
#endif
}
#endif

#ifndef BTRFS_IOCTL_MAGIC
#define BTRFS_IOCTL_MAGIC 0x94
#endif

#ifndef BTRFS_PATH_NAME_MAX
#define BTRFS_PATH_NAME_MAX 4087
#endif

#ifndef BTRFS_DEVICE_PATH_NAME_MAX
#define BTRFS_DEVICE_PATH_NAME_MAX 1024
#endif

#ifndef BTRFS_FSID_SIZE
#define BTRFS_FSID_SIZE 16
#endif

#ifndef BTRFS_UUID_SIZE
#define BTRFS_UUID_SIZE 16
#endif

#ifndef HAVE_LINUX_BTRFS_H
struct btrfs_ioctl_vol_args {
        int64_t fd;
        char name[BTRFS_PATH_NAME_MAX + 1];
};

struct btrfs_ioctl_dev_info_args {
        uint64_t devid;                         /* in/out */
        uint8_t uuid[BTRFS_UUID_SIZE];          /* in/out */
        uint64_t bytes_used;                    /* out */
        uint64_t total_bytes;                   /* out */
        uint64_t unused[379];                   /* pad to 4k */
        char path[BTRFS_DEVICE_PATH_NAME_MAX];  /* out */
};

struct btrfs_ioctl_fs_info_args {
        uint64_t max_id;                        /* out */
        uint64_t num_devices;                   /* out */
        uint8_t fsid[BTRFS_FSID_SIZE];          /* out */
        uint64_t reserved[124];                 /* pad to 1k */
};
#endif

#ifndef BTRFS_IOC_DEVICES_READY
#define BTRFS_IOC_DEVICES_READY _IOR(BTRFS_IOCTL_MAGIC, 39, \
                                 struct btrfs_ioctl_vol_args)
#endif

#ifndef MS_PRIVATE
#define MS_PRIVATE  (1 << 18)
#endif

#if !HAVE_DECL_GETTID
static inline pid_t gettid(void) {
        return (pid_t) syscall(SYS_gettid);
}
#endif

#ifndef MS_REC
#define MS_REC 16384
#endif

#ifndef MAX_HANDLE_SZ
#define MAX_HANDLE_SZ 128
#endif

#ifndef __NR_name_to_handle_at
#  if defined(__x86_64__)
#    define __NR_name_to_handle_at 303
#  elif defined(__i386__)
#    define __NR_name_to_handle_at 341
#  elif defined(__arm__)
#    define __NR_name_to_handle_at 370
#  elif defined(__powerpc__)
#    define __NR_name_to_handle_at 345
#  else
#    error "__NR_name_to_handle_at is not defined"
#  endif
#endif

#if !HAVE_DECL_NAME_TO_HANDLE_AT
struct file_handle {
        unsigned int handle_bytes;
        int handle_type;
        unsigned char f_handle[0];
};

static inline int name_to_handle_at(int fd, const char *name, struct file_handle *handle, int *mnt_id, int flags) {
        return syscall(__NR_name_to_handle_at, fd, name, handle, mnt_id, flags);
}
#endif

#ifndef HAVE_SECURE_GETENV
#  ifdef HAVE___SECURE_GETENV
#    define secure_getenv __secure_getenv
#  else
#    error "neither secure_getenv nor __secure_getenv are available"
#  endif
#endif

#ifndef __NR_setns
#  if defined(__x86_64__)
#    define __NR_setns 308
#  elif defined(__i386__)
#    define __NR_setns 346
#  else
#    error "__NR_setns is not defined"
#  endif
#endif

#if !HAVE_DECL_SETNS
static inline int setns(int fd, int nstype) {
        return syscall(__NR_setns, fd, nstype);
}
#endif
