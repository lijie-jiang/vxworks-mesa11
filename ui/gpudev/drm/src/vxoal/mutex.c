/* mutex.c - mutex management functions */

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
27mar15,rpc  Static analysis fixes (US50633) semGive and semDelete
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides compatibility functions for the Linux mutex operations.

NOMANUAL

*/

/* includes */

#include <semLib.h> /* for semMCreate */
#include <errno.h> /* for EALREADY and EDEADLK */
#include <vxoal/krnl/mutex.h> /* for extern */
#include <vxoal/krnl/log.h> /* for pr_err */

#if 0
#define DEBUG_WW pr_err
#else
#define DEBUG_WW(...)
#endif

/*******************************************************************************
*
* drm_rwlock_init - initialize a rwlock
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_rwlock_init
    (
    rwlock_t *lock
    )
    {
    lock->semId = semRWCreate(SEM_Q_FIFO, 32);
    if (lock->semId == SEM_ID_NULL)
        {
        pr_err ("semRWCreate failed\n");
        }
    }

/*******************************************************************************
*
* drm_write_lock - take a write lock
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_write_lock
    (
    rwlock_t *lock
    )
    {
    (void) semWTake (lock->semId, WAIT_FOREVER);
    }

/*******************************************************************************
*
* drm_write_unlock - give a write lock
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_write_unlock
    (
    rwlock_t *lock
    )
    {
    (void) semGive (lock->semId);
    }

/*******************************************************************************
*
* drm_read_lock - take a read lock
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_read_lock
    (
    rwlock_t *lock
    )
    {
    (void) semRTake (lock->semId, WAIT_FOREVER);
    }

/*******************************************************************************
*
* drm_read_unlock - give a read lock
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_read_unlock
    (
    rwlock_t *lock
    )
    {
    (void) semGive (lock->semId);
    }

/*******************************************************************************
*
* drm_mutex_init - initialize a mutex
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_mutex_init
    (
    struct mutex *pMutex
    )
    {
    /* Graphics should not invert the priority of a user task */
    pMutex->semId = semMCreate (SEM_Q_FIFO);
    if (pMutex->semId == SEM_ID_NULL)
        {
        pr_err ("semMCreate failed\n");
        }
    pMutex->taken = 0;
    }


/*******************************************************************************
*
* drm_mutex_destroy - destroy a mutex
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_mutex_destroy
    (
    struct mutex *pMutex
    )
    {
    (void) semDelete (pMutex->semId);
    }


/*******************************************************************************
*
* drm_mutex_lock - take a mutex
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_mutex_lock
    (
    struct mutex *pMutex
    )
    {
    STATUS status;

    status = semTake (pMutex->semId, WAIT_FOREVER);
    if (OK == status)
        pMutex->taken = 1;
    }


/*******************************************************************************
*
* drm_mutex_unlock - give a mutex
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_mutex_unlock
    (
    struct mutex *pMutex
    )
    {
    pMutex->taken = 0;
    (void) semGive (pMutex->semId);
    }

/*******************************************************************************
*
* drm_mutex_is_locked - check if mutex is locked
*
* RETURNS: 1 when mutex is taken; 0 otherwise
*
* SEE ALSO:
*/
int drm_mutex_is_locked
    (
    struct mutex *pMutex
    )
    {
    return (pMutex->taken);
    }

/*******************************************************************************
*
* drm_mutex_lock_interruptible - take a mutex
*
* VxWorks mutexes allow for interruptible locks only if they are created
* using the SEM_INTERRUPTIBLE option.  The Linux mutex APIs however are in
* contrast to this; locks are specified as interruptible when they are taken
* rather than created.
*
* At this time the contrast in APIs prevents the use of interruptible mutexes.
* However, current usage of the mutex_lock_interruptible() routine does not
* obligate that mutexes be interruptible.  If this requirement changes,
* revision of the following routine must be revised.
*
* RETURNS: Always 0
*
* SEE ALSO:
*/
int drm_mutex_lock_interruptible
    (
    struct mutex *pMutex
    )
    {
    STATUS status;

    status = semTake (pMutex->semId, WAIT_FOREVER);
    if (OK == status)
        pMutex->taken = 1;

    return (0);
    }

/*******************************************************************************
*
* drm_mutex_trylock - take a mutex, if available
*
* RETURNS: 1 when mutex taken, 0 otherwise
*
* SEE ALSO:
*/
int drm_mutex_trylock
    (
    struct mutex *pMutex
    )
    {
    STATUS status;
    
    status = semTake (pMutex->semId, NO_WAIT);
    if (OK == status)
        pMutex->taken = 1;
    
    return (OK == status);
    }


/*******************************************************************************
*
* ww_acquire_init - Set up for new lock attempts for a given thread.
*
* RETURNS: N/A
*
* SEE ALSO: ww_acquire_done, ww_acquire_fini
*/
void ww_acquire_init
    (
    struct ww_acquire_ctx *aCtx,
    struct ww_class *ww_class
    )
    {
    aCtx->className = ww_class->name;
    DEBUG_WW ("aCtx %p  class name '%s'\n", aCtx, aCtx->className);
    }

