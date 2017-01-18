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


str_t* str_new(size_t size)
{
	str_t *str;

	str = calloc(1, sizeof(str_t) + size);
	if (!str)
		return NULL;

	str->size = size;

	return str;
}


#ifdef TESTS
#include <assert.h>


int main()
{
	str_t *str;

	str = str_new(0);
	assert(str);
	assert(!str->len);
	assert(!str->size);

	free(str);

	return EXIT_SUCCESS;
}

#endif // TESTS