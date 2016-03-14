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
31jul15,yat  Written (US60452)
*/

#include <stdio.h> /* for snprintf */
#include <vxoal/krnl/log.h> /* for SET_CUR */
#include <vxoal/krnl/device.h> /* for file_operations */
#include <vxoal/krnl/list.h> /* for list_for_each_entry_safe */
#include <vxoal/krnl/fb.h> /* for fb_info */
#include <drm/drm_vxworks.h> /* for struct drm_vxdev */
#include <drm/drmP.h> /* for drm_driver */
#include <drm/drm_legacy.h> /* for drm_driver */
#include <drm/drm_gem.h> /* for drm_gem_object */

/* This needs to match the same struct in drm_context.c */
struct drm_ctx_list
    {
    struct list_head head;
    drm_context_t handle;
    struct drm_file *tag;
    };

/* This needs to match the same struct in drm_vm.c */
struct drm_vma_entry
    {
    struct list_head head;
    struct vm_area_struct *vma;
    pid_t pid;
    };

/*******************************************************************************
 *
 * Show routines static declaration
 *
 */

static void show_drm_ctx_list
    (
    struct drm_ctx_list *cur,
    const char *tab
    );

static void show_drm_encoder
    (
    struct drm_encoder *cur,
    const char *tab
    );

static void show_drm_connector
    (
    struct drm_connector *cur,
    const char *tab
    );

static void show_drm_vma_entry
    (
    struct drm_vma_entry *cur,
    const char *tab
    );

static void show_drm_file
    (
    struct drm_file *cur,
    const char *tab
    );

static void show_drm_map_list
    (
    struct drm_map_list *cur,
    const char *tab
    );

static void show_drm_master
    (
    struct drm_master *cur,
    const char *tab
    );

static void show_drm_minor
    (
    struct drm_minor *cur,
    const char *tab
    );

static void show_drm_vma_offset_manager
    (
    struct drm_vma_offset_manager *cur,
    const char *tab
    );

/*******************************************************************************
 *
 * Show routines to get string
 *
 */

static const char *getConnectorTypeStr(uint32_t connector_type)
    {
    switch (connector_type)
        {
        case DRM_MODE_CONNECTOR_VGA:
            return "VGA";
        case DRM_MODE_CONNECTOR_DVII:
            return "DVI-I";
        case DRM_MODE_CONNECTOR_DVID:
            return "DVI-D";
        case DRM_MODE_CONNECTOR_DVIA:
            return "DVI-A";
        case DRM_MODE_CONNECTOR_Composite:
            return "composite";
        case DRM_MODE_CONNECTOR_SVIDEO:
            return "s-video";
        case DRM_MODE_CONNECTOR_LVDS:
            return "LVDS";
        case DRM_MODE_CONNECTOR_Component:
            return "component";
        case DRM_MODE_CONNECTOR_9PinDIN:
            return "9-pin DIN";
        case DRM_MODE_CONNECTOR_DisplayPort:
            return "DP";
        case DRM_MODE_CONNECTOR_HDMIA:
            return "HDMI-A";
        case DRM_MODE_CONNECTOR_HDMIB:
            return "HDMI-B";
        case DRM_MODE_CONNECTOR_TV:
            return "TV";
        case DRM_MODE_CONNECTOR_eDP:
            return "eDP";
        default:
            return "unknown";
        }
    }
#if 0
static const char *getConnectionStr(drmModeConnection connection)
    {
    switch (connection)
        {
        case DRM_MODE_CONNECTED:
            return "connected";
        case DRM_MODE_DISCONNECTED:
            return "disconnected";
        default:
            return "unknown";
        }
    }
#endif
static const char *getEncoderStr(uint32_t encoder_type)
    {
    switch (encoder_type)
        {
        case DRM_MODE_ENCODER_NONE:
            return "none";
        case DRM_MODE_ENCODER_DAC:
            return "DAC";
        case DRM_MODE_ENCODER_TMDS:
            return "TMDS";
        case DRM_MODE_ENCODER_LVDS:
            return "LVDS";
        case DRM_MODE_ENCODER_TVDAC:
            return "TVDAC";
        default:
            return "unknown";
        }
    }

