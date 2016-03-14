/* workqueue.c - workqueue management functions */

/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
04feb16,qsn  Re-init timer for NULL-timerId in queue_delayed_work (US73810)
04aug15,qsn  Implement more APIs (US63364)
05mar15,qsn  Written for VxWorks 7 release.
*/

/*

DESCRIPTION

This file provides compatibility functions for part of the Linux workqueue operations.

NOMANUAL

*/

/* includes */

#include <intLib.h>
#include <taskLib.h>
#include <timerDev.h> /* for sysClkRateGet */
#include <vxoal/krnl/workqueue.h>
#include <vxoal/krnl/spinlock.h>
#include <vxoal/krnl/mm.h> /* for kmalloc, kfree */
#include <string.h>

/* globals */

struct workqueue_struct *system_wq = NULL;
struct workqueue_struct *system_long_wq = NULL;

/* locals */

static struct workqueue_struct *system_unbound_wq = NULL;

static DEFINE_SPINLOCK(asyncLock);
static async_cookie_t nextCookie = 0;

#ifndef _WRS_CONFIG_SMP
static int wqListLevel;
#else
static spinlockIsr_t wqListSpinLock;
#endif
static struct workqueue_list_struct wqList;

/* externs */

extern unsigned int gfxDrmWorkqueueTaskPriority(void);
extern unsigned int gfxDrmWorkqueueStackSize(void);

/*******************************************************************************
*
* init_workqueues - init system workqueue
*
* RETURNS: 0
*
* ERRNOS: N/A
*/
int init_workqueues
    (
    void
    )
    {
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_INIT(&(wqListSpinLock), 0);
#endif /* _WRS_CONFIG_SMP */

    INIT_LIST_HEAD(&wqList.entry);

    system_wq = alloc_workqueue("system_wkqueue", 0, 0);
    system_unbound_wq = alloc_workqueue("system_unbound_wkqueue", 0, 0);
    system_long_wq = alloc_workqueue("events_long", 0, 0);

    return 0;
    }

/*******************************************************************************
*
* delayed_work_timer_fn - timer function for delayed work
*
* RETURNS: N/A
*
* ERRNOS: N/A
*/
void delayed_work_timer_fn
    (
    unsigned long data
    )
    {
    struct delayed_work *dwork;
    struct timer_list *timer;

    dwork = (struct delayed_work *)data;

    timer = &dwork->timer;
    if (timer)
        {
        (void)del_timer(timer);
        destroy_timer_on_stack(timer);
        }

    (void)queue_work(dwork->wq, &(dwork->work));
    }

/*******************************************************************************
*
* exist_workqueue - check existence of a workqueue
*
* RETURNS: true if the workqueue exists, false otherwise.
*
* ERRNOS: N/A
*/
static bool exist_workqueue
    (
    struct workqueue_struct *wq
    )
    {
    struct workqueue_list_struct *ptmp;
    struct list_head *pos;
    bool ret = false;

    if (!wq)
        return ret;

#ifndef _WRS_CONFIG_SMP
    wqListLevel = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&wqListSpinLock);
#endif

    list_for_each(pos, &wqList.entry)
        {
        ptmp= list_entry(pos, struct workqueue_list_struct, entry);

        if (ptmp == NULL)
            goto EW_OUT;

        if (ptmp->wq == wq)
            {
            ret = true;
            goto EW_OUT;
            }
        }

EW_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
    intUnlock(wqListLevel);
#endif
    return ret;
    }

/*******************************************************************************
*
* queue_work - queue work on a workqueue
*
* RETURNS: true if queue action is actually performed, false otherwise.
*
* ERRNOS: N/A
*/
bool queue_work
    (
    struct workqueue_struct *wq,
    struct work_struct *work
    )
    {
    struct list_head *head;
    struct list_head *pos;
    struct work_struct *twork;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif
    bool ret = false;

    if ((!wq) || !exist_workqueue(wq) || (!work))
        return ret;

#ifndef _WRS_CONFIG_SMP
    level = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

    if (wq->isDestroyInProgress)
        {
        /* the workqueue is being destroyed */
        goto QW_OUT;
        }

    head = &(wq->worklist);

    /* check if work was already on the workqueue */
    pos = head->next;
    for (;;)
        {
        if (!((pos->next) && pos != head))
            break;

        twork = list_entry(pos, struct work_struct, entry);
        if (twork == work)
            {
            /* work was already on the workqueue */
            goto QW_OUT;
            }

         pos = pos->next;
        }

    /* check if the work is currently running */
    if (!((wq->isRunningWork) && (wq->rwork == work)))
        {
        work->isMarked = false;
        list_add_tail(&work->entry, head);
        ret = true;
        (void) semGive(&(wq->isrJobSyncSem));
        }

QW_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
    intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

    return ret;
    }

