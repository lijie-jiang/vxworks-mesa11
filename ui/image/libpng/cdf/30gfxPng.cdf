/* 30gfxPng.cdf - libpng CDF file */

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
24jan14,mgc  Modified for VxWorks 7 release
*/

Component INCLUDE_PNG {
    NAME            PNG library
    SYNOPSIS        PNG image processing library
    _CHILDREN       FOLDER_IMAGE
    REQUIRES        INCLUDE_ZLIB
    LINK_SYMS       png_create_read_struct
}