/*******************************************************************************
 *
 * Show routines for list
 *
 */

void show_drm_list_head
    (
    struct list_head *cur,
    struct list_head *head,
    const char *tab
    )
    {
    static int list_count = 0;

    printf("%s%s cur:%p\n", tab, __func__, cur);
    printf("%s%s head:%p\n", tab, __func__, head);
    if (!cur) return;

    printf("%s%s.next:%p\n", tab, __func__, cur->next);
    printf("%s%s.prev:%p\n", tab, __func__, cur->prev);
    printf("%s%s.list_is_last:%d-%d\n", tab, __func__, ++list_count, list_is_last(cur, head));

    if (!list_is_last(cur, head)) show_list_head(cur->next, head, tab);
    list_count = 0;
    }

static void show_encoder_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_encoder *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, head)
        {
        show_drm_encoder(curr, tab2);
        }
    }

static void show_connector_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_connector *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, head)
        {
        show_drm_connector(curr, tab2);
        }
    }

static void show_ctxlist
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_ctx_list *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, head)
        {
        show_drm_ctx_list(curr, tab2);
        }
    }

static void show_vmalist
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_vma_entry *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, head)
        {
        show_drm_vma_entry(curr, tab2);
        }
    }

static void show_filelist
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_file *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, lhead)
        {
        show_drm_file(curr, tab2);
        }
    }

static void show_maplist
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_map_list *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, head)
        {
        show_drm_map_list(curr, tab2);
        }
    }

static void show_legacy_dev_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_device *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, legacy_dev_list)
        {
        printf("%s%s.drm_device:%p\n", tab2, __func__, curr);
        show_drm_device(curr, tab2);
        }
    }
#if 0
static void show_hole_stack
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_mm_node *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, hole_stack)
        {
        show_drm_mm_node(curr, tab2);
        }
    }

static void show_node_list
    (
    struct list_head *cur,
    const char *tab
    )
    {
    struct drm_mm_node *curr, *next;
    SET_CUR;

    show_list_head(cur, cur, tab2);
    if (cur->next == 0) return;
    list_for_each_entry_safe(curr, next, cur, node_list)
        {
        show_drm_mm_node(curr, tab2);
        }
    }
#endif
/*******************************************************************************
 *
 * Show routines for non-DRM struct from atomic.h
 *
 */

void show_kref
    (
    struct kref *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.refcount:%ld\n", tab2, __func__, atomic_read(&(cur->refcount)));
    }

/*******************************************************************************
 *
 * Show routines for non-DRM struct from mm.h
 *
 */

static void show_address_space
    (
    struct address_space *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.flags:%lx\n", tab2, __func__, cur->flags);
    printf("%s%s.private_data:%p\n", tab2, __func__, cur->private_data);
    }

static void show_vm_area_struct
    (
    struct vm_area_struct *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.vm_start:%lx\n", tab2, __func__, cur->vm_start);
    printf("%s%s.vm_start_phys:%lx\n", tab2, __func__, cur->vm_start_phys);
    printf("%s%s.vm_end:%lx\n", tab2, __func__, cur->vm_end);
    printf("%s%s.vm_flags:%lx\n", tab2, __func__, cur->vm_flags);
    }

void show_io_mapping
    (
    struct io_mapping *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.base:%llx\n", tab2, __func__, cur->base);
    printf("%s%s.size:%lx\n", tab2, __func__, cur->size);
    printf("%s%s.prot:%lx\n", tab2, __func__, cur->prot);
    }

/*******************************************************************************
 *
 * Show routines for non-DRM struct from slab.h
 *
 */

void show_kmem_cache
    (
    struct kmem_cache *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.obj_size:%ld\n", tab2, __func__, cur->obj_size);
    printf("%s%s.alignment:%ld\n", tab2, __func__, cur->alignment);
    printf("%s%s.name:%s\n", tab2, __func__, cur->name);
    }

/*******************************************************************************
 *
 * Show routines for non-DRM struct from scatterlist.h
 *
 */

