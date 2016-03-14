/* waitqueue.c - wait queue functions */

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
27mar15,rpc  Static analysis fixes (US50633) semDelete
05mar15,qsn  Implemented wake_up related operations
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides compatibility wait queue for the Linux operations.

NOMANUAL

*/

/* includes */

#include <vxoal/krnl/list.h> /* for INIT_LIST_HEAD */
#include <vxoal/krnl/spinlock.h> /* for spinlock */
#include <vxoal/krnl/waitqueue.h> /* for wait */

/*******************************************************************************
*
* init_waitqueue_head
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void init_waitqueue_head
    (
    wait_queue_head_t *q
    )
    {
    spin_lock_init(&q->lock);
    INIT_LIST_HEAD(&q->task_list);
    }

/*******************************************************************************
*
* __wake_up
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void __wake_up
    (
    wait_queue_head_t *q, 
    unsigned int mode,
    int nr_exclusive, 
    void *key
    )
    {
    unsigned long flags = 0;
    wait_queue_t *curr, *next;

    spin_lock_irqsave(&q->lock, flags);
    list_for_each_entry_safe(curr, next, &q->task_list, task_list)
        {
        unsigned flags = curr->flags;

        if (curr->func(curr, mode, 0, key) &&
            (flags & WQ_FLAG_EXCLUSIVE) &&
            (!--nr_exclusive))
            break;
        }
    spin_unlock_irqrestore(&q->lock, flags);
    }

/*******************************************************************************
*
* waitqueue_active
*
* RETURNS: 1 if active, 0 otherwise
*
* SEE ALSO: 
*/
int waitqueue_active
    (
    wait_queue_head_t *q
    )
    {
    return (!list_empty(&q->task_list));
    }

/*******************************************************************************
*
* add_wait_queue
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void add_wait_queue
    (
    wait_queue_head_t *q, 
    wait_queue_t *wait
    )
    {
    unsigned long flags = 0;

    spin_lock_irqsave(&q->lock, flags);
    list_add(&wait->task_list, &q->task_list);
    spin_unlock_irqrestore(&q->lock, flags);
    }

/*******************************************************************************
*
* remove_wait_queue
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void remove_wait_queue
    (
    wait_queue_head_t *q, 
    wait_queue_t *wait
    )
    {
    unsigned long flags = 0;

    spin_lock_irqsave(&q->lock, flags);
    (void) semDelete(wait->sem);
    list_del(&wait->task_list);
    spin_unlock_irqrestore(&q->lock, flags);
    }

/*******************************************************************************
*
* default_wake_function
*
* RETURNS: 1 if woken, 0 otherwise
*
* SEE ALSO: 
*/
int default_wake_function
    (
    wait_queue_t *wait, 
    unsigned mode, 
    int sync,
    void *key
    )
    {
    if (NULL == wait->sem)
        return 0;

    if (ERROR == semGive(wait->sem))
        return 0;

    return 1;
    }

/*******************************************************************************
*
* autoremove_wake_function
*
* RETURNS:
*
* SEE ALSO: 
*/
int autoremove_wake_function
    (
    wait_queue_t *wait,
    unsigned mode,
    int sync,
    void *key
    )
    {
    int ret;

    ret = default_wake_function(wait, mode, sync, key);
    if (ret)
        list_del_init(&wait->task_list);

    return ret;
    }

/*******************************************************************************
*
* prepare_to_wait
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void prepare_to_wait
    (
    wait_queue_head_t *q,
    wait_queue_t *wait,
    int state
    )
    {
    unsigned long flags = 0;

    wait->flags &= ~WQ_FLAG_EXCLUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    if (list_empty(&wait->task_list))
        __add_wait_queue(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
    }

/*******************************************************************************
*
* finish_wait
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void finish_wait
    (
    wait_queue_head_t *q,
    wait_queue_t *wait
    )
    {
    unsigned long flags = 0;

    if (!list_empty_careful(&wait->task_list))
        {
        spin_lock_irqsave(&q->lock, flags);
        list_del_init(&wait->task_list);
        spin_unlock_irqrestore(&q->lock, flags);
        }
    }
