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


int path_create(const char *path, mode_t mode)
{
	char dir[PATH_SIZE];
	char *p = dir;
	struct stat st;

	*p++ = *path++;

	while (*path)
	{
		*p = *path++;
		if (*p++ == '/')
		{
			*p = '\0';

			if (!mkdir(dir, mode))
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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>


static void test_root()
{
	int res;

	res = path_create("/", 0755);
	assert(!res);
}

static void test_exist(const char *path)
{
	char dir[PATH_SIZE];
	struct stat st;
	int res;

	str_copy(dir, path, sizeof(dir));

	char *p = strrchr(dir, '/');
	assert(p);

	*p = '\0';
	res = stat(dir, &st);
	assert(!res);
	assert(S_ISDIR(st.st_mode));

	res = path_create(path, 0755);
	assert(!res);

	st = (struct stat){};
	res = stat(dir, &st);
	assert(!res);
	assert(S_ISDIR(st.st_mode));

	printf("test create exist path '%s': %s\n", path, dir);
}

static void test_create(const char *path)
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

	printf("test create path '%s'(mode %o): %s\n", path, st.st_mode, dir);
}

static void test_file(const char *path)
{
	char file[PATH_SIZE];
	int res;

	str_copy(file, path, sizeof(file));

	char *p = strrchr(file, '/');
	assert(p);

	*p = '\0';

	res = open(file, O_CREAT, 0400);
	assert(res > 0);
	close(res);

	res = path_create(path, 0755);
	assert(res < 0);
	assert(errno == ENOTDIR);

	printf("test create file path '%s': %s\n", path, file);
}

int main()
{
	setbuf(stdout, NULL);

	test_root();
	test_exist("/tmp/one");

	test_create("/tmp/test-path-create/one");
	test_exist("/tmp/test-path-create/one");

	test_create("/tmp/test-path-create/test1/new");
	test_exist("/tmp/test-path-create/test1/new");
	test_exist("/tmp/test-path-create/test1");

	test_create("/tmp/test-path-create/a/b/c/d/e/f");

	test_file("/tmp/test-path-create/file1/two");

	execl("/usr/bin/rm", "rm", "-rfv", "/tmp/test-path-create", NULL);

	return EXIT_SUCCESS;
}

#endif // TESTS