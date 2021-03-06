
/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
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


#ifndef S_FEEDBACK_H
#define S_FEEDBACK_H


#include "swrast.h"


extern void _swrast_feedback_point( struct gl_context *ctx, const SWvertex *v );

extern void _swrast_feedback_line( struct gl_context *ctx,
                              const SWvertex *v1, const SWvertex *v2 );

extern void _swrast_feedback_triangle( struct gl_context *ctx, const SWvertex *v0,
                                  const SWvertex *v1, const SWvertex *v2 );

extern void _swrast_select_point( struct gl_context *ctx, const SWvertex *v );

extern void _swrast_select_line( struct gl_context *ctx,
                            const SWvertex *v1, const SWvertex *v2 );

extern void _swrast_select_triangle( struct gl_context *ctx, const SWvertex *v0,
                                const SWvertex *v1, const SWvertex *v2 );

#endif
