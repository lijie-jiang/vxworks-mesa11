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

/**
 * \file program.c
 * Vertex and fragment program support functions.
 * \author Brian Paul
 */


#include "main/glheader.h"
#include "main/context.h"
#include "main/hash.h"
#include "main/macros.h"
#include "program.h"
#include "prog_cache.h"
#include "prog_parameter.h"
#include "prog_instruction.h"
#include "util/ralloc.h"


/**
 * A pointer to this dummy program is put into the hash table when
 * glGenPrograms is called.
 */
struct gl_program _mesa_DummyProgram;


/**
 * Init context's vertex/fragment program state
 */
void
_mesa_init_program(struct gl_context *ctx)
{
   /*
    * If this assertion fails, we need to increase the field
    * size for register indexes (see INST_INDEX_BITS).
    */
   assert(ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents / 4
          <= (1 << INST_INDEX_BITS));
   assert(ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents / 4
          <= (1 << INST_INDEX_BITS));

   assert(ctx->Const.Program[MESA_SHADER_VERTEX].MaxTemps <= (1 << INST_INDEX_BITS));
   assert(ctx->Const.Program[MESA_SHADER_VERTEX].MaxLocalParams <= (1 << INST_INDEX_BITS));
   assert(ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTemps <= (1 << INST_INDEX_BITS));
   assert(ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxLocalParams <= (1 << INST_INDEX_BITS));

   assert(ctx->Const.Program[MESA_SHADER_VERTEX].MaxUniformComponents <= 4 * MAX_UNIFORMS);
   assert(ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxUniformComponents <= 4 * MAX_UNIFORMS);

   assert(ctx->Const.Program[MESA_SHADER_VERTEX].MaxAddressOffset <= (1 << INST_INDEX_BITS));
   assert(ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxAddressOffset <= (1 << INST_INDEX_BITS));

   /* If this fails, increase prog_instruction::TexSrcUnit size */
   STATIC_ASSERT(MAX_TEXTURE_UNITS <= (1 << 5));

   /* If this fails, increase prog_instruction::TexSrcTarget size */
   STATIC_ASSERT(NUM_TEXTURE_TARGETS <= (1 << 4));

   ctx->Program.ErrorPos = -1;
   ctx->Program.ErrorString = strdup("");

   ctx->VertexProgram.Enabled = GL_FALSE;
   ctx->VertexProgram.PointSizeEnabled =
      (ctx->API == API_OPENGLES2) ? GL_TRUE : GL_FALSE;
   ctx->VertexProgram.TwoSideEnabled = GL_FALSE;
   _mesa_reference_vertprog(ctx, &ctx->VertexProgram.Current,
                            ctx->Shared->DefaultVertexProgram);
   assert(ctx->VertexProgram.Current);
   ctx->VertexProgram.Cache = _mesa_new_program_cache();

   ctx->FragmentProgram.Enabled = GL_FALSE;
   _mesa_reference_fragprog(ctx, &ctx->FragmentProgram.Current,
                            ctx->Shared->DefaultFragmentProgram);
   assert(ctx->FragmentProgram.Current);
   ctx->FragmentProgram.Cache = _mesa_new_program_cache();

   /* XXX probably move this stuff */
   ctx->ATIFragmentShader.Enabled = GL_FALSE;
   ctx->ATIFragmentShader.Current = ctx->Shared->DefaultFragmentShader;
   assert(ctx->ATIFragmentShader.Current);
   ctx->ATIFragmentShader.Current->RefCount++;
}


/**
 * Free a context's vertex/fragment program state
 */
void
_mesa_free_program_data(struct gl_context *ctx)
{
   _mesa_reference_vertprog(ctx, &ctx->VertexProgram.Current, NULL);
   _mesa_delete_program_cache(ctx, ctx->VertexProgram.Cache);
   _mesa_reference_fragprog(ctx, &ctx->FragmentProgram.Current, NULL);
   _mesa_delete_shader_cache(ctx, ctx->FragmentProgram.Cache);

   /* XXX probably move this stuff */
   if (ctx->ATIFragmentShader.Current) {
      ctx->ATIFragmentShader.Current->RefCount--;
      if (ctx->ATIFragmentShader.Current->RefCount <= 0) {
         free(ctx->ATIFragmentShader.Current);
      }
   }

   free((void *) ctx->Program.ErrorString);
}


/**
 * Update the default program objects in the given context to reference those
 * specified in the shared state and release those referencing the old
 * shared state.
 */
