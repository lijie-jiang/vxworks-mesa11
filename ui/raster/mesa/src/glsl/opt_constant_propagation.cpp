/*
 * Copyright © 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * constant of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, constant, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above constantright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR CONSTANTRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file opt_constant_propagation.cpp
 *
 * Tracks assignments of constants to channels of variables, and
 * usage of those constant channels with direct usage of the constants.
 *
 * This can lead to constant folding and algebraic optimizations in
 * those later expressions, while causing no increase in instruction
 * count (due to constants being generally free to load from a
 * constant push buffer or as instruction immediate values) and
 * possibly reducing register pressure.
 */

#include "ir.h"
#include "ir_visitor.h"
#include "ir_rvalue_visitor.h"
#include "ir_basic_block.h"
#include "ir_optimization.h"
#include "glsl_types.h"
#include "util/hash_table.h"

namespace {

class acp_entry : public exec_node
{
public:
   acp_entry(ir_variable *var, unsigned write_mask, ir_constant *constant)
   {
      assert(var);
      assert(constant);
      this->var = var;
      this->write_mask = write_mask;
      this->constant = constant;
      this->initial_values = write_mask;
   }

   acp_entry(const acp_entry *src)
   {
      this->var = src->var;
      this->write_mask = src->write_mask;
      this->constant = src->constant;
      this->initial_values = src->initial_values;
   }

   ir_variable *var;
   ir_constant *constant;
   unsigned write_mask;

   /** Mask of values initially available in the constant. */
   unsigned initial_values;
};


class kill_entry : public exec_node
{
public:
   kill_entry(ir_variable *var, unsigned write_mask)
   {
      assert(var);
      this->var = var;
      this->write_mask = write_mask;
   }

   ir_variable *var;
   unsigned write_mask;
};

class ir_constant_propagation_visitor : public ir_rvalue_visitor {
public:
   ir_constant_propagation_visitor()
   {
      progress = false;
      killed_all = false;
      mem_ctx = ralloc_context(0);
      this->acp = new(mem_ctx) exec_list;
      this->kills = _mesa_hash_table_create(mem_ctx, _mesa_hash_pointer,
                                            _mesa_key_pointer_equal);
   }
   ~ir_constant_propagation_visitor()
   {
      ralloc_free(mem_ctx);
   }

   virtual ir_visitor_status visit_enter(class ir_loop *);
   virtual ir_visitor_status visit_enter(class ir_function_signature *);
   virtual ir_visitor_status visit_enter(class ir_function *);
   virtual ir_visitor_status visit_leave(class ir_assignment *);
   virtual ir_visitor_status visit_enter(class ir_call *);
   virtual ir_visitor_status visit_enter(class ir_if *);

   void add_constant(ir_assignment *ir);
   void constant_folding(ir_rvalue **rvalue);
   void constant_propagation(ir_rvalue **rvalue);
   void kill(ir_variable *ir, unsigned write_mask);
   void handle_if_block(exec_list *instructions);
   void handle_rvalue(ir_rvalue **rvalue);

   /** List of acp_entry: The available constants to propagate */
   exec_list *acp;

   /**
    * List of kill_entry: The masks of variables whose values were
    * killed in this block.
    */
   hash_table *kills;

   bool progress;

   bool killed_all;

