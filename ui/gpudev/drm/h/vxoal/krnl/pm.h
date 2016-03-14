/* pm.h - pm functionality header file*/

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
15jul15,yat  Clean up vxoal (US60452)
25jun15,yat  Add PM_EVENT defines (US24946)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux pm operations.

NOMANUAL
*/

#ifndef _VXOAL_PM_H_
#define _VXOAL_PM_H_

#include <vxoal/krnl/device.h> /* for device */

#define PM_EVENT_FREEZE  0x0001 
#define PM_EVENT_SUSPEND 0x0002
#define PM_EVENT_PRETHAW 0x0008

#define PMSG_FREEZE      ((struct pm_message){ .event = PM_EVENT_FREEZE, })
#define PMSG_SUSPEND     ((struct pm_message){ .event = PM_EVENT_SUSPEND, })

typedef struct pm_message
    {
    int                         event;
    } pm_message_t;

struct device;

struct dev_pm_ops
    {
    int (*prepare)(struct device *dev);
    void (*complete)(struct device *dev);
    int (*suspend)(struct device *dev);
    int (*resume)(struct device *dev);
    int (*freeze)(struct device *dev);
    int (*thaw)(struct device *dev);
    int (*poweroff)(struct device *dev);
    int (*restore)(struct device *dev);
    int (*suspend_late)(struct device *dev);
    int (*resume_early)(struct device *dev);
    int (*freeze_late)(struct device *dev);
    int (*thaw_early)(struct device *dev);
    int (*poweroff_late)(struct device *dev);
    int (*restore_early)(struct device *dev);
    int (*suspend_noirq)(struct device *dev);
    int (*resume_noirq)(struct device *dev);
    int (*freeze_noirq)(struct device *dev);
    int (*thaw_noirq)(struct device *dev);
    int (*poweroff_noirq)(struct device *dev);
    int (*restore_noirq)(struct device *dev);
    int (*runtime_suspend)(struct device *dev);
    int (*runtime_resume)(struct device *dev);
    int (*runtime_idle)(struct device *dev);
    };

struct pm_qos_request
    {
    };

static inline int pm_dummy_function
    (
    struct device *dev
    )
    {
    return 0;
    }

/* Do not implement */
#define pm_runtime_disable pm_dummy_function
#define pm_runtime_set_active pm_dummy_function
#define pm_runtime_get_sync pm_dummy_function
#define pm_runtime_get_noresume pm_dummy_function
#define pm_runtime_mark_last_busy pm_dummy_function
#define pm_runtime_set_autosuspend_delay(pm, delay) pm_dummy_function(pm)
#define pm_runtime_put_autosuspend pm_dummy_function
#define pm_runtime_use_autosuspend pm_dummy_function

/* Do not implement */
#define pm_qos_add_request(qos, pm_class, value)
#define pm_qos_remove_request(qos)
#define pm_qos_update_request(qos, value)

/* Do not implement */
#define pm_generic_suspend pm_dummy_function
#define pm_generic_resume pm_dummy_function
#define pm_generic_runtime_suspend pm_dummy_function
#define pm_generic_runtime_resume pm_dummy_function
#define pm_generic_freeze pm_dummy_function
#define pm_generic_thaw pm_dummy_function
#define pm_generic_poweroff pm_dummy_function
#define pm_generic_restore pm_dummy_function

#endif /* _VXOAL_PM_H_ */