void
_mesa_update_default_objects_program(struct gl_context *ctx)
{
   _mesa_reference_vertprog(ctx, &ctx->VertexProgram.Current,
                            ctx->Shared->DefaultVertexProgram);
   assert(ctx->VertexProgram.Current);

   _mesa_reference_fragprog(ctx, &ctx->FragmentProgram.Current,
                            ctx->Shared->DefaultFragmentProgram);
   assert(ctx->FragmentProgram.Current);

   /* XXX probably move this stuff */
   if (ctx->ATIFragmentShader.Current) {
      ctx->ATIFragmentShader.Current->RefCount--;
      if (ctx->ATIFragmentShader.Current->RefCount <= 0) {
         free(ctx->ATIFragmentShader.Current);
      }
   }
   ctx->ATIFragmentShader.Current = (struct ati_fragment_shader *) ctx->Shared->DefaultFragmentShader;
   assert(ctx->ATIFragmentShader.Current);
   ctx->ATIFragmentShader.Current->RefCount++;
}


/**
 * Set the vertex/fragment program error state (position and error string).
 * This is generally called from within the parsers.
 */
void
_mesa_set_program_error(struct gl_context *ctx, GLint pos, const char *string)
{
   ctx->Program.ErrorPos = pos;
   free((void *) ctx->Program.ErrorString);
   if (!string)
      string = "";
   ctx->Program.ErrorString = strdup(string);
}


/**
 * Find the line number and column for 'pos' within 'string'.
 * Return a copy of the line which contains 'pos'.  Free the line with
 * free().
 * \param string  the program string
 * \param pos     the position within the string
 * \param line    returns the line number corresponding to 'pos'.
 * \param col     returns the column number corresponding to 'pos'.
 * \return copy of the line containing 'pos'.
 */
const GLubyte *
_mesa_find_line_column(const GLubyte *string, const GLubyte *pos,
                       GLint *line, GLint *col)
{
   const GLubyte *lineStart = string;
   const GLubyte *p = string;
   GLubyte *s;
   int len;

   *line = 1;

   while (p != pos) {
      if (*p == (GLubyte) '\n') {
         (*line)++;
         lineStart = p + 1;
      }
      p++;
   }

   *col = (pos - lineStart) + 1;

   /* return copy of this line */
   while (*p != 0 && *p != '\n')
      p++;
   len = p - lineStart;
   s = malloc(len + 1);
   memcpy(s, lineStart, len);
   s[len] = 0;

   return s;
}


/**
 * Initialize a new gl_program object.
 */
static void
init_program_struct(struct gl_program *prog, GLenum target, GLuint id)
{
   GLuint i;

   assert(prog);

   memset(prog, 0, sizeof(*prog));
   prog->Id = id;
   prog->Target = target;
   prog->RefCount = 1;
   prog->Format = GL_PROGRAM_FORMAT_ASCII_ARB;

   /* default mapping from samplers to texture units */
   for (i = 0; i < MAX_SAMPLERS; i++)
      prog->SamplerUnits[i] = i;
}


/**
 * Initialize a new fragment program object.
 */
struct gl_program *
_mesa_init_fragment_program(struct gl_context *ctx,
                            struct gl_fragment_program *prog,
                            GLenum target, GLuint id)
{
   if (prog) {
      init_program_struct(&prog->Base, target, id);
      return &prog->Base;
   }
   return NULL;
}


/**
 * Initialize a new vertex program object.
 */
struct gl_program *
_mesa_init_vertex_program(struct gl_context *ctx,
                          struct gl_vertex_program *prog,
                          GLenum target, GLuint id)
{
   if (prog) {
      init_program_struct(&prog->Base, target, id);
      return &prog->Base;
   }
   return NULL;
}


/**
 * Initialize a new compute program object.
 */
struct gl_program *
_mesa_init_compute_program(struct gl_context *ctx,
                           struct gl_compute_program *prog,
                           GLenum target, GLuint id)
{
   if (prog) {
      init_program_struct(&prog->Base, target, id);
      return &prog->Base;
   }
   return NULL;
}


/**
 * Initialize a new tessellation control program object.
 */
struct gl_program *
_mesa_init_tess_ctrl_program(struct gl_context *ctx,
                             struct gl_tess_ctrl_program *prog,
                             GLenum target, GLuint id)
{
   if (prog) {
      init_program_struct(&prog->Base, target, id);
      return &prog->Base;
   }
   return NULL;
}


/**
 * Initialize a new tessellation evaluation program object.
 */
struct gl_program *
_mesa_init_tess_eval_program(struct gl_context *ctx,
                             struct gl_tess_eval_program *prog,
                             GLenum target, GLuint id)
{
   if (prog) {
      init_program_struct(&prog->Base, target, id);
      return &prog->Base;
   }
   return NULL;
}


/**
 * Initialize a new geometry program object.
 */
struct gl_program *
_mesa_init_geometry_program(struct gl_context *ctx,
                            struct gl_geometry_program *prog,
                            GLenum target, GLuint id)
{
   if (prog) {
      init_program_struct(&prog->Base, target, id);
      return &prog->Base;
   }
   return NULL;
}


