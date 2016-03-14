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
23jul15,yat  Written (US60452)
*/

#include <stdio.h> /* printf */
#include <drm/drm_vxworks.h> /* for DRM_VXDEV */
#include <drm/drmP.h> /* for DRM_MINOR_CNT */
#include <i915_vxworks.h> /* for extern */
#include <i915_drv.h> /* for drm_i915_private, drm_i915_cmd_descriptor */
#include <vxoal/krnl/log.h> /* for SET_CUR */
#include <vxoal/krnl/list.h> /* hlist_node, for hash_for_each_safe */
#include <vxoal/krnl/scatterlist.h> /* show_sg_table */

/* This needs to match the same struct in i915_cmd_parser.c */
struct cmd_node
    {
    const struct drm_i915_cmd_descriptor *desc;
    struct hlist_node node;
    };

/*******************************************************************************
 *
 * Show routines static declaration
 *
 */

static void show_i915_vma
    (
    struct i915_vma *cur,
    const char *tab
    );

static void show_i915_address_space
    (
    struct i915_address_space *cur,
    const char *tab
    );

static void show_drm_i915_gem_object
    (
    struct drm_i915_gem_object *cur,
    const char *tab
    );

static void show_intel_engine_cs
    (
    struct intel_engine_cs *cur,
    const char *tab
    );

/*******************************************************************************
 *
 * Show routines for list
 *
 */

static void show_vm_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct i915_address_space *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, global_link)
        {
        show_i915_address_space(curr, tab2);
        }
    }

static void show_active_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct i915_vma *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, mm_list)
        {
        show_i915_vma(curr, tab2);
        }
    }

static void show_inactive_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct i915_vma *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, mm_list)
        {
        show_i915_vma(curr, tab2);
        }
    }

static void show_bound_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_i915_gem_object *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, global_list)
        {
        show_drm_i915_gem_object(curr, tab2);
        }
    }

static void show_unbound_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_i915_gem_object *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, global_list)
        {
        show_drm_i915_gem_object(curr, tab2);
        }
    }

/*******************************************************************************
 *
 * Show routines from i915 struct from i915_gem_gtt.h
 *
 */

static void show_i915_vma
    (
    struct i915_vma *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_i915_gem_object(cur->obj, tab2);

    printf("%s%s.exec_handle:%lx\n", tab2, __func__, cur->exec_handle);
    printf("%s%s.pin_count:%d\n", tab2, __func__, cur->pin_count);
    }

static void show_i915_address_space
    (
    struct i915_address_space *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_mm(&(cur->mm), tab2);

    printf("%s%s.start:%llx\n", tab2, __func__, cur->start);
    printf("%s%s.total:%llx\n", tab2, __func__, cur->total);

    show_active_list(&(cur->active_list), tab2);
    show_inactive_list(&(cur->inactive_list), tab2);
    }

static void show_i915_gtt
    (
    struct i915_gtt *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_i915_address_space(&(cur->base), tab2);

    printf("%s%s.stolen_size:%ld\n", tab2, __func__, cur->stolen_size);
    printf("%s%s.mappable_end:%llx\n", tab2, __func__, cur->mappable_end);

    show_io_mapping(cur->mappable, tab2);

    printf("%s%s.mappable_base:%llx\n", tab2, __func__, cur->mappable_base);
    printf("%s%s.gsm:%p\n", tab2, __func__, cur->gsm);
    printf("%s%s.do_idle_maps:%d\n", tab2, __func__, cur->do_idle_maps);
    printf("%s%s.mtrr:%d\n", tab2, __func__, cur->mtrr);
    }

static void show_i915_hw_ppgtt
    (
    struct i915_hw_ppgtt *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_i915_address_space(&(cur->base), tab2);
    show_kref(&(cur->ref), tab2);
    }

/*******************************************************************************
 *
 * Show routines from i915 struct from i915_drv.h
 *
 */

