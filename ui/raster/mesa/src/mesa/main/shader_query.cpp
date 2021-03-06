/*
 * Copyright © 2011 Intel Corporation
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

/**
 * \file shader_query.cpp
 * C-to-C++ bridge functions to query GLSL shader data
 *
 * \author Ian Romanick <ian.d.romanick@intel.com>
 */

#include "main/context.h"
#include "main/core.h"
#include "glsl_symbol_table.h"
#include "ir.h"
#include "shaderobj.h"
#include "program/hash_table.h"
#include "../glsl/program.h"
#include "uniforms.h"
#include "main/enums.h"

extern "C" {
#include "shaderapi.h"
}

static GLint
program_resource_location(struct gl_shader_program *shProg,
                          struct gl_program_resource *res, const char *name,
                          unsigned array_index);

/**
 * Declare convenience functions to return resource data in a given type.
 * Warning! this is not type safe so be *very* careful when using these.
 */
#define DECL_RESOURCE_FUNC(name, type) \
const type * RESOURCE_ ## name (gl_program_resource *res) { \
   assert(res->Data); \
   return (type *) res->Data; \
}

DECL_RESOURCE_FUNC(VAR, ir_variable);
DECL_RESOURCE_FUNC(UBO, gl_uniform_block);
DECL_RESOURCE_FUNC(UNI, gl_uniform_storage);
DECL_RESOURCE_FUNC(ATC, gl_active_atomic_buffer);
DECL_RESOURCE_FUNC(XFB, gl_transform_feedback_varying_info);
DECL_RESOURCE_FUNC(SUB, gl_subroutine_function);

void GLAPIENTRY
_mesa_BindAttribLocation(GLhandleARB program, GLuint index,
                            const GLcharARB *name)
{
   GET_CURRENT_CONTEXT(ctx);

   struct gl_shader_program *const shProg =
      _mesa_lookup_shader_program_err(ctx, program, "glBindAttribLocation");
   if (!shProg)
      return;

   if (!name)
      return;

   if (strncmp(name, "gl_", 3) == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glBindAttribLocation(illegal name)");
      return;
   }

   if (index >= ctx->Const.Program[MESA_SHADER_VERTEX].MaxAttribs) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glBindAttribLocation(index)");
      return;
   }

   /* Replace the current value if it's already in the list.  Add
    * VERT_ATTRIB_GENERIC0 because that's how the linker differentiates
    * between built-in attributes and user-defined attributes.
    */
   shProg->AttributeBindings->put(index + VERT_ATTRIB_GENERIC0, name);

   /*
    * Note that this attribute binding won't go into effect until
    * glLinkProgram is called again.
    */
}

static bool
is_active_attrib(const ir_variable *var)
{
   if (!var)
      return false;

   switch (var->data.mode) {
   case ir_var_shader_in:
      return var->data.location != -1;

   case ir_var_system_value:
      /* From GL 4.3 core spec, section 11.1.1 (Vertex Attributes):
       * "For GetActiveAttrib, all active vertex shader input variables
       * are enumerated, including the special built-in inputs gl_VertexID
       * and gl_InstanceID."
       */
      return var->data.location == SYSTEM_VALUE_VERTEX_ID ||
             var->data.location == SYSTEM_VALUE_VERTEX_ID_ZERO_BASE ||
             var->data.location == SYSTEM_VALUE_INSTANCE_ID;

   default:
      return false;
   }
}

void GLAPIENTRY
_mesa_GetActiveAttrib(GLhandleARB program, GLuint desired_index,
                         GLsizei maxLength, GLsizei * length, GLint * size,
                         GLenum * type, GLcharARB * name)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_shader_program *shProg;

   if (maxLength < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetActiveAttrib(maxLength < 0)");
      return;
   }

   shProg = _mesa_lookup_shader_program_err(ctx, program, "glGetActiveAttrib");
   if (!shProg)
      return;

   if (!shProg->LinkStatus) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "glGetActiveAttrib(program not linked)");
      return;
   }

   if (shProg->_LinkedShaders[MESA_SHADER_VERTEX] == NULL) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetActiveAttrib(no vertex shader)");
      return;
   }

   struct gl_program_resource *res =
      _mesa_program_resource_find_index(shProg, GL_PROGRAM_INPUT,
                                        desired_index);

   /* User asked for index that does not exist. */
   if (!res) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetActiveAttrib(index)");
      return;
   }

   const ir_variable *const var = RESOURCE_VAR(res);

   if (!is_active_attrib(var))
      return;

   const char *var_name = var->name;

   /* Since gl_VertexID may be lowered to gl_VertexIDMESA, we need to
    * consider gl_VertexIDMESA as gl_VertexID for purposes of checking
    * active attributes.
    */
   if (var->data.mode == ir_var_system_value &&
       var->data.location == SYSTEM_VALUE_VERTEX_ID_ZERO_BASE) {
      var_name = "gl_VertexID";
   }

   _mesa_copy_string(name, maxLength, length, var_name);

   if (size)
      _mesa_program_resource_prop(shProg, res, desired_index, GL_ARRAY_SIZE,
                                  size, "glGetActiveAttrib");

   if (type)
      _mesa_program_resource_prop(shProg, res, desired_index, GL_TYPE,
                                  (GLint *) type, "glGetActiveAttrib");
}