/*******************************************************************************
*
* async_run_fn - helper function to enable queueing async work to workqueue
*
* RETURNS: N/A
*
* ERRNOS: N/A
*/
static void async_run_fn
    (
    struct work_struct *work
    )
    {
    struct async_entry *entry = container_of(work, struct async_entry, work);

    entry->func(entry->data, entry->cookie);
    (void)kfree ((void *)entry);
    }

/*******************************************************************************
*
* async_schedule - schedule a function for asynchronous execution
*
* Note:
*     If memory allocation fails, the function is executed immediately;
*     otherwise, the work will be queued to the system_unbound_wq  
*     workqueue for its execution.
*
* RETURNS: new cookie
*
* ERRNOS: N/A
*/
async_cookie_t async_schedule
    (
    async_func_t func,
    void *data
    )
    {
    async_cookie_t newCookie;
    unsigned long flags;
    struct async_entry *entry;

    /* allocate new cookie */ 
    spin_lock_irqsave(&asyncLock, flags);
    newCookie = nextCookie++;
    spin_unlock_irqrestore(&asyncLock, flags);

    entry = kmalloc(sizeof(struct async_entry), GFP_KERNEL);
    if (!entry)
        {
        /* execute immediately */
        func(data, newCookie);
        return newCookie;
        }

    /* queue work to system_unbound_wq */
    entry->func = func;
    entry->data = data;
    entry->cookie = newCookie;
    INIT_WORK(&entry->work, async_run_fn);
    (void)queue_work(system_unbound_wq, &entry->work);

    return newCookie;
    }

/*******************************************************************************
*
* exist_work_on_queue - check existence of a work on a workqueue
*
* Note:
*     Lock must be held and workqueue must be vaild before calling this function.
*
* RETURNS: true if the work is on the queue, false otherwise.
*
* ERRNOS: N/A
*/
static bool exist_work_on_queue
    (
    struct workqueue_struct *wq,
    struct work_struct *work
    )
    {
    struct list_head *head;
    struct list_head *pos;
    struct work_struct *twork;

    if (!work)
        return false;

    if ((wq->isRunningWork) && (wq->rwork == work))
        return true;

    head = &(wq->worklist);
    pos = head->next;
    for (;;)
        {
        if (!((pos->next) && pos != head))
            return false;

        twork = list_entry(pos, struct work_struct, entry);
        if (twork == work)
            return true;

         pos = pos->next;
        }
    }

/*******************************************************************************
*
* cancel_work_on_queue - cancel work on a workqueue
*
* Note:
*     Cancel a work and wait for its execution to finish if needed. On return
*     from this function, the work is guaranteed to be not pending. The
*     caller must ensure that the workqueue can't be destroyed before this
*     function returns.
*
* RETURNS: 0 if work was pending, 1 if work was executing, 2 otherwise.
*
* ERRNOS: N/A
*/
static int cancel_work_on_queue
    (
    struct workqueue_struct *wq,
    struct work_struct *work,
    bool   need2wait4Finishing
    )
    {
    struct list_head *head;
    struct list_head *pos;
    struct work_struct *twork;
    bool isRunning;
    int ret = 2;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif

    if ((!wq) || !exist_workqueue(wq) || (!work))
        return ret;

#ifndef _WRS_CONFIG_SMP
    level = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

    head = &(wq->worklist);

    /* check if work was already on the workqueue */
    pos = head->next;
    for (;;)
        {
        if (!((pos->next) && pos != head))
            break;

        twork = list_entry(pos, struct work_struct, entry);
        if (twork == work)
            {
            /* work was already on the workqueue, cancel it */
            list_del(pos);
            ret = 0;
            goto CWSOQ_OUT;
            }

         pos = pos->next;
        }

    isRunning = (wq->isRunningWork) && (wq->rwork == work);
    if (isRunning)
        ret = 1;

    if (isRunning && need2wait4Finishing)
        {
        /* wait until the work is done */
        for (;;)
            {
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
            intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

            /* delay for 100 msecs */
            (void)taskDelay ((100 * sysClkRateGet ()) / 1000);

#ifndef _WRS_CONFIG_SMP
            level = intLock();
#else
            SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

            if (!((wq->isRunningWork) && (wq->rwork == work)))
                break;
            }
        }

CWSOQ_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
    intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */
    return ret;
    }

/*******************************************************************************
*
* cancel_work_sync - cancel work and wait for it to finish
*
* Note:
*     Cancel a work and wait for its execution to finish. On return from this
*     function, the work is guaranteed to be not pending or executing on any
*     workqueue. The caller must ensure that the workqueue on which the work
*     was last queued can't be destroyed before this function returns.
*
* RETURNS: true if work was pending and cancelled, false otherwise.
*
* ERRNOS: N/A
*/
bool cancel_work_sync
    (
    struct work_struct *work
    )
    {
    struct workqueue_list_struct *ptmp;
    struct list_head *pos;
    int  retmp;

#ifndef _WRS_CONFIG_SMP
    wqListLevel = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&wqListSpinLock);