/*******************************************************************************
*
* ww_acquire_fini - Indicate that all locks have been released.
*
* RETURNS: N/A
*
* SEE ALSO: ww_acquire_done, ww_acquire_fini
*/
void ww_acquire_fini
    (
    struct ww_acquire_ctx *aCtx
    )
    {
    DEBUG_WW ("aCtx %p  class name '%s'\n", aCtx, aCtx->className);
    aCtx->className = "";
    }

/*******************************************************************************
*
* ww_mutex_init - Init the underlying mutex and any other housekeeping.
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void ww_mutex_init
    (
    struct ww_mutex *lock,
    struct ww_class *wwClass
    )
    {
    DEBUG_WW ("ww_mutex %p\n", lock);
    mutex_init(&lock->basic);
    lock->acquired = 0;         /* Not acquired yet */
    lock->acquiredCtx = NULL;
    }


#if 0 /* Do not enable until ww_mutex is completed to avoid deadlock */

/*******************************************************************************
*
* ww_mutex_lock - take a wait/wound mutex, if available
*
* RETURNS: 0 when mutex taken,
*          -EALREADY if already taken by the same acquired_ctx,
*          -EDEADLK if already taken but by another context,
*          -EAGAIN if something went wrong with mutex_lock.
*
* SEE ALSO:
*/
int ww_mutex_lock
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    )
    {
    DEBUG_WW ("ww_mutex %p locked %d acquired %d aCtx %p class name '%s'\n",
              lock, lock->basic.taken, lock->acquired,
              lock->acquiredCtx,
              lock->acquiredCtx ? lock->acquiredCtx->className : "");

    if (aCtx)
        {
        if (ww_mutex_trylock(lock))
            if (lock->acquired)
                {
                if (lock->acquiredCtx == aCtx)  /* recursion ? */
                    return -EALREADY;
                else
                    {
                    DEBUG_WW ("took it but already owned: aCtx %p class '%s'\n",
                              lock->acquiredCtx, lock->acquiredCtx->className);
                    lock->acquiredCtx = aCtx;  /* Steal it, shouldn't happen */
                    return 0;
                    }
                }
            else  /* Usual path: not previously taken */
                {
                lock->acquired = 1;
                lock->acquiredCtx = aCtx;   /* Remember it is ours */
                return 0;
                }
        else    /* We did not get it */
            if (lock->acquired)     /* Already taken ? */
                {
                 DEBUG_WW ("DEADLOCK - requesting aCtx %p class name '%s'\n",
                           aCtx, aCtx->className);
                return -EDEADLK;
                }
            else
                return -EAGAIN;  /* other mutex_lock error */
        }

    mutex_lock (&lock->basic);
    return 0;
    }

/*******************************************************************************
*
* ww_mutex_lock_slow - Wait forever on a lock after releasing all others in same class.
*
* RETURNS: 0 when mutex taken
*
* SEE ALSO:
*/
int ww_mutex_lock_slow
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    )
    {
    DEBUG_WW ("ww_mutex %p  aCtx %p\n", lock, aCtx);
    return ww_mutex_lock (lock, aCtx);
    }

/*******************************************************************************
*
* ww_mutex_unlock - release a wait/wound mutex
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void ww_mutex_unlock
    (
    struct ww_mutex *lock
    )
    {
    DEBUG_WW ("ww_mutex %p\n", lock);
    lock->acquired = 0;             /* No longer acquired */
    lock->acquiredCtx = NULL;
    mutex_unlock (&lock->basic);
    }

/*******************************************************************************
*
* ww_mutex_is_locked - Check if given mutex is in use.
*
* RETURNS: Status of lock
*
* SEE ALSO:
*/
int ww_mutex_is_locked
    (
    struct ww_mutex *lock
    )
    {
    DEBUG_WW ("ww_mutex %p acquired %d acquiredCtx %p class name '%s'\n",
              lock, lock->acquired, lock->acquiredCtx,
              lock->acquiredCtx ? lock->acquiredCtx->className : "");
    return mutex_is_locked (&lock->basic);
    }

/*******************************************************************************
*
* ww_mutex_trylock - Try to acquire ww lock without waiting
*
* RETURNS: Underlying mutex_trylock return value
*
* SEE ALSO:
*/
int ww_mutex_trylock
    (
    struct ww_mutex *lock
    )
    {
    /* Does not check ctx to determine EALREADY or EDEADLK conditions. */
    /* Only called through DRM modeset_lock for panic handlers so 'should' be okay.  */
    DEBUG_WW ("ww_mutex %p\n", lock);
    return mutex_trylock (&lock->basic);
    }

/*******************************************************************************
*
* ww_mutex_lock_interruptible - take a wait/wound mutex
*
* RETURNS: 0 when mutex taken
*
* SEE ALSO: mutex_lock_interruptible comments
*/
int ww_mutex_lock_interruptible
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    )
    {
    DEBUG_WW ("ww_mutex %p  aCtx %p\n", lock, aCtx);
    return ww_mutex_lock (lock, aCtx);
    }

/*******************************************************************************
*
* ww_mutex_lock_slow_interruptible - take a wait/wound mutex, if available
*
* RETURNS: 0 when mutex taken
*
* SEE ALSO:
*/
int ww_mutex_lock_slow_interruptible
    (
    struct ww_mutex *lock,
    struct ww_acquire_ctx *aCtx
    )
    {
    DEBUG_WW ("ww_mutex %p  aCtx %p\n", lock, aCtx);
    return ww_mutex_lock_interruptible (lock, aCtx);
    }
#endif
