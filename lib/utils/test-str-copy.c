/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "utils.h"


static void test_copy(const char *str)
{
	char copy[50];
	size_t len;

	str_copy(copy, str, sizeof(copy));
	len = strlen(copy);

	assert(!strncmp(str, copy, len));

	printf("test copy string '%s': '%s'(len %zu)\n", str, copy, len);
}

int main()
{
	test_copy("");

	test_copy("a");
	test_copy("test copy");

	test_copy("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");

	return EXIT_SUCCESS;
}