/**
 * Allocate and initialize a new fragment/vertex program object but
 * don't put it into the program hash table.  Called via
 * ctx->Driver.NewProgram.  May be overridden (ie. replaced) by a
 * device driver function to implement OO deriviation with additional
 * types not understood by this function.
 *
 * \param ctx  context
 * \param id   program id/number
 * \param target  program target/type
 * \return  pointer to new program object
 */
struct gl_program *
_mesa_new_program(struct gl_context *ctx, GLenum target, GLuint id)
{
   struct gl_program *prog;
   switch (target) {
   case GL_VERTEX_PROGRAM_ARB: /* == GL_VERTEX_PROGRAM_NV */
      prog = _mesa_init_vertex_program(ctx, CALLOC_STRUCT(gl_vertex_program),
                                       target, id );
      break;
   case GL_FRAGMENT_PROGRAM_NV:
   case GL_FRAGMENT_PROGRAM_ARB:
      prog =_mesa_init_fragment_program(ctx,
                                         CALLOC_STRUCT(gl_fragment_program),
                                         target, id );
      break;
   case GL_GEOMETRY_PROGRAM_NV:
      prog = _mesa_init_geometry_program(ctx,
                                         CALLOC_STRUCT(gl_geometry_program),
                                         target, id);
      break;
   case GL_TESS_CONTROL_PROGRAM_NV:
      prog = _mesa_init_tess_ctrl_program(ctx,
                                          CALLOC_STRUCT(gl_tess_ctrl_program),
                                          target, id);
      break;
   case GL_TESS_EVALUATION_PROGRAM_NV:
      prog = _mesa_init_tess_eval_program(ctx,
                                         CALLOC_STRUCT(gl_tess_eval_program),
                                         target, id);
      break;
   case GL_COMPUTE_PROGRAM_NV:
      prog = _mesa_init_compute_program(ctx,
                                        CALLOC_STRUCT(gl_compute_program),
                                        target, id);
      break;
   default:
      _mesa_problem(ctx, "bad target in _mesa_new_program");
      prog = NULL;
   }
   return prog;
}


/**
 * Delete a program and remove it from the hash table, ignoring the
 * reference count.
 * Called via ctx->Driver.DeleteProgram.  May be wrapped (OO deriviation)
 * by a device driver function.
 */
void
_mesa_delete_program(struct gl_context *ctx, struct gl_program *prog)
{
   (void) ctx;
   assert(prog);
   assert(prog->RefCount==0);

   if (prog == &_mesa_DummyProgram)
      return;

   free(prog->String);
   free(prog->LocalParams);

   if (prog->Instructions) {
      _mesa_free_instructions(prog->Instructions, prog->NumInstructions);
   }
   if (prog->Parameters) {
      _mesa_free_parameter_list(prog->Parameters);
   }

   if (prog->nir) {
      ralloc_free(prog->nir);
   }

   free(prog);
}


/**
 * Return the gl_program object for a given ID.
 * Basically just a wrapper for _mesa_HashLookup() to avoid a lot of
 * casts elsewhere.
 */
struct gl_program *
_mesa_lookup_program(struct gl_context *ctx, GLuint id)
{
   if (id)
      return (struct gl_program *) _mesa_HashLookup(ctx->Shared->Programs, id);
   else
      return NULL;
}


/**
 * Reference counting for vertex/fragment programs
 * This is normally only called from the _mesa_reference_program() macro
 * when there's a real pointer change.
 */
void
_mesa_reference_program_(struct gl_context *ctx,
                         struct gl_program **ptr,
                         struct gl_program *prog)
{
#ifndef NDEBUG
   assert(ptr);
   if (*ptr && prog) {
      /* sanity check */
      if ((*ptr)->Target == GL_VERTEX_PROGRAM_ARB)
         assert(prog->Target == GL_VERTEX_PROGRAM_ARB);
      else if ((*ptr)->Target == GL_FRAGMENT_PROGRAM_ARB)
         assert(prog->Target == GL_FRAGMENT_PROGRAM_ARB ||
                prog->Target == GL_FRAGMENT_PROGRAM_NV);
      else if ((*ptr)->Target == GL_GEOMETRY_PROGRAM_NV)
         assert(prog->Target == GL_GEOMETRY_PROGRAM_NV);
   }
#endif

   if (*ptr) {
      GLboolean deleteFlag;

      /*mtx_lock(&(*ptr)->Mutex);*/
#if 0
      printf("Program %p ID=%u Target=%s  Refcount-- to %d\n",
             *ptr, (*ptr)->Id,
             ((*ptr)->Target == GL_VERTEX_PROGRAM_ARB ? "VP" :
              ((*ptr)->Target == GL_GEOMETRY_PROGRAM_NV ? "GP" : "FP")),
             (*ptr)->RefCount - 1);
#endif
      assert((*ptr)->RefCount > 0);
      (*ptr)->RefCount--;

      deleteFlag = ((*ptr)->RefCount == 0);
      /*mtx_lock(&(*ptr)->Mutex);*/

      if (deleteFlag) {
         assert(ctx);
         ctx->Driver.DeleteProgram(ctx, *ptr);
      }

      *ptr = NULL;
   }

