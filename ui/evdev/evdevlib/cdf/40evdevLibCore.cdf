/* 40evdevLibCore.cdf - Core Library Component Bundles */

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
26jun13,y_f  written
*/

Parameter EVDEV_MSG_NUM
    {
    NAME            Message Number
    TYPE            uint
    DEFAULT         64
    }

Component   INCLUDE_EVDEV_LIB_CORE
    {
    NAME        Core Library
    SYNOPSIS    Event Devices Core Library
    MODULES     evdevLibCore.o
    _CHILDREN   FOLDER_EVDEV_LIB
    PROTOTYPE   STATUS evdevInit (UINT32 msgNum);
    _INIT_ORDER usrIosExtraInit
    INIT_RTN    evdevInit (EVDEV_MSG_NUM);
    CFG_PARAMS  EVDEV_MSG_NUM
    }