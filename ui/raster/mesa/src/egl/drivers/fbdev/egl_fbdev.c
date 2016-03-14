/*
 * Copyright © 2010 Intel Corporation
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
15sep15,yat  Port Mesa DRI i965 to VxWorks 7 (US24710)
22dec14,yat  Port Mesa to VxWorks 7 (US24705)
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/internal/dri_interface.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "egl_fbdev.h"

EGLint fbdev_to_egl_attribute_map[] = {
   0,
   EGL_BUFFER_SIZE,		/* __DRI_ATTRIB_BUFFER_SIZE */
   EGL_LEVEL,			/* __DRI_ATTRIB_LEVEL */
   EGL_RED_SIZE,		/* __DRI_ATTRIB_RED_SIZE */
   EGL_GREEN_SIZE,		/* __DRI_ATTRIB_GREEN_SIZE */
   EGL_BLUE_SIZE,		/* __DRI_ATTRIB_BLUE_SIZE */
   EGL_LUMINANCE_SIZE,		/* __DRI_ATTRIB_LUMINANCE_SIZE */
   EGL_ALPHA_SIZE,		/* __DRI_ATTRIB_ALPHA_SIZE */
   0,				/* __DRI_ATTRIB_ALPHA_MASK_SIZE */
   EGL_DEPTH_SIZE,		/* __DRI_ATTRIB_DEPTH_SIZE */
   EGL_STENCIL_SIZE,		/* __DRI_ATTRIB_STENCIL_SIZE */
   0,				/* __DRI_ATTRIB_ACCUM_RED_SIZE */
   0,				/* __DRI_ATTRIB_ACCUM_GREEN_SIZE */
   0,				/* __DRI_ATTRIB_ACCUM_BLUE_SIZE */
   0,				/* __DRI_ATTRIB_ACCUM_ALPHA_SIZE */
   EGL_SAMPLE_BUFFERS,		/* __DRI_ATTRIB_SAMPLE_BUFFERS */
   EGL_SAMPLES,			/* __DRI_ATTRIB_SAMPLES */
   0,				/* __DRI_ATTRIB_RENDER_TYPE, */
   0,				/* __DRI_ATTRIB_CONFIG_CAVEAT */
   0,				/* __DRI_ATTRIB_CONFORMANT */
   0,				/* __DRI_ATTRIB_DOUBLE_BUFFER */
   0,				/* __DRI_ATTRIB_STEREO */
   0,				/* __DRI_ATTRIB_AUX_BUFFERS */
   0,				/* __DRI_ATTRIB_TRANSPARENT_TYPE */
   0,				/* __DRI_ATTRIB_TRANSPARENT_INDEX_VALUE */
   0,				/* __DRI_ATTRIB_TRANSPARENT_RED_VALUE */
   0,				/* __DRI_ATTRIB_TRANSPARENT_GREEN_VALUE */
   0,				/* __DRI_ATTRIB_TRANSPARENT_BLUE_VALUE */
   0,				/* __DRI_ATTRIB_TRANSPARENT_ALPHA_VALUE */
   0,				/* __DRI_ATTRIB_FLOAT_MODE (deprecated) */
   0,				/* __DRI_ATTRIB_RED_MASK */
   0,				/* __DRI_ATTRIB_GREEN_MASK */
   0,				/* __DRI_ATTRIB_BLUE_MASK */
   0,				/* __DRI_ATTRIB_ALPHA_MASK */
   EGL_MAX_PBUFFER_WIDTH,	/* __DRI_ATTRIB_MAX_PBUFFER_WIDTH */
   EGL_MAX_PBUFFER_HEIGHT,	/* __DRI_ATTRIB_MAX_PBUFFER_HEIGHT */
   EGL_MAX_PBUFFER_PIXELS,	/* __DRI_ATTRIB_MAX_PBUFFER_PIXELS */
   0,				/* __DRI_ATTRIB_OPTIMAL_PBUFFER_WIDTH */
   0,				/* __DRI_ATTRIB_OPTIMAL_PBUFFER_HEIGHT */
   0,				/* __DRI_ATTRIB_VISUAL_SELECT_GROUP */
   0,				/* __DRI_ATTRIB_SWAP_METHOD */
   EGL_MAX_SWAP_INTERVAL,	/* __DRI_ATTRIB_MAX_SWAP_INTERVAL */
   EGL_MIN_SWAP_INTERVAL,	/* __DRI_ATTRIB_MIN_SWAP_INTERVAL */
   0,				/* __DRI_ATTRIB_BIND_TO_TEXTURE_RGB */
   0,				/* __DRI_ATTRIB_BIND_TO_TEXTURE_RGBA */
   0,				/* __DRI_ATTRIB_BIND_TO_MIPMAP_TEXTURE */
   0,				/* __DRI_ATTRIB_BIND_TO_TEXTURE_TARGETS */
   EGL_Y_INVERTED_NOK,		/* __DRI_ATTRIB_YINVERTED */
   0,				/* __DRI_ATTRIB_FRAMEBUFFER_SRGB_CAPABLE */
};

static EGLBoolean
fbdev_match_config(const _EGLConfig *conf, const _EGLConfig *criteria)
{
   if (_eglCompareConfigs(conf, criteria, NULL, EGL_FALSE) != 0)
      return EGL_FALSE;

   if (!_eglMatchConfig(conf, criteria))
      return EGL_FALSE;

   return EGL_TRUE;
}