GLint GLAPIENTRY
_mesa_GetAttribLocation(GLhandleARB program, const GLcharARB * name)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_shader_program *const shProg =
      _mesa_lookup_shader_program_err(ctx, program, "glGetAttribLocation");

   if (!shProg) {
      return -1;
   }

   if (!shProg->LinkStatus) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glGetAttribLocation(program not linked)");
      return -1;
   }

   if (!name)
      return -1;

   /* Not having a vertex shader is not an error.
    */
   if (shProg->_LinkedShaders[MESA_SHADER_VERTEX] == NULL)
      return -1;

   unsigned array_index = 0;
   struct gl_program_resource *res =
      _mesa_program_resource_find_name(shProg, GL_PROGRAM_INPUT, name,
                                       &array_index);

   if (!res)
      return -1;

   GLint loc = program_resource_location(shProg, res, name, array_index);

   /* The extra check against against 0 is made because of builtin-attribute
    * locations that have offset applied. Function program_resource_location
    * can return built-in attribute locations < 0 and glGetAttribLocation
    * cannot be used on "conventional" attributes.
    *
    * From page 95 of the OpenGL 3.0 spec:
    *
    *     "If name is not an active attribute, if name is a conventional
    *     attribute, or if an error occurs, -1 will be returned."
    */
   return (loc >= 0) ? loc : -1;
}

unsigned
_mesa_count_active_attribs(struct gl_shader_program *shProg)
{
   if (!shProg->LinkStatus
       || shProg->_LinkedShaders[MESA_SHADER_VERTEX] == NULL) {
      return 0;
   }

   struct gl_program_resource *res = shProg->ProgramResourceList;
   unsigned count = 0;
   for (unsigned j = 0; j < shProg->NumProgramResourceList; j++, res++) {
      if (res->Type == GL_PROGRAM_INPUT &&
          res->StageReferences & (1 << MESA_SHADER_VERTEX) &&
          is_active_attrib(RESOURCE_VAR(res)))
         count++;
   }
   return count;
}


size_t
_mesa_longest_attribute_name_length(struct gl_shader_program *shProg)
{
   if (!shProg->LinkStatus
       || shProg->_LinkedShaders[MESA_SHADER_VERTEX] == NULL) {
      return 0;
   }

   struct gl_program_resource *res = shProg->ProgramResourceList;
   size_t longest = 0;
   for (unsigned j = 0; j < shProg->NumProgramResourceList; j++, res++) {
      if (res->Type == GL_PROGRAM_INPUT &&
          res->StageReferences & (1 << MESA_SHADER_VERTEX)) {

          const size_t length = strlen(RESOURCE_VAR(res)->name);
          if (length >= longest)
             longest = length + 1;
      }
   }

   return longest;
}

void GLAPIENTRY
_mesa_BindFragDataLocation(GLuint program, GLuint colorNumber,
			   const GLchar *name)
{
   _mesa_BindFragDataLocationIndexed(program, colorNumber, 0, name);
}

void GLAPIENTRY
_mesa_BindFragDataLocationIndexed(GLuint program, GLuint colorNumber,
                                  GLuint index, const GLchar *name)
{
   GET_CURRENT_CONTEXT(ctx);

   struct gl_shader_program *const shProg =
      _mesa_lookup_shader_program_err(ctx, program, "glBindFragDataLocationIndexed");
   if (!shProg)
      return;

   if (!name)
      return;

   if (strncmp(name, "gl_", 3) == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "glBindFragDataLocationIndexed(illegal name)");
      return;
   }

   if (index > 1) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glBindFragDataLocationIndexed(index)");
      return;
   }

   if (index == 0 && colorNumber >= ctx->Const.MaxDrawBuffers) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glBindFragDataLocationIndexed(colorNumber)");
      return;
   }

   if (index == 1 && colorNumber >= ctx->Const.MaxDualSourceDrawBuffers) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glBindFragDataLocationIndexed(colorNumber)");
      return;
   }

   /* Replace the current value if it's already in the list.  Add
    * FRAG_RESULT_DATA0 because that's how the linker differentiates
    * between built-in attributes and user-defined attributes.
    */
   shProg->FragDataBindings->put(colorNumber + FRAG_RESULT_DATA0, name);
   shProg->FragDataIndexBindings->put(index, name);
   /*
    * Note that this binding won't go into effect until
    * glLinkProgram is called again.
    */

}

GLint GLAPIENTRY
_mesa_GetFragDataIndex(GLuint program, const GLchar *name)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_shader_program *const shProg =
      _mesa_lookup_shader_program_err(ctx, program, "glGetFragDataIndex");

   if (!shProg) {
      return -1;
   }

   if (!shProg->LinkStatus) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glGetFragDataIndex(program not linked)");
      return -1;
   }

   if (!name)
      return -1;

   if (strncmp(name, "gl_", 3) == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glGetFragDataIndex(illegal name)");
      return -1;
   }

   /* Not having a fragment shader is not an error.
    */
   if (shProg->_LinkedShaders[MESA_SHADER_FRAGMENT] == NULL)
      return -1;

   return _mesa_program_resource_location_index(shProg, GL_PROGRAM_OUTPUT,
                                                name);
}

