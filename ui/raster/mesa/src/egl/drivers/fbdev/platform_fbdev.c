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
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
23nov15,rpc  Port Mesa 11.0 to VxWorks 7 (US70717)
08oct15,yat  Add GFX_FIXED_FBMODE_OFFSET (US67554)
14sep15,yat  Fix build warnings (US24710)
12jun15,yat  Add offset for xlnxlcvc (US58560)
22dec14,yat  Port Mesa to VxWorks 7 (US24705)
*/

#include <vxWorks.h>
#include <ioLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "egl_fbdev.h"
#include <fbdev.h>

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

static void
swrastGetFbInfo(__DRIdrawable *drawable,
                int *x, int *y, int *width, int *height,
                void *loaderPrivate)
{
   struct fbdev_egl_surface *fbdev_surf = loaderPrivate;
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(fbdev_surf->base.Resource.Display);

   *x = 0;
   *y = 0;
   *width = fbdev_dpy->width;
   *height = fbdev_dpy->height;
}

static void
swrastPutImage(__DRIdrawable *drawable, int op,
               int x, int y, int width, int height,
               char *data, void *loaderPrivate)
{
   struct fbdev_egl_surface *fbdev_surf = loaderPrivate;
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(fbdev_surf->base.Resource.Display);
   FB_IOCTL_ARG arg;

   switch (op) {
   case __DRI_SWRAST_IMAGE_OP_DRAW:
      break;
   case __DRI_SWRAST_IMAGE_OP_SWAP:
      arg.setFb.pFb  = data;
      arg.setFb.when = fbdev_surf->base.SwapInterval;
      if (ioctl (fbdev_dpy->fd, FB_IOCTL_SET_FB, &arg) == -1) {
          _eglError(EGL_BAD_ACCESS, "FB_IOCTL_SET_FB");
          return;
      }
      break;
   default:
      return;
   }
}

static void
swrastGetImage(__DRIdrawable *readable,
               int x, int y, int width, int height,
               char *data, void *loaderPrivate)
{
}

static void
swrastGetBackBuffer(char **data, void *loaderPrivate)
{
   struct fbdev_egl_surface *fbdev_surf = loaderPrivate;
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(fbdev_surf->base.Resource.Display);

   *data = fbdev_dpy->pBackBuf;
}

static void
swrastSwapBuffers(char **data, void *loaderPrivate)
{
   struct fbdev_egl_surface *fbdev_surf = loaderPrivate;
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(fbdev_surf->base.Resource.Display);
   
   if (fbdev_dpy->buffers == 2)
      EXCHANGE(fbdev_dpy->pFrontBuf, fbdev_dpy->pBackBuf);
   else if (fbdev_dpy->buffers > 2)
      EXCHANGE3(fbdev_dpy->pFrontBuf, fbdev_dpy->pBackBuf,
                fbdev_dpy->pThirdBuf);
   *data = fbdev_dpy->pBackBuf;
}

static _EGLSurface *
fbdev_create_surface(_EGLDriver *drv, _EGLDisplay *disp, EGLint type,
            _EGLConfig *conf, EGLNativeWindowType window,
            const EGLint *attrib_list)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_config *fbdev_conf = fbdev_egl_config(conf);
   struct fbdev_egl_surface *fbdev_surf;

   (void) drv;

   fbdev_surf = calloc(1, sizeof *fbdev_surf);
   if (!fbdev_surf) {
      _eglError(EGL_BAD_ALLOC, "fbdev_create_surface");
      return NULL;
   }

   if (!_eglInitSurface(&fbdev_surf->base, disp, type, conf, attrib_list))
      goto cleanup_surf;

   switch (type) {
   case EGL_WINDOW_BIT:
      fbdev_surf->base.Width =  fbdev_dpy->width;
      fbdev_surf->base.Height = fbdev_dpy->height;
      break;
   default:
      goto cleanup_surf;
   }

   fbdev_surf->dri_drawable =
      (*fbdev_dpy->swrast->createNewDrawable) (fbdev_dpy->dri_screen,
                                              fbdev_conf->dri_double_config,
                                              fbdev_surf);
   if (fbdev_surf->dri_drawable == NULL) {
      _eglError(EGL_BAD_ALLOC, "createNewDrawable");
      goto cleanup_surf;
   }

   return &fbdev_surf->base;

cleanup_surf:
   free(fbdev_surf);

   return NULL;
}

static _EGLSurface *
fbdev_create_window_surface(_EGLDriver *drv, _EGLDisplay *disp,
               _EGLConfig *conf, EGLNativeWindowType window,
               const EGLint *attrib_list)
{
   return fbdev_create_surface(drv, disp, EGL_WINDOW_BIT, conf,
                  window, attrib_list);
}

static EGLBoolean
fbdev_destroy_surface(_EGLDriver *drv, _EGLDisplay *disp, _EGLSurface *surf)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_surface *fbdev_surf = fbdev_egl_surface(surf);

   if (!_eglPutSurface(surf))
      return EGL_TRUE;

   (*fbdev_dpy->core->destroyDrawable)(fbdev_surf->dri_drawable);

   free(surf);

   return EGL_TRUE;
}