struct fbdev_egl_config *
fbdev_add_config(_EGLDisplay *disp, const __DRIconfig *dri_config, int id,
		int depth, EGLint surface_type, const EGLint *attr_list,
		const unsigned int *rgba_masks)
{
   struct fbdev_egl_config *conf;
   struct fbdev_egl_display *fbdev_dpy;
   _EGLConfig base;
   unsigned int attrib, value, double_buffer;
   EGLint key, bind_to_texture_rgb, bind_to_texture_rgba;
   unsigned int dri_masks[4] = { 0, 0, 0, 0 };
   _EGLConfig *matching_config;
   EGLint num_configs = 0;
   EGLint config_id;
   int i;

   fbdev_dpy = disp->DriverData;
   _eglInitConfig(&base, disp, id);
   
   i = 0;
   double_buffer = 0;
   bind_to_texture_rgb = 0;
   bind_to_texture_rgba = 0;

   while (fbdev_dpy->core->indexConfigAttrib(dri_config, i++, &attrib, &value)) {
      switch (attrib) {
      case __DRI_ATTRIB_RENDER_TYPE:
	 if (value & __DRI_ATTRIB_RGBA_BIT)
	    value = EGL_RGB_BUFFER;
	 else if (value & __DRI_ATTRIB_LUMINANCE_BIT)
	    value = EGL_LUMINANCE_BUFFER;
	 else
	    return NULL;
	 _eglSetConfigKey(&base, EGL_COLOR_BUFFER_TYPE, value);
	 break;	 

      case __DRI_ATTRIB_CONFIG_CAVEAT:
         if (value & __DRI_ATTRIB_NON_CONFORMANT_CONFIG)
            value = EGL_NON_CONFORMANT_CONFIG;
         else if (value & __DRI_ATTRIB_SLOW_BIT)
            value = EGL_SLOW_CONFIG;
	 else
	    value = EGL_NONE;
	 _eglSetConfigKey(&base, EGL_CONFIG_CAVEAT, value);
         break;

      case __DRI_ATTRIB_BIND_TO_TEXTURE_RGB:
	 bind_to_texture_rgb = value;
	 break;

      case __DRI_ATTRIB_BIND_TO_TEXTURE_RGBA:
	 bind_to_texture_rgba = value;
	 break;

      case __DRI_ATTRIB_DOUBLE_BUFFER:
	 double_buffer = value;
	 break;

      case __DRI_ATTRIB_RED_MASK:
         dri_masks[0] = value;
         break;

      case __DRI_ATTRIB_GREEN_MASK:
         dri_masks[1] = value;
         break;

      case __DRI_ATTRIB_BLUE_MASK:
         dri_masks[2] = value;
         break;

      case __DRI_ATTRIB_ALPHA_MASK:
         dri_masks[3] = value;
         break;

      default:
	 key = fbdev_to_egl_attribute_map[attrib];
	 if (key != 0)
	    _eglSetConfigKey(&base, key, value);
	 break;
      }
   }

   if (attr_list)
      for (i = 0; attr_list[i] != EGL_NONE; i += 2)
         _eglSetConfigKey(&base, attr_list[i], attr_list[i+1]);

   /* Allow a 24-bit RGB visual to match a 32-bit RGBA EGLConfig.  Otherwise
    * it will only match a 32-bit RGBA visual.  On a composited window manager
    * on X11, this will make all of the EGLConfigs with destination alpha get
    * blended by the compositor.  This is probably not what the application
    * wants... especially on drivers that only have 32-bit RGBA EGLConfigs!
    */
   if (depth > 0 && depth != base.BufferSize
       && !(depth == 24 && base.BufferSize == 32))
      return NULL;

   if (rgba_masks && memcmp(rgba_masks, dri_masks, sizeof(dri_masks)))
      return NULL;

   base.NativeRenderable = EGL_TRUE;

   base.SurfaceType = surface_type;
   if (surface_type & (EGL_PBUFFER_BIT |
       (disp->Extensions.NOK_texture_from_pixmap ? EGL_PIXMAP_BIT : 0))) {
      base.BindToTextureRGB = bind_to_texture_rgb;
      if (base.AlphaSize > 0)
         base.BindToTextureRGBA = bind_to_texture_rgba;
   }

   base.RenderableType = disp->ClientAPIs;
   base.Conformant = disp->ClientAPIs;

   base.MinSwapInterval = fbdev_dpy->min_swap_interval;
   base.MaxSwapInterval = fbdev_dpy->max_swap_interval;

   if (!_eglValidateConfig(&base, EGL_FALSE)) {
      _eglLog(_EGL_DEBUG, "DRI2: failed to validate config %d", id);
      return NULL;
   }

   config_id = base.ConfigID;
   base.ConfigID    = EGL_DONT_CARE;
   base.SurfaceType = EGL_DONT_CARE;
   num_configs = _eglFilterArray(disp->Configs, (void **) &matching_config, 1,
                                 (_EGLArrayForEach) fbdev_match_config, &base);

   if (num_configs == 1) {
      conf = (struct fbdev_egl_config *) matching_config;

      if (double_buffer && !conf->dri_double_config)
         conf->dri_double_config = dri_config;
      else if (!double_buffer && !conf->dri_single_config)
         conf->dri_single_config = dri_config;
      else
         /* a similar config type is already added (unlikely) => discard */
         return NULL;
   }
   else if (num_configs == 0) {
      conf = malloc(sizeof *conf);
      if (conf == NULL)
         return NULL;

      memcpy(&conf->base, &base, sizeof base);
      if (double_buffer) {
         conf->dri_double_config = dri_config;
         conf->dri_single_config = NULL;
      } else {
         conf->dri_single_config = dri_config;
         conf->dri_double_config = NULL;
      }
      conf->base.SurfaceType = 0;
      conf->base.ConfigID = config_id;

      _eglLinkConfig(&conf->base);
   }
   else {
      assert(0);
      return NULL;
   }

   if (double_buffer) {
      surface_type &= ~EGL_PIXMAP_BIT;
   }

   conf->base.SurfaceType |= surface_type;

   return conf;
}

__DRIimage *
fbdev_lookup_egl_image(__DRIscreen *screen, void *image, void *data)
{
   _EGLDisplay *disp = data;
   struct fbdev_egl_image *fbdev_img;
   _EGLImage *img;

   (void) screen;

   img = _eglLookupImage(image, disp);
   if (img == NULL) {
      _eglError(EGL_BAD_PARAMETER, "fbdev_lookup_egl_image");
      return NULL;
   }

   fbdev_img = fbdev_egl_image(image);

   return fbdev_img->dri_image;
}

struct fbdev_extension_match {
   const char *name;
   int version;
   int offset;
};