#endif

    list_for_each(pos, &wqList.entry)
        {
        ptmp= list_entry(pos, struct workqueue_list_struct, entry);

        if (ptmp == NULL)
            goto CWS_OUT;

        if (exist_work_on_queue(ptmp->wq, work))
            {
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
            intUnlock(wqListLevel);
#endif
            retmp = cancel_work_on_queue(ptmp->wq, work, true);
            if (!retmp)
                return true;
            else
                return false;
            }
        }

CWS_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
    intUnlock(wqListLevel);
#endif
    return false;
    }

/*******************************************************************************
*
* cancel_delayed_work_common - cancel a delayed work
*
* Note:
*     Kill off a pending delayed_work. On return from this function, the work
*     is guaranteed not to be pending.
*
* RETURNS: true if work was pending and cancelled, false otherwise.
*
* ERRNOS: N/A
*/
static bool cancel_delayed_work_common
    (
    struct delayed_work *dwork,
    bool   need2wait4Finishing
    )
    {
    struct workqueue_list_struct *ptmp;
    struct list_head *pos;
    int  retmp;
    struct timer_list *timer;

    /* delete the timer of the delayed work */
    timer = &dwork->timer;
    if (timer && (timer->timerId != NULL))
        {
        /* cancel timer */
        (void)del_timer(timer);
        }
    if (timer)
        {
        /* destroy timer */
        destroy_timer_on_stack(timer);
        }

#ifndef _WRS_CONFIG_SMP
    wqListLevel = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&wqListSpinLock);
#endif

    list_for_each(pos, &wqList.entry)
        {
        ptmp= list_entry(pos, struct workqueue_list_struct, entry);

        if (ptmp == NULL)
            goto CDW_OUT;

        if (exist_work_on_queue(ptmp->wq, &(dwork->work)))
            {
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
            intUnlock(wqListLevel);
#endif
            retmp = cancel_work_on_queue(ptmp->wq, &(dwork->work),
                                         need2wait4Finishing);
            if (!retmp)
                return true;
            else
                return false;
            }
        }

CDW_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
    intUnlock(wqListLevel);
#endif
    return false;
    }

/*******************************************************************************
*
* cancel_delayed_work - cancel a delayed work
*
* Note:
*     Kill off a pending delayed_work. On return from this function, the work
*     is guaranteed not to be pending, but it may be running on return.
*
* RETURNS: true if work was pending and cancelled, false otherwise.
*
* ERRNOS: N/A
*/
bool cancel_delayed_work
    (
    struct delayed_work *dwork
    )
    {
    return cancel_delayed_work_common(dwork, false);
    }

/*******************************************************************************
*
* cancel_delayed_work_sync - cancel a delayed work and wait for it to finish
*
* Note:
*     Kill off a pending delayed_work. On return from this function, the work
*     is guaranteed not to be pending or executing.
*
* RETURNS: true if work was pending and cancelled, false otherwise.
*
* ERRNOS: N/A
*/
bool cancel_delayed_work_sync
    (
    struct delayed_work *dwork
    )
    {
    return cancel_delayed_work_common(dwork, true);
    }
 
/*******************************************************************************
*
* queue_delayed_work - queue work on a workqueue after delay
*
* Note:
*     Queue work on a workqueue after delay (i.e. number of jiffies) of waiting.
*
* RETURNS: true if work was successfully added, false otherwise.
*
* ERRNOS: N/A
*/
bool queue_delayed_work
    (
    struct workqueue_struct *wq,
    struct delayed_work *dwork,
    unsigned long delay
    )
    {
    struct timer_list *timer;
    struct work_struct *work;

    if ((!wq) || !exist_workqueue(wq) || (!dwork))
        return false;
    timer = &dwork->timer;
    if (!timer)
        return false;

    if (!timer->timerId)
        {
        (void)memset((void*)timer, 0, sizeof(*timer));
        init_timer(timer);
        }

    work = &dwork->work;
    if ((!timer->timerId) || (!work))
        return false;

    if (!delay)
        {
        (void)del_timer(timer);
        destroy_timer_on_stack(timer);
        return queue_work(wq, work);
        }

    dwork->wq = wq;

    /* timer expires at (jiffies + delay) */
    timer->expires = jiffies + delay;
    (void)add_timer(timer);

    return true;
    }

