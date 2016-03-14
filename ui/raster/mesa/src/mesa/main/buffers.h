/**
 * \file buffers.h
 * Frame buffer management functions declarations.
 */

/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
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



#ifndef BUFFERS_H
#define BUFFERS_H


#include "glheader.h"

struct gl_context;
struct gl_framebuffer;

extern void
_mesa_draw_buffer(struct gl_context *ctx, struct gl_framebuffer *fb,
                  GLenum buffer, const char *caller);

extern void GLAPIENTRY
_mesa_DrawBuffer( GLenum mode );

extern void GLAPIENTRY
_mesa_NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf);

extern void
_mesa_draw_buffers(struct gl_context *ctx, struct gl_framebuffer *fb,
                   GLsizei n, const GLenum *buffers, const char *caller);

extern void GLAPIENTRY
_mesa_DrawBuffers(GLsizei n, const GLenum *buffers);

extern void GLAPIENTRY
_mesa_NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n,
                                  const GLenum *bufs);

extern void
_mesa_drawbuffers(struct gl_context *ctx, struct gl_framebuffer *fb,
                  GLuint n, const GLenum *buffers,
                  const GLbitfield *destMask);

extern void
_mesa_readbuffer(struct gl_context *ctx, struct gl_framebuffer *fb,
                 GLenum buffer, GLint bufferIndex);

extern void
_mesa_update_draw_buffers(struct gl_context *ctx);


extern void
_mesa_read_buffer(struct gl_context *ctx, struct gl_framebuffer *fb,
                  GLenum buffer, const char *caller);

extern void GLAPIENTRY
_mesa_ReadBuffer( GLenum mode );

extern void GLAPIENTRY
_mesa_NamedFramebufferReadBuffer(GLuint framebuffer, GLenum src);


#endif