static struct fbdev_extension_match fbdev_core_extensions[] = {
   { __DRI2_FLUSH, 1, offsetof(struct fbdev_egl_display, flush) },
   { __DRI_TEX_BUFFER, 2, offsetof(struct fbdev_egl_display, tex_buffer) },
   { __DRI_IMAGE, 1, offsetof(struct fbdev_egl_display, image) },
   { NULL, 0, 0 }
};

static struct fbdev_extension_match swrast_driver_extensions[] = {
   { __DRI_CORE, 1, offsetof(struct fbdev_egl_display, core) },
   { __DRI_SWRAST, 2, offsetof(struct fbdev_egl_display, swrast) },
   { NULL, 0, 0 }
};

static struct fbdev_extension_match swrast_core_extensions[] = {
   { __DRI_TEX_BUFFER, 2, offsetof(struct fbdev_egl_display, tex_buffer) },
   { NULL, 0, 0 }
};

static EGLBoolean
fbdev_bind_extensions(struct fbdev_egl_display *fbdev_dpy,
		     struct fbdev_extension_match *matches,
		     const __DRIextension **extensions)
{
   int i, j, ret = EGL_TRUE;
   void *field;

   for (i = 0; extensions[i]; i++) {
      _eglLog(_EGL_DEBUG, "DRI2: found extension `%s'", extensions[i]->name);
      for (j = 0; matches[j].name; j++) {
	 if (strcmp(extensions[i]->name, matches[j].name) == 0 &&
	     extensions[i]->version >= matches[j].version) {
	    field = ((char *) fbdev_dpy + matches[j].offset);
	    *(const __DRIextension **) field = extensions[i];
	    _eglLog(_EGL_INFO, "DRI2: found extension %s version %d",
		    extensions[i]->name, extensions[i]->version);
	 }
      }
   }
   
   for (j = 0; matches[j].name; j++) {
      field = ((char *) fbdev_dpy + matches[j].offset);
      if (*(const __DRIextension **) field == NULL) {
	 _eglLog(_EGL_FATAL, "DRI2: did not find extension %s version %d",
		 matches[j].name, matches[j].version);
	 ret = EGL_FALSE;
      }
   }

   return ret;
}

EGLBoolean
fbdev_load_driver_swrast(_EGLDisplay *disp)
{
   struct fbdev_egl_display *fbdev_dpy = disp->DriverData;
   const __DRIextension **extensions;
   extern const __DRIextension **__driDriverGetExtensions_swrast(void);

   fbdev_dpy->driver_name = "swrast";
   extensions = __driDriverGetExtensions_swrast();
   if (!fbdev_bind_extensions(fbdev_dpy, swrast_driver_extensions, extensions)) {
      return EGL_FALSE;
   }

   return EGL_TRUE;
}

void
fbdev_setup_screen(_EGLDisplay *disp)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   unsigned int api_mask;

   if (fbdev_dpy->fbdev) {
      api_mask = fbdev_dpy->fbdev->getAPIMask(fbdev_dpy->dri_screen);
   } else {
      assert(fbdev_dpy->swrast);
      api_mask = 1 << __DRI_API_OPENGL |
                 1 << __DRI_API_GLES;
   }

   disp->ClientAPIs = 0;
   if (api_mask & (1 <<__DRI_API_OPENGL))
      disp->ClientAPIs |= EGL_OPENGL_BIT;
   if (api_mask & (1 <<__DRI_API_GLES))
      disp->ClientAPIs |= EGL_OPENGL_ES_BIT;
   if (api_mask & (1 << __DRI_API_GLES2))
      disp->ClientAPIs |= EGL_OPENGL_ES2_BIT;
   if (api_mask & (1 << __DRI_API_GLES3))
      disp->ClientAPIs |= EGL_OPENGL_ES3_BIT_KHR;

   assert(fbdev_dpy->fbdev || fbdev_dpy->swrast);
   disp->Extensions.KHR_surfaceless_context = EGL_TRUE;

   if (fbdev_dpy->fbdev && fbdev_dpy->fbdev->base.version >= 3) {
      disp->Extensions.KHR_create_context = EGL_TRUE;

      if (fbdev_dpy->robustness)
         disp->Extensions.EXT_create_context_robustness = EGL_TRUE;
   }

   if (fbdev_dpy->image) {
      disp->Extensions.MESA_drm_image = EGL_TRUE;
      disp->Extensions.KHR_image_base = EGL_TRUE;
      disp->Extensions.KHR_gl_renderbuffer_image = EGL_TRUE;
      if (fbdev_dpy->image->base.version >= 5 &&
          fbdev_dpy->image->createImageFromTexture) {
         disp->Extensions.KHR_gl_texture_2D_image = EGL_TRUE;
         disp->Extensions.KHR_gl_texture_cubemap_image = EGL_TRUE;
      }
   }
}

EGLBoolean
fbdev_create_screen(_EGLDisplay *disp)
{
   const __DRIextension **extensions;
   struct fbdev_egl_display *fbdev_dpy;

   fbdev_dpy = disp->DriverData;

   if (fbdev_dpy->fbdev) {
      fbdev_dpy->dri_screen =
         fbdev_dpy->fbdev->createNewScreen(0, fbdev_dpy->fd, fbdev_dpy->extensions,
				         &fbdev_dpy->driver_configs, disp);
   } else {
      assert(fbdev_dpy->swrast);
      fbdev_dpy->dri_screen =
         fbdev_dpy->swrast->createNewScreen(0, fbdev_dpy->extensions,
                                           &fbdev_dpy->driver_configs, disp);
   }

   if (fbdev_dpy->dri_screen == NULL) {
      _eglLog(_EGL_WARNING, "DRI2: failed to create dri screen");
      return EGL_FALSE;
   }

   fbdev_dpy->own_dri_screen = 1;

   extensions = fbdev_dpy->core->getExtensions(fbdev_dpy->dri_screen);
   
   if (fbdev_dpy->fbdev) {
      unsigned i;

      if (!fbdev_bind_extensions(fbdev_dpy, fbdev_core_extensions, extensions))
         goto cleanup_dri_screen;

      for (i = 0; extensions[i]; i++) {
	 if (strcmp(extensions[i]->name, __DRI2_ROBUSTNESS) == 0) {
            fbdev_dpy->robustness = (__DRIrobustnessExtension *) extensions[i];
	 }
	 if (strcmp(extensions[i]->name, __DRI2_CONFIG_QUERY) == 0) {
	    fbdev_dpy->config = (__DRI2configQueryExtension *) extensions[i];
	 }
      }
   } else {
      assert(fbdev_dpy->swrast);
      if (!fbdev_bind_extensions(fbdev_dpy, swrast_core_extensions, extensions))
         goto cleanup_dri_screen;
   }

   fbdev_setup_screen(disp);

   return EGL_TRUE;

 cleanup_dri_screen:
   fbdev_dpy->core->destroyScreen(fbdev_dpy->dri_screen);

   return EGL_FALSE;
}