GLint GLAPIENTRY
_mesa_GetFragDataLocation(GLuint program, const GLchar *name)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_shader_program *const shProg =
      _mesa_lookup_shader_program_err(ctx, program, "glGetFragDataLocation");

   if (!shProg) {
      return -1;
   }

   if (!shProg->LinkStatus) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glGetFragDataLocation(program not linked)");
      return -1;
   }

   if (!name)
      return -1;

   if (strncmp(name, "gl_", 3) == 0) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glGetFragDataLocation(illegal name)");
      return -1;
   }

   /* Not having a fragment shader is not an error.
    */
   if (shProg->_LinkedShaders[MESA_SHADER_FRAGMENT] == NULL)
      return -1;

   unsigned array_index = 0;
   struct gl_program_resource *res =
      _mesa_program_resource_find_name(shProg, GL_PROGRAM_OUTPUT, name,
                                       &array_index);

   if (!res)
      return -1;

   GLint loc = program_resource_location(shProg, res, name, array_index);

   /* The extra check against against 0 is made because of builtin-attribute
    * locations that have offset applied. Function program_resource_location
    * can return built-in attribute locations < 0 and glGetFragDataLocation
    * cannot be used on "conventional" attributes.
    *
    * From page 95 of the OpenGL 3.0 spec:
    *
    *     "If name is not an active attribute, if name is a conventional
    *     attribute, or if an error occurs, -1 will be returned."
    */
   return (loc >= 0) ? loc : -1;
}

const char*
_mesa_program_resource_name(struct gl_program_resource *res)
{
   const ir_variable *var;
   switch (res->Type) {
   case GL_UNIFORM_BLOCK:
      return RESOURCE_UBO(res)->Name;
   case GL_TRANSFORM_FEEDBACK_VARYING:
      return RESOURCE_XFB(res)->Name;
   case GL_PROGRAM_INPUT:
      var = RESOURCE_VAR(res);
      /* Special case gl_VertexIDMESA -> gl_VertexID. */
      if (var->data.mode == ir_var_system_value &&
          var->data.location == SYSTEM_VALUE_VERTEX_ID_ZERO_BASE) {
         return "gl_VertexID";
      }
   /* fallthrough */
   case GL_PROGRAM_OUTPUT:
      return RESOURCE_VAR(res)->name;
   case GL_UNIFORM:
      return RESOURCE_UNI(res)->name;
   case GL_VERTEX_SUBROUTINE_UNIFORM:
   case GL_GEOMETRY_SUBROUTINE_UNIFORM:
   case GL_FRAGMENT_SUBROUTINE_UNIFORM:
   case GL_COMPUTE_SUBROUTINE_UNIFORM:
   case GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
   case GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
      return RESOURCE_UNI(res)->name + MESA_SUBROUTINE_PREFIX_LEN;
   case GL_VERTEX_SUBROUTINE:
   case GL_GEOMETRY_SUBROUTINE:
   case GL_FRAGMENT_SUBROUTINE:
   case GL_COMPUTE_SUBROUTINE:
   case GL_TESS_CONTROL_SUBROUTINE:
   case GL_TESS_EVALUATION_SUBROUTINE:
      return RESOURCE_SUB(res)->name;
   default:
      assert(!"support for resource type not implemented");
   }
   return NULL;
}


unsigned
_mesa_program_resource_array_size(struct gl_program_resource *res)
{
   switch (res->Type) {
   case GL_TRANSFORM_FEEDBACK_VARYING:
      return RESOURCE_XFB(res)->Size > 1 ?
             RESOURCE_XFB(res)->Size : 0;
   case GL_PROGRAM_INPUT:
   case GL_PROGRAM_OUTPUT:
      return RESOURCE_VAR(res)->type->length;
   case GL_UNIFORM:
   case GL_VERTEX_SUBROUTINE_UNIFORM:
   case GL_GEOMETRY_SUBROUTINE_UNIFORM:
   case GL_FRAGMENT_SUBROUTINE_UNIFORM:
   case GL_COMPUTE_SUBROUTINE_UNIFORM:
   case GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
   case GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
      return RESOURCE_UNI(res)->array_elements;
   case GL_VERTEX_SUBROUTINE:
   case GL_GEOMETRY_SUBROUTINE:
   case GL_FRAGMENT_SUBROUTINE:
   case GL_COMPUTE_SUBROUTINE:
   case GL_TESS_CONTROL_SUBROUTINE:
   case GL_TESS_EVALUATION_SUBROUTINE:
   case GL_ATOMIC_COUNTER_BUFFER:
   case GL_UNIFORM_BLOCK:
      return 0;
   default:
      assert(!"support for resource type not implemented");
   }
   return 0;
}

