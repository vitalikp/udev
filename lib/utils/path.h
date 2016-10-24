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


typedef int (*mkdir_func)(const char *path, mode_t mode);


int path_mkdir(const char *path, mode_t mode, mkdir_func pmkdir);


#endif	/* _UDEV_UTILS_PATH_H_ */