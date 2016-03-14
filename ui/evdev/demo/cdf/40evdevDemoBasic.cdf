/* 40evdevDemoBasic.cdf - Event Devices Framework Basic Demo */

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
03jul13,y_f  create
*/

Component INCLUDE_EVDEV_DEMO_BASIC
    {
    NAME        Basic Demo
    SYNOPSIS    Event Devices Framework Basic Demo
    _CHILDREN   FOLDER_EVDEV_DEMO
#if (defined(_WRS_CONFIG_CPU_SIMLINUX) || defined(_WRS_CONFIG_CPU_SIMNT))
    REQUIRES    INCLUDE_FBDEV           \
                INCLUDE_EVDEV_LIB_PTR   \
                INCLUDE_EVDEV_LIB_TS    \
                INCLUDE_EVDEV_LIB_KBD
#else  
    REQUIRES    INCLUDE_EVDEV_LIB_PTR   \
                INCLUDE_EVDEV_LIB_TS    \
                INCLUDE_EVDEV_LIB_KBD
#endif
    LINK_SYMS   evdevDemoBasic
    }
