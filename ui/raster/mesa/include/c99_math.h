/**************************************************************************
 *
 * Copyright 2007-2015 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

/**
 * Wrapper for math.h which makes sure we have definitions of all the c99
 * functions.
 */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
16dec15,rpc  Implement fpclassify, fmin and fmax (US71491)
*/

#ifndef _C99_MATH_H_
#define _C99_MATH_H_

#include <math.h>
#include "c99_compat.h"


#if defined(_WRS_KERNEL)
#ifndef _DENORM
#define _DENORM         (-2)    /* C9X only */
#endif
#ifndef _FINITE
#define _FINITE         (-1)
#endif
#ifndef _INFCODE
#define _INFCODE        1
#endif
#ifndef _NANCODE
#define _NANCODE        2
#endif
#ifndef FP_NAN
#define FP_NAN          _NANCODE
#endif
#ifndef FP_INFINITE
#define FP_INFINITE     _INFCODE
#endif
#ifndef FP_ZERO
#define FP_ZERO         0
#endif
#ifndef FP_SUBNORMAL
#define FP_SUBNORMAL    _DENORM
#endif
#ifndef FP_NORMAL
#define FP_NORMAL       _FINITE
#endif
#ifndef _HUGE_ENUF
#define _HUGE_ENUF 1e+300
#endif
#ifndef INFINITY
#define INFINITY        ((float)(_HUGE_ENUF * _HUGE_ENUF))
#endif

/* No users of FP_SUBNORMAL so not implemented */
#define fpclassify(x) ( \
                       (x) != (x) ? FP_NAN : \
                       (x) == INFINITY ? FP_INFINITE : \
                       (x) == -INFINITY ? FP_INFINITE : \
                       (x) == 0.0 ? FP_ZERO : \
                       FP_NORMAL \
                      )
#ifndef isnormal
# define isnormal(x)    (fpclassify (x) == FP_NORMAL)
#endif

#ifndef M_PI
#define M_PI          3.14159265358979323846  /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2        1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_PI_4
#define M_PI_4        0.78539816339744830962  /* pi/4 */
#endif

#define acosf(f) ((float) acos(f))
#define asinf(f) ((float) asin(f))
#define atan2f(x,y) ((float) atan2(x,y))
#define atanf(f) ((float) atan(f))
#define ceilf(f) ((float) ceil(f))
#define cosf(f) ((float) cos(f))
#define floorf(f) ((float) floor(f))
#define truncf(f) ((float) trunc(f))
#define roundf(f) ((float) round(f))
#define logf(f) ((float) log(f))
#define log2f(f) (logf(f) * (float)1.4426950408889634094121704103378)
#define powf(x,y) ((float) pow(x,y))
#define sinf(f) ((float) sin(f))
#define sqrtf(f) ((float) sqrt(f))
#define tanf(f) ((float) tan(f))
#define tanhf(f) ((float) tanh(f))
#define acoshf(f) ((float) acosh(f))
#define asinhf(f) ((float) asinh(f))
#define atanhf(f) ((float) atanh(f))
#define expf(x) ((float) exp(x))
#define ldexpf(x, y) ((float) ldexp(x, y))
#define frexpf(x, y) ((float) frexp(x, y))
#define fabsf(x) ((float) fabs(x))
#define rintf(x) ((float) rint(x))
/* To test for NAN:  x is NAN if "x != x" is True
 * If x is NAN
 *     return y  (y is either valid or NAN too!)
 * else          (x is valid)
 *     if y is NAN
 *         return x
 *     else
 *         return which ever is the min or max as appropriate.
 */
#define fmin(x, y) ( \
                    (x) != (x) ? (y) : \
                    (y) != (y) ? (x) : \
                    (x) < (y)  ? (x) : (y) \
                   )
#define fmax(x, y) ( \
                    (x) != (x) ? (y) : \
                    (y) != (y) ? (x) : \
                    (x) > (y)  ? (x) : (y) \
                   )
#define fminf(x, y) ((float) fmin(x, y))
#define fmaxf(x, y) ((float) fmax(x, y))
#define copysignf(x, y) ((float) copysign(x, y))
#endif /* _WRS_KERNEL */

#if defined(_MSC_VER)

/* This is to ensure that we get M_PI, etc. definitions */
#if !defined(_USE_MATH_DEFINES)
#error _USE_MATH_DEFINES define required when building with MSVC
#endif 

