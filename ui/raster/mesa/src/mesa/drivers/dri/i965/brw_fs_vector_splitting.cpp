/*
 * Copyright © 2010 Intel Corporation
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
 * \file brw_wm_vector_splitting.cpp
 *
 * If a vector is only ever referenced by its components, then
 * split those components out to individual variables so they can be
 * handled normally by other optimization passes.
 *
 * This skips vectors in uniforms and varyings, which need to be
 * accessible as vectors for their access by the GL.  Also, vector
 * results of non-variable-derefs in assignments aren't handled
 * because to do so we would have to store the vector result to a
 * temporary in order to unload each channel, and to do so would just
 * loop us back to where we started.  For the 965, this is exactly the
 * behavior we want for the results of texture lookups, but probably not for
 */

#include "main/core.h"
#include "brw_context.h"
#include "glsl/ir.h"
#include "glsl/ir_visitor.h"
#include "glsl/ir_rvalue_visitor.h"
#include "glsl/glsl_types.h"
#include "util/hash_table.h"

static bool debug = false;

class variable_entry : public exec_node
{
public:
   variable_entry(ir_variable *var)
   {
      this->var = var;
      this->whole_vector_access = 0;
      this->mem_ctx = NULL;
   }

   ir_variable *var; /* The key: the variable's pointer. */

   /** Number of times the variable is referenced, including assignments. */
   unsigned whole_vector_access;

   ir_variable *components[4];

   /** ralloc_parent(this->var) -- the shader's ralloc context. */
   void *mem_ctx;
};

class ir_vector_reference_visitor : public ir_hierarchical_visitor {
public:
   ir_vector_reference_visitor(void)
   {
      this->mem_ctx = ralloc_context(NULL);
      this->ht = _mesa_hash_table_create(mem_ctx, _mesa_hash_pointer,
                                         _mesa_key_pointer_equal);
   }

   ~ir_vector_reference_visitor(void)
   {
      ralloc_free(mem_ctx);
   }

   virtual ir_visitor_status visit(ir_variable *);
   virtual ir_visitor_status visit(ir_dereference_variable *);
   virtual ir_visitor_status visit_enter(ir_swizzle *);
   virtual ir_visitor_status visit_enter(ir_assignment *);
   virtual ir_visitor_status visit_enter(ir_function_signature *);

   variable_entry *get_variable_entry(ir_variable *var);

   /* List of variable_entry */
   struct hash_table *ht;

   void *mem_ctx;
};

variable_entry *
ir_vector_reference_visitor::get_variable_entry(ir_variable *var)
{
   assert(var);

   if (!var->type->is_vector())
      return NULL;

   switch (var->data.mode) {
   case ir_var_uniform:
   case ir_var_shader_in:
   case ir_var_shader_out:
   case ir_var_system_value:
   case ir_var_function_in:
   case ir_var_function_out:
   case ir_var_function_inout:
      /* Can't split varyings or uniforms.  Function in/outs won't get split
       * either.
       */
      return NULL;
   case ir_var_auto:
   case ir_var_temporary:
      break;
   }

   struct hash_entry *hte = _mesa_hash_table_search(ht, var);
   if (hte)
      return (struct variable_entry *) hte->data;

   variable_entry *entry = new(mem_ctx) variable_entry(var);
   _mesa_hash_table_insert(ht, var, entry);
   return entry;
}


ir_visitor_status
ir_vector_reference_visitor::visit(ir_variable *ir)
{
   /* Make sure splitting looks at splitting this variable */
   (void)this->get_variable_entry(ir);

   return visit_continue;
}

ir_visitor_status
ir_vector_reference_visitor::visit(ir_dereference_variable *ir)
{
   ir_variable *const var = ir->var;
   variable_entry *entry = this->get_variable_entry(var);

   if (entry)
      entry->whole_vector_access++;

   return visit_continue;
}

ir_visitor_status
ir_vector_reference_visitor::visit_enter(ir_swizzle *ir)
{
   /* Don't descend into a vector ir_dereference_variable below. */
   if (ir->val->as_dereference_variable() && ir->type->is_scalar())
      return visit_continue_with_parent;

   return visit_continue;
}

ir_visitor_status
ir_vector_reference_visitor::visit_enter(ir_assignment *ir)
{
   if (ir->lhs->as_dereference_variable() &&
       ir->rhs->as_dereference_variable() &&
       !ir->condition) {
      /* We'll split copies of a vector to copies of channels, so don't
       * descend to the ir_dereference_variables.
       */
      return visit_continue_with_parent;
   }
   if (ir->lhs->as_dereference_variable() &&
       _mesa_is_pow_two(ir->write_mask) &&
       !ir->condition) {
      /* If we're writing just a channel, then channel-splitting the LHS is OK.
       */
      ir->rhs->accept(this);
      return visit_continue_with_parent;
   }
   return visit_continue;
}

ir_visitor_status
ir_vector_reference_visitor::visit_enter(ir_function_signature *ir)
{
   /* We don't want to descend into the function parameters and
    * split them, so just accept the body here.
    */
   visit_list_elements(this, &ir->body);
   return visit_continue_with_parent;
}

class ir_vector_splitting_visitor : public ir_rvalue_visitor {
public:
   ir_vector_splitting_visitor(struct hash_table *vars)
   {
      this->ht = vars;
   }

   virtual ir_visitor_status visit_leave(ir_assignment *);

   void handle_rvalue(ir_rvalue **rvalue);
   variable_entry *get_splitting_entry(ir_variable *var);

