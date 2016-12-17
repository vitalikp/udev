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

#include "utils/path.h"


/* The max size of udev path */
#define PATH_SIZE		1024


/**
 * str_empty:
 * @str: input string
 *
 * Returns: 1 if string str is empty, or 0 otherwise
 */
static inline int str_empty(const char *str)
{
	return (!str || *str == '\0');
}

/**
 * str_copy:
 * @dst: destination string
 * @src: source string
 * @size: maximum size of the destination string
 *
 * Copy source string src to destination dst with length len-1
 * and put zero at the end.
 *
 * Returns: length of destination string
 */
static inline size_t str_copy(char *dst, const char *src, size_t size)
{
	size_t len = size;

	while (*src && size > 1)
	{
		*dst++ = *src++;
		size--;
	}

	*dst = '\0';

	return len - size;
}

int parse_uint(const char *str, uint32_t *pval);


/**
 * Path util functions
 */

/**
 * path_relative:
 * @dst: destination string
 * @from: source path
 * @to: destination path
 * @size: maximum size of the destination string
 *
 * Build relative path from 'to' to 'from', store string in dst.
 *
 * Returns: length of destination string, or 0 if error
 */
size_t path_relative(char *dst, const char *from, const char *to, size_t size);
int path_create(const char *path, mode_t mode);
int path_remove(const char *path);

int lookup_user(const char *user, uid_t *puid);
int lookup_group(const char *group, gid_t *pgid);

#endif	/* _UDEV_UTILS_H_ */