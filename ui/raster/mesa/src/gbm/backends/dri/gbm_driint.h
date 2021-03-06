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
 *    Benjamin Franzke <benjaminfranzke@googlemail.com>
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

#ifndef _GBM_DRI_INTERNAL_H_
#define _GBM_DRI_INTERNAL_H_

#include <sys/mman.h>
#include "gbmint.h"

#if defined(__vxworks)
#include <xf86drm.h>
#endif /* __vxworks */
#include "common_drm.h"

#include <GL/gl.h> /* dri_interface needs GL types */
#include "GL/internal/dri_interface.h"

struct gbm_dri_surface;
struct gbm_dri_bo;

struct gbm_dri_device {
   struct gbm_drm_device base;

   void *driver;

   __DRIscreen *screen;

   const __DRIcoreExtension   *core;
   const __DRIdri2Extension   *dri2;
   const __DRIimageExtension  *image;
   const __DRIswrastExtension *swrast;
   const __DRI2flushExtension *flush;
   const __DRIdri2LoaderExtension *loader;

   const __DRIconfig   **driver_configs;
   const __DRIextension **extensions;
   const __DRIextension **driver_extensions;

   __DRIimage *(*lookup_image)(__DRIscreen *screen, void *image, void *data);
   void *lookup_user_data;

   __DRIbuffer *(*get_buffers)(__DRIdrawable * driDrawable,
                               int *width, int *height,
                               unsigned int *attachments, int count,
                               int *out_count, void *data);
   void (*flush_front_buffer)(__DRIdrawable * driDrawable, void *data);
   __DRIbuffer *(*get_buffers_with_format)(__DRIdrawable * driDrawable,
			     int *width, int *height,
			     unsigned int *attachments, int count,
			     int *out_count, void *data);
   int (*image_get_buffers)(__DRIdrawable *driDrawable,
                            unsigned int format,
                            uint32_t *stamp,
                            void *loaderPrivate,
                            uint32_t buffer_mask,
                            struct __DRIimageList *buffers);
   void (*swrast_put_image2)(__DRIdrawable *driDrawable,
                             int            op,
                             int            x,
                             int            y,
                             int            width,
                             int            height,
                             int            stride,
                             char          *data,
                             void          *loaderPrivate);
   void (*swrast_get_image)(__DRIdrawable *driDrawable,
                            int            x,
                            int            y,
                            int            width,
                            int            height,
                            char          *data,
                            void          *loaderPrivate);

   struct wl_drm *wl_drm;
};

struct gbm_dri_bo {
   struct gbm_drm_bo base;

   __DRIimage *image;

   /* Used for cursors and the swrast front BO */
   uint32_t handle, size;
   void *map;
};

struct gbm_dri_surface {
   struct gbm_surface base;

   void *dri_private;
};
#ifdef __GNUC__
#define inline __inline
#endif /* __GNUC__ */

static inline struct gbm_dri_device *
gbm_dri_device(struct gbm_device *gbm)
{
   return (struct gbm_dri_device *) gbm;
}

static inline struct gbm_dri_bo *
gbm_dri_bo(struct gbm_bo *bo)
{
   return (struct gbm_dri_bo *) bo;
}

static inline struct gbm_dri_surface *
gbm_dri_surface(struct gbm_surface *surface)
{
   return (struct gbm_dri_surface *) surface;
}

static inline void *
gbm_dri_bo_map(struct gbm_dri_bo *bo)
{
   struct drm_mode_map_dumb map_arg;
   int ret;

   if (bo->image != NULL)
      return NULL;

   if (bo->map != NULL)
      return bo->map;

   memset(&map_arg, 0, sizeof(map_arg));
   map_arg.handle = bo->handle;

   ret = drmIoctl(bo->base.base.gbm->fd, DRM_IOCTL_MODE_MAP_DUMB, &map_arg);
   if (ret)
      return NULL;

#if defined(__vxworks)
    {
        struct drm_vxmmap mapreq;
        mapreq.size = bo->size;
        mapreq.offset = map_arg.offset;
        mapreq.virtAddr = 0;
        mapreq.physAddr = 0;
        if (drmIoctl(bo->base.base.gbm->fd, DRM_IOCTL_VXMMAP, &mapreq)) {
            return NULL;
        }
        bo->map = (void *)mapreq.virtAddr;
    }
#else
   bo->map = mmap(0, bo->size, PROT_WRITE,
                  MAP_SHARED, bo->base.base.gbm->fd, map_arg.offset);
   if (bo->map == MAP_FAILED) {
      bo->map = NULL;
      return NULL;
   }
#endif /* __vxworks */

   return bo->map;
}

static inline void
gbm_dri_bo_unmap(struct gbm_dri_bo *bo)
{
   munmap(bo->map, bo->size);
   bo->map = NULL;
}

#endif