/**
 * Called via eglInitialize(), GLX_drv->API.Initialize().
 */
static EGLBoolean
fbdev_initialize(_EGLDriver *drv, _EGLDisplay *disp)
{
   /* not until swrast_dri is supported */
   if (disp->Options.UseFallback)
      return EGL_FALSE;

   switch (disp->Platform) {
   case _EGL_PLATFORM_DRM:
      if (disp->Options.TestOnly)
         return EGL_TRUE;
      return fbdev_initialize_drm(drv, disp);

   default:
      return EGL_FALSE;
   }
}

/**
 * Called via eglTerminate(), drv->API.Terminate().
 */
static EGLBoolean
fbdev_terminate(_EGLDriver *drv, _EGLDisplay *disp)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);

   _eglReleaseDisplayResources(drv, disp);
   _eglCleanupDisplay(disp);

   if (fbdev_dpy->own_dri_screen)
      fbdev_dpy->core->destroyScreen(fbdev_dpy->dri_screen);
   if (fbdev_dpy->fd)
      close(fbdev_dpy->fd);
   free(fbdev_dpy->device_name);

   free(fbdev_dpy);
   disp->DriverData = NULL;

   return EGL_TRUE;
}

/**
 * Set the error code after a call to
 * fbdev_egl_display::fbdev::createContextAttribs.
 */
static void
fbdev_create_context_attribs_error(int dri_error)
{
   EGLint egl_error;

   switch (dri_error) {
   case __DRI_CTX_ERROR_SUCCESS:
      return;

   case __DRI_CTX_ERROR_NO_MEMORY:
      egl_error = EGL_BAD_ALLOC;
      break;

  /* From the EGL_KHR_create_context spec, section "Errors":
   *
   *   * If <config> does not support a client API context compatible
   *     with the requested API major and minor version, [...] context flags,
   *     and context reset notification behavior (for client API types where
   *     these attributes are supported), then an EGL_BAD_MATCH error is
   *     generated.
   *
   *   * If an OpenGL ES context is requested and the values for
   *     attributes EGL_CONTEXT_MAJOR_VERSION_KHR and
   *     EGL_CONTEXT_MINOR_VERSION_KHR specify an OpenGL ES version that
   *     is not defined, than an EGL_BAD_MATCH error is generated.
   *
   *   * If an OpenGL context is requested, the requested version is
   *     greater than 3.2, and the value for attribute
   *     EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR has no bits set; has any
   *     bits set other than EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR and
   *     EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR; has more than
   *     one of these bits set; or if the implementation does not support
   *     the requested profile, then an EGL_BAD_MATCH error is generated.
   */
   case __DRI_CTX_ERROR_BAD_API:
   case __DRI_CTX_ERROR_BAD_VERSION:
   case __DRI_CTX_ERROR_BAD_FLAG:
      egl_error = EGL_BAD_MATCH;
      break;

  /* From the EGL_KHR_create_context spec, section "Errors":
   *
   *   * If an attribute name or attribute value in <attrib_list> is not
   *     recognized (including unrecognized bits in bitmask attributes),
   *     then an EGL_BAD_ATTRIBUTE error is generated."
   */
   case __DRI_CTX_ERROR_UNKNOWN_ATTRIBUTE:
   case __DRI_CTX_ERROR_UNKNOWN_FLAG:
      egl_error = EGL_BAD_ATTRIBUTE;
      break;

   default:
      assert(0);
      egl_error = EGL_BAD_MATCH;
      break;
   }

   _eglError(egl_error, "fbdev_create_context");
}

/**
 * Called via eglCreateContext(), drv->API.CreateContext().
 */
