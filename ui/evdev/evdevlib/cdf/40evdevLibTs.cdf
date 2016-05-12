/* 40evdevLibTs.cdf - Touch Screen Library Component Bundles */

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

Component   INCLUDE_EVDEV_LIB_TS
    {
    NAME        Touch Screen Library
    SYNOPSIS    Event Devices Touch Screen Library
    MODULES     evdevLibTs.o
    _CHILDREN   FOLDER_EVDEV_LIB
    REQUIRES    INCLUDE_EVDEV_LIB_PTR
    }