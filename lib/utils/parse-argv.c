/*
 * Copyright © 2018 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdlib.h>
#include <errno.h>

#include "utils.h"


size_t parse_argv(const char *cmd, char *argv[])
{
	char *p;
	size_t i = 0;
	ssize_t res;

	p = (char*)cmd;

	while (p[0])
	{
		/* do not separate quotes or double quotes */
		if (p[0] == '\'' || p[0] == '"')
		{
			res = parse_value(p, p + 1);
			if (res < 0)
				break;
			argv[i++] = p + 1;
			p += res;
			continue;
		}

		if (p[0] == ' ')
		{
			if (argv[i])
			{
				*p = '\0';
				i++;
			}

			p++;
			continue;
		}

		if (!argv[i])
			argv[i] = p;
		p++;
	}

	if (argv[i])
		i++;

	return i;
}


#ifdef TESTS
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>


static void test_empty(const char *cmd)
{
	int argc = 0;
	char *argv[128] = {};
	ssize_t res;

	res = parse_argv(cmd, argv);
	assert(!res);
	assert(!argv[0]);

	printf("test empty parse argv '%s'\n", cmd);
}

static void test_parse(const char *cmd, size_t argc, const char *args[])
{
	char val[100];
	char *argv[128] = {};
	size_t res;

	str_copy(val, cmd, sizeof(val));

	res = parse_argv(val, argv);
	assert(res == argc);
	while (argc-- > 0)
		assert(str_eq(argv[argc], args[argc]));

	printf("test parse argv '%s': %u “%s”\n", cmd, res, argv[0]);
}

int main()
{
	test_empty("");

	test_parse("/bin/sh", 1, (const char*[]){ "/bin/sh" });
	test_parse("/bin/sh -a", 2, (const char*[]){ "/bin/sh", "-a" });
	test_parse("/bin/sh -b 'one'", 3, (const char*[]){ "/bin/sh", "-b", "one" });
	test_parse("/bin/sh -c 'one two' 'three'", 4, (const char*[]){ "/bin/sh", "-c", "one two", "three" });
	test_parse("/bin/sh  -d   'one two'    'three'", 4, (const char*[]){ "/bin/sh", "-d", "one two", "three" });

	return EXIT_SUCCESS;
}

#endif // TESTS