/* 40audioDrvWm8962.cdf - Wolfson Microelectronics 8962 Audio Codec */

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
02sep14,y_f  removed INIT_RTN (V7GFX-208)
17feb14,y_f  written
*/

Component   DRV_AUDIO_WM8962
    {
    NAME        Wolfson Microelectronics 8962 Audio Codec Driver
    SYNOPSIS    Wolfson Microelectronics 8962 audio codec driver
    MODULES     audioDrvWm8962.o
    _CHILDREN   FOLDER_AUDIO_DRV
    LINK_SYMS   vxbFdtWm8962AudDrv
    REQUIRES    INCLUDE_VXBUS               \
                DRV_BUS_FDT_ROOT            \
                INCLUDE_AUDIO_LIB_CORE      \
                INCLUDE_GPIO_SYS
    }