/* device.h - device functionality header file*/

/*
 * Copyright (c) 1999-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
17sep15,qsn  Clean up seq_printf and seq_puts (US66439)
15jul15,yat  Clean up vxoal (US60452)
10jun15,rpc  Stubs for irq routines (US59495)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux device operations.

NOMANUAL
*/

#ifndef _VXOAL_DEVICE_H_
#define _VXOAL_DEVICE_H_

#include <vxoal/krnl/types.h> /* for kasprintf, __stringify, loff_t */
#include <vxoal/krnl/mm.h> /* for address_space, vm_area_struct, GFP_KERNEL */
#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/pm.h> /* for dev_pm_ops */

/* Device */

typedef unsigned long dev_t;

typedef unsigned short umode_t;

struct kobject
    {
    /* Do not use pointer as name may be used unmalloc */
    char                        name[32];
    };

struct device;
struct device_driver;

struct device_type
    {
    const char                  *name;
    void (*release)(struct device *dev);
    };

struct bus_type
    {
    const char                  *name;
    const struct dev_pm_ops     *pm;
    int (*match) (struct device *dev, struct device_driver *drv);
    };

struct device_node
    {
    const char                  *full_name;
    };

struct class
    {
    };

struct platform_device
    {
    };

struct virtio_device
    {
    };

struct device
    {
    struct device               *parent;
    struct kobject              kobj;
    const struct device_type    *type;
    struct bus_type             *bus;
    struct device_driver        *driver;
    void                        *driver_data;
    struct device_node          *of_node;
    dev_t                       devt;
    };

struct device_driver
    {
    struct bus_type             *bus;
    struct module               *owner;
    const struct dev_pm_ops     *pm;
    int (*probe) (struct device *dev);
    int (*remove) (struct device *dev);
    void (*shutdown) (struct device *dev);
    };

#define device_initialize(dev)
#define device_del(dev)
#define put_device(dev)

#define dev_name(dev) ((dev)->kobj.name)
#define dev_set_name(dev, fmt, ...)
#define MAJOR(dev) ((dev) >> 8)
#define MINOR(dev) ((dev) & 0xff)
#define old_encode_dev(dev) ((MAJOR(dev) << 8) | MINOR(dev))

#define dev_get_drvdata(dev) ((dev)->driver_data)
#define dev_set_drvdata(dev, data) ((dev)->driver_data = (data))

#define device_unregister(dev)
#define device_is_registered(dev) (1)

#define driver_register(drv) (0)
#define driver_unregister(drv)

#define bus_register(bus) (0)

#define kobject_uevent_env(kobj, action, envp) (*(envp)=*(envp))

#define register_chrdev(major, name, fops) (0)
#define unregister_chrdev(major, name)

#define bus_find_device(bus, dev, data, match) (NULL)

#define of_node_put(node)
#define of_node_get(node) (NULL)
#define of_property_read_u32(node, name, p) ({*(p) = 0;0;})
#define of_find_property(node, name, p) (0)
#define of_driver_match_device(dev, drv) (0)
#define for_each_available_child_of_node(of_node, node) while (0)
#define device_for_each_child(dev, p, func)

/* IRQ */

#define IRQF_SHARED             0x00000080

enum irqreturn
    {
    IRQ_NONE,
    IRQ_HANDLED,
    IRQ_WAKE_THREAD
    };

typedef enum irqreturn irqreturn_t;

typedef irqreturn_t (*irq_handler_t) (void*);

#define synchronize_irq(irq)                                                    

/* Do not implement */
#define local_irq_enable()
#define local_irq_disable()
#define local_irq_save(flags)    (flags=flags)
#define local_irq_restore(flags) (flags=flags)

/* File operations */

struct module
    {
    };

struct inode
    {
    struct address_space        *i_mapping;
    dev_t                       i_rdev;
    };

#define iminor(inode) MINOR(inode->i_rdev)

struct file;

struct poll_table_struct
    {
    };

