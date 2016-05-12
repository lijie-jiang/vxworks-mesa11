/* 40gfxRasterVG.cdf - Vector Graphics raster library CDF file */

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
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
24jan14,mgc  Modified for VxWorks 7 release
17may12,rfm  Written
*/

Folder FOLDER_RASTER_VG {
    NAME            Frame buffer software Vector Graphics components
    SYNOPSIS        Frame buffer software Vector Graphics
    DEFAULTS        INCLUDE_VG
    _CHILDREN       FOLDER_RASTER
}

Component INCLUDE_VG {
    NAME            Frame buffer software Vector Graphics raster library
    SYNOPSIS        Frame buffer software Vector Graphics raster library
    _CHILDREN       FOLDER_RASTER_VG
    ARCHIVE         libgfxVg.a
#if defined(_WRS_CONFIG_FBDEV_INIT)
    REQUIRES        INCLUDE_FBDEV_INIT \
                    INCLUDE_TLS \
                    INCLUDE_POSIX_PTHREADS
#else
    REQUIRES        INCLUDE_FBDEV \
                    INCLUDE_TLS \
                    INCLUDE_POSIX_PTHREADS
#endif
}

Component INCLUDE_VG_PNG_LOADER {
    NAME            Frame buffer software Vector Graphics PNG loader
    SYNOPSIS        Frame buffer software Vector Graphics PNG loader
    _CHILDREN       FOLDER_RASTER_VG
    REQUIRES        INCLUDE_VG \
                    INCLUDE_PNG
    LINK_SYMS       png_load_vg
}

Component INCLUDE_VG_JPEG_LOADER {
    NAME            Frame buffer software Vector Graphics JPEG loader
    SYNOPSIS        Frame buffer software Vector Graphics JPEG loader
    _CHILDREN       FOLDER_RASTER_VG
    REQUIRES        INCLUDE_VG \
                    INCLUDE_JPEG
    LINK_SYMS       jpeg_load_vg
}