#if _MSC_VER < 1800
#define isfinite(x) _finite((double)(x))
#define isnan(x) _isnan((double)(x))
#endif /* _MSC_VER < 1800 */

#if _MSC_VER < 1800
static inline double log2( double x )
{
   const double invln2 = 1.442695041;
   return log( x ) * invln2;
}

static inline double
round(double x)
{
   return x >= 0.0 ? floor(x + 0.5) : ceil(x - 0.5);
}

static inline float
roundf(float x)
{
   return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

#ifndef INFINITY
#include <float.h> // DBL_MAX
#define INFINITY (DBL_MAX + DBL_MAX)
#endif

#ifndef NAN
#define NAN (INFINITY - INFINITY)
#endif

#endif /* _MSC_VER */


#if (defined(_MSC_VER) && _MSC_VER < 1800) || \
    defined(_WRS_KERNEL) || \
    (!defined(_MSC_VER) && \
     __STDC_VERSION__ < 199901L && \
     (!defined(_XOPEN_SOURCE) || _XOPEN_SOURCE < 600) && \
     !defined(__cplusplus))

static inline long int
lrint(double d)
{
   long int rounded = (long int)(d + 0.5);

   if (d - floor(d) == 0.5) {
      if (rounded % 2 != 0)
         rounded += (d > 0) ? -1 : 1;
   }

   return rounded;
}

static inline long int
lrintf(float f)
{
   long int rounded = (long int)(f + 0.5f);

   if (f - floorf(f) == 0.5f) {
      if (rounded % 2 != 0)
         rounded += (f > 0) ? -1 : 1;
   }

   return rounded;
}

static inline long long int
llrint(double d)
{
   long long int rounded = (long long int)(d + 0.5);

   if (d - floor(d) == 0.5) {
      if (rounded % 2 != 0)
         rounded += (d > 0) ? -1 : 1;
   }

   return rounded;
}

static inline long long int
llrintf(float f)
{
   long long int rounded = (long long int)(f + 0.5f);

   if (f - floorf(f) == 0.5f) {
      if (rounded % 2 != 0)
         rounded += (f > 0) ? -1 : 1;
   }

   return rounded;
}

static inline float
exp2f(float f)
{
   return powf(2.0f, f);
}

static inline double
exp2(double d)
{
   return pow(2.0, d);
}

#endif /* C99 */


/*
 * signbit() is a macro on Linux.  Not available on Windows.
 */
#ifndef signbit
#define signbit(x) ((x) < 0.0f)
#endif


#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef M_E
#define M_E (2.7182818284590452354)
#endif

#ifndef M_LOG2E
#define M_LOG2E (1.4426950408889634074)
#endif

#ifndef FLT_MAX_EXP
#define FLT_MAX_EXP 128
#endif


#if defined(fpclassify)
/* ISO C99 says that fpclassify is a macro.  Assume that any implementation
 * of fpclassify, whether it's in a C99 compiler or not, will be a macro.
 */
#elif defined(__cplusplus)
/* For C++, fpclassify() should be defined in <cmath> */
#elif defined(_MSC_VER)
/* Not required on VS2013 and above.  Oddly, the fpclassify() function
 * doesn't exist in such a form on MSVC.  This is an implementation using
 * slightly different lower-level Windows functions.
 */
#include <float.h>

static inline enum {FP_NAN, FP_INFINITE, FP_ZERO, FP_SUBNORMAL, FP_NORMAL}
fpclassify(double x)
{
   switch(_fpclass(x)) {
   case _FPCLASS_SNAN: /* signaling NaN */
   case _FPCLASS_QNAN: /* quiet NaN */
      return FP_NAN;
   case _FPCLASS_NINF: /* negative infinity */
   case _FPCLASS_PINF: /* positive infinity */
      return FP_INFINITE;
   case _FPCLASS_NN:   /* negative normal */
   case _FPCLASS_PN:   /* positive normal */
      return FP_NORMAL;
   case _FPCLASS_ND:   /* negative denormalized */
   case _FPCLASS_PD:   /* positive denormalized */
      return FP_SUBNORMAL;
   case _FPCLASS_NZ:   /* negative zero */
   case _FPCLASS_PZ:   /* positive zero */
      return FP_ZERO;
   default:
      /* Should never get here; but if we do, this will guarantee
       * that the pattern is not treated like a number.
       */
      return FP_NAN;
   }
}
#elif defined(__vxworks)
#else
#error "Need to include or define an fpclassify function"
#endif


#endif /* #define _C99_MATH_H_ */
