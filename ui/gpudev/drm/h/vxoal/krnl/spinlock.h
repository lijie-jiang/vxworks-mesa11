/* spinlock.h - spinlock functionality header file*/

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
21jul15,yat  Clean up vxoal (US63380)
09mar15,qsn  Added spin_is_locked (US50613)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux spinlock operations.

NOMANUAL
*/

#ifndef _VXOAL_SPINLOCK_H
#define _VXOAL_SPINLOCK_H

#include <spinLockLib.h> /* for spinlockIsr_t */
#include <private/spinLockLibP.h> /* for spinLockIsrHeld */
#include <vxoal/krnl/log.h> /* for BUG_ON */

#define spinlock_t              spinlockIsr_t
#define DEFINE_SPINLOCK(x)      SPIN_LOCK_ISR_DECL(x,0)
#define spin_lock_init(x)       SPIN_LOCK_ISR_INIT(x,0)

#define spin_lock(x)            taskCpuLock(); SPIN_LOCK_ISR_TAKE(x);
#define spin_unlock(x)          SPIN_LOCK_ISR_GIVE(x); taskCpuUnlock();
#define spin_lock_irq(x)        taskCpuLock(); SPIN_LOCK_ISR_TAKE(x);
#define spin_unlock_irq(x)      SPIN_LOCK_ISR_GIVE(x); taskCpuUnlock();
#define spin_lock_irqsave(x,f)  taskCpuLock(); SPIN_LOCK_ISR_TAKE(x);f=0;
#define spin_unlock_irqrestore(x,f) SPIN_LOCK_ISR_GIVE(x);f=f;taskCpuUnlock();
#define spin_lock_bh(x)         taskCpuLock(); SPIN_LOCK_ISR_TAKE(x);
#define spin_unlock_bh(x)       SPIN_LOCK_ISR_GIVE(x); taskCpuUnlock();
#if defined(_WRS_CONFIG_SMP)
#define spin_is_locked(x)       spinLockIsrHeld(x)
#else
/* always 1 since there is only one CPU for UP */
#define spin_is_locked(x)       (1)
#endif
#define assert_spin_locked(x)   BUG_ON(!spin_is_locked(x))
#define lockdep_assert_held(x)  do { (void)(x); } while (0)

#endif /* _VXOAL_SPINLOCK_H */