/**
 * Checks if array subscript is valid and if so sets array_index.
 */
static bool
valid_array_index(const GLchar *name, unsigned *array_index)
{
   long idx = 0;
   const GLchar *out_base_name_end;

   idx = parse_program_resource_name(name, &out_base_name_end);
   if (idx < 0)
      return false;

   if (array_index)
      *array_index = idx;

   return true;
}

/* Find a program resource with specific name in given interface.
 */
struct gl_program_resource *
_mesa_program_resource_find_name(struct gl_shader_program *shProg,
                                 GLenum programInterface, const char *name,
                                 unsigned *array_index)
{
   struct gl_program_resource *res = shProg->ProgramResourceList;
   for (unsigned i = 0; i < shProg->NumProgramResourceList; i++, res++) {
      if (res->Type != programInterface)
         continue;

      /* Resource basename. */
      const char *rname = _mesa_program_resource_name(res);
      unsigned baselen = strlen(rname);

      if (strncmp(rname, name, baselen) == 0) {
         switch (programInterface) {
         case GL_UNIFORM_BLOCK:
            /* Basename match, check if array or struct. */
            if (name[baselen] == '\0' ||
                name[baselen] == '[' ||
                name[baselen] == '.') {
               return res;
            }
            break;
         case GL_TRANSFORM_FEEDBACK_VARYING:
         case GL_UNIFORM:
         case GL_VERTEX_SUBROUTINE_UNIFORM:
         case GL_GEOMETRY_SUBROUTINE_UNIFORM:
         case GL_FRAGMENT_SUBROUTINE_UNIFORM:
         case GL_COMPUTE_SUBROUTINE_UNIFORM:
         case GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
         case GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
         case GL_VERTEX_SUBROUTINE:
         case GL_GEOMETRY_SUBROUTINE:
         case GL_FRAGMENT_SUBROUTINE:
         case GL_COMPUTE_SUBROUTINE:
         case GL_TESS_CONTROL_SUBROUTINE:
         case GL_TESS_EVALUATION_SUBROUTINE:
            if (name[baselen] == '.') {
               return res;
            }
            /* fall-through */
         case GL_PROGRAM_INPUT:
         case GL_PROGRAM_OUTPUT:
            if (name[baselen] == '\0') {
               return res;
            } else if (name[baselen] == '[' &&
                valid_array_index(name, array_index)) {
               return res;
            }
            break;
         default:
            assert(!"not implemented for given interface");
         }
      }
   }
   return NULL;
}

static GLuint
calc_resource_index(struct gl_shader_program *shProg,
                    struct gl_program_resource *res)
{
   unsigned i;
   GLuint index = 0;
   for (i = 0; i < shProg->NumProgramResourceList; i++) {
      if (&shProg->ProgramResourceList[i] == res)
         return index;
      if (shProg->ProgramResourceList[i].Type == res->Type)
         index++;
   }
   return GL_INVALID_INDEX;
}

/**
 * Calculate index for the given resource.
 */
GLuint
_mesa_program_resource_index(struct gl_shader_program *shProg,
                             struct gl_program_resource *res)
{
   if (!res)
      return GL_INVALID_INDEX;

   switch (res->Type) {
   case GL_UNIFORM_BLOCK:
      return RESOURCE_UBO(res)- shProg->UniformBlocks;
   case GL_ATOMIC_COUNTER_BUFFER:
      return RESOURCE_ATC(res) - shProg->AtomicBuffers;
   case GL_TRANSFORM_FEEDBACK_VARYING:
   default:
      return calc_resource_index(shProg, res);
   }
}

/* Find a program resource with specific index in given interface.
 */
struct gl_program_resource *
_mesa_program_resource_find_index(struct gl_shader_program *shProg,
                                  GLenum programInterface, GLuint index)
{
   struct gl_program_resource *res = shProg->ProgramResourceList;
   int idx = -1;

   for (unsigned i = 0; i < shProg->NumProgramResourceList; i++, res++) {
      if (res->Type != programInterface)
         continue;

      switch (res->Type) {
      case GL_UNIFORM_BLOCK:
      case GL_ATOMIC_COUNTER_BUFFER:
         if (_mesa_program_resource_index(shProg, res) == index)
            return res;
         break;
      case GL_TRANSFORM_FEEDBACK_VARYING:
      case GL_PROGRAM_INPUT:
      case GL_PROGRAM_OUTPUT:
      case GL_UNIFORM:
      case GL_VERTEX_SUBROUTINE_UNIFORM:
      case GL_GEOMETRY_SUBROUTINE_UNIFORM:
      case GL_FRAGMENT_SUBROUTINE_UNIFORM:
      case GL_COMPUTE_SUBROUTINE_UNIFORM:
      case GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
      case GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
      case GL_VERTEX_SUBROUTINE:
      case GL_GEOMETRY_SUBROUTINE:
      case GL_FRAGMENT_SUBROUTINE:
      case GL_COMPUTE_SUBROUTINE:
      case GL_TESS_CONTROL_SUBROUTINE:
      case GL_TESS_EVALUATION_SUBROUTINE:
         if (++idx == (int) index)
            return res;
         break;
      default:
         assert(!"not implemented for given interface");
      }
   }
   return NULL;
}