/*******************************************************************************
*
* flush_work_on_queue - wait for a work to finish executing on a workqueue
*
* Note:
*     On return, the work is guaranteed to be idle if it hasn't been requeued
*     since flush started.
*
* RETURNS: true if waited for the work to finish execution, false otherwise.
*
* ERRNOS: N/A
*/
static bool flush_work_on_queue
    (
    struct workqueue_struct *wq,
    struct work_struct *work
    )
    {
    struct list_head *head;
    struct list_head *pos;
    struct work_struct *twork;
    bool isFirstChecked = true;
    bool isWorkPending = true;
    bool ret = false;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif

    if ((!wq) || !exist_workqueue(wq) || (!work))
        return ret;

#ifndef _WRS_CONFIG_SMP
    level = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

    while (isWorkPending)
        {
        /* check if work was really peinding on the workqueue */
        head = &(wq->worklist);
        pos = head->next;
        for (;;)
            {
            if (!((pos->next) && pos != head))
                {
                isWorkPending = false;
                break;
                }

            twork = list_entry(pos, struct work_struct, entry);
            if (twork == work)
                {
                if (isFirstChecked)
                    {
                    isFirstChecked =  false;
                    twork->isMarked = true;
                    ret = true;
                    }
                if (!twork->isMarked)
                    {
                    /* work re-queued since flush started */
                    goto FWOQ_OUT;
                    }
 
                /* work was pending on the workqueue */
#ifdef _WRS_CONFIG_SMP
                    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
                    intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

                    /* delay for 100 msecs */
                    (void)taskDelay ((100 * sysClkRateGet ()) / 1000);

#ifndef _WRS_CONFIG_SMP
                    level = intLock();
#else
                    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

                break;
                }

            pos = pos->next;
            }
        }

    if ((wq->isRunningWork) && (wq->rwork == work))
        {
        if (!ret)
            ret = true;

        if (!(wq->rwork->isMarked))
            wq->rwork->isMarked = true;

        /* wait until the work is done */
        for (;;)
            {
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
            intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

            /* delay for 100 msecs */
            (void)taskDelay ((100 * sysClkRateGet ()) / 1000);

#ifndef _WRS_CONFIG_SMP
            level = intLock();
#else
            SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

            if (!((wq->isRunningWork) && (wq->rwork == work) &&
                (wq->rwork->isMarked)))
                break;
            }
        }

FWOQ_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
    intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */
    return ret;
    }

/*******************************************************************************
*
* flush_work - wait for a work to finish executing
*
* Note:
*     Wait until the work has finished execution. On return, the work is 
*     guaranteed to be idle if it hasn't been requeued since flush started.
*
* RETURNS: true if waited for the work to finish execution, false otherwise.
*
* ERRNOS: N/A
*/
bool flush_work
    (
    struct work_struct *work
    )
    {
    struct workqueue_list_struct *ptmp;
    struct list_head *pos;

#ifndef _WRS_CONFIG_SMP
    wqListLevel = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&wqListSpinLock);
#endif

    list_for_each(pos, &wqList.entry)
        {
        ptmp= list_entry(pos, struct workqueue_list_struct, entry);

        if (ptmp == NULL)
            goto FW_OUT;

        if (exist_work_on_queue(ptmp->wq, work))
            {
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
            intUnlock(wqListLevel);
#endif
            return flush_work_on_queue(ptmp->wq, work);
            }
        }

FW_OUT:
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
    intUnlock(wqListLevel);
#endif
    return false;
    }

/*******************************************************************************
*
* flush_delayed_work - wait for a delayed work to finish executing
*
* Note:
*     Delay timer is cancelled and the pending work is queued for immediate
*     execution. On return, the work is guaranteed to be idle.
*
* RETURNS: true if waited for the work to finish execution, false otherwise.
*
* ERRNOS: N/A
*/
bool flush_delayed_work
    (
    struct delayed_work *dwork
    )
    {
    struct timer_list *timer;
    struct work_struct *work;

    if (!dwork)
        return false;

    timer = &dwork->timer;
    if (!timer)
        return false;
    work = &dwork->work;
    if ((!timer->timerId) || (!work))
        return false;

    (void)del_timer(timer);
    destroy_timer_on_stack(timer);
    (void)queue_work(dwork->wq, &dwork->work);

    return flush_work_on_queue(dwork->wq, &dwork->work);
    }

/*******************************************************************************
*
* flush_workqueue - ensure that any work on the workqueue has run to completion
*
* Note:
*     This function sleeps until all work items on the queue have finished
*     execution, but it is not livelocked by new incoming ones.
*
* RETURNS: N/A
*
* ERRNOS: N/A
*/
void flush_workqueue
    (
    struct workqueue_struct *wq
    )
    {
    struct list_head *head;
    struct list_head *pos;
    struct work_struct *twork = NULL;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif

    if (!wq || !exist_workqueue(wq))
        return;

#ifndef _WRS_CONFIG_SMP
    level = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

    head = &(wq->worklist);
    pos = head->prev;
    if (!((pos->prev) && pos != head))
        {
        if (wq->isRunningWork)
            twork = wq->rwork;
        }
    else
        twork = list_entry(pos, struct work_struct, entry);

#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
    intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

    if (twork)
        {
        /* do the flushing work */
        (void) flush_work_on_queue(wq, twork);
        }
    }

