/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef _UDEV_DEVICE_H_
#define _UDEV_DEVICE_H_

#include "libudev0.h"


struct device
{
	int action;
};


int device_action(const char *action);

#endif	/* _UDEV_DEVICE_H_ */