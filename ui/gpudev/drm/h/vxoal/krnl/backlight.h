/* backlight.h - backlight device functionality header file*/

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
30jun15,rpc  Written
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux backlight operations.

NOMANUAL
*/

#ifndef _VXOAL_BACKLIGHT_H_
#define _VXOAL_BACKLIGHT_H_

#include <vxoal/krnl/device.h>

enum backlight_type {
    BACKLIGHT_NONE,
    BACKLIGHT_RAW,
    BACKLIGHT_PLATFORM,
    BACKLIGHT_FIRMWARE,
    BACKLIGHT_TYPE_MAX
};

struct backlight_properties {
    int brightness;
    int max_brightness;
    int power;
    enum backlight_type type;
};

struct backlight_device {
    struct device dev;
    struct backlight_properties props;
};

struct backlight_ops {
    int (*update_status)(struct backlight_device *);
    int (*get_brightness)(struct backlight_device *);
};

#define bl_get_data(bld) ((bld)->dev.driver_data)

extern struct backlight_device *backlight_device_register(
                                     const char *name,
                                     struct device *dev,
                                     void *devdata,
                                     const struct backlight_ops *ops,
                                     const struct backlight_properties *props);
extern void backlight_device_unregister(struct backlight_device *bld);

#endif /* _VXOAL_BACKLIGHT_H_ */
