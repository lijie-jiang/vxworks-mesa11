/* mutex.h - mutex functionality header file*/

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
03sep15,rpc  Implement ww_mutex (US65662)
21jul15,yat  Clean up vxoal (US63380)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux mutex operations.

NOMANUAL
*/

#ifndef _VXOAL_MUTEX_H_
#define _VXOAL_MUTEX_H_

#include <vxWorks.h>
#include <semLib.h> /* for SEM_ID */
#include <vxoal/krnl/log.h> /* for pr_debug */

/* rwlock */

typedef struct rwlock
    {
    SEM_ID      semId;
    } rwlock_t;

/* rw_semaphore */

struct rw_semaphore
    {
    rwlock_t lock;
    };

extern void drm_rwlock_init
    (
    rwlock_t *lock
    );
#define rwlock_init drm_rwlock_init

extern void drm_write_lock
    (
    rwlock_t *lock
    );
#define write_lock drm_write_lock
#define down_write(rw_sem) drm_write_lock(&(rw_sem)->lock)

extern void drm_write_unlock
    (
    rwlock_t *lock
    );
#define write_unlock drm_write_unlock
#define up_write(rw_sem) drm_write_unlock(&(rw_sem)->lock)

extern void drm_read_lock
    (
    rwlock_t *lock
    );
#define read_lock drm_read_lock
#define down_read(rw_sem) drm_read_lock(&(rw_sem)->lock)

extern void drm_read_unlock
    (
    rwlock_t *lock
    );
#define read_unlock drm_read_unlock
#define up_read(rw_sem) drm_read_unlock(&(rw_sem)->lock)

/* mutex */

typedef struct mutex
    {
    SEM_ID      semId;
    int         taken;
    } mutex_t;

#define DEFINE_MUTEX(a) struct mutex a

extern void drm_mutex_init
    (
    struct mutex * pMutex
    );
#define mutex_init drm_mutex_init

extern void drm_mutex_destroy
    (
    struct mutex * pMutex
    );
#define mutex_destroy drm_mutex_destroy

extern void drm_mutex_lock
    (
    struct mutex * pMutex
    );
#define mutex_lock drm_mutex_lock

extern void drm_mutex_unlock
    (
    struct mutex * pMutex
    );
#define mutex_unlock drm_mutex_unlock

extern int drm_mutex_is_locked
    (
    struct mutex *pMutex
    );
#define mutex_is_locked drm_mutex_is_locked

extern int  drm_mutex_lock_interruptible
    (
    struct mutex *pMutex
    );
#define mutex_lock_interruptible drm_mutex_lock_interruptible

extern int drm_mutex_trylock
    (
    struct mutex *pMutex
    );
#define mutex_trylock drm_mutex_trylock

/* Wait/Wound (ww_) mutex for multiple locks in random order must be done by class */

struct ww_class
    {
    const char *name;             /* For debugging */
    };

#define DEFINE_WW_CLASS(classname)                                             \
    struct ww_class classname = { .name = #classname }

struct ww_acquire_ctx
    {
    const char *className;        /* For debugging */
    };

extern void ww_acquire_init
    (
    struct ww_acquire_ctx *aCtx,
    struct ww_class *ww_class
    );

/* ww_acquire_done not implemented at this time. */
#define ww_acquire_done(p1)

extern void ww_acquire_fini
    (
    struct ww_acquire_ctx *aCtx
    );

struct ww_mutex
    {
    struct mutex basic;
    struct ww_acquire_ctx *acquiredCtx; /* For EALREADY/EDEADLK check */
    int acquired;                       /* 0 for not acquired yet */
    };

extern void ww_mutex_init
    (
    struct ww_mutex *lock,
    struct ww_class *wwClass
    );

#if 0 /* Do not enable until ww_mutex is completed to avoid deadlock */

extern int ww_mutex_lock
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    );

extern int ww_mutex_lock_slow
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    );

extern void ww_mutex_unlock
    (
    struct ww_mutex *lock
    );

extern int ww_mutex_is_locked
    (
    struct ww_mutex *lock
    );

extern int ww_mutex_trylock
    (
    struct ww_mutex *lock
    );

extern int ww_mutex_lock_interruptible
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    );

extern int ww_mutex_lock_slow_interruptible
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    );
#endif

#define might_lock(p1) do {} while (0)

#endif /* _VXOAL_MUTEX_H_ */
