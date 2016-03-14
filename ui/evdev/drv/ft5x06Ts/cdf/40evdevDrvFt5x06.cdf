/* 40evdevDrvFt5x06.cdf - FocalTech Systems FT5X06 Series Multi-touch Controller Driver */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
25jul14,y_f  created (US41404)
*/

Component   DRV_TOUCH_SCREEN_FT_5X06
    {
    NAME        FocalTech FT5X06 Multi-touch Controller Driver
    SYNOPSIS    FocalTech Systems FT5X06 Series Multi-touch Controller Driver
    MODULES     evdevDrvFt5x06.o
    _CHILDREN   FOLDER_EVDEV_DRV
    LINK_SYMS   vxbFdtFt5x06TsDrv
    REQUIRES    INCLUDE_VXBUS           \
                INCLUDE_EVDEV_LIB_TS    \
                INCLUDE_GPIO_SYS
    }