/* Function returns if resource name is expected to have index
 * appended into it.
 *
 *
 * Page 61 (page 73 of the PDF) in section 2.11 of the OpenGL ES 3.0
 * spec says:
 *
 *     "If the active uniform is an array, the uniform name returned in
 *     name will always be the name of the uniform array appended with
 *     "[0]"."
 *
 * The same text also appears in the OpenGL 4.2 spec.  It does not,
 * however, appear in any previous spec.  Previous specifications are
 * ambiguous in this regard.  However, either name can later be passed
 * to glGetUniformLocation (and related APIs), so there shouldn't be any
 * harm in always appending "[0]" to uniform array names.
 *
 * Geometry shader stage has different naming convention where the 'normal'
 * condition is an array, therefore for variables referenced in geometry
 * stage we do not add '[0]'.
 *
 * Note, that TCS outputs and TES inputs should not have index appended
 * either.
 */
static bool
add_index_to_name(struct gl_program_resource *res)
{
   bool add_index = !(((res->Type == GL_PROGRAM_INPUT) &&
                       res->StageReferences & (1 << MESA_SHADER_GEOMETRY)));

   /* Transform feedback varyings have array index already appended
    * in their names.
    */
   if (res->Type == GL_TRANSFORM_FEEDBACK_VARYING)
      add_index = false;

   return add_index;
}

/* Get name length of a program resource. This consists of
 * base name + 3 for '[0]' if resource is an array.
 */
extern unsigned
_mesa_program_resource_name_len(struct gl_program_resource *res)
{
   unsigned length = strlen(_mesa_program_resource_name(res));
   if (_mesa_program_resource_array_size(res) && add_index_to_name(res))
      length += 3;
   return length;
}

/* Get full name of a program resource.
 */
bool
_mesa_get_program_resource_name(struct gl_shader_program *shProg,
                                GLenum programInterface, GLuint index,
                                GLsizei bufSize, GLsizei *length,
                                GLchar *name, const char *caller)
{
   GET_CURRENT_CONTEXT(ctx);

   /* Find resource with given interface and index. */
   struct gl_program_resource *res =
      _mesa_program_resource_find_index(shProg, programInterface, index);

   /* The error INVALID_VALUE is generated if <index> is greater than
   * or equal to the number of entries in the active resource list for
   * <programInterface>.
   */
   if (!res) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(index %u)", caller, index);
      return false;
   }

   if (bufSize < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "%s(bufSize %d)", caller, bufSize);
      return false;
   }

   GLsizei localLength;

   if (length == NULL)
      length = &localLength;

   _mesa_copy_string(name, bufSize, length, _mesa_program_resource_name(res));

   if (_mesa_program_resource_array_size(res) && add_index_to_name(res)) {
      int i;

      /* The comparison is strange because *length does *NOT* include the
       * terminating NUL, but maxLength does.
       */
      for (i = 0; i < 3 && (*length + i + 1) < bufSize; i++)
         name[*length + i] = "[0]"[i];

      name[*length + i] = '\0';
      *length += i;
   }
   return true;
}

static GLint
program_resource_location(struct gl_shader_program *shProg,
                          struct gl_program_resource *res, const char *name,
                          unsigned array_index)
{
   /* Built-in locations should report GL_INVALID_INDEX. */
   if (is_gl_identifier(name))
      return GL_INVALID_INDEX;

   /* VERT_ATTRIB_GENERIC0 and FRAG_RESULT_DATA0 are decremented as these
    * offsets are used internally to differentiate between built-in attributes
    * and user-defined attributes.
    */
   switch (res->Type) {
   case GL_PROGRAM_INPUT:
      /* If the input is an array, fail if the index is out of bounds. */
      if (array_index > 0
          && array_index >= RESOURCE_VAR(res)->type->length) {
         return -1;
      }
      return RESOURCE_VAR(res)->data.location + array_index - VERT_ATTRIB_GENERIC0;
   case GL_PROGRAM_OUTPUT:
      /* If the output is an array, fail if the index is out of bounds. */
      if (array_index > 0
          && array_index >= RESOURCE_VAR(res)->type->length) {
         return -1;
      }
      return RESOURCE_VAR(res)->data.location + array_index - FRAG_RESULT_DATA0;
   case GL_UNIFORM:
      /* If the uniform is built-in, fail. */
      if (RESOURCE_UNI(res)->builtin)
         return -1;

      /* From the GL_ARB_uniform_buffer_object spec:
       *
       *     "The value -1 will be returned if <name> does not correspond to an
       *     active uniform variable name in <program>, if <name> is associated
       *     with a named uniform block, or if <name> starts with the reserved
       *     prefix "gl_"."
       */
      if (RESOURCE_UNI(res)->block_index != -1 ||
          RESOURCE_UNI(res)->atomic_buffer_index != -1)
         return -1;

      /* fallthrough */
   case GL_VERTEX_SUBROUTINE_UNIFORM:
   case GL_GEOMETRY_SUBROUTINE_UNIFORM:
   case GL_FRAGMENT_SUBROUTINE_UNIFORM:
   case GL_COMPUTE_SUBROUTINE_UNIFORM:
   case GL_TESS_CONTROL_SUBROUTINE_UNIFORM:
   case GL_TESS_EVALUATION_SUBROUTINE_UNIFORM:
      /* If the uniform is an array, fail if the index is out of bounds. */
      if (array_index > 0
          && array_index >= RESOURCE_UNI(res)->array_elements) {
         return -1;
      }

      /* location in remap table + array element offset */
      return RESOURCE_UNI(res)->remap_location + array_index;
   default:
      return -1;
   }
}