   assert(!*ptr);
   if (prog) {
      /*mtx_lock(&prog->Mutex);*/
      prog->RefCount++;
#if 0
      printf("Program %p ID=%u Target=%s  Refcount++ to %d\n",
             prog, prog->Id,
             (prog->Target == GL_VERTEX_PROGRAM_ARB ? "VP" :
              (prog->Target == GL_GEOMETRY_PROGRAM_NV ? "GP" : "FP")),
             prog->RefCount);
#endif
      /*mtx_unlock(&prog->Mutex);*/
   }

   *ptr = prog;
}


/**
 * Return a copy of a program.
 * XXX Problem here if the program object is actually OO-derivation
 * made by a device driver.
 */
struct gl_program *
_mesa_clone_program(struct gl_context *ctx, const struct gl_program *prog)
{
   struct gl_program *clone;

   clone = ctx->Driver.NewProgram(ctx, prog->Target, prog->Id);
   if (!clone)
      return NULL;

   assert(clone->Target == prog->Target);
   assert(clone->RefCount == 1);

   clone->String = (GLubyte *) strdup((char *) prog->String);
   clone->Format = prog->Format;
   clone->Instructions = _mesa_alloc_instructions(prog->NumInstructions);
   if (!clone->Instructions) {
      _mesa_reference_program(ctx, &clone, NULL);
      return NULL;
   }
   _mesa_copy_instructions(clone->Instructions, prog->Instructions,
                           prog->NumInstructions);
   clone->InputsRead = prog->InputsRead;
   clone->OutputsWritten = prog->OutputsWritten;
   clone->SamplersUsed = prog->SamplersUsed;
   clone->ShadowSamplers = prog->ShadowSamplers;
   memcpy(clone->TexturesUsed, prog->TexturesUsed, sizeof(prog->TexturesUsed));

   if (prog->Parameters)
      clone->Parameters = _mesa_clone_parameter_list(prog->Parameters);
   if (prog->LocalParams) {
      clone->LocalParams = malloc(MAX_PROGRAM_LOCAL_PARAMS *
                                  sizeof(float[4]));
      if (!clone->LocalParams) {
         _mesa_reference_program(ctx, &clone, NULL);
         return NULL;
      }
      memcpy(clone->LocalParams, prog->LocalParams,
             MAX_PROGRAM_LOCAL_PARAMS * sizeof(float[4]));
   }
   clone->IndirectRegisterFiles = prog->IndirectRegisterFiles;
   clone->NumInstructions = prog->NumInstructions;
   clone->NumTemporaries = prog->NumTemporaries;
   clone->NumParameters = prog->NumParameters;
   clone->NumAttributes = prog->NumAttributes;
   clone->NumAddressRegs = prog->NumAddressRegs;
   clone->NumNativeInstructions = prog->NumNativeInstructions;
   clone->NumNativeTemporaries = prog->NumNativeTemporaries;
   clone->NumNativeParameters = prog->NumNativeParameters;
   clone->NumNativeAttributes = prog->NumNativeAttributes;
   clone->NumNativeAddressRegs = prog->NumNativeAddressRegs;
   clone->NumAluInstructions = prog->NumAluInstructions;
   clone->NumTexInstructions = prog->NumTexInstructions;
   clone->NumTexIndirections = prog->NumTexIndirections;
   clone->NumNativeAluInstructions = prog->NumNativeAluInstructions;
   clone->NumNativeTexInstructions = prog->NumNativeTexInstructions;
   clone->NumNativeTexIndirections = prog->NumNativeTexIndirections;

   switch (prog->Target) {
   case GL_VERTEX_PROGRAM_ARB:
      {
         const struct gl_vertex_program *vp = gl_vertex_program_const(prog);
         struct gl_vertex_program *vpc = gl_vertex_program(clone);
         vpc->IsPositionInvariant = vp->IsPositionInvariant;
      }
      break;
   case GL_FRAGMENT_PROGRAM_ARB:
      {
         const struct gl_fragment_program *fp = gl_fragment_program_const(prog);
         struct gl_fragment_program *fpc = gl_fragment_program(clone);
         fpc->UsesKill = fp->UsesKill;
         fpc->UsesDFdy = fp->UsesDFdy;
         fpc->OriginUpperLeft = fp->OriginUpperLeft;
         fpc->PixelCenterInteger = fp->PixelCenterInteger;
      }
      break;
   case GL_GEOMETRY_PROGRAM_NV:
      {
         const struct gl_geometry_program *gp = gl_geometry_program_const(prog);
         struct gl_geometry_program *gpc = gl_geometry_program(clone);
         gpc->VerticesOut = gp->VerticesOut;
         gpc->InputType = gp->InputType;
         gpc->Invocations = gp->Invocations;
         gpc->OutputType = gp->OutputType;
         gpc->UsesEndPrimitive = gp->UsesEndPrimitive;
         gpc->UsesStreams = gp->UsesStreams;
      }
      break;
   case GL_TESS_CONTROL_PROGRAM_NV:
      {
         const struct gl_tess_ctrl_program *tcp = gl_tess_ctrl_program_const(prog);
         struct gl_tess_ctrl_program *tcpc = gl_tess_ctrl_program(clone);
         tcpc->VerticesOut = tcp->VerticesOut;
      }
      break;
   case GL_TESS_EVALUATION_PROGRAM_NV:
      {
         const struct gl_tess_eval_program *tep = gl_tess_eval_program_const(prog);
         struct gl_tess_eval_program *tepc = gl_tess_eval_program(clone);
         tepc->PrimitiveMode = tep->PrimitiveMode;
         tepc->Spacing = tep->Spacing;
         tepc->VertexOrder = tep->VertexOrder;
         tepc->PointMode = tep->PointMode;
      }
      break;
   default:
      _mesa_problem(NULL, "Unexpected target in _mesa_clone_program");
   }

   return clone;
}


