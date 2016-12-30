/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include "path.h"
#include "utils.h"


size_t path_encode(char *dst, const char *src, size_t size)
{
	size_t len = size;

	while (*src)
	{
		if (src[0] == '/' || src[0] == '\\')
		{
			char_encode(dst, src[0]);
			src++;
			dst += 4;
			size -= 4;
			continue;
		}

		*dst++ = *src++;
		size--;
	}

	*dst = '\0';

	return len - size;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


static void test_encode(const char *path, const char *test)
{
	char dst[PATH_SIZE] = {};
	size_t res;

	res = path_encode(dst, path, sizeof(dst));
	assert(res == strlen(test));
	assert(!strcmp(dst, test));

	printf("test encode path: '%s'(%zd) ['%s']\n", dst, res, path);
}

int main()
{
	// test slash
	test_encode("/cdrom", "\\x2fcdrom");
	test_encode("/disk/by-id/one", "\\x2fdisk\\x2fby-id\\x2fone");
	test_encode("/disk/by-id/two", "\\x2fdisk\\x2fby-id\\x2ftwo");
	test_encode("/disk/by-id/three", "\\x2fdisk\\x2fby-id\\x2fthree");

	// test space
	test_encode("/disk/by-partlabel/EFI\\x20System", "\\x2fdisk\\x2fby-partlabel\\x2fEFI\\x5cx20System");

	return EXIT_SUCCESS;
}

#endif // TESTS