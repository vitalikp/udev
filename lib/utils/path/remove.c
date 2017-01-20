/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include "path.h"
#include "utils.h"


int path_remove(const char *path)
{
	char dir[PATH_SIZE];
	char *p = dir;
	bool last = false;

	p += str_copy(dir, path, sizeof(dir));

	while (p > dir)
	{
		if (*p == '/')
		{
			if (last)
			{
				if (rmdir(dir) < 0)
				{
					if (errno != ENOENT)
						return -1;

					break;
				}
			}

			*p = '\0';

			last = true;
		}

		p--;
	}

	return 0;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "utils.h"


static void test_root(const char *path)
{
	int res;

	res = path_remove(path);
	assert(!res);

	printf("test remove root path '%s'\n", path);
}

static void test_empty(const char *path)
{
	int res;

	res = path_remove(path);
	assert(!res);

	printf("test remove empty path '%s': %m\n", path);
}

static void test_remove(const char *path)
{
	char dir[PATH_SIZE];
	struct stat st;
	int res;

	str_copy(dir, path, sizeof(dir));

	char *p = strrchr(dir, '/');
	assert(p);

	*p = '\0';

	res = path_create(path, 0755);
	assert(!res);

	res = stat(dir, &st);
	assert(!res);
	assert(S_ISDIR(st.st_mode));

	res = path_remove(path);
	assert(!res);

	res = stat(dir, &st);
	assert(res < 0);
	assert(errno == ENOENT);

	printf("test remove path '%s': %s\n", path, dir);
}

static void test_file(const char *path)
{
	char file[PATH_SIZE];
	struct stat st;
	int res;

	str_copy(file, path, sizeof(file));

	char *p = strrchr(file, '/');
	assert(p);

	*p = '\0';

	res = path_create(file, 0755);
	assert(!res);

	res = open(file, O_CREAT, 0400);
	assert(res > 0);
	close(res);

	res = path_remove(path);
	assert(res < 0);
	assert(errno == ENOTDIR);

	res = unlink(file);
	assert(!res);

	res = path_remove(path);
	assert(!res);

	printf("test remove file path '%s': %s\n", path, file);
}

static void test_data(const char *path)
{
	int res;

	res = path_create(path, 0755);
	assert(!res);

	res = open(path, O_CREAT, 0400);
	assert(res > 0);
	close(res);

	res = path_remove(path);
	assert(res < 0);
	assert(errno == ENOTEMPTY);

	res = unlink(path);
	assert(!res);

	res = path_remove(path);
	assert(!res);

	printf("test remove data path '%s'\n", path);
}

int main()
{
	setbuf(stdout, NULL);

	test_root("/");
	test_root("/tmp/two");

	test_empty("/tmp/test-remove-empty/old");

	test_remove("/tmp/test-remove-path/two");
	test_empty("/tmp/test-remove-path/two");

	test_remove("/tmp/test-remove-path/test2/old");
	test_empty("/tmp/test-remove-path/test2/old");
	test_empty("/tmp/test-remove-path/test2");

	test_remove("/tmp/test-remove-path/a/b/c/d/e/f");

	test_file("/tmp/test-remove-path/file2/one");
	test_data("/tmp/test-remove-path/file2/data");

	return EXIT_SUCCESS;
}

#endif // TESTS