/**
 * Insert 'count' NOP instructions at 'start' in the given program.
 * Adjust branch targets accordingly.
 */
GLboolean
_mesa_insert_instructions(struct gl_program *prog, GLuint start, GLuint count)
{
   const GLuint origLen = prog->NumInstructions;
   const GLuint newLen = origLen + count;
   struct prog_instruction *newInst;
   GLuint i;

   /* adjust branches */
   for (i = 0; i < prog->NumInstructions; i++) {
      struct prog_instruction *inst = prog->Instructions + i;
      if (inst->BranchTarget > 0) {
         if ((GLuint)inst->BranchTarget >= start) {
            inst->BranchTarget += count;
         }
      }
   }

   /* Alloc storage for new instructions */
   newInst = _mesa_alloc_instructions(newLen);
   if (!newInst) {
      return GL_FALSE;
   }

   /* Copy 'start' instructions into new instruction buffer */
   _mesa_copy_instructions(newInst, prog->Instructions, start);

   /* init the new instructions */
   _mesa_init_instructions(newInst + start, count);

   /* Copy the remaining/tail instructions to new inst buffer */
   _mesa_copy_instructions(newInst + start + count,
                           prog->Instructions + start,
                           origLen - start);

   /* free old instructions */
   _mesa_free_instructions(prog->Instructions, origLen);

   /* install new instructions */
   prog->Instructions = newInst;
   prog->NumInstructions = newLen;

   return GL_TRUE;
}

/**
 * Delete 'count' instructions at 'start' in the given program.
 * Adjust branch targets accordingly.
 */
GLboolean
_mesa_delete_instructions(struct gl_program *prog, GLuint start, GLuint count)
{
   const GLuint origLen = prog->NumInstructions;
   const GLuint newLen = origLen - count;
   struct prog_instruction *newInst;
   GLuint i;

   /* adjust branches */
   for (i = 0; i < prog->NumInstructions; i++) {
      struct prog_instruction *inst = prog->Instructions + i;
      if (inst->BranchTarget > 0) {
         if (inst->BranchTarget > (GLint) start) {
            inst->BranchTarget -= count;
         }
      }
   }

   /* Alloc storage for new instructions */
   newInst = _mesa_alloc_instructions(newLen);
   if (!newInst) {
      return GL_FALSE;
   }

   /* Copy 'start' instructions into new instruction buffer */
   _mesa_copy_instructions(newInst, prog->Instructions, start);

   /* Copy the remaining/tail instructions to new inst buffer */
   _mesa_copy_instructions(newInst + start,
                           prog->Instructions + start + count,
                           newLen - start);

   /* free old instructions */
   _mesa_free_instructions(prog->Instructions, origLen);

   /* install new instructions */
   prog->Instructions = newInst;
   prog->NumInstructions = newLen;

   return GL_TRUE;
}


/**
 * Search instructions for registers that match (oldFile, oldIndex),
 * replacing them with (newFile, newIndex).
 */
static void
replace_registers(struct prog_instruction *inst, GLuint numInst,
                  GLuint oldFile, GLuint oldIndex,
                  GLuint newFile, GLuint newIndex)
{
   GLuint i, j;
   for (i = 0; i < numInst; i++) {
      /* src regs */
      for (j = 0; j < _mesa_num_inst_src_regs(inst[i].Opcode); j++) {
         if (inst[i].SrcReg[j].File == oldFile &&
             inst[i].SrcReg[j].Index == oldIndex) {
            inst[i].SrcReg[j].File = newFile;
            inst[i].SrcReg[j].Index = newIndex;
         }
      }
      /* dst reg */
      if (inst[i].DstReg.File == oldFile && inst[i].DstReg.Index == oldIndex) {
         inst[i].DstReg.File = newFile;
         inst[i].DstReg.Index = newIndex;
      }
   }
}


