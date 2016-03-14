/* system.h - system functionality header file*/

/*
 * Copyright (c) 1999-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
04jan16,qsn  Fix hanging problem on Broadwell (US72969)
17sep15,qsn  Re-implement tsc_khz (US66439)
15jul15,yat  Clean up vxoal (US60452)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux system operations.

NOMANUAL
*/

#ifndef _VXOAL_SYSTEM_H_
#define _VXOAL_SYSTEM_H_

#include <taskLib.h> /* for pid_t, taskIdSelf */
#include <intLib.h> /* for INT_CONTEXT */
#include <timerDev.h> /* for sysClkRateGet */
#include <vxoal/krnl/mm.h> /* for mm_struct */
#include <vxoal/krnl/log.h> /* for pr_err */

/* System */

#define SYS_DOWN        0x0001
#define SYS_RESTART     SYS_DOWN
#define SYS_HALT        0x0002
#define SYS_POWER_OFF   0x0003

/* System capabilitities */

#define CAP_SYS_ADMIN   1
#define CAP_SYS_MODULE  16

#define capable(cap) (1)

/* Tasking */

#define TASK_COMM_LEN 16

struct task_struct
    {
    pid_t pid;
    struct mm_struct *mm;
    };

extern struct task_struct current_thread_info;

static inline struct task_struct *get_current
    (
    void
    )
    {
    current_thread_info.pid = (pid_t)taskIdSelf();

    return (struct task_struct *)(&current_thread_info);
    }

static inline struct pid *task_pid
    (
    struct task_struct *tsk
    )
    {
    return (struct pid *)(tsk->pid);
    }

#define current get_current()
#define task_pid_nr(tsk) task_pid(tsk)
#define get_pid(pid) (pid)
#define put_pid(pid)
#define pid_vnr(pid) ((unsigned long)(pid))
#define fput(file)

#define get_task_mm(tsk) (tsk->mm)
#define mmput(mm)
#define mmdrop(mm)

#define cpu_has_pat (0)
#define cpu_has_clflush (0)

/* Do not implement */
#define get_task_struct(task)
#define put_task_struct(task)
#define wake_up_process(p)

/* Must have taskDelay */
#define schedule() taskDelay(1)
#define io_schedule() taskDelay(1)

/* These two do nothing at all */
#define cpu_relax()
#define cpu_relax_lowlatency()

/* Do not schedule timeout as VxWorks jiffies are different and
   return timeout since not scheduled */
#define schedule_timeout(t) ({taskDelay((_Vx_ticks_t)t); t;})
/* Schedule timeout even when VxWorks jiffies are different and
   return 0 as this is called from while loop */
#define schedule_timeout_uninterruptible(t) ({taskDelay((_Vx_ticks_t)t+1); 0;})
#define schedule_timeout_killable(t) schedule_timeout(t)

#define cond_resched()
#define need_resched() (0)
#define signal_pending(t) (0)
#define fatal_signal_pending(t) (0)
#define in_dbg_master() (0)
#define oops_in_progress (0)
#define panic_timeout (0)
#define in_atomic() (0)
#define irqs_disabled() (0)

#define in_interrupt() INT_CONTEXT()

/* User */

typedef unsigned long kuid_t;

#define current_euid() (0)
#define current_user_ns() (0)
#define from_kuid_munged(p1, p2) (0)

/* Notify */

#define NOTIFY_DONE 0
#define NOTIFY_OK   1

struct notifier_block
    {
    int (*notifier_call)(struct notifier_block *, unsigned long, void *);
    struct notifier_block *next;
    int priority;
    };

struct atomic_notifier_head
    {
    };

extern struct atomic_notifier_head panic_notifier_list;

static int atomic_notifier_chain_register
    (
    struct atomic_notifier_head *nh,
    struct notifier_block *n
    )
    {
    return 0;
    }

static int atomic_notifier_chain_unregister
    (
    struct atomic_notifier_head *nh,
    struct notifier_block *n
    )
    {
    return 0;
    }

/* Do not implement */
#define acpi_lid_open() (1)
#define acpi_lid_notifier_register(p1) (0)
#define acpi_lid_notifier_unregister(p1)

/* Do not implement */
#define register_reboot_notifier(p1)
#define unregister_reboot_notifier(p1)

/* Do not implement */
#define register_oom_notifier(p1)
#define unregister_oom_notifier(p1) (0)

/* Do not implement */
#define register_shrinker(p1)
#define unregister_shrinker(p1)

/* sysrq */

struct sysrq_key_op
    {
    };

static inline int register_sysrq_key
    (
    int key,
    struct sysrq_key_op *op
    )
    {
    return 0;
    }

static inline int unregister_sysrq_key
    (
    int key,
    struct sysrq_key_op *op
    )
    {
    return 0;
    }

/* CPU freq */

#define HZ sysClkRateGet()
extern UINT64 sysGetTSCCountPerSec (void);
#define tsc_khz (sysGetTSCCountPerSec()/1000)

struct cpufreq_cpuinfo
    {
    unsigned int max_freq;
    unsigned int min_freq;
    };

struct cpufreq_policy
    {
    struct cpufreq_cpuinfo cpuinfo;
    };

/* Do not implement */
#define cpufreq_cpu_get(p1) (0)
#define cpufreq_cpu_put(p1)

#endif /* _VXOAL_SYSTEM_H_ */
