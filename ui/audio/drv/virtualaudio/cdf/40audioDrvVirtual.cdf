/* 40audioDrvVirtual.cdf - Virtual Audio Driver */

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
12mar14,y_f  written
*/

Parameter DRV_AUDIO_VIRTUAL_OBSERVE
    {
    NAME            Enable Output Log
    TYPE            BOOL
    DEFAULT         TRUE
    }

Parameter DRV_AUDIO_VIRTUAL_MAX_CHANNELS
    {
    NAME            Maximum Channels
    TYPE            uint
    DEFAULT         2
    }

Component   DRV_AUDIO_VIRTUAL
    {
    NAME        Virtual Audio Driver
    SYNOPSIS    Virtual audio driver
    MODULES     audioDrvVirtual.o
    _CHILDREN   FOLDER_AUDIO_DRV
    PROTOTYPE   void audVirInit (BOOL observeEn, UINT8 maxChannels);
    _INIT_ORDER usrRoot
    INIT_RTN    audVirInit (DRV_AUDIO_VIRTUAL_OBSERVE, DRV_AUDIO_VIRTUAL_MAX_CHANNELS);
    INIT_AFTER  usrIosExtraInit
    INIT_BEFORE INCLUDE_USER_APPL
    REQUIRES    INCLUDE_AUDIO_LIB_CORE
    CFG_PARAMS  DRV_AUDIO_VIRTUAL_OBSERVE   \
                DRV_AUDIO_VIRTUAL_MAX_CHANNELS
    }