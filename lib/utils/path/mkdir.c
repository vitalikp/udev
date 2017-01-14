/*
 * Copyright © 2016-2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include "path.h"
#include "utils.h"


int path_mkdir(const char *path, mode_t mode, mkdir_func pmkdir)
{
	char dir[PATH_SIZE];
	char *p = dir;
	struct stat st;
	bool first = true;

	while (*path)
	{
		*p++ = *path++;

		if (*path == '/')
		{
			*p = '\0';

			if (first)
			{
				first = false;
				continue;
			}

			if (!pmkdir(dir, mode))
				continue;

			if (errno == EEXIST)
			{
				st = (struct stat){};
				if (stat(dir, &st) < 0)
					return -1;
				if (S_ISDIR(st.st_mode))
					continue;

				errno = ENOTDIR;
				return -1;
			}

			if (errno != ENOENT)
				return -1;
		}
	}

	return 0;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static int test_mkdir(const char *path, mode_t mode)
{
	printf("mkdir: '%s'(mode %#o)\n", path, mode);

	return 0;
}

int main()
{
	int res;

	res = path_mkdir("/path/a/b/c/d/e/f", 0755, test_mkdir);
	assert(!res);

	res = path_mkdir("/path1/1/new", 0400, test_mkdir);
	assert(!res);

	return EXIT_SUCCESS;
}
#endif // TESTS