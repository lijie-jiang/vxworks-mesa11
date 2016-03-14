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

#include <mod_devicetable.h> /* for pci_device_id */
#include <drm/drmP.h> /* for drm_driver, drm_pci_init */
#include <drm/drm_vxworks.h> /* for drm_vx_pci_init */
#include <vxoal/krnl/pm.h> /* for dev_pm_ops */
#include <vxoal/krnl/pci.h> /* for pci_driver, pci_dev */
#include <drm/drm_gem.h>
#include <drm/drm_crtc_helper.h>

#define DRIVER_MAJOR            1
#define DRIVER_MINOR            0
#define DRIVER_PATCHLEVEL       0
#define DRIVER_NAME             "sample"
#define DRIVER_DESC             "Sample"
#define DRIVER_DATE             "20150928"

#define SAMPLE_VGA_DEVICE(id, info)                                            \
    {                                                                          \
    .class = 0x030000,                                                         \
    .class_mask = 0xff0000,                                                    \
    .vendor = 0x8086,                                                          \
    .device = id,                                                              \
    .subvendor = PCI_ANY_ID,                                                   \
    .subdevice = PCI_ANY_ID,                                                   \
    .driver_data = (unsigned long) info                                        \
    }

static const struct pci_device_id pciidlist[] =
    {
    SAMPLE_VGA_DEVICE(0x166, NULL),
    {0, 0, 0}
    };

const struct pci_device_id *gfxDrmDevPciList
    (
    void
    )
    {
    return pciidlist;
    }

struct drm_sample_private
    {
    struct drm_device *dev;
    };

static struct drm_framebuffer *sample_fb_create
    (
    struct drm_device *dev,
    struct drm_file *filp,
    struct drm_mode_fb_cmd2 *mode
    )
    {
    return NULL;
    }

static void sample_output_poll_changed
    (
    struct drm_device *dev
    )
    {
    }

static const struct drm_mode_config_funcs sample_mode_funcs =
    {
    .fb_create = sample_fb_create,
    .output_poll_changed = sample_output_poll_changed,
    };

static int sample_driver_load
    (
    struct drm_device *dev,
    unsigned long flags
    )
    {
    struct drm_sample_private *dev_priv;

    dev_priv = calloc(1, sizeof(*dev_priv));
    if (dev_priv == NULL)
        {
        return -ENOMEM;
        }

    dev->dev_private = dev_priv;
    dev_priv->dev = dev;

    drm_mode_config_init(dev);

    dev->mode_config.funcs = &sample_mode_funcs;

    drm_kms_helper_poll_init(dev);

    return 0;
    }

static int sample_driver_unload
    (
    struct drm_device *dev
    )
    {
    drm_kms_helper_poll_fini(dev);

    drm_mode_config_cleanup(dev);

    free(dev->dev_private);
    dev->dev_private = NULL;

    return 0;
    }

static int sample_driver_open
    (
    struct drm_device *dev,
    struct drm_file *file
    )
    {
    return 0;
    }

static const struct drm_ioctl_desc sample_ioctls[] =
    {
    };

static const struct file_operations sample_driver_fops =
    {
    .open = drm_open,
    .release = drm_release,
    .unlocked_ioctl = drm_ioctl,
    .mmap = drm_gem_mmap,
    .poll = drm_poll,
    .read = drm_read,
    .llseek = noop_llseek,
    };

static struct drm_driver driver =
    {
    .driver_features =
        DRIVER_HAVE_IRQ | DRIVER_IRQ_SHARED | DRIVER_GEM | DRIVER_PRIME |
        DRIVER_RENDER,
    .load = sample_driver_load,
    .unload = sample_driver_unload,
    .open = sample_driver_open,
    .ioctls = sample_ioctls,
    .num_ioctls = ARRAY_SIZE(sample_ioctls),
    .fops = &sample_driver_fops,
    .name = DRIVER_NAME,
    .desc = DRIVER_DESC,
    .date = DRIVER_DATE,
    .major = DRIVER_MAJOR,
    .minor = DRIVER_MINOR,
    .patchlevel = DRIVER_PATCHLEVEL,
    };

static int sample_pci_probe
    (
    struct pci_dev *pdev,
    const struct pci_device_id *ent
    )
    {
    if (PCI_FUNC(pdev->devfn))
        return -ENODEV;

    return drm_get_pci_dev(pdev, ent, &driver);
    }

static void sample_pci_remove
    (
    struct pci_dev *pdev
    )
    {
    struct drm_device *dev = pci_get_drvdata(pdev);

    drm_put_dev(dev);
    }

static const struct dev_pm_ops sample_pm_ops =
    {
    };

static struct pci_driver sample_pci_driver =
    {
    .name = DRIVER_NAME,
    .id_table = pciidlist,
    .probe = sample_pci_probe,
    .remove = sample_pci_remove,
    .driver.pm = &sample_pm_ops,
    };

int gfxDrmDevInit(void)
    {
    driver.driver_features |= DRIVER_MODESET;

    return drm_pci_init(&driver, &sample_pci_driver);
    }

void gfxDrmDevDeinit(void)
    {
    }
