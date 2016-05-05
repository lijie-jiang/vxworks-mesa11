/* time.h - time functionality header file*/

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
04feb16,qsn  Fix round_jiffies_up_relative (US73810)
27oct15,qsn  Fix GPU gear demo issue on Sabino Canyon (US69065)
22oct15,qsn  Make add_timer callable in interrupt context (US68598)
04aug15,qsn  Add round_jiffies_up_relative (US63364)
20jul15,yat  Clean up vxoal (US63367)
09mar15,qsn  Added more functions (US50613)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux time operations.

NOMANUAL
*/

#ifndef _VXOAL_TIME_H_
#define _VXOAL_TIME_H_

#include <vxWorks.h> /* for _Vx_ticks_t */
#include <tickLib.h> /* for tickGet */
#include <drv/timer/timerDev.h> /* for sysClkRateGet */
#include <taskLib.h> /* for taskDelay */
#include <sys/times.h> /* for timeval */
#define _WRS_CONFIG_VXBUS_LEGACY
#if defined(_WRS_CONFIG_VXBUS_LEGACY)
#include <vxbTimerLib.h> /* for vxbUsDelay */
#else
#include <subsys/timer/vxbTimerLib.h> /* for vxbUsDelay */
#endif
#include <vxoal/krnl/types.h> /* for typecheck */
#include <wdLib.h> /* for watchdog timer */

#ifndef MAX_JIFFY_OFFSET
#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1) - 1)
#endif

#ifndef jiffies
#define jiffies         ((unsigned long)tickGet())
#endif

#ifndef get_jiffies_64
#define get_jiffies_64() (tick64Get())
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC    1000L
#endif

#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC   1000L
#endif

#ifndef NSEC_PER_USEC
#define NSEC_PER_USEC   1000L
#endif

#ifndef NSEC_PER_MSEC
#define NSEC_PER_MSEC   1000000L
#endif

#ifndef USEC_PER_SEC
#define USEC_PER_SEC    1000000L
#endif

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC    1000000000L
#endif

#ifndef BILLION
#define BILLION         1000000000L
#endif

#ifndef SEC_PER_DAY
#define SEC_PER_DAY     86400
#endif

#ifndef time_after
#define time_after(a,b)                                                        \
        (typecheck(unsigned long, a) &&                                        \
         typecheck(unsigned long, b) &&                                        \
         ((long)((b) - (a)) < 0L))
#endif

#ifndef time_after_eq
#define time_after_eq(a,b)                                                     \
        (typecheck(unsigned long, a) &&                                        \
         typecheck(unsigned long, b) &&                                        \
         ((long)((a) - (b)) >= 0L))
#endif

#ifndef time_before_eq
#define time_before_eq(a,b) time_after_eq(b,a)
#endif

#ifndef time_in_range
#define time_in_range(a,b,c)                                                   \
        (time_after_eq(a,b) &&                                                 \
         time_before_eq(a,c))
#endif

extern void drm_nsleep
    (
    unsigned int val
    );

extern void drm_usleep
    (
    unsigned int val
    );

extern void drm_msleep
    (
    unsigned int val
    );

#define nsleep(n) drm_nsleep(n)
#define usleep(n) drm_usleep(n)
#define msleep(n) drm_msleep(n)
#define usleep_range(min, max) drm_usleep(max)
#define ndelay(n) drm_nsleep(n)
#define udelay(n) vxbUsDelay(n)
#define mdelay(n) vxbUsDelay((n) * USEC_PER_MSEC)

#ifndef TV_CONVERT_TO_SEC
#define TV_CONVERT_TO_SEC(a,b)                                  \
    do {                                                        \
       register UINT32 hz = sysClkRateGet();                    \
       (a).tv_sec  = (time_t)((b) / hz);                        \
       (a).tv_nsec = (long)(((b) % hz) * (BILLION / hz));       \
       } while (0)
#endif

#ifndef TV_CONVERT_TO_TICK
#define TV_CONVERT_TO_TICK(a,b)                                 \
    do  {                                                       \
        register ULONG hz = (ULONG) sysClkRateGet();            \
        register ULONG res = (BILLION / hz);                    \
        (a) = (ULONG)((UINT64) ((b).tv_sec) * hz +              \
                      (UINT64) ((b).tv_nsec) / res +            \
                      (((UINT64)((b).tv_nsec) % res) ? 2 : 1)); \
        } while (0)
#endif

