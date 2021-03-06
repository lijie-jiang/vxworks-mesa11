/*
 * Copyright © 2014 Intel Corporation
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
 */

#include "glheader.h"
#include "compute.h"
#include "context.h"

void GLAPIENTRY
_mesa_DispatchCompute(GLuint num_groups_x,
                      GLuint num_groups_y,
                      GLuint num_groups_z)
{
   GET_CURRENT_CONTEXT(ctx);
   int i;
   struct gl_shader_program *prog;
   const GLuint num_groups[3] = { num_groups_x, num_groups_y, num_groups_z };

   if (ctx->Extensions.ARB_compute_shader) {
      for (i = 0; i < 3; i++) {
         if (num_groups[i] > ctx->Const.MaxComputeWorkGroupCount[i]) {
            _mesa_error(ctx, GL_INVALID_VALUE,
                        "glDispatchCompute(num_groups_%c)", 'x' + i);
            return;
         }
      }
      if (!_mesa_valid_to_render(ctx, "glDispatchCompute"))
         return;
      prog = ctx->Shader.CurrentProgram[MESA_SHADER_COMPUTE];
      if (prog == NULL || prog->_LinkedShaders[MESA_SHADER_COMPUTE] == NULL) {
         _mesa_error(ctx, GL_INVALID_OPERATION,
                     "glDispatchCompute(no active compute shader)");
         return;
      }
      ctx->Driver.DispatchCompute(ctx, num_groups);
   } else {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "unsupported function (glDispatchCompute) called");
   }
}

extern void GLAPIENTRY
_mesa_DispatchComputeIndirect(GLintptr indirect)
{
   GET_CURRENT_CONTEXT(ctx);

   if (ctx->Extensions.ARB_compute_shader) {
      assert(!"TODO");
   } else {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "unsupported function (glDispatchComputeIndirect) called");
   }
}
