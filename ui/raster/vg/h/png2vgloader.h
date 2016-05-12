/* png2vgloader.h - PNG library interface for Wind River VG */

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
25jun13,mgc  Modified for VxWorks 7 release
15feb12,m_c  Written
*/

/*
DESCRIPTION
This routine assists in interfacing with libpng to load and convert images into
a VG format.
*/

#ifndef __INC_png2vgloader_h
#define __INC_png2vgloader_h

#ifdef  __cplusplus
extern "C" {
#endif

/* includes */

#include <assert.h>
#include <stdlib.h>
#include <VG/openvg.h>
#include <vxWorks.h>
#include <errno.h>
#include <png.h>
#include <pngconf.h>

/* PNG loader routine */
VGImage png_load_vg (const char* filename, VGImageFormat format);

#ifdef  __cplusplus
}
#endif

#endif  /* __INC_png2vgloader_h */