static void show_i915_gem_mm
    (
    struct i915_gem_mm *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_mm(&(cur->stolen), tab2);

    show_bound_list(&(cur->bound_list), tab2);
    show_unbound_list(&(cur->unbound_list), tab2);

    printf("%s%s.stolen_base:%lx\n", tab2, __func__, cur->stolen_base);

    show_i915_hw_ppgtt(cur->aliasing_ppgtt, tab2);

    printf("%s%s.interruptible:%d\n", tab2, __func__, cur->interruptible);
    printf("%s%s.busy:%d\n", tab2, __func__, cur->busy);
    printf("%s%s.bsd_ring_dispatch_index:%d\n", tab2, __func__, cur->bsd_ring_dispatch_index);
    printf("%s%s.bit_6_swizzle_x:%d\n", tab2, __func__, cur->bit_6_swizzle_x);
    printf("%s%s.bit_6_swizzle_y:%d\n", tab2, __func__, cur->bit_6_swizzle_y);
    printf("%s%s.object_memory:%ld\n", tab2, __func__, cur->object_memory);
    printf("%s%s.object_count:%d\n", tab2, __func__, cur->object_count);
    }

static void show_drm_i915_gem_object_ops
    (
    const struct drm_i915_gem_object_ops *cur,
    const char *tab
    )
    {
    SET_CUR;
    }

static void show_drm_i915_gem_object
    (
    struct drm_i915_gem_object *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_gem_object(&(cur->base), tab2);
    show_drm_i915_gem_object_ops(cur->ops, tab2);
    show_drm_mm_node(cur->stolen, tab2);

    printf("%s%s.active:%d\n", tab2, __func__, cur->active);
    printf("%s%s.dirty:%d\n", tab2, __func__, cur->dirty);
    printf("%s%s.fence_reg:%d\n", tab2, __func__, cur->fence_reg);
    printf("%s%s.madv:%d\n", tab2, __func__, cur->madv);
    printf("%s%s.tiling_mode:%d\n", tab2, __func__, cur->tiling_mode);
    printf("%s%s.fence_dirty:%d\n", tab2, __func__, cur->fence_dirty);
    printf("%s%s.map_and_fenceable:%d\n", tab2, __func__, cur->map_and_fenceable);
    printf("%s%s.fault_mappable:%d\n", tab2, __func__, cur->fault_mappable);
    printf("%s%s.pin_display:%d\n", tab2, __func__, cur->pin_display);
    printf("%s%s.gt_ro:%d\n", tab2, __func__, cur->gt_ro);
    printf("%s%s.cache_level:%d\n", tab2, __func__, cur->cache_level);
    printf("%s%s.frontbuffer_bits:%d\n", tab2, __func__, cur->frontbuffer_bits);

    show_sg_table(cur->pages, tab2);

    printf("%s%s.pages_pin_count:%d\n", tab2, __func__, cur->pages_pin_count);
    printf("%s%s.vmapping_count:%d\n", tab2, __func__, cur->vmapping_count);
    printf("%s%s.stride:%d\n", tab2, __func__, cur->stride);
    printf("%s%s.framebuffer_references:%ld\n", tab2, __func__, cur->framebuffer_references);
    }

static void show_drm_i915_fence_reg
    (
    struct drm_i915_fence_reg *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_i915_gem_object(cur->obj, tab2);

    printf("%s%s.pin_count:%d\n", tab2, __func__, cur->pin_count);
    }

static void show_intel_hw_status_page
    (
    struct intel_hw_status_page *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.page_addr:%p\n", tab2, __func__, cur->page_addr);
    printf("%s%s.gfx_addr:%x\n", tab2, __func__, cur->gfx_addr);

    show_drm_i915_gem_object(cur->obj, tab2);
    }

