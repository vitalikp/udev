/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdlib.h>

#include "str.h"


int str_resize(str_t **dst, size_t size)
{
	str_t *str = *dst;

	if (str && str->size >= size)
		return 0;

	str = realloc(str, sizeof(str_t) + size);
	if (!str)
		return -1;

	str->size = size;
	*dst = str;

	return 0;
}