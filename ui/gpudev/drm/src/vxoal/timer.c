/* timer.c - timer management functions */

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
03nov15,qsn  Fix forcewake timer issues in drm/i915 v4.4 (US69587 & US69589)
27oct15,qsn  Fix GPU gear demo issue on Sabino Canyon (US69065)
22oct15,qsn  Make add_timer callable in interrupt context (US68598)
20jul15,yat  Clean up vxoal (US63367)
27mar15,rpc  Static analysis fixes (US50633) clock_gettime
01b,09mar15,qsn  Updated various functions (US50613)
01a,20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides compatibility functions for the Linux timer operations.

NOMANUAL

*/

/* includes */

#include <timerLib.h> /* for nanosleep, itimerspec, timer_create, etc */
#include <clockLib.h> /* for clock_gettime */
#include <sys/time.h> /* for timeval */
#include <base/b_struct_timespec.h> /* for timespec */
#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/time.h> /* for ktime_t, timer_list, extern */

/*******************************************************************************
*
* drm_nsleep
*
* This routine suspend execution for nanosecond intervals.
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void drm_nsleep
    (
    unsigned int val
    )
    {
    struct timespec ntp, otp;

    ntp.tv_sec = 0;
    ntp.tv_nsec = val;
    (void)nanosleep (&ntp, &otp);
    }

/*******************************************************************************
*
* drm_usleep
*
* This routine suspend execution for microsecond intervals.
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void drm_usleep
    (
    unsigned int val
    )
    {
    struct timespec ntp, otp;

    ntp.tv_sec = 0;
    ntp.tv_nsec = val * NSEC_PER_USEC;
    (void)nanosleep (&ntp, &otp);
    }

/*******************************************************************************
*
* drm_msleep
*
* This routine suspend execution for millisecond intervals.
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void drm_msleep
    (
    unsigned int val
    )
    {
    struct timespec ntp, otp;

    ntp.tv_sec = (time_t)val / MSEC_PER_SEC;
    ntp.tv_nsec = (val % MSEC_PER_SEC) * NSEC_PER_MSEC;
    (void)nanosleep (&ntp, &otp);
    }

/*******************************************************************************
*
* drm_ktime_get - get kernel time
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
ktime_t drm_ktime_get
    (
    void
    )
    {
    struct timespec timep;
    ktime_t rc;

    (void) clock_gettime (CLOCK_REALTIME, &timep);
    rc.tv64 = (INT64)((timep.tv_sec * NSEC_PER_SEC) + timep.tv_nsec);

    return rc;
    }

/*******************************************************************************
*
* drm_ktime_get_raw_ns - get kernel time
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
unsigned long long drm_ktime_get_raw_ns
    (
    void
    )
    {
    struct timespec timep;

    (void) clock_gettime (CLOCK_REALTIME, &timep);

    return ((timep.tv_sec * NSEC_PER_SEC) + timep.tv_nsec);
    }

/*******************************************************************************
*
* drm_ktime_to_timeval - convert kernel time to timeval
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
struct timeval drm_ktime_to_timeval
    (
    ktime_t tk
    )
    {
    struct timeval tv;
 
    tv.tv_sec = tk.tv64 / NSEC_PER_SEC;
    tv.tv_usec = (tk.tv64 % NSEC_PER_SEC) / NSEC_PER_USEC;

    return tv;
    }

/*******************************************************************************
*
* drm_get_seconds - get seconds
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
unsigned long drm_get_seconds
    (
    void
    )
    {
    struct timespec timep;

    (void) clock_gettime (CLOCK_REALTIME, &timep);

    return ((unsigned long)timep.tv_sec);
    }

/*******************************************************************************
*
* drm_init_timer - init timer
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void drm_init_timer
    (
    struct timer_list *timer
    )
    {
    DEBUG_TIMER(timer);

    if (timer == NULL)
        return;

    if (timer->timerId != NULL)
        {
        pr_err ("timer already created\n");
        }
    else
        {
        if (timer_create (CLOCK_REALTIME, NULL, &(timer->timerId)) == ERROR)
            {
            pr_err ("timer_create error\n");
            return;
            }
        objOwnerSet (timer->timerId, kernelId);
        }

    if (timer->intTimerId != NULL)
        {
        pr_err ("wd timer already created\n");
        }
    else
        {
        if ((timer->intTimerId = wdCreate ()) == NULL)
            {
            pr_err ("wdCreate error\n");
            return;
            }
        objOwnerSet (timer->intTimerId, kernelId);
        }
    
    }

/*******************************************************************************
*
* drm_setup_timer - setup a timer
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_setup_timer
    (
    struct timer_list *timer,
    void (*function)(unsigned long),
    unsigned long data
    )
    {
    DEBUG_TIMER(timer);

    if (timer == NULL)
        return;

    timer->function = function;
    timer->data = data;

    drm_init_timer (timer);
    }

/*******************************************************************************
*
* drm_setup_timer_on_stack - setup a timer
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void drm_setup_timer_on_stack
    (
    struct timer_list *timer,
    void (*function)(unsigned long),
    unsigned long data
    )
    {
    DEBUG_TIMER(timer);

    setup_timer(timer, function, data);
    }
    
/*******************************************************************************
*
* timer_function - timer handle
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
static void timer_function
    (
    timer_t timerId,
    _Vx_usr_arg_t arg
    )
    {
    struct timer_list *timer = (struct timer_list *)arg;

    DEBUG_TIMER(timer);

    if (timer == NULL)
        return;

    if (timer->function == NULL)
        return;

    timer->function(timer->data);
    }

/*******************************************************************************
*
* drm_add_timer - add a timer
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void drm_add_timer
    (
    struct timer_list *timer
    )
    {
    struct itimerspec value;
    int expticks;

    DEBUG_TIMER(timer);

    if (timer == NULL)
        return;

    if (timer->function == NULL)
        return;

    if (timer->expires == 0)
        return;

    if (INT_CONTEXT())
        {
        if (timer->intTimerId == NULL)
            return;

        expticks = (int)(timer->expires - jiffies);
        if (expticks <= 0)
            expticks = 1;

        if (wdStart (timer->intTimerId,
                     (_Vx_ticks_t)(expticks),
                     (FUNCPTR)timer->function,
                     (_Vx_usr_arg_t)timer->data) == ERROR)
            {
            pr_err ("wdStart error\n");
            return;
            }
        }
    else
        {
        if (timer->timerId == NULL)
            return;

        if (timer_connect (timer->timerId, timer_function, (_Vx_usr_arg_t)timer) == ERROR)
            {
            pr_err ("timer_connect error\n");
            return;
            }

        TV_CONVERT_TO_SEC(timer->it_value, timer->expires);
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_nsec = 0;
        value.it_value = timer->it_value;
        if (timer_settime (timer->timerId, TIMER_ABSTIME, &value, NULL) == ERROR)
            {
            pr_err ("timer_settime error\n");
            return;
            }
        }
    }

/*******************************************************************************
*
* drm_del_timer - delete a timer
* 
* RETURNS: 0 if timer inactive, 1 otherwise
*
* SEE ALSO: 
*/
int drm_del_timer
    (
    struct timer_list *timer
    )
    {
    DEBUG_TIMER(timer);

    if (timer == NULL)
        return 0;

    if (timer->expires == 0)
        return 0;

    if (timer->timerId != NULL)
        {
        (void)timer_cancel (timer->timerId);
        }
    if (timer->intTimerId != NULL)
        {
        (void)wdCancel (timer->intTimerId);
        }

    timer->expires = 0;

    return 1;
    }

