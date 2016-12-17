/*
 * Copyright © 2016 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef _UDEV_UTILS_PATH_H_
#define _UDEV_UTILS_PATH_H_

#include <sys/types.h>
#include <sys/stat.h>


/* The max size of udev path */
#define PATH_SIZE		1024


typedef int (*mkdir_func)(const char *path, mode_t mode);


/**
 * Path util functions
 */

int path_mkdir(const char *path, mode_t mode, mkdir_func pmkdir);

/**
 * path_relative:
 * @dst: destination string
 * @from: source path
 * @to: destination path
 * @size: maximum size of the destination string
 *
 * Build relative path from 'to' to 'from', store string in dst.
 *
 * Returns: length of destination string, or 0 if error
 */
size_t path_relative(char *dst, const char *from, const char *to, size_t size);

/**
 * path_create:
 * @path: path string
 * @mode: path mode
 *
 * Create a directory with name 'path' and mode 'mode'.
 *
 * Returns: 0 on success, or -1 if error
 */
static inline int path_create(const char *path, mode_t mode)
{
	return path_mkdir(path, mode, mkdir);
}

int path_remove(const char *path);


#endif	/* _UDEV_UTILS_PATH_H_ */