#define jiffies_to_nsecs(j) ((NSEC_PER_SEC / sysClkRateGet()) * j)
#define nsecs_to_jiffies(n) ((n * sysClkRateGet()) / NSEC_PER_SEC)
#define jiffies_to_usecs(j) ((USEC_PER_SEC / sysClkRateGet()) * j)
#define usecs_to_jiffies(u) ((u * sysClkRateGet()) / USEC_PER_SEC)
#define jiffies_to_msecs(j) ((MSEC_PER_SEC / sysClkRateGet()) * j)
#define msecs_to_jiffies(m) ((m * sysClkRateGet()) / MSEC_PER_SEC)
#define jiffies_to_secs(j)  (j / sysClkRateGet())
#define secs_to_jiffies(s)  (s * sysClkRateGet())

#define nsecs_to_jiffies64(n) ((UINT64) nsecs_to_jiffies((n)))

#define round_jiffies_up(j) (j)
#define round_jiffies_up_relative(j) (j)

#define timeval_to_ns(tvp)                                                     \
    (((tvp)->tv_sec * NSEC_PER_SEC) +                                          \
    ((tvp)->tv_usec * NSEC_PER_USEC))

#define timespec_to_jiffies(tsp)                                               \
    ({TV_CONVERT_TO_TICK(j, (*(tsp))); j;})

union ktime
    {
    INT64 tv64; /* in nanoseconds */
    };
typedef union ktime ktime_t; 

#define ktime_sub(lhs, rhs)                                                    \
    ({ (ktime_t){ .tv64 = (lhs).tv64 - (rhs).tv64 }; })

#define ktime_get_monotonic_offset()                                           \
    ({ (ktime_t){ .tv64 = 0 }; })

#define ktime_add_ns(kt, ns)                                                   \
    ({ (ktime_t){ .tv64 = (kt).tv64 + (ns) }; })

#define ktime_sub_ns(kt, ns)                                                   \
    ({ (ktime_t){ .tv64 = (kt).tv64 - (ns) }; })

#define ktime_to_ns(kt) ((kt).tv64)
#define ktime_to_us(kt) ((kt).tv64 / 1000LL)
#define ktime_us_delta(lhs, rhs) ktime_to_us(ktime_sub(lhs, rhs))

extern ktime_t drm_ktime_get
    (
    void
    );
#define ktime_get drm_ktime_get

extern unsigned long long drm_ktime_get_raw_ns
    (
    void
    );
#define ktime_get_raw_ns drm_ktime_get_raw_ns

extern struct timeval drm_ktime_to_timeval
    (
    ktime_t tk
    );
#define ktime_to_timeval drm_ktime_to_timeval

extern unsigned long drm_get_seconds
    (
    void
    );
#define get_seconds drm_get_seconds

#if 0
#define DEBUG_TIMER(t) pr_err("timer:%p jiffies:%d expires:%d\n",              \
                              (t), jiffies, (t)->expires)
#else
#define DEBUG_TIMER(t)
#endif

struct  timer_list
    {
    unsigned long       expires;
    void (*function)(unsigned long);
    unsigned long       data;
    timer_t             timerId;
    struct timespec     it_value;
    WDOG_ID             intTimerId;
    };

extern void drm_init_timer
   (
   struct timer_list *timer
   );
#define init_timer drm_init_timer

extern void drm_setup_timer
   (
   struct timer_list * timer,
   void (*function)(unsigned long),
   unsigned long data
   );
#define setup_timer drm_setup_timer

extern void drm_setup_timer_on_stack
   (
   struct timer_list * timer,
   void (*function)(unsigned long),
   unsigned long data
   );
#define setup_timer_on_stack drm_setup_timer_on_stack

extern void drm_add_timer
    (
    struct timer_list *timer
    );
#define add_timer drm_add_timer

extern int drm_del_timer
    (
    struct timer_list * timer
    );
#define del_timer drm_del_timer
#define del_timer_sync del_timer
#define del_singleshot_timer_sync del_timer

extern void drm_destroy_timer_on_stack
    (
    struct timer_list *timer
    );
#define destroy_timer_on_stack drm_destroy_timer_on_stack

extern int drm_timer_pending
   (
   struct timer_list *timer
   );
#define timer_pending drm_timer_pending

extern int drm_mod_timer
    (
    struct timer_list *timer,
    unsigned long expires
    );
#define mod_timer drm_mod_timer
#define mod_timer_pinned drm_mod_timer

#endif /* _VXOAL_TIME_H_ */
