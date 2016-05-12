/* 30gfxJpeg.cdf - libjpeg CDF file */

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

Component INCLUDE_JPEG {
    NAME            JPEG library
    SYNOPSIS        JPEG image processing library
    _CHILDREN       FOLDER_IMAGE
    LINK_SYMS       jpeg_CreateDecompress
}
