/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <sys/stat.h>
#include <errno.h>

#include "utils.h"
#include "path.h"


size_t path_relative(char *dst, const char *from, const char *to, size_t size)
{
	if (str_empty(from) || str_empty(to))
	{
		errno = EINVAL;
		return 0;
	}

	size_t len = size;

	char *p = (char*)from;

	while (*from)
	{
		if (*from != *to)
			break;

		if (*from == '/')
			p = (char*)from;

		to++;
		from++;
	}

	if (p != from)
	{
		p++;

		while (*to)
		{
			if (*to == '/')
			{
				if (size < 4)
				{
					errno = EOVERFLOW;
					return 0;
				}

				*dst++ = '.';
				*dst++ = '.';
				*dst++ = '/';
				size -= 3;
			}

			to++;
		}
	}

	while (*p)
	{
		if (size < 2)
		{
			errno = EOVERFLOW;
			return 0;
		}

		*dst++ = *p++;
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


void test_empty(const char *from, const char *to)
{
	char dst[PATH_SIZE] = {};

	size_t res;

	res = path_relative(dst, from, to, sizeof(dst));
	assert(!res);
	assert(errno == EINVAL);

	printf("test empty relative path: '%s' => '%s'\n", to, from);
}

void test_overflow(const char *from, const char *to, size_t len)
{
	char dst[len];

	size_t res;

	res = path_relative(dst, from, to, len);
	assert(!res);
	assert(errno == EOVERFLOW);

	printf("test overflow relative path[%zu]: '%s'(%zu) ['%s' => '%s']\n", len, dst, res, to, from);
}

void test_full(const char *from, const char *to, size_t len)
{
	char dst[len];

	size_t res;

	res = path_relative(dst, from, to, sizeof(dst));
	assert(res + 1 == len);

	printf("test full relative path[%zu]: '%s'(%zu) ['%s' => '%s']\n", len, dst, res, to, from);
}

void test_path(const char *from, const char *to, const char *test)
{
	char dst[PATH_SIZE] = {};
	size_t res;

	res = path_relative(dst, from, to, sizeof(dst));
	assert(res == strlen(test));
	assert(!strcmp(dst, test));

	printf("test relative path: '%s'(%zd) ['%s' => '%s']\n", dst, res, to, from);
}

int main()
{
	test_empty(NULL, NULL);
	test_empty("", "");
	test_empty("/path/1", "");
	test_empty("", "/path/2");

	test_overflow("/path1", "/path2", 4);
	test_overflow("/path1", "/path2", 5);

	test_full("/path", "/link", 5);

	// test root path
	test_path("/one/two", "/two/one", "../one/two");

	// test different paths
	test_path("/one/two", "two/one", "/one/two");
	test_path("one/two", "/two/one", "one/two");

	// test many slash symbols
	test_path("/path", "/a/b/c/d/e/f/g/h", "../../../../../../../path");
	test_path("/a/b/c/d/e/f/g/h", "/path", "a/b/c/d/e/f/g/h");

	// test current directory
	test_path("/path/1", "/path/2", "1");
	test_path("/path/1", "/path/onew/2", "../1");
	test_path("/path/1", "/path/1/onew/2", "../../1");

	// test full string match
	test_path("/path/1", "/path/1onew/2", "../1");
	test_path("/path/1onew/2", "/path/1", "1onew/2");

	return EXIT_SUCCESS;
}

#endif // TESTS