/**
 * Search instructions for references to program parameters.  When found,
 * increment the parameter index by 'offset'.
 * Used when combining programs.
 */
static void
adjust_param_indexes(struct prog_instruction *inst, GLuint numInst,
                     GLuint offset)
{
   GLuint i, j;
   for (i = 0; i < numInst; i++) {
      for (j = 0; j < _mesa_num_inst_src_regs(inst[i].Opcode); j++) {
         GLuint f = inst[i].SrcReg[j].File;
         if (f == PROGRAM_CONSTANT ||
             f == PROGRAM_UNIFORM ||
             f == PROGRAM_STATE_VAR) {
            inst[i].SrcReg[j].Index += offset;
         }
      }
   }
}


/**
 * Combine two programs into one.  Fix instructions so the outputs of
 * the first program go to the inputs of the second program.
 */
struct gl_program *
_mesa_combine_programs(struct gl_context *ctx,
                       const struct gl_program *progA,
                       const struct gl_program *progB)
{
   struct prog_instruction *newInst;
   struct gl_program *newProg;
   const GLuint lenA = progA->NumInstructions - 1; /* omit END instr */
   const GLuint lenB = progB->NumInstructions;
   const GLuint numParamsA = _mesa_num_parameters(progA->Parameters);
   const GLuint newLength = lenA + lenB;
   GLboolean usedTemps[MAX_PROGRAM_TEMPS];
   GLuint firstTemp = 0;
   GLbitfield64 inputsB;
   GLuint i;

   assert(progA->Target == progB->Target);

   newInst = _mesa_alloc_instructions(newLength);
   if (!newInst)
      return GL_FALSE;

   _mesa_copy_instructions(newInst, progA->Instructions, lenA);
   _mesa_copy_instructions(newInst + lenA, progB->Instructions, lenB);

   /* adjust branch / instruction addresses for B's instructions */
   for (i = 0; i < lenB; i++) {
      newInst[lenA + i].BranchTarget += lenA;
   }

   newProg = ctx->Driver.NewProgram(ctx, progA->Target, 0);
   newProg->Instructions = newInst;
   newProg->NumInstructions = newLength;

   /* find used temp regs (we may need new temps below) */
   _mesa_find_used_registers(newProg, PROGRAM_TEMPORARY,
                             usedTemps, MAX_PROGRAM_TEMPS);

   if (newProg->Target == GL_FRAGMENT_PROGRAM_ARB) {
      const struct gl_fragment_program *fprogA, *fprogB;
      struct gl_fragment_program *newFprog;
      GLbitfield64 progB_inputsRead = progB->InputsRead;
      GLint progB_colorFile, progB_colorIndex;

      fprogA = gl_fragment_program_const(progA);
      fprogB = gl_fragment_program_const(progB);
      newFprog = gl_fragment_program(newProg);

      newFprog->UsesKill = fprogA->UsesKill || fprogB->UsesKill;
      newFprog->UsesDFdy = fprogA->UsesDFdy || fprogB->UsesDFdy;

      /* We'll do a search and replace for instances
       * of progB_colorFile/progB_colorIndex below...
       */
      progB_colorFile = PROGRAM_INPUT;
      progB_colorIndex = VARYING_SLOT_COL0;

      /*
       * The fragment program may get color from a state var rather than
       * a fragment input (vertex output) if it's constant.
       * See the texenvprogram.c code.
       * So, search the program's parameter list now to see if the program
       * gets color from a state var instead of a conventional fragment
       * input register.
       */
      for (i = 0; i < progB->Parameters->NumParameters; i++) {
         struct gl_program_parameter *p = &progB->Parameters->Parameters[i];
         if (p->Type == PROGRAM_STATE_VAR &&
             p->StateIndexes[0] == STATE_INTERNAL &&
             p->StateIndexes[1] == STATE_CURRENT_ATTRIB &&
             (int) p->StateIndexes[2] == (int) VERT_ATTRIB_COLOR0) {
            progB_inputsRead |= VARYING_BIT_COL0;
            progB_colorFile = PROGRAM_STATE_VAR;
            progB_colorIndex = i;
            break;
         }
      }

      /* Connect color outputs of fprogA to color inputs of fprogB, via a
       * new temporary register.
       */
      if ((progA->OutputsWritten & BITFIELD64_BIT(FRAG_RESULT_COLOR)) &&
          (progB_inputsRead & VARYING_BIT_COL0)) {
         GLint tempReg = _mesa_find_free_register(usedTemps, MAX_PROGRAM_TEMPS,
                                                  firstTemp);
         if (tempReg < 0) {
            _mesa_problem(ctx, "No free temp regs found in "
                          "_mesa_combine_programs(), using 31");
            tempReg = 31;
         }
         firstTemp = tempReg + 1;

         /* replace writes to result.color[0] with tempReg */
         replace_registers(newInst, lenA,
                           PROGRAM_OUTPUT, FRAG_RESULT_COLOR,
                           PROGRAM_TEMPORARY, tempReg);
         /* replace reads from the input color with tempReg */
         replace_registers(newInst + lenA, lenB,
                           progB_colorFile, progB_colorIndex, /* search for */
                           PROGRAM_TEMPORARY, tempReg  /* replace with */ );
      }

      /* compute combined program's InputsRead */
      inputsB = progB_inputsRead;
      if (progA->OutputsWritten & BITFIELD64_BIT(FRAG_RESULT_COLOR)) {
         inputsB &= ~(1 << VARYING_SLOT_COL0);
      }
      newProg->InputsRead = progA->InputsRead | inputsB;
      newProg->OutputsWritten = progB->OutputsWritten;
      newProg->SamplersUsed = progA->SamplersUsed | progB->SamplersUsed;
   }
   else {
      /* vertex program */
      assert(0);      /* XXX todo */
   }

   /*
    * Merge parameters (uniforms, constants, etc)
    */
   newProg->Parameters = _mesa_combine_parameter_lists(progA->Parameters,
                                                       progB->Parameters);

   adjust_param_indexes(newInst + lenA, lenB, numParamsA);


   return newProg;
}


