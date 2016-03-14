/* backlight.c - backlight device management functions */

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

This file provides compatibility functions for the Linux system operations.

NOMANUAL

*/

#include <vxoal/krnl/backlight.h>

/*******************************************************************************
*
* backlight_device_register - Register backlight device with ?
*
* RETURNS: Pointer to backlight_device structure.
*
* SEE ALSO: 
*/
struct backlight_device *backlight_device_register
    (
    const char *name,
    struct device *dev,
    void *devdata,
    const struct backlight_ops *ops,
    const struct backlight_properties *props
    )
    {
    return NULL;
    }

/*******************************************************************************
*
* backlight_device_unregister - Remove backlight device from ?
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void backlight_device_unregister
    (
    struct backlight_device *bld
    )
    {
    }
