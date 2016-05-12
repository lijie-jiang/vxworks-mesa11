/* gfxVgConfig.h - Wind River VG Platform Header */

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
22dec14,yat  Enable GFX_THREAD_VARIABLE for thread variable (US50977)
22dec14,yat  Fix build warning for LP64 (US50456)
25jun13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
*/

#ifndef __gfxVgConfig_h
#define __gfxVgConfig_h

/* includes */

#include <sysLib.h>
#include <taskLib.h>
#include <tickLib.h>
#include <vxWorks.h>

/*
 * Platform based definitions
 */

/*
 * Enable graphics context as thread variable instead of static variable
 */
#ifdef GFX_THREAD_VARIABLE
#define GC_DECL             __thread
#else
#define GC_DECL             LOCAL
#endif

#define GET_GC()            gc_t* pGc = initGc()
#define DELETE_GC()         deleteGc()

#define CURRENT_THREAD_ID   taskIdSelf()
#define INVALID_THREAD_ID   0

/*
 * Configuration based definitions
 */

#define SUPPORT_OPENVG

#ifdef  SUPPORT_OPENVG
/*
 * number of vertices per vertex bucket
 */
#define BUCKET_SIZE                 100

/*
 * size (base 2) of a gradient's color ramp
 */
#define COLOR_RAMP_SIZE2            8

/*
 * normalized size (base 2) of a paint pattern
 */
#define PATTERN_NORM_SIZE2          9

/*
 * define/undefine to have the denormalization look-up table
 * dynamically/statically allocated
 */
#undef  PATTERN_DENORM_LUT_IS_DYNAMIC

#define FLATNESS                    0.5

/*
 * ratio between the number of vertices and triangles in a path
 */
#define VERTEX_TO_TRIANGLE_RATIO    2.2f

#endif  /* SUPPORT_OPENVG */

#endif /* #ifndef __gfxVgConfig_h */
