/*
 * Copyright © 2011 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Kristian Høgsberg <krh@bitplanet.net>
 */

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
22dec14,yat  Port Mesa to VxWorks 7 (US24705)
*/

#ifndef EGL_FB_INCLUDED
#define EGL_FB_INCLUDED

#include <GL/gl.h>
#include <GL/internal/dri_interface.h>

#include "eglconfig.h"
#include "eglcontext.h"
#include "egldisplay.h"
#include "egldriver.h"
#include "eglcurrent.h"
#include "egllog.h"
#include "eglsurface.h"
#include "eglimage.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct fbdev_egl_driver
{
   _EGLDriver base;

   void *handle;
   _EGLProc (*get_proc_address)(const char *procname);
   void (*glFlush)(void);
};

struct fbdev_egl_display
{
   int                       fbdev_major;
   int                       fbdev_minor;
   __DRIscreen              *dri_screen;
   int                       own_dri_screen;
   const __DRIconfig       **driver_configs;
   __DRIcoreExtension       *core;
   __DRIdri2Extension       *fbdev;
   __DRIswrastExtension     *swrast;
   __DRI2flushExtension     *flush;
   __DRItexBufferExtension  *tex_buffer;
   __DRIimageExtension      *image;
   __DRIrobustnessExtension *robustness;
   __DRI2configQueryExtension *config;
   int                       fd;

   int                       swap_available;
   int                       invalidate_available;
   int                       min_swap_interval;
   int                       max_swap_interval;
   int                       default_swap_interval;

   char                     *device_name;
   char                     *driver_name;

   __DRIdri2LoaderExtension    fbdev_loader_extension;
   __DRIswrastLoaderExtension  swrast_loader_extension;
   const __DRIextension     *extensions[4];

   unsigned int        buffers;         /* number of buffers */
   void *              pFrontBuf;       /* front buffer */
   void *              pBackBuf;        /* back buffer */
   void *              pThirdBuf;       /* third buffer */
   int                 width, stride;   /* width in pixels, stride in bytes */
   int                 height;          /* height in lines */
};

struct fbdev_egl_context
{
   _EGLContext   base;
   __DRIcontext *dri_context;
};

struct fbdev_egl_surface
{
   _EGLSurface          base;
   __DRIdrawable       *dri_drawable;
   int                  swap_interval;
};

struct fbdev_egl_config
{
   _EGLConfig         base;
   const __DRIconfig *dri_single_config;
   const __DRIconfig *dri_double_config;
};

struct fbdev_egl_image
{
   _EGLImage   base;
   __DRIimage *dri_image;
};

/* standard typecasts */
_EGL_DRIVER_STANDARD_TYPECASTS(fbdev_egl)
_EGL_DRIVER_TYPECAST(fbdev_egl_image, _EGLImage, obj)

/* Helper for platforms not using fbdev_create_screen */
void
fbdev_setup_screen(_EGLDisplay *disp);

EGLBoolean
fbdev_load_driver_swrast(_EGLDisplay *disp);

EGLBoolean
fbdev_create_screen(_EGLDisplay *disp);

__DRIimage *
fbdev_lookup_egl_image(__DRIscreen *screen, void *image, void *data);

struct fbdev_egl_config *
fbdev_add_config(_EGLDisplay *disp, const __DRIconfig *dri_config, int id,
		int depth, EGLint surface_type, const EGLint *attr_list,
		const unsigned int *rgba_masks);

_EGLImage *
fbdev_create_image_khr(_EGLDriver *drv, _EGLDisplay *disp,
		      _EGLContext *ctx, EGLenum target,
		      EGLClientBuffer buffer, const EGLint *attr_list);

EGLBoolean
fbdev_initialize_drm(_EGLDriver *drv, _EGLDisplay *disp);

#endif /* EGL_FB_INCLUDED */