struct file_operations
    {
    struct module               *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    unsigned int (*poll) (struct file *, struct poll_table_struct *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    int (*open) (struct inode *, struct file *);
    int (*release) (struct inode *, struct file *);
    };

struct file
    {
    const char                  *name;
    struct inode                *f_inode;
    const struct file_operations *f_op;
    unsigned int                f_flags;
    loff_t                      f_pos;
    void                        *private_data;
    struct address_space        *f_mapping;
    };

#define file_inode(file)        ((file)->f_inode)
#define fops_get(_fops)         (_fops)
#define replace_fops(_f, _fops) ((_f)->f_op = (_fops)) 

static inline loff_t noop_llseek
    (
    struct file *file,
    loff_t offset,
    int whence
    )
    {
    return file->f_pos;
    }

/* Attributes */

struct attribute
    {
    const char                  *name;
    umode_t                     mode;
    };

struct vm_area_struct;

struct bin_attribute
    {
    struct attribute            attr;
    size_t                      size;
    void                        *private;
    ssize_t (*read)(struct file *, struct kobject *, struct bin_attribute *,
                    char *, loff_t, size_t);
    ssize_t (*write)(struct file *, struct kobject *, struct bin_attribute *,
                     char *, loff_t, size_t);
    int (*mmap)(struct file *, struct kobject *, struct bin_attribute *attr,
                struct vm_area_struct *vma);
    };

struct device_attribute
    {
    struct attribute            attr;
    ssize_t (*show)(struct device *dev, struct device_attribute *attr,
                    char *buf);
    ssize_t (*store)(struct device *dev, struct device_attribute *attr,
                     const char *buf, size_t count);
    };

struct attribute_group
    {
    const char                  *name;
    umode_t                     (*is_visible)(struct kobject *,
                                              struct attribute *, int);
    struct attribute            **attrs;
    struct bin_attribute        **bin_attrs;
    };

#define __ATTR(_name, _mode, _show, _store)                                    \
    {                                                                          \
    .attr = { .name = __stringify(_name), .mode = (_mode) },                   \
    .show = _show,                                                             \
    .store = _store,                                                           \
    }

#define __ATTR_RO(_name)                                                       \
    {                                                                          \
    .attr = { .name = __stringify(_name), .mode = S_IRUGO },                   \
    }

#define __ATTR_RW(_name)                                                       \
    {                                                                          \
    .attr = { .name = __stringify(_name), .mode = (S_IWUSR | S_IRUGO) },       \
    }

#define DEVICE_ATTR(_name, _mode, _show, _store)                               \
    struct device_attribute dev_attr_##_name =                                 \
        __ATTR(_name, _mode, _show, _store)

#define DEVICE_ATTR_RO(_name)                                                  \
    struct device_attribute dev_attr_##_name = __ATTR_RO(_name)

#define DEVICE_ATTR_RW(_name)                                                  \
    struct device_attribute dev_attr_##_name = __ATTR_RW(_name)

/* SEQ file */

struct seq_file
    {
    };

#define seq_printf(m, args...) pr_info(args)
#define seq_puts(m, args...) pr_info(args)

/* Sysfs */

#define sysfs_create_link(kobj, target, name) (0)
#define sysfs_remove_link(kobj, name)
#define sysfs_create_files(kobj, attr) (0)
#define sysfs_remove_files(kobj, attr)
#define sysfs_create_bin_file(kobj, attr) (0)
#define sysfs_remove_bin_file(kobj, attr)
#define device_create_bin_file(dev, attr) (0)
#define device_remove_bin_file(dev, attr)

/* Debugfs */

struct dentry
    {
    };

#define debugfs_create_dir(name, p)                                            \
    ((struct dentry*)kcalloc(1, sizeof(struct dentry), 0))
#define debugfs_remove(d) (void)kfree(d)

/* Poll */

#define POLLIN          0x0001
#define POLLRDNORM      0x0040

/* Do not implement */
#define poll_wait(file, wait, p)

/* Firmware */

struct firmware
    {
    size_t size;
    unsigned char *data;
    };

#define request_firmware(fwp, name, dev) ({WARN_DEV; *(fwp) = NULL; 1;})
#define release_firmware(fwp)

#endif /* _VXOAL_DEVICE_H_ */