static void show_scatterlist
    (
    struct scatterlist *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.page_link:%lx\n", tab2, __func__, cur->page_link);
    printf("%s%s.offset:%d\n", tab2, __func__, cur->offset);
    printf("%s%s.length:%d\n", tab2, __func__, cur->length);
    printf("%s%s.dma_address:%llx\n", tab2, __func__, cur->dma_address);
    printf("%s%s.index:%d\n", tab2, __func__, cur->index);

    show_sg_table(cur->table, tab2);
    }

void show_sg_table
    (
    struct sg_table *cur,
    const char *tab
    )
    {
    int i;
    SET_CUR;

    printf("%s%s.nents:%d\n", tab2, __func__, cur->nents);
    printf("%s%s.orig_nents:%d\n", tab2, __func__, cur->orig_nents);

    for (i = 0; i < cur->orig_nents; i++)
        {
        show_scatterlist(&(cur->sgl[i]), tab2);
        if (i > 10)
            {
            printf("%s%s skipping\n", tab2, __func__);
            break;
            }
        }
    }

/*******************************************************************************
 *
 * Show routines for non-DRM struct from device.h
 *
 */

static void show_device
    (
    struct device *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_device(cur->parent, tab2);

    printf("%s%s.kobj.name:%s\n", tab2, __func__, cur->kobj.name);
    printf("%s%s.type:%p\n", tab2, __func__, cur->type);
    if (cur->type)
        printf("%s%s.type.name:%s\n", tab2, __func__, cur->type->name);
    printf("%s%s.driver_data:%p\n", tab2, __func__, cur->driver_data);
    printf("%s%s.of_node:%p\n", tab2, __func__, cur->of_node);
    printf("%s%s.devt:%ld\n", tab2, __func__, cur->devt);
    }

static void show_inode
    (
    struct inode *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_address_space(cur->i_mapping, tab2);

    printf("%s%s.i_rdev:%ld\n", tab2, __func__, cur->i_rdev);
    }

static void show_file_operations
    (
    const struct file_operations *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.owner:%p\n", tab2, __func__, cur->owner);
    }

