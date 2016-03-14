/* 40evdevDrvEetiExc7200.cdf - EETI EXC7200 Series Multi-touch Controller Driver */

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
01dec14,y_f  created (US50218)
*/

Component   DRV_TOUCH_SCREEN_EETI_EXC7200
    {
    NAME        EETI EXC7200 Multi-touch Controller Driver
    SYNOPSIS    EETI EXC7200 Series Multi-touch Controller Driver
    MODULES     evdevDrvEetiExc7200.o
    _CHILDREN   FOLDER_EVDEV_DRV
    LINK_SYMS   vxbFdtExc7200TsDrv
    REQUIRES    INCLUDE_VXBUS           \
                INCLUDE_EVDEV_LIB_TS    \
                INCLUDE_GPIO_SYS        \
                DRV_BUS_FDT_ROOT
    }