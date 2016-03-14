
/* workqueue.h - workqueue header file */

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
04aug15,qsn  Implement more APIs (US63364)
05mar15,qsn  Written for VxWorks 7 release
*/

/*
DESCRIPTION

This file provides compatibility workqueue for the Linux operations.

NOMANUAL
*/

#ifndef  _VXOAL_WORKQUEUE_H
#define  _VXOAL_WORKQUEUE_H

#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/types.h> /* for async_cookie_t */
#include <vxoal/krnl/list.h> /* for list_head */
#include <vxoal/krnl/time.h> /* for timer_list */
#include <taskLib.h> /* for TASK_ID */
#ifdef _WRS_CONFIG_SMP
#include <spinLockLib.h> /* for spinlockIsr_t */
#endif

#define WQ_TASK_NAMESIZ 64

struct work_struct;

typedef void (*work_func_t)(struct work_struct *work);

struct workqueue_struct
    {
    struct list_head worklist;
    char task_name[WQ_TASK_NAMESIZ];
    TASK_ID tid;
#ifdef _WRS_CONFIG_SMP
    spinlockIsr_t wqJobSpinLock;
#endif
    SEMAPHORE isrJobSyncSem;
    bool isRunningWork;
    struct work_struct *rwork;
    bool isDestroyInProgress;
    };

struct workqueue_list_struct
    {
    struct list_head entry;
    struct workqueue_struct *wq;
    };

struct work_struct
    {
    struct list_head entry;
    work_func_t func;
    bool isMarked;
    };

struct delayed_work
    {
    struct work_struct work;
    struct timer_list timer;
    struct workqueue_struct *wq;
    };

extern void delayed_work_timer_fn
    (
    unsigned long __data
    );

#define __WORK_INITIALIZER(n, f)                                        \
    {                                                                   \
    .entry = { &(n).entry, &(n).entry },                                \
    .func = (f)                                                         \
    }

#define DECLARE_WORK(n, f)						\
    struct work_struct n = __WORK_INITIALIZER(n, f)

#define PREPARE_WORK(_work, _func)                                      \
    do                                                                  \
    {                                                                   \
    (_work)->func = (_func);                                            \
    }                                                                   \
    while (0)

#define __init_timer(_timer)                                            \
    (void)memset((void *)(_timer), 0, sizeof(*(_timer)));               \
    init_timer(_timer);

#define __setup_timer(_timer, _fn, _data)                               \
    do                                                                  \
    {                                                                   \
    __init_timer((_timer));                                             \
    (_timer)->function = (_fn);                                         \
    (_timer)->data = (_data);                                           \
    }                                                                   \
    while (0)

#define __INIT_WORK(_work, _func)                                       \
    do                                                                  \
    {                                                                   \
    INIT_LIST_HEAD(&(_work)->entry);                                    \
    PREPARE_WORK((_work), (_func));                                     \
    }                                                                   \
    while (0)

#define INIT_WORK(_work, _func)                                         \
    do                                                                  \
    {                                                                   \
    __INIT_WORK((_work), (_func));                                      \
    }                                                                   \
    while (0)

#define __INIT_DELAYED_WORK(_work, _func)                               \
    do                                                                  \
    {                                                                   \
    INIT_WORK(&(_work)->work, (_func));                                 \
    __setup_timer(&(_work)->timer, delayed_work_timer_fn,               \
                  (unsigned long)(_work));                              \
    }                                                                   \
    while (0)

#define INIT_DELAYED_WORK(_work, _func)                                 \
    __INIT_DELAYED_WORK(_work, _func)

#define alloc_workqueue(p1, p2, p3) alloc_ordered_workqueue(p1, p2)

extern struct workqueue_struct *system_wq;
extern struct workqueue_struct *system_long_wq;

typedef void (*async_func_t) (void *data, async_cookie_t cookie);
struct async_entry
    {
    struct work_struct work;
    async_cookie_t     cookie;
    async_func_t       func;
    void               *data;
    };

extern int init_workqueues
    (
    void
    );

static inline struct delayed_work *to_delayed_work
    (
    struct work_struct *work
    )
    {
    return container_of(work, struct delayed_work, work);
    }

extern struct workqueue_struct * alloc_ordered_workqueue
    (
    char *qeuename, 
    int flags
    );

extern bool queue_work
    (
    struct workqueue_struct *wq, 
    struct work_struct *work
    );

extern async_cookie_t async_schedule
    (
    async_func_t func,
    void *data
    );

static inline bool schedule_work
    (
    struct work_struct *work
    )
    {
    return queue_work(system_wq, work);
    }

extern bool queue_delayed_work
    (
    struct workqueue_struct *wq,
    struct delayed_work *dwork,
    unsigned long delay
    );

static inline bool schedule_delayed_work
    (
    struct delayed_work *dwork,
    unsigned long delay
    )
    {
    return queue_delayed_work(system_wq, dwork, delay);
    }

extern bool cancel_work_sync
    (
    struct work_struct *work
    );

extern bool cancel_delayed_work
    (
    struct delayed_work *dwork
    );

extern bool cancel_delayed_work_sync
    (
    struct delayed_work *dwork
    );

extern bool flush_work
    (
    struct work_struct *work
    );

extern bool flush_delayed_work
    (
    struct delayed_work *dwork
    );

extern void flush_workqueue
    (
    struct workqueue_struct *wq
    );

static inline void flush_scheduled_work
    (
    void
    )
    {
    flush_workqueue(system_wq);
    }

extern bool mod_delayed_work
    (
    struct workqueue_struct *wq,
    struct delayed_work *dwork,
    unsigned long delay
    );

extern void destroy_workqueue
    (
    struct workqueue_struct *wq
    );

extern void async_synchronize_full
    (
    void
    );

#endif /* _VXOAL_WORKQUEUE_H */