static void show_intel_ringbuffer
    (
    struct intel_ringbuffer *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_i915_gem_object(cur->obj, tab2);

    printf("%s%s.virtual_start:%p\n", tab2, __func__, cur->virtual_start);

    show_intel_engine_cs(cur->ring, tab2);

    printf("%s%s.head:%d\n", tab2, __func__, cur->head);
    printf("%s%s.tail:%d\n", tab2, __func__, cur->tail);
    printf("%s%s.space:%d\n", tab2, __func__, cur->space);
    printf("%s%s.size:%d\n", tab2, __func__, cur->size);
    printf("%s%s.effective_size:%d\n", tab2, __func__, cur->effective_size);
    printf("%s%s.last_retired_head:%d\n", tab2, __func__, cur->last_retired_head);
    }

static void show_intel_context
    (
    struct intel_context *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_kref(&(cur->ref), tab2);

    printf("%s%s.user_handle:%d\n", tab2, __func__, cur->user_handle);
    printf("%s%s.remap_slice:%d\n", tab2, __func__, cur->remap_slice);

    show_drm_i915_gem_object(cur->legacy_hw_ctx.rcs_state, tab2);

    printf("%s%s.initialized:%d\n", tab2, __func__, cur->legacy_hw_ctx.initialized);
    }

static void show_intel_engine_cs
    (
    struct intel_engine_cs *cur,
    const char *tab
    )
    {
    struct hlist_node *tmp;
    struct cmd_node *desc_node;
    int i;
    SET_CUR;

    printf("%s%s.name:%s\n", tab2, __func__, cur->name);
    printf("%s%s.id:%d\n", tab2, __func__, cur->id);
    printf("%s%s.mmio_base:%x\n", tab2, __func__, cur->mmio_base);
    if (!cur->mmio_base) return;

    show_intel_ringbuffer(cur->buffer, tab2);
    show_intel_hw_status_page(&(cur->status_page), tab2);

    printf("%s%s.irq_refcount:%x\n", tab2, __func__, cur->irq_refcount);
    printf("%s%s.irq_enable_mask:%x\n", tab2, __func__, cur->irq_enable_mask);

    show_intel_context(cur->default_context, tab2);
    show_intel_context(cur->last_context, tab2);

    printf("%s%s.gtt_offset:%x\n", tab2, __func__, cur->scratch.gtt_offset);
    printf("%s%s.cpu_page:%p\n", tab2, __func__, cur->scratch.cpu_page);

    printf("%s%s.cmd_hash.size:%ld\n", tab2, __func__, HASH_SIZE(cur->cmd_hash));

    hash_for_each_safe(cur->cmd_hash, i, tmp, desc_node, node)
        {
        printf("\t%s%s.cmd_hash.flags:%x value:%x mask:%x length:%x\n",
               tab2, __func__,
               desc_node->desc->flags,
               desc_node->desc->cmd.value, desc_node->desc->cmd.mask,
               desc_node->desc->length.fixed);
        }
    }

static void show_intel_device_info
    (
    const struct intel_device_info *cur,
    const char *tab
    )
    {
    int i;
    SET_CUR;

    printf("%s%s.display_mmio_offset:%x\n", tab2, __func__, cur->display_mmio_offset);
    printf("%s%s.device_id:%d\n", tab2, __func__, cur->device_id);
    printf("%s%s.num_pipes:%d\n", tab2, __func__, cur->num_pipes);

    for (i = 0; i < I915_MAX_PIPES; i++)
        {
        printf("%s%s.num_sprites:%d\n", tab2, __func__, cur->num_sprites[i]);
        }

    printf("%s%s.gen:%d\n", tab2, __func__, cur->gen);
    printf("%s%s.ring_mask:%x\n", tab2, __func__, cur->ring_mask);
    }

static void show_i915_gpu_error
    (
    struct i915_gpu_error *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.missed_irq_rings:%ld\n", tab2, __func__, cur->missed_irq_rings);
    printf("%s%s.stop_rings:%d\n", tab2, __func__, cur->stop_rings);
    printf("%s%s.test_irq_rings:%d\n", tab2, __func__, cur->test_irq_rings);
    printf("%s%s.reload_in_reset:%d\n", tab2, __func__, cur->reload_in_reset);
    }

