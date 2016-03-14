/* egl_mangle.h - Mesa EGL mangle header file */

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
22dec14,yat  Add Mesa EGL mangle (US24713)
*/

#ifndef __egl_mangle_h_
#define __egl_mangle_h_

#define MANGLE_EGL(api)                 m##api
#define eglBindAPI                      MANGLE_EGL(eglBindAPI)
#define eglBindTexImage                 MANGLE_EGL(eglBindTexImage)
#define eglChooseConfig                 MANGLE_EGL(eglChooseConfig)
#define eglCopyBuffers                  MANGLE_EGL(eglCopyBuffers)
#define eglCreateContext                MANGLE_EGL(eglCreateContext)
#define eglCreatePbufferFromClientBuffer MANGLE_EGL(eglCreatePbufferFromClientBuffer)
#define eglCreatePbufferSurface         MANGLE_EGL(eglCreatePbufferSurface)
#define eglCreatePixmapSurface          MANGLE_EGL(eglCreatePixmapSurface)
#define eglCreateWindowSurface          MANGLE_EGL(eglCreateWindowSurface)
#define eglDestroyContext               MANGLE_EGL(eglDestroyContext)
#define eglDestroySurface               MANGLE_EGL(eglDestroySurface)
#define eglGetConfigAttrib              MANGLE_EGL(eglGetConfigAttrib)
#define eglGetConfigs                   MANGLE_EGL(eglGetConfigs)
#define eglGetCurrentContext            MANGLE_EGL(eglGetCurrentContext)
#define eglGetCurrentDisplay            MANGLE_EGL(eglGetCurrentDisplay)
#define eglGetCurrentSurface            MANGLE_EGL(eglGetCurrentSurface)
#define eglGetDisplay                   MANGLE_EGL(eglGetDisplay)
#define eglGetError                     MANGLE_EGL(eglGetError)
#define eglGetProcAddress               MANGLE_EGL(eglGetProcAddress)
#define eglInitialize                   MANGLE_EGL(eglInitialize)
#define eglMakeCurrent                  MANGLE_EGL(eglMakeCurrent)
#define eglQueryAPI                     MANGLE_EGL(eglQueryAPI)
#define eglQueryContext                 MANGLE_EGL(eglQueryContext)
#define eglQueryString                  MANGLE_EGL(eglQueryString)
#define eglQuerySurface                 MANGLE_EGL(eglQuerySurface)
#define eglReleaseTexImage              MANGLE_EGL(eglReleaseTexImage)
#define eglReleaseThread                MANGLE_EGL(eglReleaseThread)
#define eglSurfaceAttrib                MANGLE_EGL(eglSurfaceAttrib)
#define eglSwapBuffers                  MANGLE_EGL(eglSwapBuffers)
#define eglSwapInterval                 MANGLE_EGL(eglSwapInterval)
#define eglTerminate                    MANGLE_EGL(eglTerminate)
#define eglWaitClient                   MANGLE_EGL(eglWaitClient)
#define eglWaitGL                       MANGLE_EGL(eglWaitGL)
#define eglWaitNative                   MANGLE_EGL(eglWaitNative)

#endif /* __egl_mangle_h_ */
