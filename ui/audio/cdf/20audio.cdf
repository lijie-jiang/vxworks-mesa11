/* 20audio.cdf - Audio Component Bundles */

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

Folder FOLDER_UI
    {
    NAME        User interface components
    SYNOPSIS    User interface library components
    _CHILDREN   FOLDER_ROOT
    }

Folder FOLDER_AUDIO
    {
    NAME        Audio Components
    SYNOPSIS    Audio driver framework and drivers
    _CHILDREN   FOLDER_UI
    }