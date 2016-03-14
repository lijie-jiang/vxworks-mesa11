/* 40audioLibCore.cdf - Core Library Component Bundles */

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
30oct13,y_f  written
*/

Parameter AUDIO_LIB_CORE_BUFFER_NUM
    {
    NAME        Buffer Number
    TYPE        uint
    DEFAULT     2
    }

Parameter AUDIO_LIB_CORE_BUFFER_TIME
    {
    NAME        Buffer Time (millisecond)
    TYPE        uint
    DEFAULT     2000
    }

Component   INCLUDE_AUDIO_LIB_CORE
    {
    NAME        Audio Driver Framework Core Library
    SYNOPSIS    Audio driver framework core library
    MODULES     audioLibCore.o
    _CHILDREN   FOLDER_AUDIO_LIB
    PROTOTYPE   void audioCoreInit (UINT32 bufNum, UINT32 bufTime);
    _INIT_ORDER usrIosExtraInit
    INIT_RTN    audioCoreInit (AUDIO_LIB_CORE_BUFFER_NUM, AUDIO_LIB_CORE_BUFFER_TIME);
    CFG_PARAMS  AUDIO_LIB_CORE_BUFFER_NUM   \
                AUDIO_LIB_CORE_BUFFER_TIME
    }