static _EGLContext *
fbdev_create_context(_EGLDriver *drv, _EGLDisplay *disp, _EGLConfig *conf,
		    _EGLContext *share_list, const EGLint *attrib_list)
{
   struct fbdev_egl_context *fbdev_ctx;
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_context *fbdev_ctx_shared = fbdev_egl_context(share_list);
   __DRIcontext *shared =
      fbdev_ctx_shared ? fbdev_ctx_shared->dri_context : NULL;
   struct fbdev_egl_config *fbdev_config = fbdev_egl_config(conf);
   const __DRIconfig *dri_config;
   int api;

   (void) drv;

   fbdev_ctx = malloc(sizeof *fbdev_ctx);
   if (!fbdev_ctx) {
      _eglError(EGL_BAD_ALLOC, "eglCreateContext");
      return NULL;
   }

   if (!_eglInitContext(&fbdev_ctx->base, disp, conf, attrib_list))
      goto cleanup;

   switch (fbdev_ctx->base.ClientAPI) {
   case EGL_OPENGL_ES_API:
      switch (fbdev_ctx->base.ClientMajorVersion) {
      case 1:
         api = __DRI_API_GLES;
         break;
      case 2:
         api = __DRI_API_GLES2;
         break;
      case 3:
         api = __DRI_API_GLES3;
         break;
      default:
	 _eglError(EGL_BAD_PARAMETER, "eglCreateContext");
	 return NULL;
      }
      break;
   case EGL_OPENGL_API:
      if ((fbdev_ctx->base.ClientMajorVersion >= 4
           || (fbdev_ctx->base.ClientMajorVersion == 3
               && fbdev_ctx->base.ClientMinorVersion >= 2))
          && fbdev_ctx->base.Profile == EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR)
         api = __DRI_API_OPENGL_CORE;
      else
         api = __DRI_API_OPENGL;
      break;
   default:
      _eglError(EGL_BAD_PARAMETER, "eglCreateContext");
      free(fbdev_ctx);
      return NULL;
   }

   if (conf != NULL) {
      /* The config chosen here isn't necessarily
       * used for surfaces later.
       * A pixmap surface will use the single config.
       * This opportunity depends on disabling the
       * doubleBufferMode check in
       * src/mesa/main/context.c:check_compatible()
       */
      if (fbdev_config->dri_double_config)
         dri_config = fbdev_config->dri_double_config;
      else
         dri_config = fbdev_config->dri_single_config;

      /* EGL_WINDOW_BIT is set only when there is a dri_double_config.  This
       * makes sure the back buffer will always be used.
       */
      if (conf->SurfaceType & EGL_WINDOW_BIT)
         fbdev_ctx->base.WindowRenderBuffer = EGL_BACK_BUFFER;
   }
   else
      dri_config = NULL;

   if (fbdev_dpy->fbdev) {
      if (fbdev_dpy->fbdev->base.version >= 3) {
         unsigned error;
         unsigned num_attribs = 0;
         uint32_t ctx_attribs[8];

         ctx_attribs[num_attribs++] = __DRI_CTX_ATTRIB_MAJOR_VERSION;
         ctx_attribs[num_attribs++] = fbdev_ctx->base.ClientMajorVersion;
         ctx_attribs[num_attribs++] = __DRI_CTX_ATTRIB_MINOR_VERSION;
         ctx_attribs[num_attribs++] = fbdev_ctx->base.ClientMinorVersion;

         if (fbdev_ctx->base.Flags != 0) {
            /* If the implementation doesn't support the __DRI2_ROBUSTNESS
             * extension, don't even try to send it the robust-access flag.
             * It may explode.  Instead, generate the required EGL error here.
             */
            if ((fbdev_ctx->base.Flags & EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR) != 0
                && !fbdev_dpy->robustness) {
               _eglError(EGL_BAD_MATCH, "eglCreateContext");
               goto cleanup;
            }

            ctx_attribs[num_attribs++] = __DRI_CTX_ATTRIB_FLAGS;
            ctx_attribs[num_attribs++] = fbdev_ctx->base.Flags;
         }

         if (fbdev_ctx->base.ResetNotificationStrategy != EGL_NO_RESET_NOTIFICATION_KHR) {
            /* If the implementation doesn't support the __DRI2_ROBUSTNESS
             * extension, don't even try to send it a reset strategy.  It may
             * explode.  Instead, generate the required EGL error here.
             */
            if (!fbdev_dpy->robustness) {
               _eglError(EGL_BAD_CONFIG, "eglCreateContext");
               goto cleanup;
            }

            ctx_attribs[num_attribs++] = __DRI_CTX_ATTRIB_RESET_STRATEGY;
            ctx_attribs[num_attribs++] = __DRI_CTX_RESET_LOSE_CONTEXT;
         }

         assert(num_attribs <= ARRAY_SIZE(ctx_attribs));

	 fbdev_ctx->dri_context =
	    fbdev_dpy->fbdev->createContextAttribs(fbdev_dpy->dri_screen,
                                                 api,
                                                 dri_config,
                                                 shared,
                                                 num_attribs / 2,
                                                 ctx_attribs,
                                                 & error,
                                                 fbdev_ctx);
	 fbdev_create_context_attribs_error(error);
      } else {
	 fbdev_ctx->dri_context =
	    fbdev_dpy->fbdev->createNewContextForAPI(fbdev_dpy->dri_screen,
						   api,
						   dri_config,
                                                   shared,
						   fbdev_ctx);
      }
   } else {
      assert(fbdev_dpy->swrast);
      fbdev_ctx->dri_context =
         fbdev_dpy->swrast->createNewContextForAPI(fbdev_dpy->dri_screen,
                                                  api,
                                                  dri_config,
                                                  shared,
                                                  fbdev_ctx);
   }

   if (!fbdev_ctx->dri_context)
      goto cleanup;

   return &fbdev_ctx->base;

 cleanup:
   free(fbdev_ctx);
   return NULL;
}

/**
 * Called via eglDestroyContext(), drv->API.DestroyContext().
 */
static EGLBoolean
fbdev_destroy_context(_EGLDriver *drv, _EGLDisplay *disp, _EGLContext *ctx)
{
   struct fbdev_egl_context *fbdev_ctx = fbdev_egl_context(ctx);
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);

   if (_eglPutContext(ctx)) {
      fbdev_dpy->core->destroyContext(fbdev_ctx->dri_context);
      free(fbdev_ctx);
   }

   return EGL_TRUE;
}

/**
 * Called via eglMakeCurrent(), drv->API.MakeCurrent().
 */
static EGLBoolean
fbdev_make_current(_EGLDriver *drv, _EGLDisplay *disp, _EGLSurface *dsurf,
		  _EGLSurface *rsurf, _EGLContext *ctx)
{
   struct fbdev_egl_driver *fbdev_drv = fbdev_egl_driver(drv);
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_surface *fbdev_dsurf = fbdev_egl_surface(dsurf);
   struct fbdev_egl_surface *fbdev_rsurf = fbdev_egl_surface(rsurf);
   struct fbdev_egl_context *fbdev_ctx = fbdev_egl_context(ctx);
   _EGLContext *old_ctx;
   _EGLSurface *old_dsurf, *old_rsurf;
   __DRIdrawable *ddraw, *rdraw;
   __DRIcontext *cctx;

   /* make new bindings */
   if (!_eglBindContext(ctx, dsurf, rsurf, &old_ctx, &old_dsurf, &old_rsurf))
      return EGL_FALSE;

   /* flush before context switch */
   if (old_ctx && fbdev_drv->glFlush)
      fbdev_drv->glFlush();

   ddraw = (fbdev_dsurf) ? fbdev_dsurf->dri_drawable : NULL;
   rdraw = (fbdev_rsurf) ? fbdev_rsurf->dri_drawable : NULL;
   cctx = (fbdev_ctx) ? fbdev_ctx->dri_context : NULL;

   if (old_ctx) {
      __DRIcontext *old_cctx = fbdev_egl_context(old_ctx)->dri_context;
      fbdev_dpy->core->unbindContext(old_cctx);
   }

   if ((cctx == NULL && ddraw == NULL && rdraw == NULL) ||
       fbdev_dpy->core->bindContext(cctx, ddraw, rdraw)) {
      if (old_dsurf)
         drv->API.DestroySurface(drv, disp, old_dsurf);
      if (old_rsurf)
         drv->API.DestroySurface(drv, disp, old_rsurf);
      if (old_ctx)
         drv->API.DestroyContext(drv, disp, old_ctx);

      return EGL_TRUE;
   } else {
      /* undo the previous _eglBindContext */
      (void)_eglBindContext(old_ctx, old_dsurf, old_rsurf, &ctx, &dsurf, &rsurf);
      assert(&fbdev_ctx->base == ctx &&
             &fbdev_dsurf->base == dsurf &&
             &fbdev_rsurf->base == rsurf);

      (void)_eglPutSurface(dsurf);
      (void)_eglPutSurface(rsurf);
      (void)_eglPutContext(ctx);

      (void)_eglPutSurface(old_dsurf);
      (void)_eglPutSurface(old_rsurf);
      (void)_eglPutContext(old_ctx);

      return EGL_FALSE;
   }
}

