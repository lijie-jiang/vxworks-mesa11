#ifndef __gl2platform_h_
#define __gl2platform_h_

/* $Revision: 23328 $ on $Date:: 2013-10-02 02:28:28 -0700 #$ */

/*
 * This document is licensed under the SGI Free Software B License Version
 * 2.0. For details, see http://oss.sgi.com/projects/FreeB/ .
 */

/* Platform-specific types and definitions for OpenGL ES 2.X  gl2.h
 *
 * Adopters may modify khrplatform.h and this file to suit their platform.
 * You are encouraged to submit all modifications to the Khronos group so that
 * they can be included in future versions of this file.  Please submit changes
 * by sending them to the public Khronos Bugzilla (http://khronos.org/bugzilla)
 * by filing a bug against product "OpenGL-ES" component "Registry".
 */

/*
 * Copyright (c) 2014, 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
04jan16,yat  Update to latest (US72407)
22dec14,yat  Add Mesa GLES2 mangle (US24713)
*/

#include <KHR/khrplatform.h>

#if defined(_FSLVIV)
#ifndef _GL_2_APPENDIX
#define _GL_2_APPENDIX _fslviv_es2
#endif
#endif

#ifndef GL_APICALL
#define GL_APICALL  KHRONOS_APICALL
#endif

#ifndef GL_APIENTRY
#define GL_APIENTRY KHRONOS_APIENTRY
#endif

#if defined(_FSLVIV_GL2RENAME)
#include <GLES2/gl2rename_vivante.h>
#endif

#if defined(USE_MGL_NAMESPACE)
#include <GLES2/gl2_mangle.h>
#endif

#if defined(__vxworks)
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#endif /* __vxworks */

#endif /* __gl2platform_h_ */
