/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2008  Brian Paul   All Rights Reserved.
 * Copyright (C) 2009  VMware, Inc.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
22feb16,yat  Port Mesa 11.0 to VxWorks 7 (US70717)
*/


#ifndef VERSION_H
#define VERSION_H

#include "mtypes.h"


extern GLuint
_mesa_get_version(const struct gl_extensions *extensions,
                  struct gl_constants *consts, gl_api api);

extern void
_mesa_compute_version(struct gl_context *ctx);

extern bool
_mesa_override_gl_version_contextless(struct gl_constants *consts,
                                      gl_api *apiOut, GLuint *versionOut);

extern void
_mesa_override_gl_version(struct gl_context *ctx);

extern void
_mesa_override_glsl_version(struct gl_constants *consts);

#if defined(__vxworks)
#define PACKAGE_VERSION "11.0.6.0"
#define PACKAGE_BUGREPORT ""
#endif /* __vxworks */

#endif /* VERSION_H */
