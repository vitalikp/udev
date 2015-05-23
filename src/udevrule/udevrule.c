/*
 * Copyright © 2015 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "udevrule.h"
#include "exit-status.h"
#include "libudev-private.h"


const char* msgs[] =
{
	"unknown key",
	"operation symbol expected",
	"invalid operation",
	"double quote expected",
	"illegal operation",
	"brace expected",
	"no attribute found"
};


static int check(char *clause)
{
	char key[UTIL_LINE_SIZE];
	int op;
	char value[UTIL_LINE_SIZE];

	int res = rule_getkey(clause, key, &op, value);
	if (res < 0)
		return res;

	/* no args tests */
	res = check_no_args(key);

	/* no args match tests */
	if (!res)
		res = check_no_args_match(key, op, value);

	/* args tests */
	if (!res)
		res = check_args(key, op, value);

	/* args match tests */
	if (!res)
		res = check_args_match(key, op, value);

	return res;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <rules file>\n", argv[0]);
		return EXIT_INVALIDARGUMENT;
	}

	const char *filename = argv[1];
	FILE *f = fopen(filename, "rb");
	if (f == NULL)
	{
		fprintf(stderr, "%s: %m\n", filename);
		return EXIT_FAILURE;
	}

	int ret = EXIT_SUCCESS;

	ssize_t sz;
	size_t len = UTIL_LINE_SIZE;
	char line[len];
	char *p = line;
	int lineno = 0;
	char ch;
	char clause[len];
	int res;
	while ((sz = getdelim(&p, &len, '\n', f)) != -1)
	{
		lineno++;

		// handle line continuation
		if (p[sz-2] == '\\')
		{
			p += sz - 2;
			continue;
		}

		if (sz < 3)
			continue;

		// filter out comments and empty lines
		p = strip(line);
		if (*p == '#')
			continue;

//			printf("%i: %s", lineno, line);

		ch = *p;
		while (ch != '\n')
		{
			p = rule_getclause(strip(p), clause, &ch);
			if (clause[0] == '\0')
				printf("empty clause %s:%i: %s", filename, lineno, line);
			else
			{
				res = check(clause);
				if (res <= 0)
				{
					printf("Invalid line %s:%i:\n %s", filename, lineno, line);
					printf("  clause: %s\n", clause);
					printf("   error: %s\n", msgs[-res]);
					ret = EXIT_FAILURE;
				}
			}

			p++;
		}
//			printf("\n");

		p = line;
	}

	fclose(f);

	return ret;
}