/*******************************************************************************
*
* mod_delayed_work - modify delay of or queue a delayed work
*
* RETURNS: true if work was pending and timer was modified and/or (re-)queued, 
*           false otherwise.
*
* ERRNOS: N/A
*/
bool mod_delayed_work
    (
    struct workqueue_struct *wq,
    struct delayed_work *dwork,
    unsigned long delay
    )
    {
    struct work_struct *work = NULL;
    struct list_head *pos;
    struct list_head *head;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif
    struct timer_list *timer;

    /* abnormal situation, work cannot be queued. */
    if ((!wq) || !exist_workqueue(wq) || (!dwork))
        return false;
    if (wq != dwork->wq)
        return false;

    /* cancel previous timer if not expired */
    timer = &dwork->timer;
    if (timer && (timer->timerId != NULL))
        {
        /* cancel timer */
        (void)del_timer(timer);
        }
    else if (timer)
        {
        /* need to init timer */
        (void)memset((void*)timer, 0, sizeof(*timer));
        init_timer(timer);
        }
    else
        {
        /* abnormal situation, work cannot queued. */
        return false;
        }

#ifndef _WRS_CONFIG_SMP
    level = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif

    /* try to remove the work from the workqueue */
    head = &(dwork->wq->worklist);
    pos = head->next;
    for (;;)
        {
        if (!((pos->next) && pos != head))
            break;

        work = list_entry(pos, struct work_struct, entry);
        if (work == &dwork->work)
            {
            /* remove this work from the list */
            list_del(pos);

#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
            intUnlock(level);
#endif

            /* re-queue the work */
            return queue_delayed_work(wq, dwork, delay);
            }

         pos = pos->next;
        }

    /* check if the work is currently running */
    if (!((wq->isRunningWork) && (wq->rwork == work)))
        {
        /* queue the work */
#ifdef _WRS_CONFIG_SMP
        SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
        intUnlock(level);
#endif
        return queue_delayed_work(wq, dwork, delay);
        }

    /* weird: the work is already running currently, too late to modify timer */
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
    intUnlock(level);
#endif
    destroy_timer_on_stack(timer);
    return false;
    }

/*******************************************************************************
*
* destroy_workqueue - safely terminate a workqueue
*
* Note:
*     All work items currently pending will be executed first.
*
* RETURNS: N/A
*
* ERRNOS: N/A
*/
void destroy_workqueue
    (
    struct workqueue_struct *wq
    )
    {
    struct list_head *head;
    struct list_head *pos;
    struct workqueue_list_struct *ptmp;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif

    if (!wq || !exist_workqueue(wq))
        return;

    for (;;)
        {
#ifndef _WRS_CONFIG_SMP
        level = intLock();
#else
        SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

        /* Not allow to add any new work to this queue */
        if (!wq->isDestroyInProgress)
            wq->isDestroyInProgress = true;

        head = &(wq->worklist);
        pos = head->next;

        if (!((pos->next) && pos != head))
            {
            /* no work is pending, need further check on running status */
            if (wq->isRunningWork)
                {
                /* wait until the work is done */
                for (;;)
                    {
#ifdef _WRS_CONFIG_SMP
                    SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
                    intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

                    /* delay for 100 msecs */
                    (void)taskDelay ((100 * sysClkRateGet ()) / 1000);

#ifndef _WRS_CONFIG_SMP
                    level = intLock();
#else
                    SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

                    if (!(wq->isRunningWork))
                        break;
                    }
                }

            /* queue completely empty, destroy the workqueue */

            (void)taskDelete(wq->tid);
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
            intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

            /* free the workqueue from the queue list */
#ifndef _WRS_CONFIG_SMP
            wqListLevel = intLock();
#else
            SPIN_LOCK_ISR_TAKE (&wqListSpinLock);
#endif

            list_for_each(pos, &wqList.entry)
                {
                ptmp = list_entry(pos, struct workqueue_list_struct, entry);

                if (ptmp == NULL)
                    goto DWQ_OUT;

                if (ptmp->wq == wq)
                    {
                    list_del(pos);
                    (void)kfree((void *)ptmp);
                    goto DWQ_OUT;
                    }
                }

DWQ_OUT:
#ifdef _WRS_CONFIG_SMP
            SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
            intUnlock(wqListLevel);
#endif

            (void)kfree ((void *)wq);
            return;
            }

        /* allow pending work to finish */
#ifdef _WRS_CONFIG_SMP
        SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#else
        intUnlock(level);
#endif /* !_WRS_CONFIG_SMP */

        /* delay for 100 msecs */
        (void)taskDelay ((100 * sysClkRateGet ()) / 1000);
        }
    }

/*******************************************************************************
*
* async_synchronize_full - synchronize all asynchronous function calls
*
* Note:
*     This function waits until all asynchronous function calls have been done.
*
* RETURNS: N/A
*
* ERRNOS: N/A
*/
void async_synchronize_full
    (
    void
    )
    {
    flush_workqueue(system_unbound_wq);
    }

