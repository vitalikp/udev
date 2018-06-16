/*
 * Copyright © 2018 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <errno.h>

#include "utils.h"


ssize_t parse_value(const char *str, char *val)
{
	size_t len;

	if (str[0] != '\"' && str[0] != '\'')
	{
		errno = EINVAL;
		return -1;
	}

	len = 1;
	while (str[len] && str[len] != str[0])
	{
		if (str[len] == '\\' && str[len+1] == str[0])
			len++;

		*val++ = str[len++];
	}

	if (!str[len])
	{
		errno = EINVAL;
		return -1;
	}

	*val = '\0';

	return len + 1;
}


#ifdef TESTS
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>


static void test_empty(const char *str)
{
	char val[256] = {};
	ssize_t res;

	res = parse_value(str, val);
	assert(res == 2);
	assert(!val[0]);

	printf("test empty parse value '%s'\n", str);
}

static void test_fail(const char *str)
{
	char val[256] = {};
	ssize_t res;

	res = parse_value(str, val);
	assert(res < 0);
	assert(!val[0]);

	printf("test fail parse value '%s': %m\n", str);
}

static void test_parse(const char *str, const char *test)
{
	char val[256] = {};
	ssize_t res;

	res = parse_value(str, val);
	assert(res == strlen(test) + 2);
	assert(str_eq(test, val));

	printf("test parse value '%s': '%s'\n", str, val);
}

static void test_escape(const char *str, const char *test)
{
	char val[256] = {};
	ssize_t res;

	res = parse_value(str, val);
	assert(res == strlen(test) + 4);
	assert(str_eq(test, val));

	printf("test parse value '%s' with escape quotes: '%s'\n", str, val);
}

int main()
{
	test_empty("\"\"");
	test_empty("''");

	test_fail("");
	test_fail("\"");
	test_fail("\'");
	test_fail("123");
	test_fail("a\"");
	test_fail("b\'");

	test_parse("\"text\"", "text");
	test_parse("\"text\"text", "text");
	test_parse("'text'text", "text");
	test_parse("\"text'quoted'rule\"", "text'quoted'rule");
	test_parse("\"text'quoted'rule\"text rule", "text'quoted'rule");

	test_escape("\"text\\\"double quoted\\\"rule\"", "text\"double quoted\"rule");
	test_escape("\"text\\\"double quoted\\\"rule\"text rule", "text\"double quoted\"rule");
	test_escape("'text\\'quoted\\'rule'", "text'quoted'rule");
	test_escape("'text\\'quoted\\'rule'text rule", "text'quoted'rule");
	test_escape("\"text 'quoted \\\"double quoted\\\" quoted' rule\"", "text 'quoted \"double quoted\" quoted' rule");
	test_escape("\"text 'quoted \\\"double quoted\\\" quoted' rule\"text rule", "text 'quoted \"double quoted\" quoted' rule");

	return EXIT_SUCCESS;
}

#endif // TESTS