/*
 * Called from eglGetProcAddress() via drv->API.GetProcAddress().
 */
static _EGLProc
fbdev_get_proc_address(_EGLDriver *drv, const char *procname)
{
   struct fbdev_egl_driver *fbdev_drv = fbdev_egl_driver(drv);

   return fbdev_drv->get_proc_address(procname);
}

static EGLBoolean
fbdev_wait_client(_EGLDriver *drv, _EGLDisplay *disp, _EGLContext *ctx)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_surface *fbdev_surf = fbdev_egl_surface(ctx->DrawSurface);

   (void) drv;

   /* FIXME: If EGL allows frontbuffer rendering for window surfaces,
    * we need to copy fake to real here.*/

   if (fbdev_dpy->flush != NULL)
      fbdev_dpy->flush->flush(fbdev_surf->dri_drawable);

   return EGL_TRUE;
}

static EGLBoolean
fbdev_wait_native(_EGLDriver *drv, _EGLDisplay *disp, EGLint engine)
{
   (void) drv;
   (void) disp;

   if (engine != EGL_CORE_NATIVE_ENGINE)
      return _eglError(EGL_BAD_PARAMETER, "eglWaitNative");
   /* glXWaitX(); */

   return EGL_TRUE;
}

static EGLBoolean
fbdev_bind_tex_image(_EGLDriver *drv,
		    _EGLDisplay *disp, _EGLSurface *surf, EGLint buffer)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_surface *fbdev_surf = fbdev_egl_surface(surf);
   struct fbdev_egl_context *fbdev_ctx;
   _EGLContext *ctx;
   GLint format = __DRI_TEXTURE_FORMAT_NONE;
   GLint target = 0;

   ctx = _eglGetCurrentContext();
   fbdev_ctx = fbdev_egl_context(ctx);

   if (!_eglBindTexImage(drv, disp, surf, buffer))
      return EGL_FALSE;

   switch (fbdev_surf->base.TextureFormat) {
   case EGL_TEXTURE_RGB:
      format = __DRI_TEXTURE_FORMAT_RGB;
      break;
   case EGL_TEXTURE_RGBA:
      format = __DRI_TEXTURE_FORMAT_RGBA;
      break;
   default:
      assert(0);
   }

   switch (fbdev_surf->base.TextureTarget) {
   case EGL_TEXTURE_2D:
      target = GL_TEXTURE_2D;
      break;
   default:
      assert(0);
   }

   (*fbdev_dpy->tex_buffer->setTexBuffer2)(fbdev_ctx->dri_context,
					  target, format,
					  fbdev_surf->dri_drawable);

   return EGL_TRUE;
}

static EGLBoolean
fbdev_release_tex_image(_EGLDriver *drv,
		       _EGLDisplay *disp, _EGLSurface *surf, EGLint buffer)
{
#if __DRI_TEX_BUFFER_VERSION >= 3
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_surface *fbdev_surf = fbdev_egl_surface(surf);
   struct fbdev_egl_context *fbdev_ctx;
   _EGLContext *ctx;
   GLint  target;

   ctx = _eglGetCurrentContext();
   fbdev_ctx = fbdev_egl_context(ctx);

   if (!_eglReleaseTexImage(drv, disp, surf, buffer))
      return EGL_FALSE;

   switch (fbdev_surf->base.TextureTarget) {
   case EGL_TEXTURE_2D:
      target = GL_TEXTURE_2D;
      break;
   default:
      assert(0);
   }
   if (fbdev_dpy->tex_buffer->releaseTexBuffer!=NULL)
    (*fbdev_dpy->tex_buffer->releaseTexBuffer)(fbdev_ctx->dri_context,
                                             target,
                                             fbdev_surf->dri_drawable);
#endif

   return EGL_TRUE;
}

static _EGLImage *
fbdev_create_image(_EGLDisplay *disp, __DRIimage *dri_image)
{
   struct fbdev_egl_image *fbdev_img;

   if (dri_image == NULL) {
      _eglError(EGL_BAD_ALLOC, "fbdev_create_image");
      return NULL;
   }

   fbdev_img = malloc(sizeof *fbdev_img);
   if (!fbdev_img) {
      _eglError(EGL_BAD_ALLOC, "fbdev_create_image");
      return NULL;
   }

   if (!_eglInitImage(&fbdev_img->base, disp)) {
      free(fbdev_img);
      return NULL;
   }

   fbdev_img->dri_image = dri_image;

   return &fbdev_img->base;
}

static _EGLImage *
fbdev_create_image_khr_renderbuffer(_EGLDisplay *disp, _EGLContext *ctx,
				   EGLClientBuffer buffer,
				   const EGLint *attr_list)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_context *fbdev_ctx = fbdev_egl_context(ctx);
   GLuint renderbuffer = (GLuint) (uintptr_t) buffer;
   __DRIimage *dri_image;

   if (renderbuffer == 0) {
      _eglError(EGL_BAD_PARAMETER, "fbdev_create_image_khr");
      return EGL_NO_IMAGE_KHR;
   }

   dri_image =
      fbdev_dpy->image->createImageFromRenderbuffer(fbdev_ctx->dri_context,
                                                   renderbuffer, NULL);

   return fbdev_create_image(disp, dri_image);
}

