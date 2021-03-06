/*
 * Copyright © 2014 Intel Corporation
 *   Daniel Vetter <daniel.vetter@ffwll.ch>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
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
11nov15,yat  Port for VxWorks 7 (US67564)
*/

/* drm_irq.c */
extern unsigned int drm_timestamp_monotonic;

/* drm_fops.c */
extern struct mutex drm_global_mutex;
int drm_lastclose(struct drm_device *dev);

/* drm_pci.c */
int drm_pci_set_unique(struct drm_device *dev,
		       struct drm_master *master,
		       struct drm_unique *u);
int drm_irq_by_busid(struct drm_device *dev, void *data,
		     struct drm_file *file_priv);

/* drm_vm.c */
int drm_vma_info(struct seq_file *m, void *data);
void drm_vm_open_locked(struct drm_device *dev, struct vm_area_struct *vma);
void drm_vm_close_locked(struct drm_device *dev, struct vm_area_struct *vma);

/* drm_prime.c */
int drm_prime_handle_to_fd_ioctl(struct drm_device *dev, void *data,
				 struct drm_file *file_priv);
int drm_prime_fd_to_handle_ioctl(struct drm_device *dev, void *data,
				 struct drm_file *file_priv);

void drm_prime_init_file_private(struct drm_prime_file_private *prime_fpriv);
void drm_prime_destroy_file_private(struct drm_prime_file_private *prime_fpriv);
void drm_prime_remove_buf_handle_locked(struct drm_prime_file_private *prime_fpriv,
					struct dma_buf *dma_buf);

/* drm_info.c */
int drm_name_info(struct seq_file *m, void *data);
int drm_vm_info(struct seq_file *m, void *data);
int drm_bufs_info(struct seq_file *m, void *data);
int drm_clients_info(struct seq_file *m, void* data);
int drm_gem_name_info(struct seq_file *m, void *data);

/* drm_irq.c */
int drm_control(struct drm_device *dev, void *data,
		struct drm_file *file_priv);
int drm_modeset_ctl(struct drm_device *dev, void *data,
		    struct drm_file *file_priv);

/* drm_auth.c */
int drm_getmagic(struct drm_device *dev, void *data,
		 struct drm_file *file_priv);
int drm_authmagic(struct drm_device *dev, void *data,
		  struct drm_file *file_priv);

/* drm_sysfs.c */
extern struct class *drm_class;
#if defined(__vxworks)
#define drm_sysfs_init() (0)
#define drm_sysfs_destroy()
static inline struct device *drm_sysfs_minor_alloc(struct drm_minor *minor)
    {
    struct device *kdev = (struct device*)kcalloc (1, sizeof (struct device), GFP_KERNEL);
    if (!kdev) return ERR_PTR(-ENOMEM);
    kdev->parent = minor->dev->dev;
    dev_set_drvdata(kdev, minor);
    return kdev;
    }
#define drm_sysfs_connector_add(connector) (0)
#define drm_sysfs_connector_remove(connector)
#else
int drm_sysfs_init(void);
void drm_sysfs_destroy(void);
struct device *drm_sysfs_minor_alloc(struct drm_minor *minor);
int drm_sysfs_connector_add(struct drm_connector *connector);
void drm_sysfs_connector_remove(struct drm_connector *connector);
#endif /* __vxworks */

/* drm_gem.c */
int drm_gem_init(struct drm_device *dev);
void drm_gem_destroy(struct drm_device *dev);
int drm_gem_handle_create_tail(struct drm_file *file_priv,
			       struct drm_gem_object *obj,
			       u32 *handlep);
int drm_gem_close_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int drm_gem_flink_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int drm_gem_open_ioctl(struct drm_device *dev, void *data,
		       struct drm_file *file_priv);
void drm_gem_open(struct drm_device *dev, struct drm_file *file_private);
void drm_gem_release(struct drm_device *dev, struct drm_file *file_private);

/* drm_drv.c */
int drm_setmaster_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv);
int drm_dropmaster_ioctl(struct drm_device *dev, void *data,
			 struct drm_file *file_priv);
struct drm_master *drm_master_create(struct drm_minor *minor);

/* drm_debugfs.c */
#if defined(CONFIG_DEBUG_FS)
int drm_debugfs_init(struct drm_minor *minor, int minor_id,
		     struct dentry *root);
int drm_debugfs_cleanup(struct drm_minor *minor);
int drm_debugfs_connector_add(struct drm_connector *connector);
void drm_debugfs_connector_remove(struct drm_connector *connector);
#else
static inline int drm_debugfs_init(struct drm_minor *minor, int minor_id,
				   struct dentry *root)
{
	return 0;
}

static inline int drm_debugfs_cleanup(struct drm_minor *minor)
{
	return 0;
}

static inline int drm_debugfs_connector_add(struct drm_connector *connector)
{
	return 0;
}
static inline void drm_debugfs_connector_remove(struct drm_connector *connector)
{
}
#endif