/**
 * Function implements following location queries:
 *    glGetUniformLocation
 */
GLint
_mesa_program_resource_location(struct gl_shader_program *shProg,
                                GLenum programInterface, const char *name)
{
   unsigned array_index = 0;
   struct gl_program_resource *res =
      _mesa_program_resource_find_name(shProg, programInterface, name,
                                       &array_index);

   /* Resource not found. */
   if (!res)
      return -1;

   return program_resource_location(shProg, res, name, array_index);
}

/**
 * Function implements following index queries:
 *    glGetFragDataIndex
 */
GLint
_mesa_program_resource_location_index(struct gl_shader_program *shProg,
                                      GLenum programInterface, const char *name)
{
   struct gl_program_resource *res =
      _mesa_program_resource_find_name(shProg, programInterface, name, NULL);

   /* Non-existent variable or resource is not referenced by fragment stage. */
   if (!res || !(res->StageReferences & (1 << MESA_SHADER_FRAGMENT)))
      return -1;

   return RESOURCE_VAR(res)->data.index;
}

static uint8_t
stage_from_enum(GLenum ref)
{
   switch (ref) {
   case GL_REFERENCED_BY_VERTEX_SHADER:
      return MESA_SHADER_VERTEX;
   case GL_REFERENCED_BY_TESS_CONTROL_SHADER:
      return MESA_SHADER_TESS_CTRL;
   case GL_REFERENCED_BY_TESS_EVALUATION_SHADER:
      return MESA_SHADER_TESS_EVAL;
   case GL_REFERENCED_BY_GEOMETRY_SHADER:
      return MESA_SHADER_GEOMETRY;
   case GL_REFERENCED_BY_FRAGMENT_SHADER:
      return MESA_SHADER_FRAGMENT;
   case GL_REFERENCED_BY_COMPUTE_SHADER:
      return MESA_SHADER_COMPUTE;
   default:
      assert(!"shader stage not supported");
      return MESA_SHADER_STAGES;
   }
}

/**
 * Check if resource is referenced by given 'referenced by' stage enum.
 * ATC and UBO resources hold stage references of their own.
 */
static bool
is_resource_referenced(struct gl_shader_program *shProg,
                       struct gl_program_resource *res,
                       GLuint index, uint8_t stage)
{
   /* First, check if we even have such a stage active. */
   if (!shProg->_LinkedShaders[stage])
      return false;

   if (res->Type == GL_ATOMIC_COUNTER_BUFFER)
      return RESOURCE_ATC(res)->StageReferences[stage];

   if (res->Type == GL_UNIFORM_BLOCK)
      return shProg->UniformBlockStageIndex[stage][index] != -1;

   return res->StageReferences & (1 << stage);
}

static unsigned
get_buffer_property(struct gl_shader_program *shProg,
                    struct gl_program_resource *res, const GLenum prop,
                    GLint *val, const char *caller)
{
   GET_CURRENT_CONTEXT(ctx);
   if (res->Type != GL_UNIFORM_BLOCK &&
       res->Type != GL_ATOMIC_COUNTER_BUFFER)
      goto invalid_operation;

   if (res->Type == GL_UNIFORM_BLOCK) {
      switch (prop) {
      case GL_BUFFER_BINDING:
         *val = RESOURCE_UBO(res)->Binding;
         return 1;
      case GL_BUFFER_DATA_SIZE:
         *val = RESOURCE_UBO(res)->UniformBufferSize;
         return 1;
      case GL_NUM_ACTIVE_VARIABLES:
         *val = 0;
         for (unsigned i = 0; i < RESOURCE_UBO(res)->NumUniforms; i++) {
            const char *iname = RESOURCE_UBO(res)->Uniforms[i].IndexName;
            struct gl_program_resource *uni =
               _mesa_program_resource_find_name(shProg, GL_UNIFORM, iname,
                                                NULL);
            if (!uni)
               continue;
            (*val)++;
         }
         return 1;
      case GL_ACTIVE_VARIABLES:
         for (unsigned i = 0; i < RESOURCE_UBO(res)->NumUniforms; i++) {
            const char *iname = RESOURCE_UBO(res)->Uniforms[i].IndexName;
            struct gl_program_resource *uni =
               _mesa_program_resource_find_name(shProg, GL_UNIFORM, iname,
                                                NULL);
            if (!uni)
               continue;
            *val++ =
               _mesa_program_resource_index(shProg, uni);
         }
         return RESOURCE_UBO(res)->NumUniforms;
      }
   } else if (res->Type == GL_ATOMIC_COUNTER_BUFFER) {
      switch (prop) {
      case GL_BUFFER_BINDING:
         *val = RESOURCE_ATC(res)->Binding;
         return 1;
      case GL_BUFFER_DATA_SIZE:
         *val = RESOURCE_ATC(res)->MinimumSize;
         return 1;
      case GL_NUM_ACTIVE_VARIABLES:
         *val = RESOURCE_ATC(res)->NumUniforms;
         return 1;
      case GL_ACTIVE_VARIABLES:
         for (unsigned i = 0; i < RESOURCE_ATC(res)->NumUniforms; i++)
            *val++ = RESOURCE_ATC(res)->Uniforms[i];
         return RESOURCE_ATC(res)->NumUniforms;
      }
   }
   assert(!"support for property type not implemented");

invalid_operation:
   _mesa_error(ctx, GL_INVALID_OPERATION, "%s(%s prop %s)", caller,
               _mesa_enum_to_string(res->Type),
               _mesa_enum_to_string(prop));

   return 0;
}

