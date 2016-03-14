/* 40evdevDrvTiAm335x.cdf - TI AM335X touch screen controller */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
01sep14,y_f  removed INIT_RTN and CFG_PARAMS (V7GFX-208)
15aug13,y_f  written
*/

Component   DRV_TOUCH_SCREEN_TI_AM335X
    {
    NAME        TI AM335X Touch Screen Driver
    SYNOPSIS    TI AM335X Touch Screen Driver
    MODULES     evdevDrvTiAm335x.o
    _CHILDREN   FOLDER_EVDEV_DRV
    LINK_SYMS   vxbFdtTiAm335xTsDrv
    REQUIRES    INCLUDE_VXBUS       \
                INCLUDE_EVDEV_LIB_TS
    }