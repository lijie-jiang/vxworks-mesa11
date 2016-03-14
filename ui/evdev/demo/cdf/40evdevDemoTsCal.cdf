/* 40evdevDemoTsCal.cdf - Touch Screen Calibration Demo */

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
16may14,yat  rename API_FBDEV to INCLUDE_FBDEV. (US11127)
08aug13,y_f  create
*/

Component INCLUDE_EVDEV_DEMO_TS_CAL
    {
    NAME        Touch Screen Calibration Demo
    SYNOPSIS    Touch Screen Calibration Demo
    _CHILDREN   FOLDER_EVDEV_DEMO
    REQUIRES    INCLUDE_EVDEV_LIB_TS    \
                INCLUDE_FBDEV
    LINK_SYMS   evdevDemoTsCal
    }
