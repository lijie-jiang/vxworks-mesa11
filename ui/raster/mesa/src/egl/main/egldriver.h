/**************************************************************************
 *
 * Copyright 2008 VMware, Inc.
 * Copyright 2009-2010 Chia-I Wu <olvaffe@gmail.com>
 * Copyright 2010-2011 LunarG, Inc.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
23nov15,rpc  Port Mesa 11.0 to VxWorks 7 (US70717)
22dec14,yat  Port Mesa to VxWorks 7 (US24705)
*/

#ifndef EGLDRIVER_INCLUDED
#define EGLDRIVER_INCLUDED


#include "c99_compat.h"

#include "egltypedefs.h"
#include "eglapi.h"
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define an inline driver typecast function.
 *
 * Note that this macro defines a function and should not be ended with a
 * semicolon when used.
 */
#define _EGL_DRIVER_TYPECAST(drvtype, egltype, code)           \
   static inline struct drvtype *drvtype(const egltype *obj)   \
   { return (struct drvtype *) code; }


/**
 * Define the driver typecast functions for _EGLDriver, _EGLDisplay,
 * _EGLContext, _EGLSurface, and _EGLConfig.
 *
 * Note that this macro defines several functions and should not be ended with
 * a semicolon when used.
 */
#define _EGL_DRIVER_STANDARD_TYPECASTS(drvname)                            \
   _EGL_DRIVER_TYPECAST(drvname ## _driver, _EGLDriver, obj)               \
   /* note that this is not a direct cast */                               \
   _EGL_DRIVER_TYPECAST(drvname ## _display, _EGLDisplay, obj->DriverData) \
   _EGL_DRIVER_TYPECAST(drvname ## _context, _EGLContext, obj)             \
   _EGL_DRIVER_TYPECAST(drvname ## _surface, _EGLSurface, obj)             \
   _EGL_DRIVER_TYPECAST(drvname ## _config, _EGLConfig, obj)


typedef _EGLDriver *(*_EGLMain_t)(const char *args);


/**
 * Base class for device drivers.
 */
struct _egl_driver
{
   const char *Name;  /**< name of this driver */

   /**
    * Release the driver resource.
    *
    * It is called before dlclose().
    */
   void (*Unload)(_EGLDriver *drv);

   _EGLAPI API;  /**< EGL API dispatch table */
};


#if defined(__vxworks)
extern _EGLDriver *
_eglBuiltInDriverFBDEV(const char *args);
#endif /* __vxworks */

extern _EGLDriver *
_eglBuiltInDriverDRI2(const char *args);


extern _EGLDriver*
_eglBuiltInDriverHaiku(const char* args);


extern _EGLDriver *
_eglMatchDriver(_EGLDisplay *dpy, EGLBoolean test_only);


extern __eglMustCastToProperFunctionPointerType
_eglGetDriverProc(const char *procname);


extern void
_eglUnloadDrivers(void);


/* defined in eglfallbacks.c */
extern void
_eglInitDriverFallbacks(_EGLDriver *drv);


extern void
_eglSearchPathForEach(EGLBoolean (*callback)(const char *, size_t, void *),
                      void *callback_data);


#ifdef __cplusplus
}
#endif


#endif /* EGLDRIVER_INCLUDED */