unsigned
_mesa_program_resource_prop(struct gl_shader_program *shProg,
                            struct gl_program_resource *res, GLuint index,
                            const GLenum prop, GLint *val, const char *caller)
{
   GET_CURRENT_CONTEXT(ctx);

#define VALIDATE_TYPE(type)\
   if (res->Type != type)\
      goto invalid_operation;

   switch(prop) {
   case GL_NAME_LENGTH:
      switch (res->Type) {
      case GL_ATOMIC_COUNTER_BUFFER:
         goto invalid_operation;
      default:
         /* Resource name length + terminator. */
         *val = _mesa_program_resource_name_len(res) + 1;
      }
      return 1;
   case GL_TYPE:
      switch (res->Type) {
      case GL_UNIFORM:
         *val = RESOURCE_UNI(res)->type->gl_type;
         return 1;
      case GL_PROGRAM_INPUT:
      case GL_PROGRAM_OUTPUT:
         *val = RESOURCE_VAR(res)->type->gl_type;
         return 1;
      case GL_TRANSFORM_FEEDBACK_VARYING:
         *val = RESOURCE_XFB(res)->Type;
         return 1;
      default:
         goto invalid_operation;
      }
   case GL_ARRAY_SIZE:
      switch (res->Type) {
      case GL_UNIFORM:
            *val = MAX2(RESOURCE_UNI(res)->array_elements, 1);
            return 1;
      case GL_PROGRAM_INPUT:
      case GL_PROGRAM_OUTPUT:
         *val = MAX2(_mesa_program_resource_array_size(res), 1);
         return 1;
      case GL_TRANSFORM_FEEDBACK_VARYING:
         *val = MAX2(RESOURCE_XFB(res)->Size, 1);
         return 1;
      default:
         goto invalid_operation;
      }
   case GL_OFFSET:
      VALIDATE_TYPE(GL_UNIFORM);
      *val = RESOURCE_UNI(res)->offset;
      return 1;
   case GL_BLOCK_INDEX:
      VALIDATE_TYPE(GL_UNIFORM);
      *val = RESOURCE_UNI(res)->block_index;
      return 1;
   case GL_ARRAY_STRIDE:
      VALIDATE_TYPE(GL_UNIFORM);
      *val = RESOURCE_UNI(res)->array_stride;
      return 1;
   case GL_MATRIX_STRIDE:
      VALIDATE_TYPE(GL_UNIFORM);
      *val = RESOURCE_UNI(res)->matrix_stride;
      return 1;
   case GL_IS_ROW_MAJOR:
      VALIDATE_TYPE(GL_UNIFORM);
      *val = RESOURCE_UNI(res)->row_major;
      return 1;
   case GL_ATOMIC_COUNTER_BUFFER_INDEX:
      VALIDATE_TYPE(GL_UNIFORM);
      *val = RESOURCE_UNI(res)->atomic_buffer_index;
      return 1;
   case GL_BUFFER_BINDING:
   case GL_BUFFER_DATA_SIZE:
   case GL_NUM_ACTIVE_VARIABLES:
   case GL_ACTIVE_VARIABLES:
      return get_buffer_property(shProg, res, prop, val, caller);
   case GL_REFERENCED_BY_COMPUTE_SHADER:
      if (!_mesa_has_compute_shaders(ctx))
         goto invalid_enum;
      /* fallthrough */
   case GL_REFERENCED_BY_VERTEX_SHADER:
   case GL_REFERENCED_BY_TESS_CONTROL_SHADER:
   case GL_REFERENCED_BY_TESS_EVALUATION_SHADER:
   case GL_REFERENCED_BY_GEOMETRY_SHADER:
   case GL_REFERENCED_BY_FRAGMENT_SHADER:
      switch (res->Type) {
      case GL_UNIFORM:
      case GL_PROGRAM_INPUT:
      case GL_PROGRAM_OUTPUT:
      case GL_UNIFORM_BLOCK:
      case GL_ATOMIC_COUNTER_BUFFER:
         *val = is_resource_referenced(shProg, res, index,
                                       stage_from_enum(prop));
         return 1;
      default:
         goto invalid_operation;
      }
   case GL_LOCATION:
      switch (res->Type) {
      case GL_UNIFORM:
      case GL_PROGRAM_INPUT:
      case GL_PROGRAM_OUTPUT:
         *val = program_resource_location(shProg, res,
                                          _mesa_program_resource_name(res),
                                          0);
         return 1;
      default:
         goto invalid_operation;
      }
   case GL_LOCATION_INDEX:
      if (res->Type != GL_PROGRAM_OUTPUT)
         goto invalid_operation;
      *val = RESOURCE_VAR(res)->data.index;
      return 1;

   case GL_NUM_COMPATIBLE_SUBROUTINES:
      if (res->Type != GL_VERTEX_SUBROUTINE_UNIFORM &&
          res->Type != GL_FRAGMENT_SUBROUTINE_UNIFORM &&
          res->Type != GL_GEOMETRY_SUBROUTINE_UNIFORM &&
          res->Type != GL_COMPUTE_SUBROUTINE_UNIFORM &&
          res->Type != GL_TESS_CONTROL_SUBROUTINE_UNIFORM &&
          res->Type != GL_TESS_EVALUATION_SUBROUTINE_UNIFORM)
         goto invalid_operation;
      *val = RESOURCE_UNI(res)->num_compatible_subroutines;
      return 1;
   case GL_COMPATIBLE_SUBROUTINES: {
      const struct gl_uniform_storage *uni;
      struct gl_shader *sh;
      unsigned count, i;
      int j;

      if (res->Type != GL_VERTEX_SUBROUTINE_UNIFORM &&
          res->Type != GL_FRAGMENT_SUBROUTINE_UNIFORM &&
          res->Type != GL_GEOMETRY_SUBROUTINE_UNIFORM &&
          res->Type != GL_COMPUTE_SUBROUTINE_UNIFORM &&
          res->Type != GL_TESS_CONTROL_SUBROUTINE_UNIFORM &&
          res->Type != GL_TESS_EVALUATION_SUBROUTINE_UNIFORM)
         goto invalid_operation;
      uni = RESOURCE_UNI(res);

      sh = shProg->_LinkedShaders[_mesa_shader_stage_from_subroutine_uniform(res->Type)];
      count = 0;
      for (i = 0; i < sh->NumSubroutineFunctions; i++) {
         struct gl_subroutine_function *fn = &sh->SubroutineFunctions[i];
         for (j = 0; j < fn->num_compat_types; j++) {
            if (fn->types[j] == uni->type) {
               val[count++] = i;
               break;
            }
         }
      }
      return count;
   }
   /* GL_ARB_tessellation_shader */
   case GL_IS_PER_PATCH:
      switch (res->Type) {
      case GL_PROGRAM_INPUT:
      case GL_PROGRAM_OUTPUT:
         *val = RESOURCE_VAR(res)->data.patch;
         return 1;
      default:
         goto invalid_operation;
      }
   default:
      goto invalid_enum;
   }

#undef VALIDATE_TYPE

invalid_enum:
   _mesa_error(ctx, GL_INVALID_ENUM, "%s(%s prop %s)", caller,
               _mesa_enum_to_string(res->Type),
               _mesa_enum_to_string(prop));
   return 0;

invalid_operation:
   _mesa_error(ctx, GL_INVALID_OPERATION, "%s(%s prop %s)", caller,
               _mesa_enum_to_string(res->Type),
               _mesa_enum_to_string(prop));
   return 0;
}

extern void
_mesa_get_program_resourceiv(struct gl_shader_program *shProg,
                             GLenum programInterface, GLuint index, GLsizei propCount,
                             const GLenum *props, GLsizei bufSize,
                             GLsizei *length, GLint *params)
{
   GET_CURRENT_CONTEXT(ctx);
   GLint *val = (GLint *) params;
   const GLenum *prop = props;
   GLsizei amount = 0;

   struct gl_program_resource *res =
      _mesa_program_resource_find_index(shProg, programInterface, index);

   /* No such resource found or bufSize negative. */
   if (!res || bufSize < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE,
                  "glGetProgramResourceiv(%s index %d bufSize %d)",
                  _mesa_enum_to_string(programInterface), index, bufSize);
      return;
   }

   /* Write propCount values until error occurs or bufSize reached. */
   for (int i = 0; i < propCount && i < bufSize; i++, val++, prop++) {
      int props_written =
         _mesa_program_resource_prop(shProg, res, index, *prop, val,
                                     "glGetProgramResourceiv");

      /* Error happened. */
      if (props_written == 0)
         return;

      amount += props_written;
   }

   /* If <length> is not NULL, the actual number of integer values
    * written to <params> will be written to <length>.
    */
   if (length)
      *length = amount;
}
