/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <string.h>
#include <grp.h>
#include <errno.h>

#include "utils.h"


int lookup_group(const char *group, gid_t *pgid)
{
	if (!group || !pgid)
	{
		errno = EINVAL;
		return -1;
	}

	if (!strcmp("root", group))
	{
		*pgid = 0;
		return 0;
	}

	gid_t gid;

	if (!parse_uint(group, &gid))
	{
		if(!getgrgid(gid))
			return -1;

		*pgid = gid;
		return 0;
	}

	struct group *gr;

	errno = 0;
	gr = getgrnam(group);
	if (gr)
	{
		*pgid = gr->gr_gid;
		return 0;
	}

	return -1;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


static void test_group_null(const char *group)
{
	int res;

	res = lookup_group(group, NULL);
	assert(res < 0);

	printf("test null lookup group '%s'\n", group);
}

static void test_group_fail(const char *group)
{
	uid_t uid = -1;

	int res;

	res = lookup_group(group, &uid);
	assert(res < 0);
	assert(uid == -1);

	printf("test fail lookup group '%s': %m\n", group);
}

static void test_group(const char *group, gid_t rgid)
{
	gid_t gid = -1;

	int res;

	res = lookup_group(group, &gid);
	assert(!res);
	assert(gid == rgid);

	printf("test lookup group '%s': %lu\n", group, gid);
}

int main()
{
	test_group_null(NULL);
	test_group_null("gr1");

	test_group_fail(NULL);
	test_group_fail("gr2");

	test_group("root", 0);
	test_group("bin", 1);
	test_group("7", 7);
	test_group_fail("7b");
	test_group_fail("127");

	return EXIT_SUCCESS;
}

#endif // TESTS