   struct hash_table *ht;
};

variable_entry *
ir_vector_splitting_visitor::get_splitting_entry(ir_variable *var)
{
   assert(var);

   if (!var->type->is_vector())
      return NULL;

   struct hash_entry *hte = _mesa_hash_table_search(ht, var);
   return hte ? (struct variable_entry *) hte->data : NULL;
}

void
ir_vector_splitting_visitor::handle_rvalue(ir_rvalue **rvalue)
{
   if (!*rvalue)
      return;

   ir_swizzle *swiz = (*rvalue)->as_swizzle();
   if (!swiz || !swiz->type->is_scalar())
      return;

   ir_dereference_variable *deref_var = swiz->val->as_dereference_variable();
   if (!deref_var)
      return;

   variable_entry *entry = get_splitting_entry(deref_var->var);
   if (!entry)
      return;

   ir_variable *var = entry->components[swiz->mask.x];
   *rvalue = new(entry->mem_ctx) ir_dereference_variable(var);
}

ir_visitor_status
ir_vector_splitting_visitor::visit_leave(ir_assignment *ir)
{
   ir_dereference_variable *lhs_deref = ir->lhs->as_dereference_variable();
   ir_dereference_variable *rhs_deref = ir->rhs->as_dereference_variable();
   variable_entry *lhs = lhs_deref ? get_splitting_entry(lhs_deref->var) : NULL;
   variable_entry *rhs = rhs_deref ? get_splitting_entry(rhs_deref->var) : NULL;

   if (lhs_deref && rhs_deref && (lhs || rhs) && !ir->condition) {
      unsigned int rhs_chan = 0;

      /* Straight assignment of vector variables. */
      for (unsigned int i = 0; i < ir->lhs->type->vector_elements; i++) {
	 ir_dereference *new_lhs;
	 ir_rvalue *new_rhs;
	 void *mem_ctx = lhs ? lhs->mem_ctx : rhs->mem_ctx;
	 unsigned int writemask;

	 if (!(ir->write_mask & (1 << i)))
	    continue;

	 if (lhs) {
	    new_lhs = new(mem_ctx) ir_dereference_variable(lhs->components[i]);
	    writemask = 1;
	 } else {
	    new_lhs = ir->lhs->clone(mem_ctx, NULL);
	    writemask = 1 << i;
	 }

	 if (rhs) {
	    new_rhs =
	       new(mem_ctx) ir_dereference_variable(rhs->components[rhs_chan]);
	 } else {
	    new_rhs = new(mem_ctx) ir_swizzle(ir->rhs->clone(mem_ctx, NULL),
					      rhs_chan, 0, 0, 0, 1);
	 }

	 ir->insert_before(new(mem_ctx) ir_assignment(new_lhs,
						      new_rhs,
						      NULL, writemask));

	 rhs_chan++;
      }
      ir->remove();
   } else if (lhs) {
      void *mem_ctx = lhs->mem_ctx;
      int elem = -1;

      switch (ir->write_mask) {
      case (1 << 0):
	 elem = 0;
	 break;
      case (1 << 1):
	 elem = 1;
	 break;
      case (1 << 2):
	 elem = 2;
	 break;
      case (1 << 3):
	 elem = 3;
	 break;
      default:
	 ir->fprint(stderr);
	 unreachable("not reached: non-channelwise dereference of LHS.");
      }

      ir->lhs = new(mem_ctx) ir_dereference_variable(lhs->components[elem]);
      ir->write_mask = (1 << 0);

      handle_rvalue(&ir->rhs);
   } else {
      handle_rvalue(&ir->rhs);
   }

   handle_rvalue(&ir->condition);

   return visit_continue;
}

bool
brw_do_vector_splitting(exec_list *instructions)
{
   struct hash_entry *hte;

   ir_vector_reference_visitor refs;

   visit_list_elements(&refs, instructions);

   /* Trim out variables we can't split. */
   hash_table_foreach(refs.ht, hte) {
      struct variable_entry *entry = (struct variable_entry *) hte->data;
      if (debug) {
	 fprintf(stderr, "vector %s@%p: whole_access %d\n",
                 entry->var->name, (void *) entry->var,
                 entry->whole_vector_access);
      }

      if (entry->whole_vector_access) {
         _mesa_hash_table_remove(refs.ht, hte);
      }
   }

   if (refs.ht->entries == 0)
      return false;

   void *mem_ctx = ralloc_context(NULL);

   /* Replace the decls of the vectors to be split with their split
    * components.
    */
   hash_table_foreach(refs.ht, hte) {
      struct variable_entry *entry = (struct variable_entry *) hte->data;
      const struct glsl_type *type;
      type = glsl_type::get_instance(entry->var->type->base_type, 1, 1);

      entry->mem_ctx = ralloc_parent(entry->var);

      for (unsigned int i = 0; i < entry->var->type->vector_elements; i++) {
         char *const name = ir_variable::temporaries_allocate_names
            ? ralloc_asprintf(mem_ctx, "%s_%c",
                              entry->var->name,
                              "xyzw"[i])
            : NULL;

	 entry->components[i] = new(entry->mem_ctx) ir_variable(type, name,
								ir_var_temporary);

         ralloc_free(name);

	 entry->var->insert_before(entry->components[i]);
      }

      entry->var->remove();
   }

   ir_vector_splitting_visitor split(refs.ht);
   visit_list_elements(&split, instructions);

   ralloc_free(mem_ctx);

   return true;
}
