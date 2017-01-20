/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include "str.h"


int str_set(str_t **dst, const char *str, size_t len)
{
	if (str_resize(dst, len + 1) < 0)
		return -1;

	(*dst)->len = str_copy((*dst)->val, str, (*dst)->size);

	return 0;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


static void test_set(str_t **pstr, const char *test, size_t len)
{
	str_t *str = *pstr;

	assert(!str_set(&str, test, len));
	assert(str);
	assert(str->len == len);
	assert(str->size >= len);
	assert(str_eq(test, str->val));

	printf("test set string '%s': '%s'(len %zu)\n", test, str->val, str->len);

	*pstr = str;
}

int main()
{
	char test[] = "test set string";
	str_t *str = NULL;
	size_t len;

	len = sizeof(test) - 1;
	test_set(&str, test, len);
	test_set(&str, "   ", 3);
	test_set(&str, test, len);

	free(str);

	return EXIT_SUCCESS;
}

#endif // TESTS