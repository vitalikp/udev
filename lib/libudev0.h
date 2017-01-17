/*
 * Copyright © 2017 - Vitaliy Perevertun
 *
 * This file is part of udev.
 *
 * This file is licensed under the MIT license.
 * See the file LICENSE.
 */

#ifndef _UDEV_H_
#define _UDEV_H_


enum Action
{
	ACTION_ADD,
	ACTION_REMOVE,
	ACTION_CHANGE,
	ACTION_MOVE,
	ACTION_ONLINE,
	ACTION_OFFLINE,
	ACTION_COUNT
};


typedef struct device device_t;


void device_init(device_t *dev);
int device_get_action(device_t *dev);
int device_set_action(device_t *dev, const char *action);

#endif	/* _UDEV_H_ */