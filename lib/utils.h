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

#include "utils/str.h"
#include "utils/path.h"


static inline char char_hex(uint8_t val)
{
	if (val < 10)
		return '0' + val;

	return 'a' + val - 10;
}

static inline void char_encode(char *dst, const char ch)
{
	*dst++ = '\\';
	*dst++ = 'x';
	*dst++ = char_hex(ch>>4 & 0xf);
	*dst++ = char_hex(ch & 0xf);
}

int parse_uint(const char *str, uint32_t *pval);


int lookup_user(const char *user, uid_t *puid);
int lookup_group(const char *group, gid_t *pgid);

#endif	/* _UDEV_UTILS_H_ */