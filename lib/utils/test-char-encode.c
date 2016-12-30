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
#include <assert.h>

#include "utils.h"


static void test_encode(const char ch, const char *test)
{
	char dst[5] = {};

	char_encode(dst, ch);
	assert(!strcmp(dst, test));

	printf("test encode char: '%s' ['%x']\n", dst, ch);
}

int main()
{
	const char hexarr[] = "0123456789abcdef";

	char ch = 0;
	char test[] = "\\x00";
	uint8_t x = 0, y = 0;

	do
	{
		test_encode(ch++, test);

		if (x++ == 0xf)
		{
			x = 0;
			if (y == 0xf)
				y = 0;
			else
				y++;
		}

		test[2] = hexarr[y];
		test[3] = hexarr[x];
	} while (ch);

	return EXIT_SUCCESS;
}
