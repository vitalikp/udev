/*
 * Copyright © 2015 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdbool.h>

#include "udevrule.h"


int rule_getkey(char *clause, char *key, int *op, char *value)
{
	char *p = clause;

	int sz;

	int i = 0;

	while (p[0] != '\0')
	{
		if (isspace(*p))
			break;
		if (p[0] == '=')
			break;
		if (p[0] == '+' || p[0] == '!' || p[0] == ':')
			break;

		key[i++] = p[0];
		p++;
	}

	key[i] = '\0';

	/* skip whitespace after key */
	p = strip(p);
	if (p[0] == '\0')
		return -EOPEREXP;

	/* get operation type */
	sz = rule_getoper(p, op);
	if (!sz)
		return -EOPER;
	p += sz;

	/* skip whitespace after operator */
	p = strip(p);
	if (p[0] == '\0' || p[0] != '"')
		return -EQUOTEXP;
	p++;

	i = 0;
	while (p[0] != '\0' && p[0] != '"')
	{
		value[i++] = p[0];
		p++;
	}
	if (p[0] != '"')
		return -EQUOTEXP;

	return 0;
}

char* rule_getclause(char *line, char *clause, char *ch)
{
	char *p = line;

	int i = 0;
	bool quoted = false;
	while (*p != '\n')
	{
		if (!quoted && *p == ',')
			break;
		if (*p == '"')
			quoted = !quoted;
		clause[i++] = *p;
		p++;
	}
	clause[i] = '\0';

	*ch = *p;

	return p;
}

int rule_getoper(char *clause, int *op)
{
	if (clause[1] == '=')
	{
		switch (clause[0])
		{
			case '=':
				*op = OP_MATCH;
				return 2;
			case '!':
				*op = OP_NOMATCH;
				return 2;
			case '+':
				*op = OP_ADD;
				return 2;
			case ':':
				*op = OP_ASSIGN_FINAL;
				return 2;
		}

		return 0;
	}

	if (clause[0] == '=')
	{
		*op = OP_ASSIGN;
		return 1;
	}

	return 0;
}