/**
 * Populate the 'used' array with flags indicating which registers (TEMPs,
 * INPUTs, OUTPUTs, etc, are used by the given program.
 * \param file  type of register to scan for
 * \param used  returns true/false flags for in use / free
 * \param usedSize  size of the 'used' array
 */
void
_mesa_find_used_registers(const struct gl_program *prog,
                          gl_register_file file,
                          GLboolean used[], GLuint usedSize)
{
   GLuint i, j;

   memset(used, 0, usedSize);

   for (i = 0; i < prog->NumInstructions; i++) {
      const struct prog_instruction *inst = prog->Instructions + i;
      const GLuint n = _mesa_num_inst_src_regs(inst->Opcode);

      if (inst->DstReg.File == file) {
         assert(inst->DstReg.Index < usedSize);
         if(inst->DstReg.Index < usedSize)
            used[inst->DstReg.Index] = GL_TRUE;
      }

      for (j = 0; j < n; j++) {
         if (inst->SrcReg[j].File == file) {
            assert(inst->SrcReg[j].Index < (GLint) usedSize);
            if (inst->SrcReg[j].Index < (GLint) usedSize)
               used[inst->SrcReg[j].Index] = GL_TRUE;
         }
      }
   }
}


/**
 * Scan the given 'used' register flag array for the first entry
 * that's >= firstReg.
 * \param used  vector of flags indicating registers in use (as returned
 *              by _mesa_find_used_registers())
 * \param usedSize  size of the 'used' array
 * \param firstReg  first register to start searching at
 * \return index of unused register, or -1 if none.
 */
GLint
_mesa_find_free_register(const GLboolean used[],
                         GLuint usedSize, GLuint firstReg)
{
   GLuint i;

   assert(firstReg < usedSize);

   for (i = firstReg; i < usedSize; i++)
      if (!used[i])
         return i;

   return -1;
}



/**
 * Check if the given register index is valid (doesn't exceed implementation-
 * dependent limits).
 * \return GL_TRUE if OK, GL_FALSE if bad index
 */
