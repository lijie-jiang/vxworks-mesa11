
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


#ifndef API_VALIDATE_H
#define API_VALIDATE_H

#include <stdbool.h>
#include "glheader.h"

struct gl_buffer_object;
struct gl_context;
struct gl_transform_feedback_object;


extern bool
_mesa_is_valid_prim_mode(struct gl_context *ctx, GLenum mode);

extern GLboolean
_mesa_valid_prim_mode(struct gl_context *ctx, GLenum mode, const char *name);


extern GLboolean
_mesa_validate_DrawArrays(struct gl_context *ctx, GLenum mode, GLsizei count);

extern GLboolean
_mesa_validate_DrawElements(struct gl_context *ctx,
			    GLenum mode, GLsizei count, GLenum type,
			    const GLvoid *indices);

extern GLboolean
_mesa_validate_MultiDrawElements(struct gl_context *ctx,
                                 GLenum mode, const GLsizei *count,
                                 GLenum type, const GLvoid * const *indices,
                                 GLuint primcount);

extern GLboolean
_mesa_validate_DrawRangeElements(struct gl_context *ctx, GLenum mode,
				 GLuint start, GLuint end,
				 GLsizei count, GLenum type,
				 const GLvoid *indices);


extern GLboolean
_mesa_validate_DrawArraysInstanced(struct gl_context *ctx, GLenum mode, GLint first,
                                   GLsizei count, GLsizei primcount);

extern GLboolean
_mesa_validate_DrawElementsInstanced(struct gl_context *ctx,
                                     GLenum mode, GLsizei count, GLenum type,
                                     const GLvoid *indices, GLsizei primcount);

extern GLboolean
_mesa_validate_DrawTransformFeedback(struct gl_context *ctx,
                                     GLenum mode,
                                     struct gl_transform_feedback_object *obj,
                                     GLuint stream,
                                     GLsizei numInstances);

extern GLboolean
_mesa_validate_DrawArraysIndirect(struct gl_context *ctx,
                                  GLenum mode,
                                  const GLvoid *indirect);

extern GLboolean
_mesa_validate_DrawElementsIndirect(struct gl_context *ctx,
                                    GLenum mode,
                                    GLenum type,
                                    const GLvoid *indirect);

extern GLboolean
_mesa_validate_MultiDrawArraysIndirect(struct gl_context *ctx,
                                       GLenum mode,
                                       const GLvoid *indirect,
                                       GLsizei primcount,
                                       GLsizei stride);

extern GLboolean
_mesa_validate_MultiDrawElementsIndirect(struct gl_context *ctx,
                                         GLenum mode,
                                         GLenum type,
                                         const GLvoid *indirect,
                                         GLsizei primcount,
                                         GLsizei stride);


#endif