static void show_intel_framebuffer
    (
    struct intel_framebuffer *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_i915_gem_object(cur->obj, tab2);
    }

static void show_intel_fbdev
    (
    struct intel_fbdev *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_fb_helper(&(cur->helper), tab2);

    show_intel_framebuffer(cur->fb, tab2);

    printf("%s%s.preferred_bpp:%d\n", tab2, __func__, cur->preferred_bpp);
    }

void show_drm_i915_private
    (
    void *dev_private,
    const char *tab
    )
    {
    struct drm_i915_private *cur = (struct drm_i915_private *)dev_private;
    int i;
    SET_CUR;

    if (gfxDrmShowDrmMm)
        {
        if (drm_mm_initialized(&(cur->gtt.base.mm))) drm_mm_debug_table(&(cur->gtt.base.mm), "");
        return;
        }

    show_intel_device_info(&(cur->info), tab2);

    printf("%s%s.relative_constants_mode:%d\n", tab2, __func__, cur->relative_constants_mode);
    printf("%s%s.regs:%p\n", tab2, __func__, cur->regs);
    printf("%s%s.gpio_mmio_base:%x\n", tab2, __func__, cur->gpio_mmio_base);
    printf("%s%s.mipi_mmio_base:%x\n", tab2, __func__, cur->mipi_mmio_base);

    for (i = 0; i < I915_NUM_RINGS; i++)
        {
        show_intel_engine_cs(&(cur->ring[i]), tab2);
        }

    show_drm_i915_gem_object(cur->semaphore_obj, tab2);

    printf("%s%s.last_seqno:%x\n", tab2, __func__, cur->last_seqno);
    printf("%s%s.next_seqno:%x\n", tab2, __func__, cur->next_seqno);

    printf("%s%s.display_irqs_enabled:%d\n",
           tab2, __func__, cur->display_irqs_enabled);

    printf("%s%s.irq_mask:%x\n", tab2, __func__, cur->irq_mask);
    printf("%s%s.gt_irq_mask:%x\n", tab2, __func__, cur->gt_irq_mask);
    printf("%s%s.pm_irq_mask:%x\n", tab2, __func__, cur->pm_irq_mask);
    printf("%s%s.pm_rps_events:%x\n", tab2, __func__, cur->pm_rps_events);

    for (i = 0; i < I915_MAX_PIPES; i++)
        {
        printf("%s%s.pipestat_irq_mask:%d-%x\n",
               tab2, __func__, i, cur->pipestat_irq_mask[i]);
        }

    printf("%s%s.no_aux_handshake:%d\n", tab2, __func__, cur->no_aux_handshake);

    for (i = 0; i < I915_MAX_NUM_FENCES; i++)
        {
        show_drm_i915_fence_reg(&(cur->fence_regs[i]), tab2);
        }

    printf("%s%s.fence_reg_start:%d\n", tab2, __func__, cur->fence_reg_start);
    printf("%s%s.num_fence_regs:%d\n", tab2, __func__, cur->num_fence_regs);

    printf("%s%s.fsb_freq:%d\n", tab2, __func__, cur->fsb_freq);
    printf("%s%s.mem_freq:%d\n", tab2, __func__, cur->mem_freq);
    printf("%s%s.is_ddr3:%d\n", tab2, __func__, cur->is_ddr3);

    printf("%s%s.pch_type:%d\n", tab2, __func__, cur->pch_type);
    printf("%s%s.pch_id:%d\n", tab2, __func__, cur->pch_id);
    printf("%s%s.quirks:%ld\n", tab2, __func__, cur->quirks);

    show_vm_list(&(cur->vm_list), tab2);

    show_i915_gtt(&(cur->gtt), tab2);

    show_i915_gem_mm(&(cur->mm), tab2);

    show_i915_gpu_error(&(cur->gpu_error), tab2);

    show_drm_i915_gem_object(cur->vlv_pctx, tab2);

    show_intel_fbdev(cur->fbdev, tab2);

    show_i915_params();
    }