GLboolean
_mesa_valid_register_index(const struct gl_context *ctx,
                           gl_shader_stage shaderType,
                           gl_register_file file, GLint index)
{
   const struct gl_program_constants *c;

   assert(0 <= shaderType && shaderType < MESA_SHADER_STAGES);
   c = &ctx->Const.Program[shaderType];

   switch (file) {
   case PROGRAM_UNDEFINED:
      return GL_TRUE;  /* XXX or maybe false? */

   case PROGRAM_TEMPORARY:
      return index >= 0 && index < (GLint) c->MaxTemps;

   case PROGRAM_UNIFORM:
   case PROGRAM_STATE_VAR:
      /* aka constant buffer */
      return index >= 0 && index < (GLint) c->MaxUniformComponents / 4;

   case PROGRAM_CONSTANT:
      /* constant buffer w/ possible relative negative addressing */
      return (index > (int) c->MaxUniformComponents / -4 &&
              index < (int) c->MaxUniformComponents / 4);

   case PROGRAM_INPUT:
      if (index < 0)
         return GL_FALSE;

      switch (shaderType) {
      case MESA_SHADER_VERTEX:
         return index < VERT_ATTRIB_GENERIC0 + (GLint) c->MaxAttribs;
      case MESA_SHADER_FRAGMENT:
         return index < VARYING_SLOT_VAR0 + (GLint) ctx->Const.MaxVarying;
      case MESA_SHADER_GEOMETRY:
         return index < VARYING_SLOT_VAR0 + (GLint) ctx->Const.MaxVarying;
      default:
         return GL_FALSE;
      }

   case PROGRAM_OUTPUT:
      if (index < 0)
         return GL_FALSE;

      switch (shaderType) {
      case MESA_SHADER_VERTEX:
         return index < VARYING_SLOT_VAR0 + (GLint) ctx->Const.MaxVarying;
      case MESA_SHADER_FRAGMENT:
         return index < FRAG_RESULT_DATA0 + (GLint) ctx->Const.MaxDrawBuffers;
      case MESA_SHADER_GEOMETRY:
         return index < VARYING_SLOT_VAR0 + (GLint) ctx->Const.MaxVarying;
      default:
         return GL_FALSE;
      }

   case PROGRAM_ADDRESS:
      return index >= 0 && index < (GLint) c->MaxAddressRegs;

   default:
      _mesa_problem(ctx,
                    "unexpected register file in _mesa_valid_register_index()");
      return GL_FALSE;
   }
}



/**
 * "Post-process" a GPU program.  This is intended to be used for debugging.
 * Example actions include no-op'ing instructions or changing instruction
 * behaviour.
 */
void
_mesa_postprocess_program(struct gl_context *ctx, struct gl_program *prog)
{
   static const GLfloat white[4] = { 0.5, 0.5, 0.5, 0.5 };
   GLuint i;
   GLuint whiteSwizzle;
   GLint whiteIndex = _mesa_add_unnamed_constant(prog->Parameters,
                                                 (gl_constant_value *) white,
                                                 4, &whiteSwizzle);

   (void) whiteIndex;

   for (i = 0; i < prog->NumInstructions; i++) {
      struct prog_instruction *inst = prog->Instructions + i;
      const GLuint n = _mesa_num_inst_src_regs(inst->Opcode);

      (void) n;

      if (_mesa_is_tex_instruction(inst->Opcode)) {
#if 0
         /* replace TEX/TXP/TXB with MOV */
         inst->Opcode = OPCODE_MOV;
         inst->DstReg.WriteMask = WRITEMASK_XYZW;
         inst->SrcReg[0].Swizzle = SWIZZLE_XYZW;
         inst->SrcReg[0].Negate = NEGATE_NONE;
#endif

#if 0
         /* disable shadow texture mode */
         inst->TexShadow = 0;
#endif
      }

      if (inst->Opcode == OPCODE_TXP) {
#if 0
         inst->Opcode = OPCODE_MOV;
         inst->DstReg.WriteMask = WRITEMASK_XYZW;
         inst->SrcReg[0].File = PROGRAM_CONSTANT;
         inst->SrcReg[0].Index = whiteIndex;
         inst->SrcReg[0].Swizzle = SWIZZLE_XYZW;
         inst->SrcReg[0].Negate = NEGATE_NONE;
#endif
#if 0
         inst->TexShadow = 0;
#endif
#if 0
         inst->Opcode = OPCODE_TEX;
         inst->TexShadow = 0;
#endif
      }

   }
}

/* Gets the minimum number of shader invocations per fragment.
 * This function is useful to determine if we need to do per
 * sample shading or per fragment shading.
 */
GLint
_mesa_get_min_invocations_per_fragment(struct gl_context *ctx,
                                       const struct gl_fragment_program *prog,
                                       bool ignore_sample_qualifier)
{
   /* From ARB_sample_shading specification:
    * "Using gl_SampleID in a fragment shader causes the entire shader
    *  to be evaluated per-sample."
    *
    * "Using gl_SamplePosition in a fragment shader causes the entire
    *  shader to be evaluated per-sample."
    *
    * "If MULTISAMPLE or SAMPLE_SHADING_ARB is disabled, sample shading
    *  has no effect."
    */
   if (ctx->Multisample.Enabled) {
      /* The ARB_gpu_shader5 specification says:
       *
       * "Use of the "sample" qualifier on a fragment shader input
       *  forces per-sample shading"
       */
      if (prog->IsSample && !ignore_sample_qualifier)
         return MAX2(ctx->DrawBuffer->Visual.samples, 1);

      if (prog->Base.SystemValuesRead & (SYSTEM_BIT_SAMPLE_ID |
                                         SYSTEM_BIT_SAMPLE_POS))
         return MAX2(ctx->DrawBuffer->Visual.samples, 1);
      else if (ctx->Multisample.SampleShading)
         return MAX2(ceil(ctx->Multisample.MinSampleShadingValue *
                          ctx->DrawBuffer->Visual.samples), 1);
      else
         return 1;
   }
   return 1;
}