/*******************************************************************************
*
* wqTask - workqueue task
*
* RETURNS: N/A
*
* ERRNOS: N/A
*/
LOCAL void wqTask
    (
    struct workqueue_struct *wq
    )
    {
    struct work_struct *work;
    struct list_head *pos;
    struct list_head *head;
#ifndef _WRS_CONFIG_SMP
    int level;
#endif /* ! _WRS_CONFIG_SMP */

    for (;;)
        {
         /* wait for someone to queue the work */
        (void) semBTake (&(wq->isrJobSyncSem), WAIT_FOREVER);

        for (;;)
            {
#ifndef _WRS_CONFIG_SMP
            level = intLock();
#else
            SPIN_LOCK_ISR_TAKE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

            wq->isRunningWork = false;

            head = &(wq->worklist);
            pos = head->next;
            if (!((pos->next) && pos != head))
                {
                /* no more jobs */
#ifndef _WRS_CONFIG_SMP
                intUnlock(level);
#else
                SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */
                break;
                }

             /* got a job */
             work = list_entry(pos, struct work_struct, entry);

              /* remove this job from the list */
             list_del(pos);

             wq->rwork = work;
             wq->isRunningWork = true;

#ifndef _WRS_CONFIG_SMP
            intUnlock(level);
#else
            SPIN_LOCK_ISR_GIVE (&(wq->wqJobSpinLock));
#endif /* !_WRS_CONFIG_SMP */

             /* run the job */
             work->func(work);
            }
        }
    }

/*******************************************************************************
*
* alloc_ordered_workqueue - allocate an ordered workqueue struct
*
* RETURNS: pointer to the allocated workqueue struct, NULL otherwise.
*
* ERRNOS: N/A
*/
struct workqueue_struct *alloc_ordered_workqueue
    (
    char *qeuename,
    int flags
    )
    {
    struct workqueue_struct *pWq;
    struct workqueue_list_struct *ptmp;
    (void) flags;

    if (!qeuename)
        return NULL;

    pWq = (struct workqueue_struct *)kmalloc(sizeof (struct workqueue_struct), GFP_KERNEL);
    if (!pWq)
        return NULL;

    ptmp = (struct workqueue_list_struct *)
           kmalloc(sizeof (struct workqueue_list_struct), GFP_KERNEL);
    if (!ptmp)
        goto ERROR_OUT1;

    /* Initialize work list */
    INIT_LIST_HEAD(&(pWq->worklist));

#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_INIT(&(pWq->wqJobSpinLock), 0);
#endif /* _WRS_CONFIG_SMP */

    /* initialize the synchronization semaphore for the tWqTask */
    if (semBInit (&(pWq->isrJobSyncSem), SEM_Q_PRIORITY, SEM_EMPTY) != OK)
        goto ERROR_OUT;

    strncpy(pWq->task_name, qeuename, (WQ_TASK_NAMESIZ - 1));
     
    pWq->isDestroyInProgress = false;

    /* Initialize the tWqTask */
    pWq->tid = taskSpawn(pWq->task_name, gfxDrmWorkqueueTaskPriority(),
                    (VX_SUPERVISOR_MODE | VX_FP_TASK),
                    gfxDrmWorkqueueStackSize(), (FUNCPTR) wqTask,
                    (_Vx_usr_arg_t)pWq, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (pWq->tid == TASK_ID_ERROR)
        goto ERROR_OUT;

    ptmp->wq = pWq;

    /* add to the workqueue list */
#ifndef _WRS_CONFIG_SMP
    wqListLevel = intLock();
#else
    SPIN_LOCK_ISR_TAKE (&wqListSpinLock);
#endif
    list_add(&(ptmp->entry), &(wqList.entry));
#ifdef _WRS_CONFIG_SMP
    SPIN_LOCK_ISR_GIVE (&wqListSpinLock);
#else
    intUnlock(wqListLevel);
#endif

    return pWq;

ERROR_OUT:
    (void)kfree ((void *)ptmp);
ERROR_OUT1:
    (void)kfree ((void *)pWq);
    return NULL;
    }


/*******************************************************************************
*                                                                              *
*                      Testing workqueue APIs                                  *
*                                                                              *
*******************************************************************************/
#ifdef VX_WORKQUEUE_TEST

#define CREATE_WORKQUEUE(my_wq)                        \
    struct workqueue_struct *my_wq;                    \
    my_wq = alloc_ordered_workqueue("myWorkqueue", 0); \
    if (my_wq == 0)                                    \
        {                                              \
        printf("Failed to create workqueue.\n");       \
        return;                                        \
        }                                            

typedef struct
    {
    struct work_struct my_work;
    int x;
    } my_work_t;

typedef struct
    {
    struct delayed_work my_delayed_work;
    int x;
    } my_delayed_work_t;

static void my_wq_function
    (
    struct work_struct *work
    )
    {
    my_work_t *mywork = (my_work_t *) work;
    printf("my_work.x = %d\n", mywork->x);
    (void)kfree((void *)work);
    }

static void my_delayed_wq_function
    (
    struct work_struct *work
    )
    {
    my_delayed_work_t *mywork = (my_delayed_work_t *) work;
    printf("my_delayed_work.x = %d\n", mywork->x);
    (void)kfree((void *)work);
    }

extern unsigned int sleep
    (
    unsigned int secs
    );

static my_work_t *work1, *work2, *work3;
static my_delayed_work_t *work4, *work5, *work6;

/*******************************************************************************
*
* workqueuetest_basics - test basic workqueue functionality
*
* This function tests the following workqueue APIs:
*    (1) alloc_ordered_workqueue()
*    (2) queue_work()
*    (3) destroy_workqueue()
* The function first creates a workqueue, queues 3 work items, and then tries
* to queue another one that was already queued. Finally, queueing a work item
* to a destroyed workqueue is attempted. 
*
*/
void workqueuetest_basics
    (
    void
    )
    {
    CREATE_WORKQUEUE(my_wq);

    /* queue work item 1 */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        (void)queue_work(my_wq, (struct work_struct *)work1);
        }

    /* queue work item 2 */
    work2 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work2)
        {
        INIT_WORK((struct work_struct *)work2, my_wq_function);
        work2->x = 2;
        (void)queue_work(my_wq, (struct work_struct *)work2);
        }

    /* queue work item 3 */
    work3 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work3)
        {
        INIT_WORK((struct work_struct *)work3, my_wq_function);
        work3->x = 3;
        (void)queue_work(my_wq, (struct work_struct *)work3);
        }

    /* re-queue the same work item 3 */
    if (!queue_work(my_wq, (struct work_struct *)work3))
        {
        printf("Failed to re-queue the same work item 3.\n"); 
        }

    sleep(1);

    /* destroy the workqueue */
    destroy_workqueue(my_wq);

    /* attempt to queue a work item to the destroyed queue */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        if (!queue_work(my_wq, (struct work_struct *)work1))
            printf("Correct behavior: cannot queue a work to a destroyed workqueue\n");
        else
            printf("Wrong behavior: queued a work to a destroyed queue\n");
        (void)kfree((void *)work1);
        }
    
}

