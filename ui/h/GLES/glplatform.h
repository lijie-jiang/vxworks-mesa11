#ifndef __glplatform_h_
#define __glplatform_h_

/* $Revision: 10601 $ on $Date:: 2010-03-04 22:15:27 -0800 #$ */

/*
 * This document is licensed under the SGI Free Software B License Version
 * 2.0. For details, see http://oss.sgi.com/projects/FreeB/ .
 */

/* Platform-specific types and definitions for OpenGL ES 1.X  gl.h
 *
 * Adopters may modify khrplatform.h and this file to suit their platform.
 * You are encouraged to submit all modifications to the Khronos group so that
 * they can be included in future versions of this file.  Please submit changes
 * by sending them to the public Khronos Bugzilla (http://khronos.org/bugzilla)
 * by filing a bug against product "OpenGL-ES" component "Registry".
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
22dec14,yat  Add Mesa GLES mangle (US24713)
*/

#include <KHR/khrplatform.h>

#if defined(_FSLVIV)
#ifndef _GL_11_APPENDIX
#define _GL_11_APPENDIX _fslviv_es11
#endif
#endif

#ifndef GL_API
#define GL_API      KHRONOS_APICALL
#endif

#ifndef GL_APIENTRY
#define GL_APIENTRY KHRONOS_APIENTRY
#endif

#if defined(_FSLVIV)
#include <GLES/glrename_vivante.h>
#endif

#if defined(USE_MGL_NAMESPACE)
#include <GLES/gl1_mangle.h>
#endif

#endif /* __glplatform_h_ */
