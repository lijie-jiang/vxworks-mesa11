/*
 * Copyright 2013 Intel Corporation
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Francisco Jerez <currojerez@riseup.net>
 */

#ifndef SHADERIMAGE_H
#define SHADERIMAGE_H

#include "glheader.h"
#include "formats.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gl_context;

/**
 * Get the matching mesa_format for a shader image format GL enum.
 */
mesa_format
_mesa_get_shader_image_format(GLenum format);

/**
 * Get a single image unit struct with the default state.
 */
struct gl_image_unit
_mesa_default_image_unit(struct gl_context *ctx);

/**
 * Initialize a context's shader image units to the default state.
 */
void
_mesa_init_image_units(struct gl_context *ctx);

/**
 * Return GL_TRUE if the state of the image unit passed as argument is valid
 * and access from the shader is allowed.  Otherwise loads from this unit
 * should return zero and stores should have no effect.
 *
 * The result depends on context state other than the passed image unit, part
 * of the _NEW_TEXTURE set.
 */
GLboolean
_mesa_is_image_unit_valid(struct gl_context *ctx, struct gl_image_unit *u);

void GLAPIENTRY
_mesa_BindImageTexture(GLuint unit, GLuint texture, GLint level,
                       GLboolean layered, GLint layer, GLenum access,
                       GLenum format);

void GLAPIENTRY
_mesa_BindImageTextures(GLuint first, GLsizei count, const GLuint *textures);

void GLAPIENTRY
_mesa_MemoryBarrier(GLbitfield barriers);

void GLAPIENTRY
_mesa_MemoryBarrierByRegion(GLbitfield barriers);

#ifdef __cplusplus
}
#endif

#endif
