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


int path_mkdir(const char *path, mkdir_func pmkdir, mode_t mode, void* data)
{
	char dir[PATH_SIZE];
	char *p = dir;
	struct stat st;
	bool first = true;

	if (!pmkdir)
		pmkdir = run_mkdir;

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

			if (!pmkdir(dir, mode, data))
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


static int test_mkdir(const char *path, mode_t mode, void* data)
{
	printf("mkdir: '%s'(mode %#o)\n", path, mode);

	return 0;
}

int main()
{
	int res;

	res = path_mkdir("/path/a/b/c/d/e/f", test_mkdir, 0755, NULL);
	assert(!res);

	res = path_mkdir("/path1/1/new", test_mkdir, 0400, NULL);
	assert(!res);

	return EXIT_SUCCESS;
}
#endif // TESTS