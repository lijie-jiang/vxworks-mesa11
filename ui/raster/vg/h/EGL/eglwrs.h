/* eglwrs.h - Wind River EGL extersions */

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

#ifndef __eglwrs_h
#define __eglwrs_h

/* includes */

#include <EGL/egl.h>

/* EGL Extensions */
EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurfaceWRS (EGLDisplay dpy,
                                                          EGLConfig config,
                                                          const EGLint *attrib_list,
                                                          EGLBoolean lock);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurfaceWRS (EGLDisplay dpy,
                                                    EGLSurface surface,
                                                    EGLBoolean force);

#endif /* #ifndef __eglwrs_h */
