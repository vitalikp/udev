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


int str_append(str_t **dst, const char *str, size_t len)
{
	if (*dst)
		len += (*dst)->len;

	if (str_resize(dst, len + 1) < 0)
		return -1;

	(*dst)->len += str_copy((*dst)->val + (*dst)->len, str, (*dst)->size);

	return 0;
}


#ifdef TESTS
#include <stdio.h>
#include <string.h>
#include <assert.h>


static void test_append(str_t **pstr, const char *test, size_t len)
{
	str_t *str = *pstr;
	size_t size;

	size = len;
	if (str)
		size += str->len;
	assert(!str_append(&str, test, len));
	assert(str);
	assert(str->len == size);
	assert(str->size == size + 1);
	size -= len;
	assert(str_eq(test, str->val + size));

	printf("test append string '%s': '%s'(len %zu)\n", test, str->val, str->len);

	*pstr = str;
}

int main()
{
	char test1[] = "test string";
	char test2[] = "append string";
	str_t *str = NULL;
	size_t len;

	len = sizeof(test1) - 1;
	test_append(&str, test1, len);
	test_append(&str, "   ", 3);
	len = sizeof(test2) - 1;
	test_append(&str, test2, len);

	free(str);

	return EXIT_SUCCESS;
}

#endif // TESTS