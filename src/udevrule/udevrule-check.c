/*
 * Copyright © 2015 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <pcre.h>

#include "udevrule.h"
#include "exit-status.h"
#include "libudev-private.h"


const char *no_args_keys[] =
{
	"NAME",
	"SYMLINK",
	"TAG",
	"OWNER",
	"GROUP",
	"MODE",
	"PROGRAM",
	"RUN",
	"LABEL",
	"GOTO",
	"WAIT_FOR",
	"WAIT_FOR_SYSFS",//+
	"OPTIONS",
	NULL
};

const char *no_args_match_keys[] =
{
	"ACTION",
	"DEVPATH",
	"KERNEL",
	"KERNELS",
	"SUBSYSTEM",
	"SUBSYSTEMS",
	"DRIVER",
	"DRIVERS",
	"TAGS",
	"TEST",
	"RESULT",
	NULL
};

const char *args_keys[] =
{
	"ATTR",
	"ENV",
	"IMPORT",
	"RUN",
	"SECLABEL",
	NULL
};

const char *args_match_keys[] =
{
	"ATTRS",
	"TEST",
	NULL
};


int check_no_args(const char *key)
{
	int i = 0;

	while (no_args_keys[i])
	{
		if (!strcmp(key, no_args_keys[i]))
			return 1;

		i++;
	}

	return EUNKEY;
}

int check_no_args_match(const char *key, int op, char *value)
{
	int i = 0;

	while (no_args_match_keys[i])
	{
		if (!strcmp(key, no_args_match_keys[i]))
		{
			if (op > OP_NOMATCH)
				return -EILOPER;

			return 1;
		}

		i++;
	}

	return EUNKEY;
}

int check_args(const char *key, int op, char *value)
{
	int i = 0;

	char *pos;
	char *p;

	while (args_keys[i])
	{
		p = strchr(key, '{');
		if (!p)
			break;

		if (!strncmp(key, args_keys[i], p - key))
		{
			pos = strchr(++p, '}');
			if (!pos)
				return -EBRACEXP;

			if (pos == p)
				return -ENOATTR;

			return 1;
		}

		i++;
	}

	return EUNKEY;
}

int check_args_match(const char *key, int op, char *value)
{
	int i = 0;

	char *pos;
	char *p;

	while (args_match_keys[i])
	{
		p = strchr(key, '{');
		if (!p)
			break;

		if (!strncmp(key, args_match_keys[i], p - key))
		{
			if (op > OP_NOMATCH)
				return -EILOPER;

			pos = strchr(++p, '}');
			if (!pos)
				return -EBRACEXP;

			if (pos == p)
				return -ENOATTR;

			return 1;
		}

		i++;
	}

	return EUNKEY;
}
