/*
 * Copyright © 2015 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef __UDEVRULE_H
#define __UDEVRULE_H

#include <ctype.h>

#define EUNKEY			0		/* Unknown key */
#define EOPEREXP		1		/* Operation symbol expected */
#define EOPEREXP		1		/* Operation symbol expected */
#define EOPER			2		/* Invalid operation */
#define EQUOTEXP		3		/* Double quote expected */
#define EILOPER			4		/* Illegal operation */
#define EBRACEXP		5		/* Brace expected */
#define ENOATTR			6		/* No attribute found */
#define EVALUE			7		/* Invalid value */

#define OP_UNSET			0	/* no operator set */
#define OP_MATCH			1	/* == */
#define OP_NOMATCH			2	/* != */
#define OP_ADD				3	/* += */
#define OP_REMOVE			4	/* -= */
#define OP_ASSIGN			5	/*  = */
#define OP_ASSIGN_FINAL		6	/* := */


static inline char* strip(char *str)
{
	while (isspace(*str))
	    ++str;

	return str;
}


int rule_getkey(char *clause, char *key, int *op, char *value);
char* rule_getclause(char *line, char *clause, char *ch);
int rule_getoper(char *clause, int *op);

int check_no_args(const char *key);
int check_no_args_match(const char *key, int op, char *value);
int check_args(const char *key, int op, char *value);
int check_args_match(const char *key, int op, char *value);

#endif