static _EGLImage *
fbdev_create_image_mesa_drm_buffer(_EGLDisplay *disp, _EGLContext *ctx,
				  EGLClientBuffer buffer, const EGLint *attr_list)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   EGLint format, name, pitch, err;
   _EGLImageAttribs attrs;
   __DRIimage *dri_image;

   name = (EGLint) (uintptr_t) buffer;

   err = _eglParseImageAttribList(&attrs, disp, attr_list);
   if (err != EGL_SUCCESS)
      return NULL;

   if (attrs.Width <= 0 || attrs.Height <= 0 ||
       attrs.DRMBufferStrideMESA <= 0) {
      _eglError(EGL_BAD_PARAMETER,
		"bad width, height or stride");
      return NULL;
   }

   switch (attrs.DRMBufferFormatMESA) {
   case EGL_DRM_BUFFER_FORMAT_ARGB32_MESA:
      format = __DRI_IMAGE_FORMAT_ARGB8888;
      pitch = attrs.DRMBufferStrideMESA;
      break;
   default:
      _eglError(EGL_BAD_PARAMETER,
		"fbdev_create_image_khr: unsupported pixmap depth");
      return NULL;
   }

   dri_image =
      fbdev_dpy->image->createImageFromName(fbdev_dpy->dri_screen,
					   attrs.Width,
					   attrs.Height,
					   format,
					   name,
					   pitch,
					   NULL);

   return fbdev_create_image(disp, dri_image);
}

/**
 * Set the error code after a call to
 * fbdev_egl_image::dri_image::createImageFromTexture.
 */
static void
fbdev_create_image_khr_texture_error(int dri_error)
{
   EGLint egl_error;

   switch (dri_error) {
   case __DRI_IMAGE_ERROR_SUCCESS:
      return;

   case __DRI_IMAGE_ERROR_BAD_ALLOC:
      egl_error = EGL_BAD_ALLOC;
      break;

   case __DRI_IMAGE_ERROR_BAD_MATCH:
      egl_error = EGL_BAD_MATCH;
      break;

   case __DRI_IMAGE_ERROR_BAD_PARAMETER:
      egl_error = EGL_BAD_PARAMETER;
      break;

   default:
      assert(0);
      egl_error = EGL_BAD_MATCH;
      break;
   }

   _eglError(egl_error, "fbdev_create_image_khr_texture");
}

static _EGLImage *
fbdev_create_image_khr_texture(_EGLDisplay *disp, _EGLContext *ctx,
				   EGLenum target,
				   EGLClientBuffer buffer,
				   const EGLint *attr_list)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_context *fbdev_ctx = fbdev_egl_context(ctx);
   struct fbdev_egl_image *fbdev_img;
   GLuint texture = (GLuint) (uintptr_t) buffer;
   _EGLImageAttribs attrs;
   GLuint depth;
   GLenum gl_target;
   unsigned error;

   if (texture == 0) {
      _eglError(EGL_BAD_PARAMETER, "fbdev_create_image_khr");
      return EGL_NO_IMAGE_KHR;
   }

   if (_eglParseImageAttribList(&attrs, disp, attr_list) != EGL_SUCCESS)
      return EGL_NO_IMAGE_KHR;

   switch (target) {
   case EGL_GL_TEXTURE_2D_KHR:
      depth = 0;
      gl_target = GL_TEXTURE_2D;
      break;
   case EGL_GL_TEXTURE_3D_KHR:
      depth = attrs.GLTextureZOffset;
      gl_target = GL_TEXTURE_3D;
      break;
   case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
      depth = target - EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR;
      gl_target = GL_TEXTURE_CUBE_MAP;
      break;
   default:
      _eglError(EGL_BAD_PARAMETER, "fbdev_create_image_khr");
      return EGL_NO_IMAGE_KHR;
   }

   fbdev_img = malloc(sizeof *fbdev_img);
   if (!fbdev_img) {
      _eglError(EGL_BAD_ALLOC, "fbdev_create_image_khr");
      return EGL_NO_IMAGE_KHR;
   }

   if (!_eglInitImage(&fbdev_img->base, disp)) {
      _eglError(EGL_BAD_ALLOC, "fbdev_create_image_khr");
      free(fbdev_img);
      return EGL_NO_IMAGE_KHR;
   }

   fbdev_img->dri_image =
      fbdev_dpy->image->createImageFromTexture(fbdev_ctx->dri_context,
                                              gl_target,
                                              texture,
                                              depth,
                                              attrs.GLTextureLevel,
                                              &error,
                                              fbdev_img);
   fbdev_create_image_khr_texture_error(error);

   if (!fbdev_img->dri_image) {
      free(fbdev_img);
      return EGL_NO_IMAGE_KHR;
   }
   return &fbdev_img->base;
}

_EGLImage *
fbdev_create_image_khr(_EGLDriver *drv, _EGLDisplay *disp,
		      _EGLContext *ctx, EGLenum target,
		      EGLClientBuffer buffer, const EGLint *attr_list)
{
   (void) drv;

   switch (target) {
   case EGL_GL_TEXTURE_2D_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR:
   case EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR:
      return fbdev_create_image_khr_texture(disp, ctx, target, buffer, attr_list);
   case EGL_GL_RENDERBUFFER_KHR:
      return fbdev_create_image_khr_renderbuffer(disp, ctx, buffer, attr_list);
   case EGL_DRM_BUFFER_MESA:
      return fbdev_create_image_mesa_drm_buffer(disp, ctx, buffer, attr_list);
   default:
      _eglError(EGL_BAD_PARAMETER, "fbdev_create_image_khr");
      return EGL_NO_IMAGE_KHR;
   }
}

static EGLBoolean
fbdev_destroy_image_khr(_EGLDriver *drv, _EGLDisplay *disp, _EGLImage *image)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_image *fbdev_img = fbdev_egl_image(image);

   (void) drv;

   fbdev_dpy->image->destroyImage(fbdev_img->dri_image);
   free(fbdev_img);

   return EGL_TRUE;
}

