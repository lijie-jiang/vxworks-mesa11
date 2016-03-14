/* 40audioLibWav.cdf - Wav Library Component Bundles */

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
15apr14,y_f  written
*/

Component   INCLUDE_AUDIO_LIB_WAV
    {
    NAME        Audio Driver Framework Wav Library
    SYNOPSIS    Audio driver framework Wav library
    MODULES     audioLibWav.o
    _CHILDREN   FOLDER_AUDIO_LIB
    LINK_SYMS   audioWavHeaderRead
    }