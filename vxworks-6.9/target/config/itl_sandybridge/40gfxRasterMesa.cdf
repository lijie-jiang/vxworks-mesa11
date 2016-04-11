/* 40gfxRasterMesa.cdf - Mesa CDF file */

/*
 * Copyright (c) 2013-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
22jan16,yat  Add requires for Posix threads (US73564)
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
15sep15,yat  Add Mesa DRI i965 (US24710)
22dec14,yat  Port Mesa to VxWorks 7 (US24705)
*/

Folder FOLDER_RASTER_MESA {
    NAME            Mesa raster components
    SYNOPSIS        Mesa raster
    DEFAULTS        INCLUDE_MESA
    _CHILDREN       FOLDER_RASTER
}

Selection INCLUDE_MESA {
    NAME            Mesa raster library
    SYNOPSIS        Mesa raster library
    COUNT           1-
    DEFAULTS        INCLUDE_MESA_GPUDEV_DRI
    _CHILDREN       FOLDER_RASTER_MESA
}

Component INCLUDE_MESA_FBDEV {
    NAME            Mesa EGL frame buffer driver
    SYNOPSIS        Mesa EGL frame buffer driver
    _CHILDREN       INCLUDE_MESA
    ARCHIVE         libgfxMesaEGL.a libgfxMesaSw.a
    REQUIRES        INCLUDE_FBDEV_INIT \
                    INCLUDE_POSIX_PTHREADS

}

Selection INCLUDE_MESA_GPUDEV_DRI {
    NAME            Mesa EGL GPU DRI raster library
    SYNOPSIS        Mesa EGL GPU DRI raster library
    COUNT           1-1
    _CHILDREN       INCLUDE_MESA
}



Component INCLUDE_MESA_GPUDEV_DRI_I965 {
    NAME            Mesa EGL GPU DRI i965 driver
    SYNOPSIS        Mesa EGL GPU DRI i965 driver
    _CHILDREN       INCLUDE_MESA_GPUDEV_DRI
    _DEFAULTS       INCLUDE_MESA_GPUDEV_DRI
    ARCHIVE         libgfxMesaEGL.a libgfxMesaSw.a libgfxMesaDriI965.a
    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_POSIX_PTHREADS
}