/*******************************************************************************
*
* workqueuetest_delayed_work - test delayed work
*
* This function tests the following delayed-work related workqueue APIs:
*    (1) queue_delayed_work()
*    (2) mod_delayed_work()
* This function first queues a delayed work with delay of 3 seconds, and then
* queues a second delayed work with delay of 15 seconds. But after that,
* the second work's delay is changed to 8 seconds. 
*
*/
void workqueuetest_delayed_work
    (
    void
    )
    {
    unsigned long delay3s = 3 * sysClkRateGet();
    unsigned long delay8s = 8 * sysClkRateGet();
    unsigned long delay15s = 15 * sysClkRateGet();

    /* queue a delayed work item */
    work4 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work4)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work4, my_delayed_wq_function);
        work4->x = 4;
        (void)queue_delayed_work(system_wq, (struct delayed_work *)work4, delay3s);
        }

    /* queue a second delayed work item */
    work5 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work5)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work5, my_delayed_wq_function);
        work5->x = 5;
        (void)queue_delayed_work(system_wq, (struct delayed_work *)work5, delay15s);
        }

    /* modify the delay of the second delayed work */
    (void)mod_delayed_work(system_wq, (struct delayed_work *)work5, delay8s);
    }

/*******************************************************************************
*
* workqueuetest_async_work - test work for asynchronous execution
*
* This function tests the following async-execution related workqueue APIs:
*    (1) async_schedule()
*    (2) async_synchronize_full()
* This function schedules 10 async work items and call async_synchronize_full()
* to wait for them all to finish.
*
*/
static void my_async_func
    (
    void *data,
    async_cookie_t cookie
    )
    {
    printf("my_async_func(): data=%d, cookie=%lu\n", *((int *)data), cookie);
    }

void workqueuetest_async_work
    (
    void
    )
    {
    int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;

    for (i = 0; i < 10; i++)
        {
        (void)async_schedule(my_async_func, (void *)&data[i]);
        }

    async_synchronize_full();

    printf("*** Test done ***\n");
    }

/*******************************************************************************
*
* workqueuetest_schedule_work - test "schedule work"
*
* This function tests the following workqueue APIs:
*    (1) schedule_work()
*    (2) schedule_delayed_work()
* This function schedules two work items and one delayed work item.
*
*/
void workqueuetest_schedule_work
    (
    void
    )
    {
    unsigned long delay3s = 3 * sysClkRateGet();

    /* schedule work item 1 */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        (void)schedule_work((struct work_struct *)work1);
        }

    /* schedule work item 2 */
    work2 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work2)
        {
        INIT_WORK((struct work_struct *)work2, my_wq_function);
        work2->x = 2;
        (void)schedule_work((struct work_struct *)work2);
        }

    /* schedule a delayed work item */
    work4 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work4)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work4, my_delayed_wq_function);
        work4->x = 4;
        (void)schedule_delayed_work((struct delayed_work *)work4, delay3s);
        }
    }

