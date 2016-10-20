/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef _UDEV_UTILS_H_
#define _UDEV_UTILS_H_

#include <stdint.h>
#include <sys/types.h>


int parse_uint(const char *str, uint32_t *pval);

int lookup_user(const char *user, uid_t *puid);
int lookup_group(const char *group, gid_t *pgid);

#endif	/* _UDEV_UTILS_H_ */