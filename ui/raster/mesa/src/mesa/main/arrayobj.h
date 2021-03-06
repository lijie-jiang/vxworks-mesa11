/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2004  Brian Paul   All Rights Reserved.
 * (C) Copyright IBM Corporation 2006
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

#ifndef ARRAYOBJ_H
#define ARRAYOBJ_H

#include "glheader.h"
#include "mtypes.h"
#include "glformats.h"

struct gl_context;

/**
 * \file arrayobj.h
 * Functions for the GL_APPLE_vertex_array_object extension.
 *
 * \author Ian Romanick <idr@us.ibm.com>
 * \author Brian Paul
 */

/*
 * Internal functions
 */

extern struct gl_vertex_array_object *
_mesa_lookup_vao(struct gl_context *ctx, GLuint id);

extern struct gl_vertex_array_object *
_mesa_lookup_vao_err(struct gl_context *ctx, GLuint id, const char *caller);

extern struct gl_vertex_array_object *
_mesa_new_vao(struct gl_context *ctx, GLuint name);

extern void
_mesa_delete_vao(struct gl_context *ctx, struct gl_vertex_array_object *obj);

extern void
_mesa_reference_vao_(struct gl_context *ctx,
                     struct gl_vertex_array_object **ptr,
                     struct gl_vertex_array_object *vao);

static inline void
_mesa_reference_vao(struct gl_context *ctx,
                    struct gl_vertex_array_object **ptr,
                    struct gl_vertex_array_object *vao)
{
   if (*ptr != vao)
      _mesa_reference_vao_(ctx, ptr, vao);
}


extern void
_mesa_initialize_vao(struct gl_context *ctx,
                     struct gl_vertex_array_object *obj, GLuint name);


extern void
_mesa_update_vao_client_arrays(struct gl_context *ctx,
                               struct gl_vertex_array_object *vao);

/*
 * API functions
 */


void GLAPIENTRY _mesa_BindVertexArray( GLuint id );

void GLAPIENTRY _mesa_BindVertexArrayAPPLE( GLuint id );

void GLAPIENTRY _mesa_DeleteVertexArrays(GLsizei n, const GLuint *ids);

void GLAPIENTRY _mesa_GenVertexArrays(GLsizei n, GLuint *arrays);

void GLAPIENTRY _mesa_GenVertexArraysAPPLE(GLsizei n, GLuint *buffer);

void GLAPIENTRY _mesa_CreateVertexArrays(GLsizei n, GLuint *arrays);

GLboolean GLAPIENTRY _mesa_IsVertexArray( GLuint id );

void GLAPIENTRY _mesa_VertexArrayElementBuffer(GLuint vaobj, GLuint buffer);

void GLAPIENTRY _mesa_GetVertexArrayiv(GLuint vaobj, GLenum pname, GLint *param);

#endif /* ARRAYOBJ_H */
