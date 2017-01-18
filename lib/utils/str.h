/*
 * Copyright © 2016-2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef _UDEV_UTILS_STR_H_
#define _UDEV_UTILS_STR_H_

#include <stddef.h>


typedef struct str_t
{
	/* current length */
	size_t	len;
	/* allocation size */
	size_t	size;
	/* string value */
	char	val[0];
} str_t;


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
 * str_eq:
 * @str1: input string 1
 * @str2: input string 2
 *
 * Returns: 1 if string str is equal, or 0 otherwise
 */
static inline int str_eq(const char *str1, const char *str2)
{
	do
	{
		if (*str1 != *str2++)
			return 0;

	} while (*str1++);

	return 1;
}

/**
 * str_copy:
 * @dst: destination string
 * @src: source string
 * @size: maximum size of the destination string
 *
 * Copy source string src to destination dst with length size-1
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


/**
 * str_new:
 * @size: allocation size of the string
 *
 * Create empty string with allocated size 'size'
 *
 * Returns: pointer to created struct str_t, or NULL on error
 */
str_t* str_new(size_t size);

/**
 * str_resize:
 * @dst: destination string
 * @size: new size of the string
 *
 * Resize string 'dst' to size 'size'
 *
 * Returns: 0 on success, or -1 if error
 */
int str_resize(str_t **dst, size_t size);

/**
 * str_reset:
 * @str: input string
 *
 * Reset string 'str' to zero size
 *
 */
static inline void str_reset(str_t *str)
{
	str->val[0] = '\0';
	str->len = 0;
}

/**
 * str_set:
 * @dst: destination string
 * @str: the string to set
 * @len: length of the string 'str'
 *
 * Set string 'str' to the 'dst' string
 *
 * Returns: destination string, or NULL on error
 */
int str_set(str_t **dst, const char *str, size_t len);

/**
 * str_append:
 * @dst: destination string
 * @str: the string to append
 * @len: length of the string 'str'
 *
 * Append string 'str' to the end of 'dst' string
 *
 * Returns: destination string, or NULL on error
 */
int str_append(str_t **dst, const char *str, size_t len);

#endif	/* _UDEV_UTILS_STR_H_ */