/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <string.h>
#include <pwd.h>
#include <errno.h>

#include "utils.h"


int lookup_user(const char *user, uid_t *puid)
{
	if (!user || !puid)
	{
		errno = EINVAL;
		return -1;
	}

	if (!strcmp("root", user))
	{
		*puid = 0;
		return 0;
	}

	uid_t uid;

	if (!parse_uint(user, &uid))
	{
		if (!getpwuid(uid))
			return -1;

		*puid = uid;
		return 0;
	}

	struct passwd *pw;

	errno = 0;
	pw = getpwnam(user);
	if (pw)
	{
		*puid = pw->pw_uid;
		return 0;
	}

	return -1;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


static void test_lookup_null(const char *user)
{
	int res;

	res = lookup_user(user, NULL);
	assert(res < 0);

	printf("test null lookup user '%s'\n", user);
}

static void test_lookup_fail(const char *user)
{
	uid_t uid = -1;

	int res;

	res = lookup_user(user, &uid);
	assert(res < 0);
	assert(uid == -1);

	printf("test fail lookup user '%s': %m\n", user);
}

static void test_lookup(const char *user, uid_t ruid)
{
	uid_t uid = -1;

	int res;

	res = lookup_user(user, &uid);
	assert(!res);
	assert(uid == ruid);

	printf("test lookup user '%s': %lu\n", user, uid);
}

int main()
{
	test_lookup_null(NULL);
	test_lookup_null("my1");

	test_lookup_fail(NULL);
	test_lookup_fail("my2");

	test_lookup("root", 0);
	test_lookup("bin", 1);
	test_lookup("5", 5);
	test_lookup_fail("5a");
	test_lookup_fail("127");

	return EXIT_SUCCESS;
}

#endif // TESTS