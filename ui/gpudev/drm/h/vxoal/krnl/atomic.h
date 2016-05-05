/* atomic.h - atomic functionality header file */

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
04mar16,qsn  Re-wrote atomic_cmpxchg and add atomic_add_unless (US76479)
19jun15,rpc  Re-wrote atomic_cmpxchg (US50495)
09mar15,qsn  Added more atomic operations (US50613)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION
This file provides compatibility functions for the Linux atomic operations.

NOMANUAL
*/

#ifndef _VXOAL_ATOMIC_H
#define _VXOAL_ATOMIC_H

#include <vxAtomicLib.h>
#include <vxoal/krnl/types.h> /* for inline */
#include <vxoal/krnl/mutex.h> /* for mutex_lock */

/* General purpose atomic counter */
struct kref
    {
    atomic_t refcount;
    };

/* Atomic operation conversions */

#ifdef _WRS_CONFIG_LP64
#define atomic_inc(v)             atomic64Inc(v)
#define atomic_dec(v)             atomic64Dec(v)
#define atomic_read(v)            atomic64Get(v)
#define atomic_add(i, v)          atomic64Add(v, (atomic64Val_t)i)
#define atomic_sub(i, v)          atomic64Sub(v, (atomic64Val_t)i)
#define atomic_set(v, n)          atomic64Set(v, (atomic64Val_t)n)
#define atomic_cas(v, o, n)       atomic64Cas(v, (atomic64Val_t)o, (atomic64Val_t)n)
#else
#define atomic_inc(v)             atomic32Inc(v)
#define atomic_dec(v)             atomic32Dec(v)
#define atomic_read(v)            atomic32Get(v)
#define atomic_add(i, v)          atomic32Add(v, (atomic32Val_t)i)
#define atomic_sub(i, v)          atomic32Sub(v, (atomic32Val_t)i)
#define atomic_set(v, n)          atomic32Set(v, (atomic32Val_t)n)
#define atomic_cas(v, o, n)       atomic32Cas(v, (atomic32Val_t)o, (atomic32Val_t)n)
#endif

/*******************************************************************************
*
* atomic_dec_and_test - atomically decrements v by 1
*
* RETURNS: 1 if the result is 0, or 0 otherwise
*
* SEE ALSO:
*/
static inline int atomic_dec_and_test
    (
    atomic_t *v
    )
    {
    return ((atomic_dec(v) - 1) == 0);
    }

/*******************************************************************************
*
* atomic_add_return - atomically adds v by i
*
* RETURNS: atomically added sum
*
* SEE ALSO:
*/
static inline int atomic_add_return
    (
    int i,
    atomic_t *v
    )
    {
    return ((int)atomic_add(i, v) + i);
    }

/*******************************************************************************
*
* atomic_cmpxchg - compare with old value and if the same, swap with new
*
* RETURNS: old value
*
* SEE ALSO:
*/
static inline long atomic_cmpxchg
    (
    atomic_t *v,
    long oldVal,
    long newVal
    )
    {
    long ret = atomic_read(v);
    (void) atomic_cas(v, oldVal, newVal);
    return ret;
    }

/*******************************************************************************
*
* atomic_xchg - swap with new
*
* RETURNS: old value
*
* SEE ALSO:
*/
static inline long atomic_xchg
    (
    atomic_t *v,
    long newVal
    )
    {
    long oldVal;

    do
        {
        oldVal = atomic_read(v);
        } while (atomic_cmpxchg(v, oldVal, newVal) != oldVal);

    return oldVal;
    }

/*******************************************************************************
*
* atomic_set_mask -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void atomic_set_mask
    (
    unsigned long i,
    atomic_t *v
    )
    {
    unsigned long oldVal;
    unsigned long newVal;

    do
        {
        oldVal = atomic_read(v);
        newVal = oldVal | i;
        } while (atomic_cmpxchg(v, oldVal, newVal) != oldVal);
    }

/*******************************************************************************
*
* atomic_or - Add more bits to current value
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void atomic_or
    (
    long more,
    atomic_t *v
    )
    {
    long oldVal;
    long newVal;
    do
        {
        oldVal = atomic_read(v);
        newVal = oldVal | more;
        } while (atomic_cmpxchg (v, oldVal, newVal) != oldVal);
    }

/*******************************************************************************
*
* kref_set -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void kref_set
    (
    struct kref *kref,
    int value
    )
    {
    (void) atomic_set(&(kref->refcount), value);
    }

/*******************************************************************************
*
* kref_get -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void kref_get
    (
    struct kref *kref
    )
    {
    (void) atomic_inc(&(kref->refcount));
    }

/*******************************************************************************
*
* kref_put - 
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/
static int kref_put
    (
    struct kref *kref,
    void (*release)(struct kref *kref)
    )
    {
    if (atomic_dec_and_test(&(kref->refcount)))
        {
        release(kref);
        return 1;
        }

    return 0;
    }

/*******************************************************************************
*
* atomic_add_unless -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline long atomic_add_unless (atomic_t *v, long a, long u)
{
    long c, old;
    c = atomic_read(v);
    while (c != u && (old = atomic_cmpxchg(v, c, c + a)) != c)
            c = old;
    return (long)(c != u);
}


/*******************************************************************************
*
* kref_put_mutex - 
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/
static int kref_put_mutex
    (
    struct kref *kref,
    void (*release)(struct kref *kref),
    struct mutex *lock
    )
    {
    if (! atomic_add_unless(&(kref->refcount), -1, 1))
        {
        mutex_lock(lock);
        if (! atomic_dec_and_test(&(kref->refcount)))
            {
            mutex_unlock(lock);
            return 0;
            }
        release(kref);
        return 1;
        }
    return 0;
    }

/*******************************************************************************
*
* kref_init -
*
* RETURNS: N/A
*
* SEE ALSO:
*/
static inline void kref_init
    (
    struct kref *kref
    )
    {
    kref_set(kref, 1);
    }

/*******************************************************************************
*
* kref_get_unless_zero -
*
* RETURNS: int
*
* SEE ALSO:
*/
static inline int kref_get_unless_zero
    (
    struct kref *kref
    )
    {
    return (int)atomic_add_unless(&(kref->refcount), 1, 0);
    }

#define atomic_inc_not_zero(v) atomic_add_unless((v), 1, 0)

/* Atomic bit operations */

extern void gfxDrmBitOpsInit
    (
    void
    );

extern void drm_clear_bit
    (
    int bitNbr,
    volatile unsigned long *addr
    );

extern void drm_change_bit
    (
    int bitNbr,
    volatile unsigned long *addr
    );

extern void drm_set_bit
    (
    int bitNbr,
    volatile unsigned long *addr
    );

extern int drm_test_and_change_bit
    (
    int bitNbr,
    volatile unsigned long *addr
    );

extern int drm_test_and_clear_bit
    (
    int bitNbr,
    volatile unsigned long *addr
    );

extern int drm_test_and_set_bit
    (
    int bitNbr,
    volatile unsigned long *addr
    );

extern int drm_test_bit
    (
    int bitNbr,
    const volatile unsigned long *addr
    );

#define clear_bit drm_clear_bit
#define change_bit drm_change_bit
#define set_bit drm_set_bit
#define test_and_change_bit drm_test_and_change_bit
#define test_and_clear_bit drm_test_and_clear_bit
#define test_and_set_bit drm_test_and_set_bit
#define test_bit drm_test_bit
#define __set_bit set_bit
#define __clear_bit clear_bit
#define __test_and_clear_bit test_and_clear_bit

#endif /* _VXOAL_ATOMIC_H */
