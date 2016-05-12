/* 40evdevLibPtr.cdf - Pointer Library Component Bundles */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
04jul13,y_f  create
*/

Component   INCLUDE_EVDEV_LIB_PTR
    {
    NAME        Pointer Library
    SYNOPSIS    Event Devices Pointer Library
    MODULES     evdevLibPtr.o
    _CHILDREN   FOLDER_EVDEV_LIB
    REQUIRES    INCLUDE_EVDEV_LIB_CORE
    }