/*******************************************************************************
*
* workqueuetest_cancel_work - test "cancel work"
*
* This function tests the following workqueue APIs:
*    (1) cancel_work_sync()
*    (2) cancel_delayed_work()
*    (3) cancel_delayed_work_sync
* This function queues 2 work items and 3 delayed work items. It cancels
* the second work item and the first and last delayed work items.
*
*/
void workqueuetest_cancel_work
    (
    void
    )
    {
    unsigned long delay1s =  sysClkRateGet();
    unsigned long delay5s = 5 * sysClkRateGet();
    unsigned long delay8s = 8 * sysClkRateGet();

    CREATE_WORKQUEUE(my_wq);

    /* queue work item 1 */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        (void)queue_work(my_wq, (struct work_struct *)work1);
        }

    /* queue work item 2 */
    work2 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work2)
        {
        INIT_WORK((struct work_struct *)work2, my_wq_function);
        work2->x = 2;
        (void)queue_work(my_wq, (struct work_struct *)work2);
        }

    /* cancel work item 2 */
    (void)cancel_work_sync((struct work_struct *)work2);

    /* queue a delayed work item 4 */
    work4 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work4)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work4, my_delayed_wq_function);
        work4->x = 4;
        (void)queue_delayed_work(my_wq, (struct delayed_work *)work4, delay1s);
        }

    /* queue a second delayed work item 5 */
    work5 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work5)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work5, my_delayed_wq_function);
        work5->x = 5;
        (void)queue_delayed_work(my_wq, (struct delayed_work *)work5, delay5s);
        }

    /* queue a third delayed work item 6 */
    work6 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work6)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work6, my_delayed_wq_function);
        work6->x = 6;
        (void)queue_delayed_work(my_wq, (struct delayed_work *)work6, delay8s);

        }

    /* cancel delayed work item 4 */
    (void)cancel_delayed_work((struct delayed_work *)work4);

    /* cancel delayed work item 6 */
    (void)cancel_delayed_work_sync((struct delayed_work *)work6);

    sleep(6);

    /* destroy the workqueue */
    destroy_workqueue(my_wq);
    }

/*******************************************************************************
*
* workqueuetest_flush_work - test "flush work"
*
* This function tests the following workqueue APIs:
*    (1) flush_work()
*    (2) flush_delayed_work()
*    (3) flush_workqueue()
*    (4) flush_scheduled_work()
* This function first queues 2 work items and flushes the second one. Next, it
* queues 2 delayed work items and flushes the one with largest delay. Then, it
* queus 2 work items and flushes the entire queue. Finally, it schedule 1 work
* item and flushes it.
*
*/
void workqueuetest_flush_work
    (
    void
    )
    {
    unsigned long delay4s = 4 * sysClkRateGet();
    unsigned long delay5s = 5 * sysClkRateGet();

    CREATE_WORKQUEUE(my_wq);

    /* queue work item 1 */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        (void)queue_work(my_wq, (struct work_struct *)work1);
        }

    /* queue work item 2 */
    work2 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work2)
        {
        INIT_WORK((struct work_struct *)work2, my_wq_function);
        work2->x = 2;
        (void)queue_work(my_wq, (struct work_struct *)work2);
        }

    /* flush_work item 2 */
    (void)flush_work((struct work_struct *)work2);

    /* queue a delayed work item 4 */
    work4 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work4)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work4, my_delayed_wq_function);
        work4->x = 4;
        (void)queue_delayed_work(my_wq, (struct delayed_work *)work4, delay4s);
        }

    /* queue a second delayed work item 5 */
    work5 = (my_delayed_work_t *) kmalloc(sizeof(my_delayed_work_t), GFP_KERNEL);
    if (work5)
        {
        INIT_DELAYED_WORK((struct delayed_work *)work5, my_delayed_wq_function);
        work5->x = 5;
        (void)queue_delayed_work(my_wq, (struct delayed_work *)work5, delay5s);
        }

    /* flush delayed work item 5 */
    (void)flush_delayed_work((struct delayed_work *)work5);

    sleep(8);

    /* queue work item 1 */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        (void)queue_work(my_wq, (struct work_struct *)work1);
        }

    /* queue work item 2 */
    work2 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work2)
        {
        INIT_WORK((struct work_struct *)work2, my_wq_function);
        work2->x = 2;
        (void)queue_work(my_wq, (struct work_struct *)work2);
        }

    /* flush workqueue */
    flush_workqueue(my_wq);

    /* schedule work item 1 */
    work1 = (my_work_t *) kmalloc(sizeof(my_work_t), GFP_KERNEL);
    if (work1)
        {
        INIT_WORK((struct work_struct *)work1, my_wq_function);
        work1->x = 1;
        (void)schedule_work((struct work_struct *)work1);
        }

    /* flush scheduled work */
    flush_scheduled_work();

    destroy_workqueue(my_wq);

    printf("Test done.\n");
    }

#endif /* VX_WORKQUEUE_TEST */
