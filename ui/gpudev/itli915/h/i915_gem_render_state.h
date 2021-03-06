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

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
01jun15,rpc  continue porting (US59495)
*/

#ifndef _I915_GEM_RENDER_STATE_H_
#define _I915_GEM_RENDER_STATE_H_

#if !defined(__vxworks)
#include <linux/types.h>
#endif /* __vxworks */

struct intel_renderstate_rodata {
	const u32 *reloc;
	const u32 *batch;
	const u32 batch_items;
};

struct render_state {
	const struct intel_renderstate_rodata *rodata;
	struct drm_i915_gem_object *obj;
	u64 ggtt_offset;
	int gen;
	u32 aux_batch_size;
	u32 aux_batch_offset;
};

int i915_gem_render_state_init(struct drm_i915_gem_request *req);
void i915_gem_render_state_fini(struct render_state *so);
int i915_gem_render_state_prepare(struct intel_engine_cs *ring,
				  struct render_state *so);

#endif /* _I915_GEM_RENDER_STATE_H_ */
