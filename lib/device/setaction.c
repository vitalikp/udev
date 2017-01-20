/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <string.h>

#include "action.h"
#include "utils.h"


int device_set_action(device_t *dev, const char *action)
{
	int act = 0;

	while (act < ACTION_COUNT)
	{
		if (str_eq(action, actions[act]))
		{
			dev->action = act;
			return 0;
		}

		act++;
	}

	return -1;
}


#ifdef TESTS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


static void test_setaction(const char *action, int test)
{
	device_t dev;
	int res;

	res = device_set_action(&dev, action);
	assert(!res);
	assert(dev.action == test);

	printf("test action: '%s'\n", action);
}

static void test_fail(const char *action)
{
	device_t dev;
	int res;

	res = device_set_action(&dev, action);
	assert(res < 0);

	printf("test fail action: '%s'\n", action);
}

int main()
{
	test_setaction("add", ACTION_ADD);
	test_setaction("remove", ACTION_REMOVE);
	test_setaction("change", ACTION_CHANGE);
	test_setaction("move", ACTION_MOVE);
	test_setaction("online", ACTION_ONLINE);
	test_setaction("offline", ACTION_OFFLINE);

	test_fail("");
	test_fail("Add");
	test_fail("Remove");
	test_fail("Change");
	test_fail("Move");
	test_fail("Online");
	test_fail("Offline");

	return EXIT_SUCCESS;
}

#endif // TESTS