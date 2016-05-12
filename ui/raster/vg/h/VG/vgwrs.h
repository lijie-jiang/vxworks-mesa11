/* vgwrs.h - Wind River VG extensions */

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
13feb12,m_c  Released to Wind River engineering
*/

#ifndef __vgwrs_h
#define __vgwrs_h

/* includes */

#include <VG/openvg.h>

/* VG Extensions */
VG_API_CALL VGuint VG_API_ENTRY vgConvertColorWRS (const VGfloat * sRGBA,
                                                   VGImageFormat format) VG_API_EXIT;
VG_API_CALL VGint VG_API_ENTRY vgGetBitDepthWRS (VGImageFormat format) VG_API_EXIT;
VG_API_CALL VGint VG_API_ENTRY vgGetSurfaceFormatWRS (EGLSurface surface) VG_API_EXIT;
VG_API_CALL void VG_API_ENTRY vgTransformWRS (const VGfloat * src,
                                              const VGfloat * matrix,
                                              VGfloat * dst) VG_API_EXIT;

#endif    /* __vgwrs_h */
