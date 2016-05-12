/* gfxVgUtils.h - Wind River VG Utility Header */

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
22dec14,yat  Fix build warning for LP64 (US50456)
25jun13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
*/

#ifndef __gfxVgUtils_h
#define __gfxVgUtils_h

/* includes */

#include <math.h>
#include <gfxVgRaster.h>

/* defines */

#define EXCHANGE(_i, _j)      do                              \
                                  {                           \
                                  __typeof__(_i) tmp = _i;    \
                                  _i = _j;                    \
                                  _j = tmp;                   \
                                  }                           \
                              while (0)

#define EXCHANGE3(_i, _j, _k) do                              \
                                  {                           \
                                  __typeof__(_i) tmp = _i;    \
                                  _i = _k;                    \
                                  _k = _j;                    \
                                  _j = tmp;                   \
                                  }                           \
                              while (0)

#define IS_NOT_ALIGNED(_ptr, _size)  (((uintptr_t)(_ptr)) % (_size) != 0)

#define DEG_TO_RAD(_angle)    ((_angle) * 0.017453292519943295769236907684886f)
#define RAD_TO_DEG(_angle)    ((_angle) * 57.295779513082320876798154814105f)

#define PI            3.1415926535897932384626433832795f
#define TWO_PI        6.283185307179586476925286766559f

#define   SQUARED(_x)   ((_x) * (_x))

/* forward declarations */

float clamp (const float, float, float);
float clamp1 (float);
float gamma (float);
int   invert3x3 (const float*, float*);
float invgamma (float);

/* inlines */

/*******************************************************************************
 *
 * blend32 - blend two sARGB_8888_PRE colors
 *
 * RETURNS: A 32-bit ARGB blended color
 *
 */
static __inline__ uint blend32
    (
    const uint foreground,
    const uint background
    )
    {
    const uchar*                    p = (uchar*)&foreground;
    uint                            a, ag, rb;

    #if        (_BYTE_ORDER == _BIG_ENDIAN)
    a = 256 - p[0];
    #else
    a = 256 - p[3];
    #endif
    ag = (background >> 8) & 0x00ff00ff;
    rb = background & 0x00ff00ff;
    return (foreground + ((ag * a) & 0xff00ff00) + (((rb * a) >> 8) & 0x00ff00ff));
    }

/*******************************************************************************
 *
 * fast_sqrt - return the square root of the specified value
 *
 */
static __inline__ float fast_sqrt
    (
    const float x
    )
    {
    union { float f; uint u; } y = {0};

    y.f = x;
    y.u = 0x5f1ffff9 - (y.u >> 1);
    return (x * 0.703952253f * y.f * (2.38924456f - (x * y.f * y.f)));
    }

/*******************************************************************************
 *
 * iceil - return the smallest integer that is no less than the specified value
 *
 */
static __inline__ int iceil
    (
    const float x
    )
    {
    int                       i = 0;
    union { int i; float f; } y = {0};

    i = (int)x;
    y.f = x - (float)i;
    return (i + (y.i >> 29));
    }

/* macros */

/* Return the largest integer less than or equal to a specified value */
#define ifloor(_x) ((int)(_x))

/* Return the base 2 logarithm of the specified value */
#ifndef log2
#define log2(_x) (log(_x) * 1.4426950408889634094121704103378)
#endif

/* Return the integer nearest to the specified value */
#define iround(_x) ((int)((_x) + 0.5))

#endif    /* #ifndef __gfxVgUtils_h */