   void *mem_ctx;
};


void
ir_constant_propagation_visitor::constant_folding(ir_rvalue **rvalue) {

   if (*rvalue == NULL || (*rvalue)->ir_type == ir_type_constant)
      return;

   /* Note that we visit rvalues one leaving.  So if an expression has a
    * non-constant operand, no need to go looking down it to find if it's
    * constant.  This cuts the time of this pass down drastically.
    */
   ir_expression *expr = (*rvalue)->as_expression();
   if (expr) {
      for (unsigned int i = 0; i < expr->get_num_operands(); i++) {
	 if (!expr->operands[i]->as_constant())
	    return;
      }
   }

   /* Ditto for swizzles. */
   ir_swizzle *swiz = (*rvalue)->as_swizzle();
   if (swiz && !swiz->val->as_constant())
      return;

   ir_constant *constant = (*rvalue)->constant_expression_value();
   if (constant) {
      *rvalue = constant;
      this->progress = true;
   }
}

void
ir_constant_propagation_visitor::constant_propagation(ir_rvalue **rvalue) {

   if (this->in_assignee || !*rvalue)
      return;

   const glsl_type *type = (*rvalue)->type;
   if (!type->is_scalar() && !type->is_vector())
      return;

   ir_swizzle *swiz = NULL;
   ir_dereference_variable *deref = (*rvalue)->as_dereference_variable();
   if (!deref) {
      swiz = (*rvalue)->as_swizzle();
      if (!swiz)
	 return;

      deref = swiz->val->as_dereference_variable();
      if (!deref)
	 return;
   }

   ir_constant_data data;
   memset(&data, 0, sizeof(data));

   for (unsigned int i = 0; i < type->components(); i++) {
      int channel;
      acp_entry *found = NULL;

      if (swiz) {
	 switch (i) {
	 case 0: channel = swiz->mask.x; break;
	 case 1: channel = swiz->mask.y; break;
	 case 2: channel = swiz->mask.z; break;
	 case 3: channel = swiz->mask.w; break;
	 default: assert(!"shouldn't be reached"); channel = 0; break;
	 }
      } else {
	 channel = i;
      }

      foreach_in_list(acp_entry, entry, this->acp) {
	 if (entry->var == deref->var && entry->write_mask & (1 << channel)) {
	    found = entry;
	    break;
	 }
      }

      if (!found)
	 return;

      int rhs_channel = 0;
      for (int j = 0; j < 4; j++) {
	 if (j == channel)
	    break;
	 if (found->initial_values & (1 << j))
	    rhs_channel++;
      }

      switch (type->base_type) {
      case GLSL_TYPE_FLOAT:
	 data.f[i] = found->constant->value.f[rhs_channel];
	 break;
      case GLSL_TYPE_DOUBLE:
	 data.d[i] = found->constant->value.d[rhs_channel];
	 break;
      case GLSL_TYPE_INT:
	 data.i[i] = found->constant->value.i[rhs_channel];
	 break;
      case GLSL_TYPE_UINT:
	 data.u[i] = found->constant->value.u[rhs_channel];
	 break;
      case GLSL_TYPE_BOOL:
	 data.b[i] = found->constant->value.b[rhs_channel];
	 break;
      default:
	 assert(!"not reached");
	 break;
      }
   }

   *rvalue = new(ralloc_parent(deref)) ir_constant(type, &data);
   this->progress = true;
}

void
ir_constant_propagation_visitor::handle_rvalue(ir_rvalue **rvalue)
{
   constant_propagation(rvalue);
   constant_folding(rvalue);
}

ir_visitor_status
ir_constant_propagation_visitor::visit_enter(ir_function_signature *ir)
{
   /* Treat entry into a function signature as a completely separate
    * block.  Any instructions at global scope will be shuffled into
    * main() at link time, so they're irrelevant to us.
    */
   exec_list *orig_acp = this->acp;
   hash_table *orig_kills = this->kills;
   bool orig_killed_all = this->killed_all;

   this->acp = new(mem_ctx) exec_list;
   this->kills = _mesa_hash_table_create(mem_ctx, _mesa_hash_pointer,
                                         _mesa_key_pointer_equal);
   this->killed_all = false;

   visit_list_elements(this, &ir->body);

   this->kills = orig_kills;
   this->acp = orig_acp;
   this->killed_all = orig_killed_all;

   return visit_continue_with_parent;
}

ir_visitor_status
ir_constant_propagation_visitor::visit_leave(ir_assignment *ir)
{
  constant_folding(&ir->rhs);

   if (this->in_assignee)
      return visit_continue;

   unsigned kill_mask = ir->write_mask;
   if (ir->lhs->as_dereference_array()) {
      /* The LHS of the assignment uses an array indexing operator (e.g. v[i]
       * = ...;).  Since we only try to constant propagate vectors and
       * scalars, this means that either (a) array indexing is being used to
       * select a vector component, or (b) the variable in question is neither
       * a scalar or a vector, so we don't care about it.  In the former case,
       * we want to kill the whole vector, since in general we can't predict
       * which vector component will be selected by array indexing.  In the
       * latter case, it doesn't matter what we do, so go ahead and kill the
       * whole variable anyway.
       *
       * Note that if the array index is constant (e.g. v[2] = ...;), we could
       * in principle be smarter, but we don't need to, because a future
       * optimization pass will convert it to a simple assignment with the
       * correct mask.
       */
      kill_mask = ~0;
   }
   kill(ir->lhs->variable_referenced(), kill_mask);

   add_constant(ir);

   return visit_continue;
}

ir_visitor_status
ir_constant_propagation_visitor::visit_enter(ir_function *ir)
{
   (void) ir;
   return visit_continue;
}

ir_visitor_status
ir_constant_propagation_visitor::visit_enter(ir_call *ir)
{
   /* Do constant propagation on call parameters, but skip any out params */
   foreach_two_lists(formal_node, &ir->callee->parameters,
                     actual_node, &ir->actual_parameters) {
      ir_variable *sig_param = (ir_variable *) formal_node;
      ir_rvalue *param = (ir_rvalue *) actual_node;
      if (sig_param->data.mode != ir_var_function_out
          && sig_param->data.mode != ir_var_function_inout) {
	 ir_rvalue *new_param = param;
	 handle_rvalue(&new_param);
         if (new_param != param)
	    param->replace_with(new_param);
	 else
	    param->accept(this);
      }
   }

   /* Since we're unlinked, we don't (necssarily) know the side effects of
    * this call.  So kill all copies.
    */
   acp->make_empty();
   this->killed_all = true;

   return visit_continue_with_parent;
}

void
ir_constant_propagation_visitor::handle_if_block(exec_list *instructions)
{
   exec_list *orig_acp = this->acp;
   hash_table *orig_kills = this->kills;
   bool orig_killed_all = this->killed_all;

   this->acp = new(mem_ctx) exec_list;
   this->kills = _mesa_hash_table_create(mem_ctx, _mesa_hash_pointer,
                                         _mesa_key_pointer_equal);
   this->killed_all = false;

   /* Populate the initial acp with a constant of the original */
   foreach_in_list(acp_entry, a, orig_acp) {
      this->acp->push_tail(new(this->mem_ctx) acp_entry(a));
   }

   visit_list_elements(this, instructions);

   if (this->killed_all) {
      orig_acp->make_empty();
   }

   hash_table *new_kills = this->kills;
   this->kills = orig_kills;
   this->acp = orig_acp;
   this->killed_all = this->killed_all || orig_killed_all;

   hash_entry *htk;
   hash_table_foreach(new_kills, htk) {
      kill_entry *k = (kill_entry *) htk->data;
      kill(k->var, k->write_mask);
   }
}

ir_visitor_status
ir_constant_propagation_visitor::visit_enter(ir_if *ir)
{
   ir->condition->accept(this);
   handle_rvalue(&ir->condition);

   handle_if_block(&ir->then_instructions);
   handle_if_block(&ir->else_instructions);

   /* handle_if_block() already descended into the children. */
   return visit_continue_with_parent;
}

ir_visitor_status
ir_constant_propagation_visitor::visit_enter(ir_loop *ir)
{
   exec_list *orig_acp = this->acp;
   hash_table *orig_kills = this->kills;
   bool orig_killed_all = this->killed_all;

   /* FINISHME: For now, the initial acp for loops is totally empty.
    * We could go through once, then go through again with the acp
    * cloned minus the killed entries after the first run through.
    */
   this->acp = new(mem_ctx) exec_list;
   this->kills = _mesa_hash_table_create(mem_ctx, _mesa_hash_pointer,
                                         _mesa_key_pointer_equal);
   this->killed_all = false;

   visit_list_elements(this, &ir->body_instructions);

   if (this->killed_all) {
      orig_acp->make_empty();
   }

   hash_table *new_kills = this->kills;
   this->kills = orig_kills;
   this->acp = orig_acp;
   this->killed_all = this->killed_all || orig_killed_all;

   hash_entry *htk;
   hash_table_foreach(new_kills, htk) {
      kill_entry *k = (kill_entry *) htk->data;
      kill(k->var, k->write_mask);
   }

   /* already descended into the children. */
   return visit_continue_with_parent;
}

void
ir_constant_propagation_visitor::kill(ir_variable *var, unsigned write_mask)
{
   assert(var != NULL);

   /* We don't track non-vectors. */
   if (!var->type->is_vector() && !var->type->is_scalar())
      return;

   /* Remove any entries currently in the ACP for this kill. */
   foreach_in_list_safe(acp_entry, entry, this->acp) {
      if (entry->var == var) {
	 entry->write_mask &= ~write_mask;
	 if (entry->write_mask == 0)
	    entry->remove();
      }
   }

   /* Add this writemask of the variable to the list of killed
    * variables in this block.
    */
   hash_entry *kill_hash_entry = _mesa_hash_table_search(this->kills, var);
   if (kill_hash_entry) {
      kill_entry *entry = (kill_entry *) kill_hash_entry->data;
      entry->write_mask |= write_mask;
      return;
   }
   /* Not already in the list.  Make new entry. */
   _mesa_hash_table_insert(this->kills, var,
                           new(this->mem_ctx) kill_entry(var, write_mask));
}

/**
 * Adds an entry to the available constant list if it's a plain assignment
 * of a variable to a variable.
 */
void
ir_constant_propagation_visitor::add_constant(ir_assignment *ir)
{
   acp_entry *entry;

   if (ir->condition)
      return;

   if (!ir->write_mask)
      return;

   ir_dereference_variable *deref = ir->lhs->as_dereference_variable();
   ir_constant *constant = ir->rhs->as_constant();

   if (!deref || !constant)
      return;

   /* Only do constant propagation on vectors.  Constant matrices,
    * arrays, or structures would require more work elsewhere.
    */
   if (!deref->var->type->is_vector() && !deref->var->type->is_scalar())
      return;

   /* We can't do copy propagation on buffer variables, since the underlying
    * memory storage is shared across multiple threads we can't be sure that
    * the variable value isn't modified between this assignment and the next
    * instruction where its value is read.
    */
   if (deref->var->data.mode == ir_var_shader_storage)
      return;

   entry = new(this->mem_ctx) acp_entry(deref->var, ir->write_mask, constant);
   this->acp->push_tail(entry);
}

} /* unnamed namespace */

/**
 * Does a constant propagation pass on the code present in the instruction stream.
 */
bool
do_constant_propagation(exec_list *instructions)
{
   ir_constant_propagation_visitor v;

   visit_list_elements(&v, instructions);

   return v.progress;
}