static _EGLImage *
fbdev_create_drm_image_mesa(_EGLDriver *drv, _EGLDisplay *disp,
			   const EGLint *attr_list)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_image *fbdev_img;
   _EGLImageAttribs attrs;
   unsigned int dri_use, valid_mask;
   int format;
   EGLint err = EGL_SUCCESS;

   (void) drv;

   fbdev_img = malloc(sizeof *fbdev_img);
   if (!fbdev_img) {
      _eglError(EGL_BAD_ALLOC, "fbdev_create_image_khr");
      return EGL_NO_IMAGE_KHR;
   }

   if (!attr_list) {
      err = EGL_BAD_PARAMETER;
      goto cleanup_img;
   }

   if (!_eglInitImage(&fbdev_img->base, disp)) {
      err = EGL_BAD_PARAMETER;
      goto cleanup_img;
   }

   err = _eglParseImageAttribList(&attrs, disp, attr_list);
   if (err != EGL_SUCCESS)
      goto cleanup_img;

   if (attrs.Width <= 0 || attrs.Height <= 0) {
      _eglLog(_EGL_WARNING, "bad width or height (%dx%d)",
            attrs.Width, attrs.Height);
      goto cleanup_img;
   }

   switch (attrs.DRMBufferFormatMESA) {
   case EGL_DRM_BUFFER_FORMAT_ARGB32_MESA:
      format = __DRI_IMAGE_FORMAT_ARGB8888;
      break;
   default:
      _eglLog(_EGL_WARNING, "bad image format value 0x%04x",
            attrs.DRMBufferFormatMESA);
      goto cleanup_img;
   }

   valid_mask =
      EGL_DRM_BUFFER_USE_SCANOUT_MESA |
      EGL_DRM_BUFFER_USE_SHARE_MESA |
      EGL_DRM_BUFFER_USE_CURSOR_MESA;
   if (attrs.DRMBufferUseMESA & ~valid_mask) {
      _eglLog(_EGL_WARNING, "bad image use bit 0x%04x",
            attrs.DRMBufferUseMESA & ~valid_mask);
      goto cleanup_img;
   }

   dri_use = 0;
   if (attrs.DRMBufferUseMESA & EGL_DRM_BUFFER_USE_SHARE_MESA)
      dri_use |= __DRI_IMAGE_USE_SHARE;
   if (attrs.DRMBufferUseMESA & EGL_DRM_BUFFER_USE_SCANOUT_MESA)
      dri_use |= __DRI_IMAGE_USE_SCANOUT;
   if (attrs.DRMBufferUseMESA & EGL_DRM_BUFFER_USE_CURSOR_MESA)
      dri_use |= __DRI_IMAGE_USE_CURSOR;

   fbdev_img->dri_image = 
      fbdev_dpy->image->createImage(fbdev_dpy->dri_screen,
				   attrs.Width, attrs.Height,
                                   format, dri_use, fbdev_img);
   if (fbdev_img->dri_image == NULL) {
      err = EGL_BAD_ALLOC;
      goto cleanup_img;
   }

   return &fbdev_img->base;

 cleanup_img:
   free(fbdev_img);
   _eglError(err, "fbdev_create_drm_image_mesa");

   return EGL_NO_IMAGE_KHR;
}

static EGLBoolean
fbdev_export_drm_image_mesa(_EGLDriver *drv, _EGLDisplay *disp, _EGLImage *img,
			  EGLint *name, EGLint *handle, EGLint *stride)
{
   struct fbdev_egl_display *fbdev_dpy = fbdev_egl_display(disp);
   struct fbdev_egl_image *fbdev_img = fbdev_egl_image(img);

   (void) drv;

   if (name && !fbdev_dpy->image->queryImage(fbdev_img->dri_image,
					    __DRI_IMAGE_ATTRIB_NAME, name)) {
      _eglError(EGL_BAD_ALLOC, "fbdev_export_drm_image_mesa");
      return EGL_FALSE;
   }

   if (handle)
      fbdev_dpy->image->queryImage(fbdev_img->dri_image,
				  __DRI_IMAGE_ATTRIB_HANDLE, handle);

   if (stride)
      fbdev_dpy->image->queryImage(fbdev_img->dri_image,
				  __DRI_IMAGE_ATTRIB_STRIDE, stride);

   return EGL_TRUE;
}

/**
 * This is the main entrypoint into the driver, called by libEGL.
 * Create a new _EGLDriver object and init its dispatch table.
 */
_EGLDriver *
_eglBuiltInDriverFBDEV(const char *args)
{
   struct fbdev_egl_driver *fbdev_drv;

   (void) args;

   fbdev_drv = calloc(1, sizeof *fbdev_drv);
   if (!fbdev_drv)
      return NULL;

   _eglInitDriverFallbacks(&fbdev_drv->base);
   fbdev_drv->base.API.Initialize = fbdev_initialize;
   fbdev_drv->base.API.Terminate = fbdev_terminate;
   fbdev_drv->base.API.CreateContext = fbdev_create_context;
   fbdev_drv->base.API.DestroyContext = fbdev_destroy_context;
   fbdev_drv->base.API.MakeCurrent = fbdev_make_current;
   fbdev_drv->base.API.GetProcAddress = fbdev_get_proc_address;
   fbdev_drv->base.API.WaitClient = fbdev_wait_client;
   fbdev_drv->base.API.WaitNative = fbdev_wait_native;
   fbdev_drv->base.API.BindTexImage = fbdev_bind_tex_image;
   fbdev_drv->base.API.ReleaseTexImage = fbdev_release_tex_image;
   fbdev_drv->base.API.CreateImageKHR = fbdev_create_image_khr;
   fbdev_drv->base.API.DestroyImageKHR = fbdev_destroy_image_khr;
   fbdev_drv->base.API.CreateDRMImageMESA = fbdev_create_drm_image_mesa;
   fbdev_drv->base.API.ExportDRMImageMESA = fbdev_export_drm_image_mesa;

   fbdev_drv->base.Name = "FBDEV";

   return &fbdev_drv->base;
}
