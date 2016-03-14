/* 40audioDemoRecord.cdf - Audio Driver Framework Record Demo */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
14feb14,y_f  create
*/

Component INCLUDE_AUDIO_DEMO_RECORD
    {
    NAME        Audio Driver Framework Record Demo
    SYNOPSIS    Audio Driver Framework Record demo
    _CHILDREN   FOLDER_AUDIO_DEMO
    REQUIRES    INCLUDE_AUDIO_LIB_CORE  \
                INCLUDE_AUDIO_LIB_WAV
    LINK_SYMS   audRec
    }