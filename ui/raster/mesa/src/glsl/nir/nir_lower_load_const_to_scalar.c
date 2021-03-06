/*
 * Copyright © 2015 Broadcom
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "util/macros.h"
#include "nir.h"
#include "nir_builder.h"

/** @file nir_lower_load_const_to_scalar.c
 *
 * Replaces vector nir_load_const instructions with a series of loads and a
 * vec[234] to reconstruct the original vector (on the assumption that
 * nir_lower_alu_to_scalar() will then be used to split it up).
 *
 * This gives NIR a chance to CSE more operations on a scalar shader, when the
 * same value was used in different vector contant loads.
 */

static void
lower_load_const_instr_scalar(nir_load_const_instr *lower)
{
   if (lower->def.num_components == 1)
      return;

   nir_builder b;
   nir_builder_init(&b, nir_cf_node_get_function(&lower->instr.block->cf_node));
   nir_builder_insert_before_instr(&b, &lower->instr);

   /* Emit the individual loads. */
   nir_ssa_def *loads[4];
   for (unsigned i = 0; i < lower->def.num_components; i++) {
      nir_load_const_instr *load_comp = nir_load_const_instr_create(b.shader, 1);
      load_comp->value.u[0] = lower->value.u[i];
      nir_builder_instr_insert(&b, &load_comp->instr);
      loads[i] = &load_comp->def;
   }

   /* Batch things back together into a vector. */
   nir_ssa_def *vec;
   switch (lower->def.num_components) {
   case 2:
      vec = nir_vec2(&b, loads[0], loads[1]);
      break;
   case 3:
      vec = nir_vec3(&b, loads[0], loads[1], loads[2]);
      break;
   case 4:
      vec = nir_vec4(&b, loads[0], loads[1], loads[2], loads[3]);
      break;
   default:
      unreachable("Unknown load_const component count.");
   }

   /* Replace the old load with a reference to our reconstructed vector. */
   nir_ssa_def_rewrite_uses(&lower->def, nir_src_for_ssa(vec),
                            ralloc_parent(b.impl));
   nir_instr_remove(&lower->instr);
}

static bool
lower_load_const_to_scalar_block(nir_block *block, void *data)
{
   nir_foreach_instr_safe(block, instr) {
      if (instr->type == nir_instr_type_load_const)
         lower_load_const_instr_scalar(nir_instr_as_load_const(instr));
   }

   return true;
}

static void
nir_lower_load_const_to_scalar_impl(nir_function_impl *impl)
{
   nir_foreach_block(impl, lower_load_const_to_scalar_block, NULL);
}

void
nir_lower_load_const_to_scalar(nir_shader *shader)
{
   nir_foreach_overload(shader, overload) {
      if (overload->impl)
         nir_lower_load_const_to_scalar_impl(overload->impl);
   }
}
