/* 40audioDrvTiAic3106.cdf - TI TLV320AIC3106 Audio Codec Driver */

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
21mar14,y_f  written
*/

Component   DRV_AUDIO_TI_AIC3106
    {
    NAME        TI TLV320AIC3106 Audio Codec Driver
    SYNOPSIS    TI TLV320AIC3106 audio codec driver
    MODULES     audioDrvTiAic3106.o
    _CHILDREN   FOLDER_AUDIO_DRV
    LINK_SYMS   vxbFdtTiAic3106AudDrv
    REQUIRES    INCLUDE_VXBUS               \
                DRV_BUS_FDT_ROOT            \
                INCLUDE_AUDIO_LIB_CORE
    }