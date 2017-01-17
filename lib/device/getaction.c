/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#include "device.h"


int device_get_action(device_t *dev)
{
	if (!dev)
		return -1;

	return dev->action;
}