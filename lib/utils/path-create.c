/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <errno.h>

#include "path.h"
#include "utils.h"


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>


static int test_mkdir(const char *path, mode_t mode)
{
	int res;

	errno = 0;
	res = mkdir(path, mode);

	printf("mkdir: '%s'(mode %o) - %m\n", path, mode);

	return res;
}

static void test_root()
{
	printf("--- test create root path ---\n");

	int res;

	res = path_mkdir("/", 0755, test_mkdir);
	assert(!res);

	printf("test create root path '/'\n\n");
}

static void test_exist(const char *path)
{
	printf("--- test create exist path ---\n");

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

	res = path_mkdir(path, 0755, test_mkdir);
	assert(!res);

	st = (struct stat){};
	res = stat(dir, &st);
	assert(!res);
	assert(S_ISDIR(st.st_mode));

	printf("test create exist path '%s': %s\n\n", path, dir);
}

static void test_create(const char *path)
{
	printf("--- test create path ---\n");

	char dir[PATH_SIZE];
	struct stat st;
	int res;

	str_copy(dir, path, sizeof(dir));

	char *p = strrchr(dir, '/');
	assert(p);

	*p = '\0';

	res = path_mkdir(path, 0755, test_mkdir);
	assert(!res);

	res = stat(dir, &st);
	assert(!res);
	assert(S_ISDIR(st.st_mode));

	printf("test create path '%s'(mode %o): %s\n\n", path, st.st_mode, dir);
}

static void test_file(const char *path)
{
	printf("--- test create file ---\n");

	char file[PATH_SIZE];
	int res;

	str_copy(file, path, sizeof(file));

	char *p = strrchr(file, '/');
	assert(p);

	*p = '\0';

	res = open(file, O_CREAT, 0400);
	assert(res > 0);
	close(res);

	res = path_mkdir(path, 0755, test_mkdir);
	assert(res < 0);
	assert(errno == ENOTDIR);

	printf("test create file path '%s': %s\n\n", path, file);
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