/*******************************************************************************
*
* drm_destroy_timer_on_stack
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void drm_destroy_timer_on_stack
    (
    struct timer_list *timer
    )
    {
    DEBUG_TIMER(timer);

    if (timer == NULL)
        return;

    if (timer->timerId != NULL)
        {
        (void)timer_delete(timer->timerId);
        timer->timerId = NULL;
        }
    if (timer->intTimerId != NULL)
        {
        (void)wdDelete (timer->intTimerId);
        timer->intTimerId = NULL;
        }
    }

void show_drm_timer
    (
    void
    )
    {
    (void)timer_show (0,0);
    }

/*******************************************************************************
*
* drm_timer_pending - test a timer
*
* RETURNS: 1 if timer pending, 0 otherwise
*
* SEE ALSO: 
*/
int drm_timer_pending
    (
    struct timer_list *timer
    )
    {
    struct itimerspec values;

    DEBUG_TIMER(timer);

    if (timer == NULL)
        return 0;

    if (timer->timerId == NULL)
        return 0;

    if (INT_CONTEXT())
        return 0;

    if (timer_gettime(timer->timerId, &values))
        return 0;

    if ((values.it_value.tv_sec == 0) && (values.it_value.tv_nsec == 0))
        return 0;

    return 1;
    }

/*******************************************************************************
*
* drm_mod_timer - modify a timer
*
* RETURNS: 1 for pending and for new timer
*
* SEE ALSO: 
*/
int drm_mod_timer
    (
    struct timer_list *timer,
    unsigned long expires
    )
    {
    if ((timer->expires == expires) && timer_pending(timer))
        {
        return 1;
        }

    (void)del_timer(timer);
    timer->expires = expires;
    add_timer (timer);

    return 1;
    }