static void show_file
    (
    struct file *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.name:%s\n", tab2, __func__, cur->name);

    show_inode(cur->f_inode, tab2);
    show_file_operations(cur->f_op, tab2);

    printf("%s%s.f_flags:%x\n", tab2, __func__, cur->f_flags);
    printf("%s%s.f_pos:%llx\n", tab2, __func__, cur->f_pos);
    printf("%s%s.private_data:%p\n", tab2, __func__, cur->private_data);

    show_address_space(cur->f_mapping, tab2);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from fb.h
 *
 */

void show_fb_info
    (
    struct fb_info *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.screen_base:%p\n", tab2, __func__, cur->screen_base);
    printf("%s%s.screen_size:%ld\n", tab2, __func__, cur->screen_size);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drm_fb_helper.c
 *
 */

void show_drm_fb_helper
    (
    struct drm_fb_helper *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.crtc_count:%d\n", tab2, __func__, cur->crtc_count);
    printf("%s%s.connector_count:%d\n", tab2, __func__, cur->connector_count);
    printf("%s%s.connector_info_alloc_count:%d\n", tab2, __func__, cur->connector_info_alloc_count);

    show_fb_info(cur->fbdev, tab2);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drm_context.c
 *
 */

static void show_drm_ctx_list
    (
    struct drm_ctx_list *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.handle:%x\n", tab2, __func__, cur->handle);

    show_drm_file(cur->tag, tab2);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drm_crtc.h
 *
 */

static void show_drm_encoder
    (
    struct drm_encoder *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_device(cur->dev, tab2);

    printf("%s%s.name:%s\n", tab2, __func__, cur->name);
    printf("%s%s.encoder_type:%d-%s\n", tab2, __func__,
           cur->encoder_type, getEncoderStr(cur->encoder_type));
    printf("%s%s.possible_crtcs:%d\n", tab2, __func__, cur->possible_crtcs);
    printf("%s%s.possible_clones:%d\n", tab2, __func__, cur->possible_clones);
    }

static void show_drm_connector
    (
    struct drm_connector *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_device(cur->dev, tab2);
    show_device(cur->kdev, tab2);

    printf("%s%s.name:%s\n", tab2, __func__, cur->name);
    printf("%s%s.connector_type:%d-%s\n", tab2, __func__,
           cur->connector_type, getConnectorTypeStr(cur->connector_type));
    printf("%s%s.connector_type_id:%d\n", tab2, __func__, cur->connector_type_id);
    }

static void show_drm_mode_config
    (
    struct drm_mode_config *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.num_fb:%d\n", tab2, __func__, cur->num_fb);
    printf("%s%s.num_connector:%d\n", tab2, __func__, cur->num_connector);

    show_connector_list(&(cur->connector_list), tab2);

    printf("%s%s.num_encoder:%d\n", tab2, __func__, cur->num_encoder);

    show_encoder_list(&(cur->encoder_list), tab2);

    printf("%s%s.num_num_overlay_plane:%d\n", tab2, __func__, cur->num_overlay_plane);
    printf("%s%s.num_total_plane:%d\n", tab2, __func__, cur->num_total_plane);
    printf("%s%s.num_crtc:%d\n", tab2, __func__, cur->num_crtc);
    printf("%s%s.min_width:%d\n", tab2, __func__, cur->min_width);
    printf("%s%s.min_height:%d\n", tab2, __func__, cur->min_height);
    printf("%s%s.max_width:%d\n", tab2, __func__, cur->max_width);
    printf("%s%s.max_height:%d\n", tab2, __func__, cur->max_height);

    printf("%s%s.fb_base:%llx\n", tab2, __func__, cur->fb_base);

    printf("%s%s.poll_enabled:%d\n", tab2, __func__, cur->poll_enabled);
    printf("%s%s.poll_running:%d\n", tab2, __func__, cur->poll_running);
    printf("%s%s.preferred_depth:%d\n", tab2, __func__, cur->preferred_depth);
    printf("%s%s.prefer_shadow:%d\n", tab2, __func__, cur->prefer_shadow);
    printf("%s%s.async_page_flip:%d\n", tab2, __func__, cur->async_page_flip);
    printf("%s%s.cursor_width:%d\n", tab2, __func__, cur->cursor_width);
    printf("%s%s.cursor_height:%d\n", tab2, __func__, cur->cursor_height);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drmP.h
 *
 */

static void show_drm_ioctl_desc
    (
    const struct drm_ioctl_desc *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.cmd:%d\n", tab2, __func__, cur->cmd);
    printf("%s%s.flags:%x\n", tab2, __func__, cur->flags);
    printf("%s%s.func:%p\n", tab2, __func__, cur->func);
    printf("%s%s.name:%s\n", tab2, __func__, cur->name);
    }

static void show_drm_vma_entry
    (
    struct drm_vma_entry *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_vm_area_struct(cur->vma, tab2);

    printf("%s%s.pid:%lx\n", tab2, __func__, cur->pid);
    }

static void show_drm_file
    (
    struct drm_file *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.authenticated:%d\n", tab2, __func__, cur->authenticated);
    printf("%s%s.is_master:%d\n", tab2, __func__, cur->is_master);
    printf("%s%s.stereo_allowed:%d\n", tab2, __func__, cur->stereo_allowed);
    printf("%s%s.universal_planes:%d\n", tab2, __func__, cur->universal_planes);
    printf("%s%s.pid:%p\n", tab2, __func__, cur->pid);
    printf("%s%s.lock_count:%ld\n", tab2, __func__, cur->lock_count);

    show_file(cur->filp, tab2);
    show_drm_master(cur->master, tab2);

    printf("%s%s.event_space:%d\n", tab2, __func__, cur->event_space);
    }

static void show_drm_sg_mem
    (
    struct drm_sg_mem *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.handle:%lx\n", tab2, __func__, cur->handle);
    printf("%s%s.virtual:%p\n", tab2, __func__, cur->virtual);
    printf("%s%s.pages:%d\n", tab2, __func__, cur->pages);
    printf("%s%s.pagelist:%p\n", tab2, __func__, cur->pagelist);
    printf("%s%s.busaddr:%p\n", tab2, __func__, cur->busaddr);
    }

static void show_drm_map_list
    (
    struct drm_map_list *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.user_token:%lld\n", tab2, __func__, cur->user_token);

    show_drm_master(cur->master, tab2);
    }

void show_drm_gem_object
    (
    struct drm_gem_object *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_kref(&(cur->refcount), tab2);

    printf("%s%s.handle_count:%d\n", tab2, __func__, cur->handle_count);

    show_drm_device(cur->dev, tab2);

    show_file(cur->filp, tab2);

    printf("%s%s.size:%ld\n", tab2, __func__, cur->size);
    printf("%s%s.name:%d\n", tab2, __func__, cur->name);
    printf("%s%s.read_domains:%d\n", tab2, __func__, cur->read_domains);
    printf("%s%s.write_domain:%d\n", tab2, __func__, cur->write_domain);
    printf("%s%s.pending_read_domains:%d\n", tab2, __func__, cur->pending_read_domains);
    printf("%s%s.pending_write_domain:%d\n", tab2, __func__, cur->pending_write_domain);
    }

static void show_drm_master
    (
    struct drm_master *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_kref(&(cur->refcount), tab2);

    show_drm_minor(cur->minor, tab2);

    printf("%s%s.unique:%s\n", tab2, __func__, cur->unique);
    printf("%s%s.unique_len:%d\n", tab2, __func__, cur->unique_len);
    printf("%s%s.driver_priv:%p\n", tab2, __func__, cur->driver_priv);
    }

static void show_drm_driver
    (
    struct drm_driver *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.gem_vm_ops:%p\n", tab2, __func__, cur->gem_vm_ops);
    printf("%s%s.major:%d\n", tab2, __func__, cur->major);
    printf("%s%s.minor:%d\n", tab2, __func__, cur->minor);
    printf("%s%s.patchlevel:%d\n", tab2, __func__, cur->patchlevel);
    printf("%s%s.name:%s\n", tab2, __func__, cur->name);
    printf("%s%s.desc:%s\n", tab2, __func__, cur->desc);
    printf("%s%s.date:%s\n", tab2, __func__, cur->date);
    printf("%s%s.driver_features:%x\n", tab2, __func__, cur->driver_features);
    printf("%s%s.dev_priv_size:%d\n", tab2, __func__, cur->dev_priv_size);

    printf("%s%s.num_ioctls:%d\n", tab2, __func__, cur->num_ioctls);
    if (cur->num_ioctls > 0)
        {
        show_drm_ioctl_desc(cur->ioctls, tab2);
        }

    show_file_operations(cur->fops, tab2);

    show_legacy_dev_list(&(cur->legacy_dev_list), tab2);
    }

static void show_drm_minor
    (
    struct drm_minor *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.index:%d\n", tab2, __func__, cur->index);
    printf("%s%s.type:%d\n", tab2, __func__, cur->type);

    show_device(cur->kdev, tab2);
    show_drm_device(cur->dev, tab2);
    show_drm_master(cur->master, tab2);
    }

void show_drm_device
    (
    struct drm_device *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_legacy_dev_list(&(cur->legacy_dev_list), tab2);

    printf("%s%s.if_version:%d\n", tab2, __func__, cur->if_version);

    show_kref(&(cur->ref), tab2);
    show_device(cur->dev, tab2);
    show_drm_driver(cur->driver, tab2);
    show_drm_dev_private(cur->dev_private, tab2);

    show_drm_minor(cur->control, tab2);
    show_drm_minor(cur->primary, tab2);
    show_drm_minor(cur->render, tab2);
    show_inode(cur->anon_inode, tab2);

    printf("%s%s.unique:%s\n", tab2, __func__, cur->unique);
    printf("%s%s.open_count:%d\n", tab2, __func__, cur->open_count);
    printf("%s%s.buf_use:%d\n", tab2, __func__, cur->buf_use);

    show_filelist(&(cur->filelist), tab2);
    show_maplist(&(cur->maplist), tab2);
    show_ctxlist(&(cur->ctxlist), tab2);
    show_vmalist(&(cur->vmalist), tab2);

    printf("%s%s.irq_enabled:%d\n", tab2, __func__, cur->irq_enabled);
    printf("%s%s.irq:%d\n", tab2, __func__, cur->irq);
    printf("%s%s.vblank_disable_allowed:%d\n",
           tab2, __func__, cur->vblank_disable_allowed);
    printf("%s%s.max_vblank_count:%d\n", tab2, __func__, cur->max_vblank_count);
    printf("%s%s.num_crtcs:%d\n", tab2, __func__, cur->num_crtcs);

    show_drm_sg_mem(cur->sg, tab2);
    show_drm_mode_config(&(cur->mode_config), tab2);

    show_drm_vma_offset_manager(cur->vma_offset_manager, tab2);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drm_vma_manager.h
 *
 */

static void show_drm_vma_offset_manager
    (
    struct drm_vma_offset_manager *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_mm(&(cur->vm_addr_space_mm), tab2);
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drm_mm.h
 *
 */

void show_drm_mm_node
    (
    struct drm_mm_node *cur,
    const char *tab
    )
    {
    SET_CUR;

    printf("%s%s.hole_follows:%d\n", tab2, __func__, cur->hole_follows);
    printf("%s%s.scanned_block:%d\n", tab2, __func__, cur->scanned_block);
    printf("%s%s.scanned_prev_free:%d\n", tab2, __func__, cur->scanned_prev_free);
    printf("%s%s.scanned_next_free:%d\n", tab2, __func__, cur->scanned_next_free);
    printf("%s%s.scanned_preceeds_hole:%d\n", tab2, __func__, cur->scanned_preceeds_hole);
    printf("%s%s.allocated:%d\n", tab2, __func__, cur->allocated);
    printf("%s%s.color:%lx\n", tab2, __func__, cur->color);
    printf("%s%s.start:%llx\n", tab2, __func__, cur->start);
    printf("%s%s.size:%llx\n", tab2, __func__, cur->size);
    }
#if 0
static void show_head_node
    (
    struct drm_mm_node *cur,
    const char *tab
    )
    {
    SET_CUR;

    show_drm_mm_node(cur, tab2);

    show_node_list(&(cur->node_list), tab2);
    }
#endif
void show_drm_mm
    (
    struct drm_mm *cur,
    const char *tab
    )
    {
    SET_CUR;
#if 0
    show_hole_stack(&(cur->hole_stack), tab2);

    show_head_node(&(cur->head_node), tab2);
#endif
    printf("%s%s.scan_check_range:%d\n", tab2, __func__, cur->scan_check_range);
    printf("%s%s.scan_alignment:%d\n", tab2, __func__, cur->scan_alignment);
    printf("%s%s.scan_color:%ld\n", tab2, __func__, cur->scan_color);
    printf("%s%s.scan_size:%lld\n", tab2, __func__, cur->scan_size);
    printf("%s%s.scan_hit_start:%lld\n", tab2, __func__, cur->scan_hit_start);
    printf("%s%s.scan_hit_end:%lld\n", tab2, __func__, cur->scan_hit_end);
    printf("%s%s.scanned_blocks:%d\n", tab2, __func__, cur->scanned_blocks);
    printf("%s%s.scan_start:%lld\n", tab2, __func__, cur->scan_start);
    printf("%s%s.scan_end:%lld\n", tab2, __func__, cur->scan_end);

    if (drm_mm_initialized(cur)) drm_mm_debug_table(cur, "");
    }

/*******************************************************************************
 *
 * Show routines for DRM struct from drm_vxworks.h
 *
 */

void show_drm_vxdev
    (
    struct drm_vxdev *cur,
    const char *tab
    )
    {
    SET_CUR;

    if (gfxDrmShowDrmMm)
        {
        show_drm_dev_private(cur->dev->dev_private, tab2);
        if (drm_mm_initialized(&(cur->dev->vma_offset_manager->vm_addr_space_mm)))
            {
            drm_mm_debug_table(&(cur->dev->vma_offset_manager->vm_addr_space_mm), "");
            }
        return;
        }

    printf("%s%s.name:%s\n", tab2, __func__, cur->name);

    show_file(&(cur->filp), tab2);
    show_inode(&(cur->inode), tab2);
    show_drm_device(cur->dev, tab2);
    }
