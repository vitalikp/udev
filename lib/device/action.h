/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef _UDEV_DEVICE_ACTION_H_
#define _UDEV_DEVICE_ACTION_H_

#include "device.h"


static const char* actions[ACTION_COUNT] =
{
	[ACTION_ADD] = "add",
	[ACTION_REMOVE] = "remove",
	[ACTION_CHANGE] = "change",
	[ACTION_MOVE] = "move",
	[ACTION_ONLINE] = "online",
	[ACTION_OFFLINE] = "offline"
};

#endif	/* _UDEV_DEVICE_ACTION_H_ */