static EGLBoolean
fbdev_swap_buffers(_EGLDriver *drv, _EGLDisplay *disp, _EGLSurface *draw)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_surface *fbdev_surf = fbdev_egl_surface(draw);

   (*fbdev_dpy->core->swapBuffers)(fbdev_surf->dri_drawable);

   return EGL_TRUE;
}

#if defined(GFX_FIXED_FBMODE_OFFSET)
/* Hack for Mesa main code to get stride and width when width != stride */
static int pixelStride = 0;
static int pixelWidth = 0;

int fbdev_get_pixel_stride(void)
{
    return pixelStride;
}

int fbdev_get_pixel_width(void)
{
    return pixelWidth;
}
#endif

EGLBoolean
fbdev_initialize_drm(_EGLDriver *drv, _EGLDisplay *disp)
{
   struct fbdev_egl_display *fbdev_dpy;
   int fd = -1;
   int i;
   int offset;

   fbdev_dpy = calloc(1, sizeof *fbdev_dpy);
   if (!fbdev_dpy)
      return _eglError(EGL_BAD_ALLOC, "eglInitialize");

   disp->DriverData = (void *) fbdev_dpy;

   {
      char deviceName[FB_MAX_STR_CHARS];
      unsigned int i;
      FB_IOCTL_ARG arg;

      for (i = 0; i < FB_MAX_DEVICE; i++) {
         (void)snprintf (deviceName, FB_MAX_STR_CHARS, "%s%d", FB_DEVICE_PREFIX, i);
         if ((fd = open (deviceName, O_RDWR, 0666)) != -1)
            break;
      }
      if (i >= FB_MAX_DEVICE)
         return EGL_FALSE;

      if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1) {
         close (fd);
         return EGL_FALSE;
      }
#if defined(GFX_FIXED_FBMODE_OFFSET)
      pixelStride = arg.getFbInfo.stride / (arg.getFbInfo.bpp>>3);
      pixelWidth = arg.getFbInfo.width;
      offset = arg.getFbInfo.offset;
#else
      offset = arg.getFbInfo.stride * arg.getFbInfo.height;
#endif
      fbdev_dpy->buffers = arg.getFbInfo.buffers;
      fbdev_dpy->pFrontBuf = arg.getFbInfo.pFirstFb;
      if (fbdev_dpy->buffers  < 2) {
         fbdev_dpy->pBackBuf = fbdev_dpy->pFrontBuf;
         fbdev_dpy->pThirdBuf = fbdev_dpy->pBackBuf;
      } else if (fbdev_dpy->buffers == 2) {
         fbdev_dpy->pBackBuf = (void *)((char*)(fbdev_dpy->pFrontBuf) + offset);
         fbdev_dpy->pThirdBuf = fbdev_dpy->pBackBuf;
      } else {
         fbdev_dpy->pBackBuf = (void *)((char*)(fbdev_dpy->pFrontBuf) + offset);
         fbdev_dpy->pThirdBuf = (void *)((char*)(fbdev_dpy->pBackBuf) + offset);
      }
      fbdev_dpy->width = arg.getFbInfo.width, fbdev_dpy->stride = arg.getFbInfo.stride;
      fbdev_dpy->height = arg.getFbInfo.height;

      if (fbdev_load_driver_swrast(disp) == EGL_FALSE) {
         close (fd);
         return EGL_FALSE;
      }

      fbdev_dpy->swrast_loader_extension.base.name = __DRI_SWRAST_LOADER;
      fbdev_dpy->swrast_loader_extension.base.version = __DRI_SWRAST_LOADER_VERSION;
      fbdev_dpy->swrast_loader_extension.getDrawableInfo = swrastGetFbInfo;
      fbdev_dpy->swrast_loader_extension.putImage = swrastPutImage;
      fbdev_dpy->swrast_loader_extension.getImage = swrastGetImage;
      fbdev_dpy->swrast_loader_extension.getBackBuffer = swrastGetBackBuffer;
      fbdev_dpy->swrast_loader_extension.swapBuffers = swrastSwapBuffers;

      fbdev_dpy->extensions[0] = &fbdev_dpy->swrast_loader_extension.base;
      fbdev_dpy->extensions[1] = NULL;
      fbdev_dpy->extensions[2] = NULL;
      fbdev_dpy->extensions[3] = NULL;

      if (fbdev_create_screen(disp) == EGL_FALSE) {
         close (fd);
         return EGL_FALSE;
      }
   }

   fbdev_dpy->fd = fd;

   fbdev_setup_screen(disp);

   for (i = 0; fbdev_dpy->driver_configs[i]; i++)
      fbdev_add_config(disp, fbdev_dpy->driver_configs[i],
                      i + 1, 0, EGL_WINDOW_BIT, NULL, NULL);

   drv->API.CreateWindowSurface = fbdev_create_window_surface;
   drv->API.DestroySurface = fbdev_destroy_surface;
   drv->API.SwapBuffers = fbdev_swap_buffers;

   /* we're supporting EGL 1.4 */
   disp->Version = 14;

   return EGL_TRUE;
}
