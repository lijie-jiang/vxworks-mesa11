/* waitqueue.h - wait queue header file*/

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
27jul15,yat  Clean up vxoal (US63367)
05mar15,qsn  Implemented wait_event related operations (US50613)
24jan14,mgc  Modified for VxWorks 7 release
*/

/*
DESCRIPTION

This file provides compatibility wait queue for the Linux operations.

NOMANUAL
*/

#ifndef _VXOAL_WAITQUEUE_H_
#define _VXOAL_WAITQUEUE_H_

#include <semLib.h> /* for semBCreate */
#include <vxoal/krnl/list.h> /* for list_head */
#include <vxoal/krnl/spinlock.h> /* for spin_lock_t */

#define TASK_INTERRUPTIBLE      1
#define TASK_UNINTERRUPTIBLE    2
#define TASK_NORMAL             (TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)

#define WQ_FLAG_EXCLUSIVE    0x01

typedef struct __wait_queue wait_queue_t;

typedef int (*wait_queue_func_t)(wait_queue_t *wait, unsigned mode, int flags, void *key);

struct __wait_queue
    {
    unsigned int flags;
    void *private;
    SEM_ID sem;
    wait_queue_func_t func;
    struct list_head task_list;
    };

struct wait_bit_key
    {
    void *flags;
    int bit_nr;
    };

struct wait_bit_queue
    {
    struct wait_bit_key key;
    wait_queue_t wait;
    };

typedef struct
    {
    spinlock_t lock;
    struct list_head task_list;
    } wait_queue_head_t;

/*
 * Macros for declaration and initialisation of the datatypes
 */

#define __WAITQUEUE_INITIALIZER(name, tsk)                                     \
    {                                                                          \
    .private     = tsk,                                                        \
    .func        = default_wake_function,                                      \
    .sem         = semBCreate (SEM_Q_FIFO, SEM_EMPTY),                         \
    .task_list   = { NULL, NULL }                                              \
    }

#define DECLARE_WAITQUEUE(name, tsk)                                           \
    wait_queue_t name = __WAITQUEUE_INITIALIZER(name, tsk)

#define DEFINE_WAIT_FUNC(name, function)                                       \
    wait_queue_t name =                                                        \
        {                                                                      \
        .private     = current,                                                \
        .func        = function,                                               \
        .sem         = semBCreate (SEM_Q_FIFO, SEM_EMPTY),                     \
        .task_list   = LIST_HEAD_INIT((name).task_list),                       \
        }

#define DEFINE_WAIT(name) DEFINE_WAIT_FUNC(name, autoremove_wake_function)

#define wake_up(x)                      __wake_up(x, TASK_NORMAL, 1, NULL)
#define wake_up_all(x)                  __wake_up(x, TASK_NORMAL, 0, NULL)
#define wake_up_interruptible(x)        __wake_up(x, TASK_INTERRUPTIBLE, 1, NULL)
#define wake_up_interruptible_all(x)    __wake_up(x, TASK_INTERRUPTIBLE, 0, NULL)

#define __wait_event(wq, cond, state, wto, ret, ticks)                         \
    ({                                                                         \
    long __ret = ret;                                                          \
    DECLARE_WAITQUEUE(entry, current);                                         \
    add_wait_queue(&(wq), &entry);                                             \
                                                                               \
    for (;;)                                                                   \
        {                                                                      \
        if (cond)                                                              \
            break;                                                             \
        (void)semBTake(entry.sem, ticks);                                      \
        if (wto)                                                               \
            {                                                                  \
            if (!(cond))                                                       \
                __ret = 0;                                                     \
            break;                                                             \
            }                                                                  \
        }                                                                      \
    remove_wait_queue(&(wq), &entry);                                          \
    __ret;                                                                     \
    })

#define wait_event(wq, cond)                                                   \
    ({                                                                         \
    int __ret = 0;                                                             \
    if (!(cond))                                                               \
        __ret = __wait_event(wq, cond, TASK_INTERRUPTIBLE, 0, 0, 1);           \
    __ret;                                                                     \
    })

#define wait_event_timeout(wq, cond, timeout)                                  \
    ({                                                                         \
    int __ret = timeout;                                                       \
    if (!(cond))                                                               \
        __ret = __wait_event(wq, cond, TASK_INTERRUPTIBLE, 1, 1, timeout);     \
    __ret;                                                                     \
    })

#define wait_event_interruptible(wq, cond)                                     \
    wait_event(wq, cond)

#define wait_event_interruptible_timeout(wq, cond, timeout)                    \
    wait_event_timeout(wq, cond, timeout)

static inline void __add_wait_queue
    (
    wait_queue_head_t *head,
    wait_queue_t *new
    )
    {
    list_add(&new->task_list, &head->task_list);
    }

extern void init_waitqueue_head
    (
    wait_queue_head_t *q
    );

extern void __wake_up
    (
    wait_queue_head_t *q,
    unsigned int mode,
    int nr_exclusive,
    void *key
    );

extern int waitqueue_active
    (
    wait_queue_head_t *q
    );

extern void add_wait_queue
    (
    wait_queue_head_t *q,
    wait_queue_t *wait
    );

extern void remove_wait_queue
    (
    wait_queue_head_t *q,
    wait_queue_t *wait
    );

extern int default_wake_function
    (
    wait_queue_t *curr,
    unsigned mode,
    int sync,
    void *key
    );

extern int autoremove_wake_function
    (
    wait_queue_t *wait,
    unsigned mode,
    int sync,
    void *key
    );

extern void prepare_to_wait
    (
    wait_queue_head_t *q,
    wait_queue_t *wait,
    int state
    );

extern void finish_wait
    (
    wait_queue_head_t *q,
    wait_queue_t *wait
    );

#endif /* _VXOAL_WAITQUEUE_H_ */
