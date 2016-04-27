/* pthreadLib.c - POSIX 1003.1 thread library interfaces */

/*
 * Copyright (c) 2000-2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
*/

/*
modification history
--------------------
03d,05nov13,to   suppress a false-positive static analysis issue
03c,04nov13,dee  fix static analysis issues
03b,13sep13,jdw  Fix Coverity issues (WIND00433558)
03a,03dec11,jpb  Removed active wait and replace with taskWait.
03c,11may12,jpb  Defect WIND00347614.  Fixed warnings.
03b,16mar12,pad  Fixed library's and pthread_mutexattr_setprotocol()' manual.
03a,22feb12,pad  Allow pthread_kill() to be called by a native VxWorks task
                 (defect 333144) and do the same for pthread_getschedparam()
                 and pthread_setschedparam().
02z,26jul11,pad  Kept only Wind River copyright (defect 288940).
02y,17jan11,pad  Made so that a pthread cancelled while holding another
                 pthread's lock will always release the lock prior to exiting
                 (defect 242878).
                 Also: synchronized most of the code between pthread_exit() and
                 pthread_abnormal_exit() via the use of an inline routine,
                 removed obsolete AE-related logic and other miscellaneous
                 enhancements.
02x,08dec10,zl  updated semXCreate() verification to use typed macro
02w,11nov10,pad  For improved performances replaced
                 PTHREAD_SELF_TO_INTERNALPTHREAD() macro with
                 _pthread_id_self() inline routine. Various code cleanups.
02v,02nov10,pad  Performance improvements to pthread_testandset_canceltype(),
                 and pthread_canceltype() as discussed in the defect 233318.
                 Also improved performances pthread_setcancelstate().
02u,01jul10,jpb  Replaced exit() with taskExit().
02t,14apr10,jpb  Updated for LP64 adaptation.
02o,12feb10,kk   modified pthread_cond_timedwait() to use
                 TV_CONVERT_TO_TICK_NO_PARTIAL() (WIND00186253)
02n,16dec09,gls  fixed condition variable/signal interaction (WIND00193764)
02s,30sep09,jpb  Defect #184564.   pthread_abnormal_exit should just return
                 instead of calling exit() in the case where
                 PTHREAD_VERIFY_AND_LOCK fails.
02s,20apr09,cww  use fixed size atomic operators
02r,27aug08,jpb  Renamed VSB header file
02q,02sep08,pad  Now the synchronization semaphore used for joining the thread
                 is properly deleted even when the thread sets itself in detach
                 mode (defect 130517).
02p,22aug08,pad  Restore proper priority when the guard mutex associated to a
                 condition variable is using the PTHREAD_PRIO_PROTECT protocol
                 (defect 130350).
                 pthread_cond_timed_wait() now decrements the guard mutex's
                 reference count when the function times out (defect 130210).
02o,18jun08,jpb  Renamed _WRS_VX_SMP to _WRS_CONFIG_SMP.  Added include path
                 for kernel configurations options set in vsb.
02n,15apr08,jpb  Defect #112751.  Fixed return value check for VALID_PTHREAD.
02m,08apr08,jpb  Defect #119383.  Edited pthread_setspecific to allow for the
                 value of the key to be NULL.
02l,21aug07,pad  Adjusted the design of the cancellation feature so that it
                 works for pthread_cond_wait() and pthread_cond_timedwait()
                 now that they use the semExchange() API (required for SMP)
                 (defect #102076). Also fixed a bug in the synchronous path of
                 the cancellation.
02k,08aug07,jpb  Removed task locks from pthread_cond_wait and replaced it
                 with semExchange.  Kept the original code for UP.  Ported
                 user-side changes of pthread_once but kept in original code
                 for UP.
02j,02may07,jpb  SMP audit.  Added a locking mechanism that will lock the
                 thread and verify its validity; keeps it locked until
                 critical section work is finished.  Removed tasklocks in
                 MUTEX_INIT_COMPLETE and COND_INIT_COMPLETE macros and
                 replaced it with using a global semaphore (user-side
                 algorithm).  Removed CANCEL_LOCKS as they are not needed
                 anymore.  Renamed pthreadDeleteTask to pthread_abnormal_exit.
                 Moved PTHREAD_LOCK, PTHREAD_UNLOCK, and MY_PTHREAD macros
                 to private header file.
02i,07nov06,jpb  Added support to changing the priority if the scheduler is
                 SCHED_OTHER (defect #70991)
02h,29sep06,pad  Make sure that pthread_join() does not use null task names in
                 string comparisons (defect #67271 / fix #67273).
02g,22sep06,pad  Prevented a race condition to occur when a priority ceiling
                 mutex is released (defect #66491 / fix #66536).
02f,11sep06,pad  Fixed implementation of pthread_cond_broadcast() (defect
                 #64617 / fix #64618).
02e,10aug06,pad  Adapted to member name change in _Sched_param structure.
02d,02aug06,pad  Adapted to new definition of pthread_attr_t structure.
                 Re-instated taskIdCurrent in mutex lock/unlock routines for
                 performance reasons.
02c,26jul06,pad  Completed proper support for the PTHREAD_PRIO_PROTECT
                 protocol. Replaced taskIdCurrent with taskIdSelf() for SMP's
                 sake.
02b,13apr06,pad  Library's documentation update.
02a,03apr06,dgp  doc: fix formatting of SEE ALSO in pthread_cond_destroy( ),
                 pthread_cond_init( ), pthread_cond_timedwait( ), and
                 pthread_condattr_init( )
01z,08mar06,jpb  Updating various SPR's to reflect user side changes
                 SPR#118167: too late condition in pthread_cond_timedoutwait()
                 SPR#117769: check range in pthread_mutex_setprioceiling
                 SPR#114130: if cancelstate is enabled and if canceltype is
                 asynchronous, kill immediately otherwise set to pending.
                 SPR#114293: Created keyStatus in pthread_key
                 SPR#114360: pthread_mutexattr_init() attribute structure
                 initialization
                 SPR#117697: added check for pending cancellation requests
                 SPR#115263: Now pthread_exit() sets the cancelability state to
                 disabled and the cancelability type to deferred ().
01y,15nov05,rp   fixed pthread_mutex_unlock to check for ownership before
                 changing task priority (SPR 115119)
01x,21sep05,pad  Fixed incorrect information in documentation (SPR 112541).
01w,27aug05,pad  Updated documentation. Changed a few routine's manuals and
                 error codes for coherency with user-side pthreadLib.
01v,01mar05,job  cleanupPrivateData() was incorrectly freeing the global key
                 data structure, as well as the per-thread copy. This should
                 only be done by a call to pthread_key_delete(). SPR#106515.
                 Updated docs to pthread_key_create() and pthread_key_delete().
01u,06jan05,pad  Added call to taskIdVerify() in VALID_PTHREAD macro so that a
                 deleted thread is not accidentally considered valid when the
                 memory previously allocated to the internalPthread structure
                 is recycled (SPR #105025).
01t,01oct04,pad  pthread_join() now exits only when the VxWorks task behind the
                 thread is actually gone (SPR #101604).
                 Fixed a potential race condition between pthread_join() and
                 pthread_exit().
01s,25sep04,pad  Fixed a hole in the uniqueness of execution of the routine
                 called via pthread_once() (SPR #98589).
                 Fixed incorrect handling of initialized unlocked mutex in
                 pthread_mutex_destroy (SPR #101108).
01r,21sep04,pad  pthread_attr_setstacksize() now returns EINVAL if the stack
                 size is lower than PTHREAD_STACK_MIN. The documentation has
                 been updated to match the user-side pthreadLib's documentation.
                 Fixed a race condition between thread_join() and
                 pthread_exit().
01q,16jun04,pad  Updated documentation regarding cancellation points.
01p,09jun04,dgp  clean up formatting to correct doc build errors, convert to
                 apigen markup
01o,04jun04,pad  Added pthread_attr_setopt() and pthread_attr_getopt() to set
                 and get the task options for the VxWorks task under the
                 thread, and adapted pthread_attr_init() accordingly (SPR
                 92417).
01n,12may04,pad  Updated wrapperFunc()'s interface to pass caller's signal
                 mask (SPR 95215). Fixed pthread_mutex_destroy().
01m,27feb04,pad  Reset condValid to false in pthread_cond_destroy() (SPR#
                 82967).
01l,10dec03,pad  Now pthread_exit() sets the TASK_EXITED state of detached
                 threads before starting the cleanup work (SPR 92453).
01k,05dec03,pad  Now pthread_join() no longer generates an exception when its
                 status parameter is NULL (SPR #92320).
01j,13jan03,gls  made pthread_cond_wait() do atomic release/wait (SPR #85235)
                 merged changes to INIT_MUTEX/INIT_COND from Dot-4 (SPR #85312)
                 added check for waiting thread in pthread_cond_signal
                 (SPR #85264).
01i,03may02,gls  updated pthread_attr_setstackaddr documentation (SPR #76769)
01h,22apr02,gls  removed references to AE (SPR #75799)
01g,05nov01,gls  merged in code from AE
01f,24oct01,jgn  correct scheduling policy inheritance (SPR #71125)
                 (docs update - main change in _pthreadLib.c)
01e,04apr01,pfl  doc update (SPR #63976)
01d,11sep00,jgn  split into user and system level components (SPR #33375)
01c,22aug00,jgn  remove dependency on TCB spare4 field (SPR #33815) +
                 fix some memory handling & memory leaks (SPR #33554) +
                 move readdir_r() to dirLib.c (SPR #34056)
01b,15aug00,jgn  add bug fixes from DOT-4
01a,17jul00,jgn  created from DOT-4 version 1.17
*/

/*
DESCRIPTION
This library provides an implementation of POSIX 1003.1c threads for VxWorks.
This provides an increased level of compatibility between VxWorks applications
and those written for other operating systems that support the POSIX threads
model (often called <pthreads>).

VxWorks implements POSIX threads in the kernel based on tasks. Because the
kernel environment is different from a process environment, in the POSIX sense,
there are a few restrictions in the implementation, but in general,
since tasks are roughly equivalent to threads, the <pthreads> support maps
well onto VxWorks. The restrictions are explained in more detail in the
following paragraphs.

SYSTEM CALL HANDLERS CONSIDERATIONS
None of the pthread API may be invoked in the context of a system call
handler, either by being called directly in the system call handler or
by being called by any routine that the system call handler may use. Not
conforming to this restriction will lead to unspecified behavior and
failure.

CONFIGURATION
To add POSIX threads support to a system, the component INCLUDE_POSIX_PTHREADS
must be added.

THREADS
A thread is essentially a VxWorks task, with some additional characteristics.
The first is detachability, where the creator of a thread can optionally block
until the thread exits. The second is cancelability, where one task or thread
can cause a thread to exit, possibly calling cleanup handlers. The next is
private data, where data private to a thread is created, accessed and deleted
via keys. Each thread has a unique ID. A thread's ID is different than it's
VxWorks task ID.

It is recommended to use the POSIX thread API only via POSIX threads, not via
native VxWorks tasks. Since pthreads are not created by default in VxWorks the
pthread_create() API can be safely used by a native VxWorks task in order to
create the first POSIX thread. If a native VxWorks task must use more pthread
API it is recommended to give this task a pthread persona by calling
pthread_self() first. Once a native VxWorks task has called pthread_self() it
is recommended that it also uses the pthread_exit() API to exit.

Note that a native VxWorks task that has been given a POSIX thread persona
differs from a regular POSIX thread (i.e. created by the pthread_create() API)
in that it cannot be joined (i.e. it is as if it were created with the
PTHREAD_CREATE_DETACHED state). The reason being that there is no practical way
for another POSIX thread to join the newly formed pthread.

MUTEXES
Included with the POSIX threads facility is a mutual exclusion facility, or
<mutex>. These are functionally similar to the VxWorks mutex semaphores (see
'semMLib' for more detail), and in fact are implemented using a VxWorks
mutex semaphore. The advantage they offer, like all of the POSIX libraries,
is the ability to run software designed for POSIX platforms under VxWorks.

There are three types of locking protocols available: PTHREAD_PRIO_NONE,
PTHREAD_PRIO_INHERIT and PTHREAD_PRIO_PROTECT. PTHREAD_PRIO_INHERIT is the
default and maps to a semaphore created with SEM_Q_PRIORITY and
SEM_INVERSION_SAFE set (see 'semMCreate' for more detail). A thread locking a
mutex created with its protocol attribute set to PTHREAD_PRIO_PROTECT has its
priority elevated to that of of the prioceiling attribute of the mutex. When
the mutex is unlocked, the priority of the calling thread is restored to its
previous value. Both protocols aim at solving the priority inversion problem
where a lower priority thread can unduly delay a higher priority thread
requiring the resource blocked by the lower priority thread. The
PTHREAD_PRIO_INHERIT protocol can be more efficient since it elevates the
priority of a thread only when needed. The PTHREAD_PRIO_PROTECT protocol gives
more control over the priority change at the cost of systematically elevating
the thread's priority as well as preventing threads to use a mutex which
priority ceiling is lower than the thread's priority. In contrast the
PTHREAD_PRIO_NONE protocol does not affect the priority and scheduling of the
thread that owns the mutex.

CONDITION VARIABLES
Condition variables are another synchronization mechanism that is included
in the POSIX threads library. A condition variable allows threads
to block until some condition is met. There are really only two basic
operations that a condition variable can be involved in: waiting and
signalling. Condition variables are always associated with a mutex.

A thread can wait for a condition to become true by taking the mutex and
then calling pthread_cond_wait(). That function will release the mutex and
wait for the condition to be signalled by another thread. When the condition
is signalled, the function will re-acquire the mutex and return to the caller.

Condition variable support two types of signalling: single thread wake-up using
pthread_cond_signal(), and multiple thread wake-up using
pthread_cond_broadcast(). The latter of these will unblock all threads that
were waiting on the specified condition variable.

It should be noted that condition variable signals are not related to POSIX
signals. In fact, they are implemented using VxWorks semaphores.

RESOURCE COMPETITION
All tasks, and therefore all POSIX threads, compete for CPU time together. For
that reason the contention scope thread attribute is always
'PTHREAD_SCOPE_SYSTEM'.

NO VXWORKS EQUIVALENT
Since there is no notion of a process (in the POSIX sense) in the kernel
environment, there is no notion of sharing of locks (mutexes) and condition
variables between processes. As a result, the POSIX symbol
'_POSIX_THREAD_PROCESS_SHARED' is not defined in this implementation, and the
routines pthread_condattr_getpshared(), pthread_condattr_setpshared(),
pthread_mutexattr_getpshared() are not implemented.

Also, since the VxWorks kernel is not a process environment, fork(), wait(),
and pthread_atfork() are unimplemented.

SCHEDULING
The default scheduling policy for a created thread is inherited from the system
setting at the time of creation.

Unlike for the pthread support in RTPs, the POSIX threads in the kernel are not
scheduled by the POSIX scheduler. They are scheduled by the VxWorks native
scheduler, like all other VxWorks tasks: scheduling policies under VxWorks are
global; they are not set per-thread, as the POSIX model describes. As a result,
the <pthread> scheduling routines, as well as the POSIX scheduling routines
native to VxWorks, do not allow you to change the scheduling policy for kernel
pthreads. Under VxWorks you may set the scheduling policy in a thread, but if
it does not match the system's scheduling policy, an error is returned.

The detailed explanation for why this situation occurs is a bit convoluted:
technically the scheduling policy is an attribute of a thread (in that there
are pthread_attr_getschedpolicy() and pthread_attr_setschedpolicy() functions
that define what the thread's scheduling policy will be once it is created,
and not what any thread should do at the time they are called). A situation
arises where the scheduling policy in force at the time of a thread's
creation is not the same as set in its attributes. In this case
pthread_create() fails with the error 'EPERM'.

The bottom line is that under VxWorks, if you wish to specify the scheduling
policy of a kernel thread, you must set the desired global scheduling policy
to match. Kernel threads must then adhere to that scheduling policy, or use
the PTHREAD_INHERIT_SCHED mode to inherit the current mode and creator's
priority. Alternatively, you can also use pthreads in an RTP.

In the kernel, the POSIX scheduling policies are therefore mapped as follows:
\is
\i SCHED_FIFO
is mapped on VxWorks' preemptive priority scheduling.
\i SCHED_RR
is mapped on VxWorks' round-robin scheduling.
\i SCHED_OTHER
is mapped on the active VxWorks scheduling policy (either preemptive priority
scheduling or round-robin scheduling). This is the only meaningful scheduling
policy for kernel pthreads.
\ie


CREATION AND CANCELLATION
Each time a thread is created, the <pthreads> library allocates resources on
behalf of it. Each time a VxWorks task (i.e. one not created by the
pthread_create() function) uses a POSIX threads feature such as thread
private data or pushes a cleanup handler, the <pthreads> library creates
resources on behalf of that task as well.

Asynchronous thread cancellation is accomplished by way of a signal. A
special signal, SIGCNCL, has been set aside in this version of VxWorks for
this purpose. Applications should take care not to block or handle SIGCNCL.

Current cancellation points in system and library calls:
\ts
'Libraries' | 'cancellation points'
----------------------------------
aioPxLib   | aio_suspend
ioLib      | creat, open, read, write, close, fsync, fdatasync, fcntl
mqPxLib    | mq_receive, mq_send
pthreadLib | pthread_cond_timedwait, pthread_cond_wait, pthread_join, pthread_testcancel
semPxLib   | sem_wait
sigLib     | pause, sigsuspend, sigtimedwait, sigwait, sigwaitinfo, waitpid
timerLib   | sleep, nanosleep
\te

Caveat: due to the implementation of some of the I/O drivers in VxWorks, it
is possible that a thread cancellation request can not actually be honored.

SUMMARY MATRIX

\ts
'<pthread> function' | 'Implemented?' | 'Note(s)'
-------------------------------------------------
pthread_attr_destroy() | Yes |
pthread_attr_getdetachstate() | Yes |
pthread_attr_getinheritsched() | Yes |
pthread_attr_getname() | Yes | 6
pthread_attr_getopt() | Yes | 6
pthread_attr_getschedparam() | Yes |
pthread_attr_getschedpolicy() | Yes |
pthread_attr_getscope() | Yes |
pthread_attr_getstackaddr() | Yes |
pthread_attr_getstacksize() | Yes |
pthread_attr_init() | Yes |
pthread_attr_setdetachstate() | Yes |
pthread_attr_setinheritsched() | Yes |
pthread_attr_setname() | Yes | 6
pthread_attr_setopt() | Yes | 6
pthread_attr_setschedparam() | Yes |
pthread_attr_setschedpolicy() | Yes |
pthread_attr_setscope() | Yes | 2
pthread_attr_setstackaddr() | Yes |
pthread_attr_setstacksize() | Yes |
pthread_atfork() | No | 1
pthread_cancel() | Yes | 5
pthread_cleanup_pop() | Yes |
pthread_cleanup_push() | Yes |
pthread_condattr_destroy() | Yes |
pthread_condattr_getpshared() | No | 3
pthread_condattr_init() | Yes |
pthread_condattr_setpshared() | No | 3
pthread_cond_broadcast() | Yes |
pthread_cond_destroy() | Yes |
pthread_cond_init() | Yes |
pthread_cond_signal() | Yes |
pthread_cond_timedwait() | Yes |
pthread_cond_wait() | Yes |
pthread_create() | Yes |
pthread_detach() | Yes |
pthread_equal() | Yes |
pthread_exit() | Yes |
pthread_getschedparam() | Yes | 4
pthread_getspecific() | Yes |
pthread_join() | Yes |
pthread_key_create() | Yes |
pthread_key_delete() | Yes |
pthread_kill() | Yes |
pthread_once() | Yes |
pthread_self() | Yes |
pthread_setcancelstate() | Yes |
pthread_setcanceltype() | Yes |
pthread_setschedparam() | Yes | 4
pthread_setspecific() | Yes |
pthread_sigmask() | Yes |
pthread_testcancel() | Yes |
pthread_mutexattr_destroy() | Yes |
pthread_mutexattr_getprioceiling() | Yes |
pthread_mutexattr_getprotocol() | Yes |
pthread_mutexattr_getpshared() | No | 3
pthread_mutexattr_init() | Yes |
pthread_mutexattr_setprioceiling() | Yes |
pthread_mutexattr_setprotocol() | Yes |
pthread_mutexattr_setpshared() | No | 3
pthread_mutex_destroy() | Yes |
pthread_mutex_getprioceiling() | Yes |
pthread_mutex_init() | Yes |
pthread_mutex_lock() | Yes |
pthread_mutex_setprioceiling() | Yes |
pthread_mutex_trylock() | Yes |
pthread_mutex_unlock() | Yes |
\te

NOTES
\ml
\m 1
The pthread_atfork() function is not implemented since fork() is not
implemented in VxWorks.
\m 2
The contention scope thread scheduling attribute is always
PTHREAD_SCOPE_SYSTEM, since threads (i.e. tasks) contend for resources
with all other threads in the system.
\m 3
The routines pthread_condattr_getpshared(), pthread_attr_setpshared(),
pthread_mutexattr_getpshared() and pthread_mutexattr_setpshared() are not
supported, since these interfaces describe how condition variables and
mutexes relate to a process, and the VxWorks kernel is not a process
environment.
\m 4
The default scheduling policy is inherited from the current system setting.
The POSIX model of per-thread scheduling policies is not supported, since a
basic tenet of the design of VxWorks is a system-wide scheduling policy.
\m 5
Thread cancellation is supported in appropriate <pthread> routines and
those routines already supported by VxWorks. However, the complete list of
cancellation points specified by POSIX is not supported because routines
such as msync(), tcdrain(), and wait() are not implemented by VxWorks.
\m 6
VxWorks-specific routines provided as an extension to IEEE Std 1003.1 in order
to handle VxWorks tasks' attributes.
\me

INCLUDE FILES: pthread.h

SEE ALSO: taskLib, semMLib, semPxLib

\INTERNAL
PTHREAD TERMINATION
POSIX specifies only two ways for a pthread to terminate: go through the
pthread_exit() API either explicitly or implicitly (i.e. when the thread's
root function simply returns), or get canceled (i.e. another pthread called the
pthread_cancel() API against the to-be-canceled pthread).

On VxWorks however a third way of terminating a pthread is available although
-on purpose- not documented: deleting the VxWorks task underlying the POSIX
thread.

This library implements the necessary logic to cleanly terminate a pthread via
any of those three ways: all the resources of the exiting pthread are freed or
released, with care to respect the POSIX specs when applicable (most notably
that a joinable pthread going away will leave behind it a memory storage
holding its exit status; that memory storage is released by any other pthread
calling the pthread_join() API).

At this time only pthread_join() prevents asynchronous cancellations while
holding another pthread's lock. All other pthread API can be cancelled
harmlessly while executing the critical section (the lock is released in
pthread_exit()). If the implementation of any other pthread API changes so that
there is a risk of leaving a pthread's control block in an incoherent state
then that API must implement the same type of logic as pthread_join() does
regarding asynchronous cancellations.
*/


/* includes */

#include <vxWorks.h>
#include <vsbConfig.h>
#include <semLib.h>
#include <taskLib.h>
#include <timers.h>
#include <taskArchLib.h>
#include <private/timerLibP.h>
#include <types/vxTypesOld.h>
#include <private/taskLibP.h>
#include <private/pthreadLibP.h>        /* pthreadLibInit */
#include <kernelLib.h>
#include <taskHookLib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <memLib.h>
#include <fcntl.h>
#include <ioLib.h>
#include <logLib.h>
#include <sched.h>
#include <edrLib.h>
#include <sysLib.h>
#include <private/schedP.h>
#include <private/semLibP.h>
#include <vxAtomicLib.h>

#define __PTHREAD_SRC
#include <pthread.h>

#undef PTHREADS_DEBUG

#ifdef PTHREADS_DEBUG
#undef LOCAL
#define LOCAL
#endif /* PTHREADS_DEBUG */

/* defines */

/*
 * Implements an alternate interpretation of the dynamic between
 * pthread_detach() and pthread_join() by which joinable pthreads that
 * dynamically detached are still considered joinable but would get their
 * resources freed by pthread_exit() in case nobody joined them before they
 * exited.
 *
 * This alternate interpretation is fine by the POSIX PSE52 conformance's
 * point of view but introduces some backward compatibility concerns.
 */

#define DETACHED_PTHREADS_MAY_BE_JOINED         FALSE

/*
 * SELF_PTHREAD_ID is a shortcut for access to the address of the currently
 * executing thread's control block (acting as thread ID).
 */

#define SELF_PTHREAD_ID ((pthreadCB *)(((WIND_TCB *)(taskIdSelf()))->pPthread))

/*
 * PTHREAD_TO_NATIVE_TASK provides the ID of the VxWorks task underlying the
 * POSIX thread.
 */

#define PTHREAD_TO_NATIVE_TASK(thId)     (((pthreadCB *)(thId))->vxTaskId)

/*
 * VALID_PTHREAD verifies whether the pthread data is valid and whether
 * the pthread ID corresponds to that registered for the underlying VxWorks
 * task (which means the pthread ID is not stale).
 */

#define VALID_PTHREAD(pThread, pTcb)                                    \
                ((pThread) && (((pThread)->flags & VALID) == VALID) &&  \
                 (((pThread)->flags & TASK_EXITED) == 0) &&             \
                 ((pTcb) = (WIND_TCB *)(pThread)->vxTaskId) &&          \
                 (pTcb)->pPthread &&                                    \
                 (pTcb)->pPthread == (pThread))

/*
 * POSIX thread's mutex can be statically initialized, i.e. they do not
 * always require a call to pthread_mutex_init() before they are used.
 * Since POSIX thread mutexes are implemented on top of VxWorks' mutual
 * exclusion semaphores, which by design require a call to the semMCreate()
 * routine, we can not actually provide statically initialized mutex.
 * The solution is to have any pthread routine which manipulates a mutex
 * initialize the underlying VxWorks semaphore if it is not already
 * initialized. Since mutex are, by nature, objects shared between threads
 * we need to ensure mutual exclusion around the initialization sequence as
 * well. This is what the INIT_MUTEX macro below is all about. The first
 * check is not protected because in the immense majority of the cases the
 * mutex is already initialized, then there is nothing to do, so we don't
 * want to get the performance penalty of mutual exclusion. If however the
 * mutex appears to be still uninitialized then we go and try to initialize
 * it. This second part is under mutual exclusion protection and re-checks
 * whether the mutex is now initialized. This prevents race conditions in
 * case more than one thread managed to see the mutex as uninitialized:
 * only one of them will perform the initialization, the others will then
 * skip this part.
 */

#define MUTEX_INIT_COMPLETE(pMutex)                                     \
    if (pMutex->mutexInitted != PTHREAD_INITIALIZED_OBJ)                \
        {                                                               \
        /*                                                              \
         * The mutex appears uninitialized still. First, protect the    \
         * mutex initialization sequence from concurrent calls.         \
         */                                                             \
                                                                        \
        if (semTake (pthreadLibMutexInitSemId, WAIT_FOREVER) == ERROR)  \
            return EINVAL;                      /* internal error */    \
                                                                        \
        if (pMutex->mutexInitted != PTHREAD_INITIALIZED_OBJ)            \
            {                                                           \
            /*                                                          \
             * If we come here, no one else has initialized this mutex  \
             * yet, so we do it.                                        \
             */                                                         \
                                                                        \
            pMutex->mutexSemId = semMCreate (                           \
                    ((pMutex->mutexAttr.mutexAttrProtocol ==            \
                      PTHREAD_PRIO_INHERIT) ? SEM_INVERSION_SAFE : 0) | \
                      SEM_DELETE_SAFE | SEM_Q_PRIORITY);                \
             if (pMutex->mutexSemId == SEM_ID_NULL)                     \
                {                                                       \
                (void)semGive (pthreadLibMutexInitSemId);               \
                return EINVAL;                  /* internal error */    \
                }                                                       \
                                                                        \
            *((volatile int *)&(pMutex)->mutexInitted) =                \
                                        PTHREAD_INITIALIZED_OBJ;        \
            }                                                           \
                                                                        \
        if (semGive (pthreadLibMutexInitSemId) == ERROR)                \
            return EINVAL;                      /* internal error */    \
        }

/*
 * The very same logic as for the POSIX thread mutexes applies to the
 * POSIX thread condition variables. Read it above.  Note also that
 * pCond may contain initialized info passed by user code, thus
 * INIT_COND needs to be careful not to modify those fields.
 */

#define COND_INIT_COMPLETE(pCond)                                       \
    if ((pCond)->condInitted != PTHREAD_INITIALIZED_OBJ)                \
        {                                                               \
        /*                                                              \
         * Protect condition variable initialization sequence from      \
         * concurrent calls.                                            \
         */                                                             \
                                                                        \
        if (semTake (pthreadLibCondInitSemId, WAIT_FOREVER) == ERROR)   \
            return EINVAL;  /* internal error */                        \
                                                                        \
        if ((pCond)->condInitted != PTHREAD_INITIALIZED_OBJ)            \
            {                                                           \
            /*                                                          \
             * If we come here, no one else has initialized this        \
             * condition variable yet, so we do it.                     \
             */                                                         \
                                                                        \
            if (((pCond)->condSemId = semBCreate((SEM_Q_PRIORITY |      \
                                        SEM_KERNEL_INTERRUPTIBLE),      \
                                        SEM_EMPTY)) == SEM_ID_NULL)     \
                {                                                       \
                (void)semGive (pthreadLibCondInitSemId);                \
                return (EINVAL); /* internal error */                   \
                }                                                       \
                                                                        \
            *((volatile int *)&(pCond)->condInitted) =                  \
                                        PTHREAD_INITIALIZED_OBJ;        \
            }                                                           \
        if (semGive (pthreadLibCondInitSemId) == ERROR)                 \
            return EINVAL;                                              \
        }


/* numeric boundary check for priority. no mention of meaning of value */

#define PRIO_LOWER_BOUND 0
#define PRIO_UPPER_BOUND 255
#define VALID_PRIORITY(pri)     (PRIO_LOWER_BOUND <= (pri) && \
                                 (pri) <= PRIO_UPPER_BOUND)
#define PX_VX_PRI_CONV(mode,pri) (mode ? (POSIX_HIGH_PRI - pri) : pri)

#define DEF_PRIORITY    (PRIO_LOWER_BOUND + \
                                (PRIO_UPPER_BOUND - PRIO_LOWER_BOUND) / 2)

/* Simple pthread lock/unlock macro pair */

#define PTHREAD_LOCK(pThread)                                   \
    {                                                           \
    if (semTake ((pThread)->lock, WAIT_FOREVER) == ERROR)       \
        return EINVAL;                                          \
    }
#define PTHREAD_UNLOCK(pThread) (void)semGive ((pThread)->lock)

/* Pthread validate-and-lock/unlock-validated macro pair */

#define PTHREAD_VALIDATE_AND_LOCK(pThread, self)        \
    pthreadValidateAndLock (pThread, self)
#define PTHREAD_VALIDATED_UNLOCK(pThread, self)         \
    {                                                   \
    PTHREAD_UNLOCK (pThread);                           \
    self->lockedPthread = 0;                            \
    }
/*
 * Key status definitions.
 *
 * SPR# 114293: It is necessary to determine that a key has been
 *              created but has not been used (which is done by a
 *              call to pthread_key_setspecific). These definitions
 *              permit this detection.
 *
 * KEY_FREE     : the key slot in the key_table is FREE
 * KEY_CREATED  : The key slot in the key_table is in use but
 *                does not contain valid data.
 *                (a destructor is bound to the key)
 * KEY_IN_USE   : The key slot in the key_table is in use and
 *                contains valid data
 */

#define KEY_FREE        0
#define KEY_CREATED     1
#define KEY_IN_USE      2

/* typedefs */

typedef struct
    {
    long               keyStatus;
    void               (*destructor)();
    } pthread_key;

/* globals */

/*
 * This is a common. DO NOT INITIALISE IT - doing so will break the scalability
 * that relies on its status as a common.
 */

FUNCPTR _pthread_setcanceltype;

/* locals */

LOCAL pthread_key       key_table[_POSIX_THREAD_KEYS_MAX];
LOCAL SEM_ID            key_mutex                       = NULL;
LOCAL SEM_ID            pthreadLibOnceSemId             = NULL;
LOCAL SEM_ID            pthreadLibMutexInitSemId        = NULL;
LOCAL SEM_ID            pthreadLibCondInitSemId         = NULL;
LOCAL SEM_ID            pthreadLibSemId                 = NULL;

/* The default mutex, condition variable and thread creation attribute objects
 * and initializers - either POSIX or implementation-defined.
 * Used by:
 *  pthread_mutex_init()
 *  pthread_cond_init()
 *  pthread_create()
 *
 * For mutexes and condition variables,
 * only the 'process-shared' attribute is defined.
 */

LOCAL pthread_mutexattr_t defaultMutexAttr =
    {
    PTHREAD_INITIALIZED_OBJ, PTHREAD_PRIO_INHERIT, 0,
    PTHREAD_MUTEX_DEFAULT
    };

/* forward declarations */

LOCAL void self_become_pthread(void);
LOCAL void pthread_abnormal_exit (pthreadCB * pThread);
LOCAL void pthreadCtrlBlockFree (pthreadCB * pThread);
LOCAL void cleanupPrivateData (pthreadCB * pThread);
LOCAL STATUS pthreadValidateAndLock (pthreadCB * pThread,
                                     pthreadCB * selfPthreadId);
_WRS_INLINE pthreadCB * _pthread_id_self (void);
_WRS_INLINE void _pthread_testcancel_inline (pthreadCB * pThread);
_WRS_INLINE void _pthread_setcanceltype_inline	(int type, int * oldtype,
                                               pthreadCB *pThread);
static const void ** pthread_key_allocate_data (void);
LOCAL void deadlock (void);
LOCAL void sigCancelHandler (int dummy);
LOCAL void wrapperFunc (void * (*function)(void *), void * arg,
                        const sigset_t creatorSigMask);
_WRS_INLINE void _pthread_onexit_cleanup (pthreadCB * pThread,
                                          BOOL abnormalTermination);

/*******************************************************************************
*
* pthreadLibInit - initialize POSIX threads support
*
* This routine initializes the POSIX threads (<pthreads>) support for
* VxWorks. It should be called before any POSIX threads functions are used;
* normally it will be called as part of the kernel's initialization sequence.
*
* RETURNS: N/A
*
* \NOMANUAL
*/

void pthreadLibInit (void)
    {
    if  (_pthreadLibInit (pthread_abnormal_exit) != ERROR)
        {

        /*
         * Mutual exclusion semaphore protecting the validation
         * of the pThread
         */

        if ((pthreadLibSemId = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
                                           SEM_INVERSION_SAFE)) == SEM_ID_NULL)
            {
            EDR_INIT_FATAL_INJECT (NULL, NULL, NULL, NULL,
                                   "pthreadLib: cannot create pThreadLibSemId "
                                   "mutex.");
            }

        /* Create the thread specific data key */

        if ((key_mutex = semMCreate (SEM_Q_PRIORITY | SEM_INVERSION_SAFE)) ==
                                     SEM_ID_NULL)
            {
            EDR_INIT_FATAL_INJECT (NULL, NULL, NULL, NULL,
                                   "pthreadLib: cannot create key_mutex.");
            }

        /*
         * Create the mutual exclusion semaphore protecting the use of the
         * pthread_once() routine.
         */

        if ((pthreadLibOnceSemId = semMCreate (SEM_Q_PRIORITY)) == SEM_ID_NULL)
            {
            EDR_INIT_FATAL_INJECT (NULL, NULL, NULL, NULL,
                                   "pthreadLib: cannot create "
                                   "pthreadLibOnceSemIdmutex.");
            }

        /*
         * Create the mutual exclusion semaphore protecting the initialization
         * of the pthread mutex.
         */

        if ((pthreadLibMutexInitSemId = semMCreate (SEM_Q_PRIORITY)) ==
                                                    SEM_ID_NULL)
            {
            EDR_INIT_FATAL_INJECT (NULL, NULL, NULL, NULL,
                                   "pthreadLib: cannot create "
                                   "pthreadLibMutexInitSemId.");
            }

        /*
         * Create the mutual exclusion semaphore protecting the initialization
         * of the pthread conditional variable.
         */

        if ((pthreadLibCondInitSemId = semMCreate (SEM_Q_PRIORITY)) ==
                                                   SEM_ID_NULL)
            {
            EDR_INIT_FATAL_INJECT (NULL, NULL, NULL, NULL,
                                   "pthreadLib: cannot create "
                                   "pthreadLibCondInitSemId.");
            }

        /*
         * Initialize the function pointer for user level access to this
         * feature.
         */

        _pthread_setcanceltype = (FUNCPTR) pthread_setcanceltype;
        }
    }

/*
 *  Section 3 - Process Primitives
 */

/*******************************************************************************
*
* pthread_sigmask - change and/or examine calling thread's signal mask (POSIX)
*
* This routine changes the signal mask for the calling thread as described
* by the <how> and <set> arguments. If <oset> is not NULL, the previous
* signal mask is stored in the location pointed to by it.
*
* The value of <how> indicates the manner in which the set is changed and
* consists of one of the following defined in 'signal.h':
* \is
* \i SIG_BLOCK
* The resulting set is the union of the current set and the signal set
* pointed to by <set>.
* \i SIG_UNBLOCK
* The resulting set is the intersection of the current set and the complement
* of the signal set pointed to by <set>.
* \i SIG_SETMASK
* The resulting set is the signal set pointed to by <oset>.
* \ie
*
* RETURNS: On success zero; on failure a 'EINVAL' error code is returned.
*
* ERRNO: N/A
*
* SEE ALSO: kill(), pthread_kill(), sigprocmask(), sigaction(),
* sigsuspend(), sigwait()
*/

int pthread_sigmask
    (
    int                 how,            /* method for changing set      */
    const sigset_t *    set,            /* new set of signals           */
    sigset_t *          oset            /* old set of signals           */
    )
    {
    if (sigprocmask(how, (sigset_t *) set, oset) == ERROR)
        return (EINVAL);

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_kill - send a signal to a thread (POSIX)
*
* This routine sends signal number <sig> to the thread specified by <thread>.
* The signal is delivered and handled as described for the kill() function.
*
* RETURNS: On success zero; on failure one of the following non-zero error
* codes: 'ESRCH', 'EINVAL'
*
* ERRNO: N/A
*
* SEE ALSO: kill(), pthread_sigmask(), sigprocmask(), sigaction(),
* sigsuspend(), sigwait()
*
* INTERNAL
* The taskKill() system call (indirectly) sets the errno to EAGAIN when on
* failure. Since this value can be directly returned by pthread_kill() we
* have a conflict with IEEE Std 1003.1 here...
*/

int pthread_kill
    (
    pthread_t   thread,         /* thread to signal */
    int         sig             /* signal to send */
    )
    {
    pthreadCB * pThread = (pthreadCB *)thread;
    WIND_TCB *  pTcb;

    if ((sig < 0) || (sig > _NSIGS))
        return (EINVAL);

    /*
     * Let's simply validate the targeted pthread. Locking it would be
     * overkill since we do not need to modify its control block.
     * There is a possibility that the targeted pthread is canceled or
     * deleted during or after the validation. However the kill() API
     * verifies the task ID for us.
     */

    if (VALID_PTHREAD (pThread, pTcb) == FALSE)
        return (ESRCH);

    if ((sig == 0) || (kill ((TASK_ID)pTcb, sig)) == OK)
        return (_RETURN_PTHREAD_SUCCESS);
    else
        return (errno);
    }

/*
 * Section 11.3 - Mutexes
 */

/*******************************************************************************
*
* pthread_mutexattr_init - initialize mutex attributes object (POSIX)
*
* This routine initializes the mutex attribute object <pAttr> and fills it
* with  default values for the attributes:
* .iP "'Mutex Protocol'" 4
* PTHREAD_PRIO_INHERIT - the priority of the owner thread is temporarily raised
* if a higher priority thread is blocked on the mutex.
* .iP "'Mutex Priority Ceiling'"
* 0 - lowest priority.
* .LP
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutexattr_destroy(), pthread_mutexattr_getprioceiling(),
* pthread_mutexattr_getprotocol(), pthread_mutexattr_setprioceiling(),
* pthread_mutexattr_setprotocol(), pthread_mutex_init()
*/

int pthread_mutexattr_init
    (
    pthread_mutexattr_t * pAttr         /* mutex attributes */
    )
    {
    if (pAttr == NULL)
        return (EINVAL);

    /* SPR# 114360 - added initialization to other members of struct */

    pAttr->mutexAttrStatus = PTHREAD_INITIALIZED_OBJ;
    pAttr->mutexAttrProtocol = PTHREAD_PRIO_INHERIT;
    pAttr->mutexAttrPrioceiling = 0;
    pAttr->mutexAttrType = PTHREAD_MUTEX_DEFAULT;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_destroy - destroy mutex attributes object (POSIX)
*
* This routine destroys a mutex attribute object. The mutex attribute object
* must not be reused until it is reinitialized.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutexattr_getprioceiling(), pthread_mutexattr_getprotocol(),
* pthread_mutexattr_init(), pthread_mutexattr_setprioceiling(),
* pthread_mutexattr_setprotocol(), pthread_mutex_init()
*/

int pthread_mutexattr_destroy
    (
    pthread_mutexattr_t * pAttr         /* mutex attributes */
    )
    {
    if (pAttr == NULL)
        return (EINVAL);

    pAttr->mutexAttrStatus = PTHREAD_DESTROYED_OBJ;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_setprotocol - set protocol attribute in mutex attribute object (POSIX)
*
* This function selects the locking protocol to be used when a mutex is created
* using this attributes object. The protocol to be selected is either
* PTHREAD_PRIO_NONE, PTHREAD_PRIO_INHERIT or PTHREAD_PRIO_PROTECT.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i ENOTSUP
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutexattr_destroy(), pthread_mutexattr_getprioceiling(),
* pthread_mutexattr_getprotocol(), pthread_mutexattr_init(),
* pthread_mutexattr_setprioceiling(), pthread_mutex_init()
*/

int pthread_mutexattr_setprotocol
    (
    pthread_mutexattr_t * pAttr,        /* mutex attributes     */
    int                   protocol      /* new protocol         */
    )
    {
    if ((pAttr == NULL) || pAttr->mutexAttrStatus != PTHREAD_INITIALIZED_OBJ)
        return (EINVAL);

    if (protocol != PTHREAD_PRIO_NONE && protocol != PTHREAD_PRIO_INHERIT &&
        protocol != PTHREAD_PRIO_PROTECT)
        {
        return (ENOTSUP);
        }

    pAttr->mutexAttrProtocol = protocol;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_getprotocol - get value of protocol in mutex attributes object (POSIX)
*
* This function gets the current value of the protocol attribute in a mutex
* attributes object.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutexattr_destroy(), pthread_mutexattr_getprioceiling(),
* pthread_mutexattr_init(), pthread_mutexattr_setprioceiling(),
* pthread_mutexattr_setprotocol(), pthread_mutex_init()
*/

int pthread_mutexattr_getprotocol
    (
    pthread_mutexattr_t * pAttr,        /* mutex attributes             */
    int *                 pProtocol     /* current protocol (out)       */
    )
    {
    if ((pAttr == NULL) || (pAttr->mutexAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    *pProtocol = pAttr->mutexAttrProtocol;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_setprioceiling - set prioceiling attribute in mutex attributes object (POSIX)
*
* This function sets the value of the prioceiling attribute in a mutex
* attributes object. Unless the protocol attribute is set to
* PTHREAD_PRIO_PROTECT, this attribute is ignored.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutexattr_destroy(), pthread_mutexattr_getprioceiling(),
* pthread_mutexattr_getprotocol(), pthread_mutexattr_init(),
* pthread_mutexattr_setprotocol(), pthread_mutex_init()
*/

int pthread_mutexattr_setprioceiling
    (
    pthread_mutexattr_t * pAttr,                /* mutex attributes     */
    int                   prioceiling           /* new priority ceiling */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->mutexAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (prioceiling < PRIO_LOWER_BOUND || prioceiling > PRIO_UPPER_BOUND))
        {
        return (EINVAL);
        }

    pAttr->mutexAttrPrioceiling = prioceiling;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_getprioceiling - get the current value of the prioceiling attribute in a mutex attributes object (POSIX)
*
* This function gets the current value of the prioceiling attribute in a mutex
* attributes object. Unless the value of the protocol attribute is
* PTHREAD_PRIO_PROTECT, this value is ignored.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutexattr_destroy(), pthread_mutexattr_getprotocol(),
* pthread_mutexattr_init(), pthread_mutexattr_setprioceiling(),
* pthread_mutexattr_setprotocol(), pthread_mutex_init()
*/

int pthread_mutexattr_getprioceiling
    (
    pthread_mutexattr_t * pAttr,        /* mutex attributes               */
    int *                 pPrioceiling  /* current priority ceiling (out) */
    )
    {
    if ((pAttr == NULL) || (pAttr->mutexAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    *pPrioceiling = pAttr->mutexAttrPrioceiling;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_getprioceiling - get the value of the prioceiling attribute of a mutex (POSIX)
*
* This function gets the current value of the prioceiling attribute of a mutex.
* Unless the mutex was created with a protocol attribute value of
* PTHREAD_PRIO_PROTECT, this value is meaningless.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutex_setprioceiling(), pthread_mutexattr_getprioceiling(),
* pthread_mutexattr_setprioceiling()
*/

int pthread_mutex_getprioceiling
    (
    pthread_mutex_t * pMutex,           /* pthread mutex                  */
    int *             pPrioceiling      /* current priority ceiling (out) */
    )
    {
    if ((pMutex == NULL) || (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    MUTEX_INIT_COMPLETE(pMutex);

    *pPrioceiling = pMutex->mutexAttr.mutexAttrPrioceiling;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_setprioceiling - dynamically set the prioceiling attribute of a mutex (POSIX)
*
* This function dynamically sets the value of the prioceiling attribute of a
* mutex. Unless the mutex was created with a protocol value of
* PTHREAD_PRIO_PROTECT, this function does nothing.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i EPERM
* \i 'S_objLib_OBJ_ID_ERROR'
* \i 'S_semLib_NOT_ISR_CALLABLE'
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: pthread_mutex_getprioceiling(), pthread_mutexattr_getprioceiling(),
* pthread_mutexattr_setprioceiling()
*/

int pthread_mutex_setprioceiling
    (
    pthread_mutex_t * pMutex,           /* pthread mutex                */
    int               prioceiling,      /* new priority ceiling         */
    int *             pOldPrioceiling   /* old priority ceiling (out)   */
    )
    {
    if ((pMutex == NULL) || (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    /* SPR# 117769 - checks to see if prioceiling is out of range */

    if ((prioceiling < PRIO_LOWER_BOUND) || (prioceiling > PRIO_UPPER_BOUND))
        return (EINVAL);

    MUTEX_INIT_COMPLETE(pMutex);

    if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
        return (EINVAL);                        /* internal error */

    /* SPR# 117249 - checks to see if pOldPrioceiling is NULL */

    if (pOldPrioceiling)
        *pOldPrioceiling = pMutex->mutexAttr.mutexAttrPrioceiling;

    pMutex->mutexAttr.mutexAttrPrioceiling = prioceiling;

    /* Clear errno before trying to test it */

    errno = 0;

    if (semGive(pMutex->mutexSemId) == ERROR)
        {
        if (errno == S_semLib_INVALID_OPERATION)
            return (EPERM);                     /* not owner - can't happen */
        else
            return (errno);                     /* some other error */
        }

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_settype - set type attribute in mutex attributes object (POSIX)
*
* This function sets the type attribute in a mutex attributes object. The
* default value of the type attribute is PTHREAD_MUTEX_DEFAULT.
* Valid mutex types are: 
* \is
* \i PTHREAD_MUTEX_NORMAL
* deadlock detection is not provided; attempt to relock causes deadlock;
* attempt to unlock a mutex owned by another thread or unlock a unlocked mutex
* returns error.
* \i PTHREAD_MUTEX_ERRORCHECK
* error checking is provided; attempt to relock a mutex or unlock a mutex owned
* by another thread or unlock a unlocked mutex returns error.
* \i PTHREAD_MUTEX_RECURSIVE
* can be relocked by a thread.
* \i PTHREAD_MUTEX_DEFAULT
* set to PTHREAD_MUTEX_NORMAL in VxWorks implementation.
* \ie
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:  pthread_mutexattr_destroy(), pthread_mutexattr_gettype(),
* pthread_mutex_lock(), pthread_mutex_trylock(), pthread_mutex_unlock(),
* pthread_mutexattr_init(), pthread_mutex_init()
*/

int pthread_mutexattr_settype
    (
    pthread_mutexattr_t * pAttr,         /* mutex attributes     */
    int                   type           /* mutex type           */
    )
    {
    if ((pAttr == NULL) || (pAttr->mutexAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    switch (type)
        {
        case PTHREAD_MUTEX_NORMAL:
        case PTHREAD_MUTEX_ERRORCHECK:
        case PTHREAD_MUTEX_RECURSIVE:

        /* case PTHREAD_MUTEX_DEFAULT: not required, mapped to normal */

            pAttr->mutexAttrType = type;
            break;
        default:
            return (EINVAL);
        }

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutexattr_gettype - get the current value of the type attribute in a mutex attributes object (POSIX)
*
* This function gets the current value of the type attribute in a mutex
* attributes object.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:  pthread_mutexattr_destroy(), pthread_mutexattr_init(),
* pthread_mutexattr_settype(), pthread_mutex_init()
*/

int pthread_mutexattr_gettype
    (
    pthread_mutexattr_t * pAttr,       /* mutex attributes      */
    int *                 pType        /* current mutex type (out) */
    )
    {
    if ((pAttr == NULL) || (pType == NULL) ||
        (pAttr->mutexAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    *pType = pAttr->mutexAttrType;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_init - initialize mutex from attributes object (POSIX)
*
* This routine initializes the mutex object pointed to by <pMutex> according
* to the mutex attributes specified in <pAttr>.  If <pAttr> is NULL, default
* attributes are used as defined in the POSIX specification. If <pAttr> is
* non-NULL then it is assumed to point to a mutex attributes object initialized
* by pthread_mutexattr_init(), and those are the attributes used to create the
* mutex.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: semLib, semMLib, pthread_mutex_destroy(), pthread_mutex_lock(),
* pthread_mutex_trylock(), pthread_mutex_unlock(), pthread_mutexattr_init(),
* semMCreate()
*/

int pthread_mutex_init
    (
    pthread_mutex_t * pMutex,           /* pthread mutex                */
    const pthread_mutexattr_t * pAttr   /* mutex attributes             */
    )
    {
    pthread_mutexattr_t *pSource;

    if (pMutex == NULL)
        return (EINVAL);

    pSource = pAttr ? (pthread_mutexattr_t *)pAttr : &defaultMutexAttr;

    bcopy((const char *)pSource, (char *)&pMutex->mutexAttr,
          sizeof (pthread_mutexattr_t));

    pMutex->mutexSemId          = NULL;
    pMutex->mutexValid          = PTHREAD_VALID_OBJ;
    pMutex->mutexInitted        = PTHREAD_UNUSED_YET_OBJ;
    pMutex->mutexCondRefCount   = 0;
    pMutex->mutexSavPriority    = -1;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_destroy - destroy a mutex (POSIX)
*
* This routine destroys a mutex object, freeing the resources it might hold.
* The mutex can be safely destroyed when unlocked. On VxWorks a thread may
* destroy a mutex that it owns (i.e. that the thread has locked). If the
* mutex is locked by an other thread this routine will return an error
* ('EBUSY').
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i EBUSY
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: semLib, semMLib, pthread_mutex_init(), pthread_mutex_lock(),
* pthread_mutex_trylock(), pthread_mutex_unlock(), pthread_mutexattr_init(),
* semDelete()
*/

int pthread_mutex_destroy
    (
    pthread_mutex_t * pMutex            /* pthread mutex                */
    )
    {
    TASK_ID owner;

    if ((pMutex == NULL) || (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    /*
     * POSIX says that it is allowed to destroy an initialized but
     * unlocked (i.e. no owner) mutex. Let's move the mutex initialization of
     * the mutex earlier (SPR #101108).
     */

    MUTEX_INIT_COMPLETE(pMutex);

    if (pMutex->mutexCondRefCount)
        return (EBUSY);

    owner = _pthreadSemOwnerGet (pMutex->mutexSemId);

    if ((owner != 0) && (owner != taskIdSelf()))
        return (EBUSY);

    /*
     * Underlying VxWorks semaphore is acquired to make sure
     * that it can be deleted
     */

    if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
        return (EINVAL);

    (void)pthread_mutexattr_destroy(&(pMutex->mutexAttr));

    if (semDelete(pMutex->mutexSemId) == ERROR)
        return (EINVAL);

    pMutex->mutexValid          = PTHREAD_INVALID_OBJ;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_lock - lock a mutex (POSIX)
*
* This routine locks the mutex specified by <pMutex>. If the mutex is
* currently unlocked, it becomes locked, and is said to be owned by the
* calling thread. In this case pthread_mutex_lock() returns immediately.
*
* If the mutex is already locked by another thread, pthread_mutex_lock()
* blocks the calling thread until the mutex is unlocked by its current owner.
*
* If it is already locked by the calling thread, pthread_mutex_lock will
* deadlock on itself and the thread will block indefinitely.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: semLib, semMLib, pthread_mutex_init(), pthread_mutex_lock(),
* pthread_mutex_trylock(), pthread_mutex_unlock(), pthread_mutexattr_init(),
* semTake()
*/

int pthread_mutex_lock
    (
    pthread_mutex_t * pMutex            /* POSIX mutex          */
    )
    {
    pthreadCB * tid;    /* caller thread */
    int priority;       /* underlying VxWorks task's priority */

    if ((pMutex == NULL) || (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    MUTEX_INIT_COMPLETE(pMutex);

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    tid = _pthread_id_self();

    /*
     * The following statement checks the assertion that, when the mutex
     * protocol is set to PTHREAD_PRIO_PROTECT, the routine should fail with
     * EINVAL if the calling thread's priority is higher (i.e. is lower in
     * terms of VxWorks priority range) than the mutex's priority ceiling.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        {
        (void) taskPriorityGet (PTHREAD_TO_NATIVE_TASK (tid), &priority);
        if (priority <
                PX_VX_PRIORITY_CONVERT(pMutex->mutexAttr.mutexAttrPrioceiling))
            return (EINVAL);            /* would violate priority ceiling */
        }

    /*
     * Plain old POSIX mutexes deadlock if the owner of a lock tries to
     * acquire it again. The Open Group's Unix98 standard as well as POSIX
     * P1003.1j Draft standard allow different lock types that at this point
     * would have different behavior - an error checking mutex would return
     * EDEADLK, and a recursive lock would bump a ref count and return
     * success.
     *
     * XXX PAD - we should port to the kernel the new mutex types supported
     *           in RTP.
     */

    if (_pthreadSemOwnerGet (pMutex->mutexSemId) == PTHREAD_TO_NATIVE_TASK(tid))
        {
        if (pMutex->mutexAttr.mutexAttrType == PTHREAD_MUTEX_NORMAL)
            deadlock();
        else if (pMutex->mutexAttr.mutexAttrType == PTHREAD_MUTEX_ERRORCHECK)
            return (EDEADLK);
        }

    /*
     * If the mutex protocol is set to PTHREAD_PRIO_PROTECT the priority of
     * the calling thread is raised to the ceiling. We know (see logic above)
     * that the calling thread's priority is not higher than the ceiling so we
     * can safely directly set the new priority (a no-op if the thread's
     * priority is equal to the ceiling).
     *
     * Note that this has to happened before the VxWorks semaphore is taken to
     * avoid priority inversion but there is a window during which the thread's
     * priority is elevated although it has not yet taken the underlying
     * VxWorks semaphore. This cannot be solved satisfactorily without
     * getting native support for priority ceiling in vxWorks semaphores.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        taskPrioritySet (PTHREAD_TO_NATIVE_TASK (tid),
            PX_VX_PRIORITY_CONVERT(pMutex->mutexAttr.mutexAttrPrioceiling));

    /*
     * Some other internal error occurred, lets go back to the original
     * priority if the PTHREAD_PRIO_PROTECT protocol is used.
     */

    if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
        {
        if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
            taskPrioritySet (PTHREAD_TO_NATIVE_TASK (tid), priority);

        return (EINVAL);
        }

    /*
     * If this is a priority ceiling mutex, store the thread's original
     * priority in the mutex structure now that the mutex is safely acquired
     * by the thread. Doing this allows for acquiring nested priority ceiling
     * mutex while restoring the appropriate priority level when each of
     * those mutex are released in turn.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        pMutex->mutexSavPriority = priority;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_trylock - lock mutex if it is available (POSIX)
*
* This routine locks the mutex specified by <pMutex>. If the mutex is
* currently unlocked, it becomes locked and owned by the calling thread. In
* this case pthread_mutex_trylock() returns immediately.
*
* If the mutex is already locked by another thread, pthread_mutex_trylock()
* returns immediately with the error code 'EBUSY'.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i EBUSY
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: semLib, semMLib, pthread_mutex_init(), pthread_mutex_lock(),
* pthread_mutex_trylock(), pthread_mutex_unlock(), pthread_mutexattr_init(),
* semTake()
*/

int pthread_mutex_trylock
    (
    pthread_mutex_t * pMutex            /* pthread mutex                */
    )
    {
    pthreadCB * tid;    /* caller thread */
    int priority;       /* underlying VxWorks task's priority */

    if ((pMutex == NULL) || (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    MUTEX_INIT_COMPLETE(pMutex);

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    tid = _pthread_id_self();

    /*
     * The following statement checks the assertion that, when the mutex
     * protocol is set to PTHREAD_PRIO_PROTECT, the routine should fail with
     * EINVAL if the calling thread's priority is higher (i.e. is lower in
     * terms of VxWorks priority range) than the mutex's priority ceiling.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        {
        (void) taskPriorityGet (PTHREAD_TO_NATIVE_TASK (tid), &priority);
        if (priority <
                PX_VX_PRIORITY_CONVERT(pMutex->mutexAttr.mutexAttrPrioceiling))
            return (EINVAL);            /* would violate priority ceiling */
        }

    /*
     * If the thread already owns the mutex and the mutex type is either
     * PTHREAD_MUTEX_NORMAL/PTHREAD_MUTEX_DEFAULT or PTHREAD_MUTEX_ERRORCHECK, 
     * return immediately with EBUSY error. 
     */

    if ((_pthreadSemOwnerGet (pMutex->mutexSemId) == PTHREAD_TO_NATIVE_TASK(tid)) &&
        ((pMutex->mutexAttr.mutexAttrType == PTHREAD_MUTEX_NORMAL) ||
         (pMutex->mutexAttr.mutexAttrType == PTHREAD_MUTEX_ERRORCHECK)))
        return(EBUSY);

    /*
     * If the mutex protocol is set to PTHREAD_PRIO_PROTECT the priority of
     * the calling thread is raised to the ceiling. We know (see logic above)
     * that the calling thread's priority is not higher than the ceiling so we
     * can safely directly set the new priority (a no-op if the thread's
     * priority is equal to the ceiling).
     *
     * Note that this has to happened before the VxWorks semaphore is taken to
     * avoid priority inversion but there is a window during which the thread's
     * priority is elevated although it has not yet taken the underlying
     * VxWorks semaphore. This cannot be solved satisfactorily without
     * getting native support for priority ceiling in vxWorks semaphores.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        taskPrioritySet (PTHREAD_TO_NATIVE_TASK (tid),
                PX_VX_PRIORITY_CONVERT(pMutex->mutexAttr.mutexAttrPrioceiling));

    /*
     * The mutex could not be locked, lets go back to the original
     * priority if the PTHREAD_PRIO_PROTECT protocol is used.
     */

    if (semTake(pMutex->mutexSemId, NO_WAIT) == ERROR)
        {
        if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
            taskPrioritySet (PTHREAD_TO_NATIVE_TASK (tid), priority);

        return (EBUSY);
        }

    /*
     * If this is a priority ceiling mutex, store the thread's original
     * priority in the mutex structure now that the mutex is safely acquired
     * by the thread. Doing this allows for acquiring nested priority ceiling
     * mutex while restoring the appropriate priority level when each of
     * those mutex are released in turn.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        pMutex->mutexSavPriority = priority;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_mutex_unlock - unlock a mutex (POSIX)
*
* This routine unlocks the mutex specified by <pMutex>. If the calling thread
* is not the current owner of the mutex, pthread_mutex_unlock() returns with
* the error code 'EPERM'.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i EPERM
* \i 'S_objLib_OBJ_ID_ERROR'
* \i 'S_semLib_NOT_ISR_CALLABLE'
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: semLib, semMLib, pthread_mutex_init(), pthread_mutex_lock(),
* pthread_mutex_trylock(), pthread_mutex_unlock(), pthread_mutexattr_init(),
* semGive()
*/

int pthread_mutex_unlock
    (
    pthread_mutex_t * pMutex    /* pthread mutex */
    )
    {
    pthreadCB * tid;    /* caller thread */
    int priority = 255;         /* Underlying VxWorks task's priority,
                                 * initialized to 255 for conformance. When
                                 * setting its priority it will use
                                 * pMutex->mutexSavPriority.
                                 */

    int status = _RETURN_PTHREAD_SUCCESS;

    if ((pMutex == NULL) || (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    MUTEX_INIT_COMPLETE(pMutex);

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    tid = _pthread_id_self();

    if (_pthreadSemOwnerGet (pMutex->mutexSemId) != PTHREAD_TO_NATIVE_TASK(tid))
        return (EPERM);

    errno = 0;

    /*
     * For priority ceiling mutex the thread's original priority kept in the
     * mutex structure must be reset while the mutex is still acquired but
     * the thread priority cannot be returned to its original value until
     * after the mutex has been released to prevent unduly delaying higher
     * priority threads that are blocked on this mutex, so we make a copy of
     * the original priority and reset the mutex structure's field now.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        {
        priority = pMutex->mutexSavPriority;
        pMutex->mutexSavPriority = -1;
        }

    if (semGive(pMutex->mutexSemId) == ERROR)
        {
        if (errno == S_semLib_INVALID_OPERATION)
            status = EPERM;                               /* not owner */
        else
            status = errno;                               /* some other error */
        }

    /*
     * In case the mutex protocol is PTHREAD_PRIO_PROTECT lets reset the
     * thread's priority to what it was originally.
     *
     * Note that there is a window during which the thread's priority is still
     * elevated although it does not hold the underlying VxWorks semaphore
     * anymore. This cannot be solved satisfactorily without getting native
     * support for priority ceiling in vxWorks semaphores.
     */

    if (pMutex->mutexAttr.mutexAttrProtocol == PTHREAD_PRIO_PROTECT)
        taskPrioritySet (PTHREAD_TO_NATIVE_TASK (tid), priority);

    return status;
    }

/*
 * Section 11.4 - Condition Variables
 */

/*******************************************************************************
*
* pthread_condattr_init - initialize a condition attribute object (POSIX)
*
* This routine initializes the condition attribute object <pAttr> and fills
* it with default values for the attributes.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \ie
*
* ERRNO
*
* SEE ALSO: pthread_cond_init(), pthread_condattr_destroy()
*/

int pthread_condattr_init
    (
    pthread_condattr_t * pAttr          /* condition variable attributes */
    )
    {
    if (pAttr == NULL)
        return (EINVAL);

    pAttr->condAttrStatus = PTHREAD_INITIALIZED_OBJ;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_condattr_destroy - destroy a condition attributes object (POSIX)
*
* This routine destroys the condition attribute object <pAttr>. It must
* not be reused until it is reinitialized.
*
* RETURNS: Always returns zero.
*
* ERRNO: None.
*
* SEE ALSO: pthread_cond_init(), pthread_condattr_init()
*/

int pthread_condattr_destroy
    (
    pthread_condattr_t * pAttr          /* condition variable attributes */
    )
    {
    pAttr->condAttrStatus = PTHREAD_DESTROYED_OBJ;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_cond_init - initialize condition variable (POSIX)
*
* DESCRIPTION
*
* This function initializes a condition variable. A condition variable
* is a synchronization device that allows threads to block until some
* predicate on shared data is satisfied. The basic operations on conditions
* are to signal the condition (when the predicate becomes true), and wait
* for the condition, blocking the thread until another thread signals the
* condition.
*
* A condition variable must always be associated with a mutex to avoid a
* race condition between the wait and signal operations.
*
* If <pAttr> is NULL then the default attributes are used as specified by
* POSIX; if <pAttr> is non-NULL then it is assumed to point to a condition
* attributes object initialized by pthread_condattr_init(), and those are
* the attributes used to create the condition variable.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \ie
*
* ERRNO
*
* SEE ALSO: pthread_condattr_init(), pthread_condattr_destroy(),
* pthread_cond_broadcast(), pthread_cond_destroy(), pthread_cond_signal(),
* pthread_cond_timedwait(), pthread_cond_wait()
*/

int pthread_cond_init
    (
    pthread_cond_t *     pCond,         /* condition variable            */
    pthread_condattr_t * pAttr          /* condition variable attributes */
    )
    {
    if (pCond == NULL)
        return (EINVAL);

    /*
     * Although we do not currently need to use the condition variable
     * attributes to initialize the condition variable object, the conformance
     * requires that the attributes initialization is verified if a non-null
     * pAttr argument is passed to this routine (SPR #117221).
     */

    if (pAttr && (pAttr->condAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    /* in VxWorks, there is nothing useful in a pthread_condattr_t */

    pCond->condValid    = PTHREAD_VALID_OBJ;
    pCond->condSemId    = NULL;
    pCond->condInitted  = PTHREAD_UNUSED_YET_OBJ;
    pCond->condMutex    = NULL;
    pCond->condRefCount = 0;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_cond_destroy - destroy a condition variable (POSIX)
*
* This routine destroys the condition variable pointed to by <pCond>. No
* threads can be waiting on the condition variable when this function is
* called. If there are threads waiting on the condition variable, then
* pthread_cond_destroy() returns 'EBUSY'.
*
* RETURNS: On success zero; on failure a non-zero error code.
* \is
* \i EINVAL
* \i EBUSY
* \ie
*
* ERRNO
*
* SEE ALSO: pthread_condattr_init(), pthread_condattr_destroy(),
* pthread_cond_broadcast(), pthread_cond_init(), pthread_cond_signal(),
* pthread_cond_timedwait(), pthread_cond_wait()
*/

int pthread_cond_destroy
    (
    pthread_cond_t * pCond              /* condition variable */
    )
    {
    if ((pCond == NULL) || (pCond->condValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    if (pCond->condRefCount != 0)
        return (EBUSY); /* someone else blocked on *pCond */

    COND_INIT_COMPLETE(pCond);

    if (semDelete(pCond->condSemId) == ERROR)
        return (errno);

    pCond->condValid = PTHREAD_INVALID_OBJ;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_cond_signal - unblock a thread waiting on a condition (POSIX)
*
* This routine unblocks one thread waiting on the specified condition
* variable <pCond>. If no threads are waiting on the condition variable then
* this routine does nothing; if more than one thread is waiting, then one will
* be released, but it is not specified which one.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_condattr_init(), pthread_condattr_destroy(),
* pthread_cond_broadcast(), pthread_cond_destroy(), pthread_cond_init(),
* pthread_cond_timedwait(), pthread_cond_wait()
*/

int pthread_cond_signal
    (
    pthread_cond_t * pCond      /* condition variable */
    )
    {
    TASK_ID taskIdList[1];

    if ((pCond == NULL) || (pCond->condValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    COND_INIT_COMPLETE(pCond);

    if (semInfo(pCond->condSemId, taskIdList, 1) == 0)
        {
        /*
         * There are no threads waiting on the signal, so
         * do nothing and return all-is-well.
         */

        return (_RETURN_PTHREAD_SUCCESS);
        }

    /* Otherwise unblock one of the pended threads */

    if (semGive (pCond->condSemId) == ERROR)
        return (EINVAL);                                   /* internal error */

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_cond_broadcast - unblock all threads waiting on a condition (POSIX)
*
* This function unblocks all threads blocked on the condition variable
* <pCond>. Nothing happens if no threads are waiting on the specified condition
* variable.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_condattr_init(), pthread_condattr_destroy(),
* pthread_cond_destroy(), pthread_cond_init(), pthread_cond_signal(),
* pthread_cond_timedwait(), pthread_cond_wait()
*/

int pthread_cond_broadcast
    (
    pthread_cond_t * pCond      /* condition variable */
    )
    {
    if ((pCond == NULL) || (pCond->condValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    COND_INIT_COMPLETE(pCond);

    /*
     * Let's unblock all threads pending on the condition variable. The
     * scheduler will elect the one thread that will get to run and the others
     * will just stay in ready mode and wait for their turn.
     */

    if (semFlush (pCond->condSemId) == ERROR)
        return (EINVAL);                                /* internal error */

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_cond_wait - wait for a condition variable (POSIX)
*
* This function atomically releases the mutex <pMutex> and waits for the
* condition variable <pCond> to be signalled by another thread. The mutex
* must be locked by the calling thread when pthread_cond_wait() is called; if
* it is not then this function returns an error ('EINVAL').
*
* Before returning to the calling thread, pthread_cond_wait() re-acquires the
* mutex.
*
* If the calling thread receives a signal while pending on the condition
* variable pthread_cond_wait() will return zero, indicating a spurious wakeup.
* Because of this, it is important for the caller to re-evaluate the predicate
* upon return.
*
* If the calling thread gets cancelled while pending on the condition
* variable pthread_cond_wait() will also re-acquire the mutex prior to
* executing the cancellation cleanup handlers (if any). The mutex will
* however be released prior to the thread exiting so that this mutex can be
* used by other threads.
*
* RETURNS: zero on success or spurious wakeup; on failure the 'EINVAL' error
* code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_condattr_init(), pthread_condattr_destroy(),
* pthread_cond_broadcast(), pthread_cond_destroy(), pthread_cond_init(),
* pthread_cond_signal(), pthread_cond_timedwait()
*/

int pthread_cond_wait
    (
    pthread_cond_t *  pCond,            /* condition variable   */
    pthread_mutex_t * pMutex            /* pthread mutex        */
    )
    {
    TASK_ID mutexOwner;
    int     savtype;
    int     callerSavedPrio;    /* original priority of caller saved by
                                   pthread_mutex_lock() */
    pthreadCB * selfPthreadId = SELF_PTHREAD_ID;

    /* validate the parameters    SPR# 114274 */

    if ((pCond == NULL) || (pMutex == NULL) ||
        (pCond->condValid != PTHREAD_VALID_OBJ) ||
        (pMutex->mutexValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    COND_INIT_COMPLETE(pCond);

    /*
     * make sure that if anyone else is blocked on this condition variable
     * that the same mutex was used to guard it. Also verify that mutex is
     * locked and that we are the owner
     */

    if (((pCond->condMutex != NULL) && (pCond->condMutex != pMutex)) ||
        (!(mutexOwner = _pthreadSemOwnerGet (pMutex->mutexSemId))) ||
        (mutexOwner != taskIdSelf()))
        {
        return (EINVAL);
        }
    else
        pCond->condMutex = pMutex;

    pCond->condRefCount++;
    pCond->condMutex->mutexCondRefCount++;

    /*
     * The guard mutex might be using the PTHREAD_PRIO_PROTECT (i.e. ceiling)
     * protocol. Since this routine is going to release the guard mutex which
     * will then be acquired by another thread, we must first keep a local
     * copy of the original thread's priority as recorded by
     * pthread_mutex_lock() so that we can restore it back in the mutex
     * structure when the guard mutex is acquired again by this routine (this
     * is addressing the defect 130350).
     *
     * Note: we don't need to know here whether the guard mutex is actually
     *       using the PTHREAD_PRIO_PROTECT protocol. We choose to blindly save
     *       and restore the content of 'mutexSavPriority'.  The
     *       pthread_mutex_unlock() routine will know what to do with it.
     */

    callerSavedPrio = pMutex->mutexSavPriority;

#ifdef _WRS_CONFIG_SMP

    if (selfPthreadId)
        selfPthreadId->cvcond = pCond;

    /*
     * From now on allow for immediate cancellation and check whether there
     * may be pending cancellation requests.
     */

    if (pthread_testandset_canceltype (PTHREAD_CANCEL_ASYNCHRONOUS, 
				       &savtype) != 0)
	{
	return (EINVAL);
	}

    /*
     * Now, let's atomically release the guard mutex and wait for the
     * condition variable to be signaled by another thread.  This is done
     * using semExchange which gives and then takes 2 different semaphores
     * atomically.
     */

    if (semExchange (pMutex->mutexSemId, pCond->condSemId,
                     WAIT_FOREVER) == ERROR)
        {
        /*
         * If errno is EINTR the caller was interrupted by a signal.  In this
         * case errno the caller returns '0' indicating a spurious wakeup.
         */

        if (errno != EINTR)
            {
            pthread_testandset_canceltype(savtype, NULL);

            return (EINVAL);                               /* internal error */
            }
        }

#else /* !_WRS_CONFIG_SMP */
    TASK_LOCK();

    if (semGive(pMutex->mutexSemId) == ERROR)
        {
        TASK_UNLOCK();

        return (EINVAL);
        }

    if (selfPthreadId)
        selfPthreadId->cvcond = pCond;

    /*
     * From now on allow for immediate cancellation and check whether there
     * may be pending cancellation requests.
     */

    if (pthread_testandset_canceltype (PTHREAD_CANCEL_ASYNCHRONOUS, 
				       &savtype) != 0)
	{
	return (EINVAL);
	}

    if (semTake(pCond->condSemId, WAIT_FOREVER) == ERROR)
        {
        /*
         * If errno is EINTR the caller was interrupted by a signal.  In this
         * case the caller returns '0' indicating a spurious wakeup.
         */

        if (errno != EINTR)
            {
            TASK_UNLOCK();

            pthread_testandset_canceltype (savtype, NULL);

            return (EINVAL);                              /* internal error */
            }
        }

    TASK_UNLOCK();
#endif /* _WRS_CONFIG_SMP */

    if (pthread_testandset_canceltype (savtype, NULL) != 0)
	{
	return (EINVAL);
	}

    if (selfPthreadId)
        selfPthreadId->cvcond = NULL;

    if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
        return (EINVAL);

    /*
     * Restore the original content of the guard mutex's 'mutexSavPriority'
     * field, i.e. the original priority of the thread in case a priority
     * ceiling protocol is used by the guard mutex (this is part of the fix
     * for the defect 130350).
     */

    pMutex->mutexSavPriority = callerSavedPrio;

    /*
     * Let's decrement the reference counts. If no one is waiting on the
     * condition variable anymore, then let's unlink it from the guard mutex.
     */

    pCond->condMutex->mutexCondRefCount--;

    if (--pCond->condRefCount == 0)
        pCond->condMutex = NULL;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_cond_timedwait - wait for a condition variable with a timeout (POSIX)
*
* This function atomically releases the mutex <pMutex> and waits for another
* thread to signal the condition variable <pCond>. As with pthread_cond_wait(),
* the mutex must be locked by the calling thread when pthread_cond_timedwait()
* is called.
*
* If the condition variable is signalled before the system time reaches the
* time specified by <pAbsTime>, then the mutex is re-acquired and the calling
* thread unblocked.
*
* If the system time reaches or exceeds the time specified by <pAbsTime> before
* the condition is signalled, then the mutex is re-acquired, the thread
* unblocked and ETIMEDOUT returned.
*
* If the calling thread receives a signal while pending on the condition
* variable pthread_cond_timedwait() will return zero, indicating a spurious
* wakeup. Because of this, it is important for the caller to re-evaluate the
* predicate upon return.
*
* If the calling thread gets cancelled while pending on the condition
* variable pthread_cond_timedwait() will also re-acquire the mutex prior to
* executing the cancellation cleanup handlers (if any). The mutex will
* however be released prior to the thread exiting so that this mutex can be
* used by other threads.
*
* NOTE
* The timeout is specified as an absolute value of the system clock in a
* <timespec> structure (see clock_gettime() for more information). This is
* different from most VxWorks timeouts which are specified in ticks relative
* to the current time.
*
* RETURNS: zero on success or spurious wakeup; on failure a non-zero error code:
* \is
* \i EINVAL
* \i ETIMEDOUT
* \ie
*
* ERRNO
*
* SEE ALSO: pthread_condattr_init(), pthread_condattr_destroy(),
* pthread_cond_broadcast(), pthread_cond_destroy(), pthread_cond_init(),
* pthread_cond_signal(), pthread_cond_wait()
*/

int pthread_cond_timedwait
    (
    pthread_cond_t *        pCond,      /* condition variable   */
    pthread_mutex_t *       pMutex,     /* pthread mutex        */
    const struct timespec * pAbstime    /* timeout time         */
    )
    {
    _Vx_ticks64_t       nTicks;
    struct timespec     tmp;
    struct timespec     now;
    TASK_ID             mutexOwner;
    int                 savtype;
    int                 callerSavedPrio; /* original priority of caller saved
                                            by pthread_mutex_lock() */
    pthreadCB * selfPthreadId = SELF_PTHREAD_ID;

    if ((pCond == NULL) || (pCond->condValid != PTHREAD_VALID_OBJ))
        return (EINVAL);

    COND_INIT_COMPLETE(pCond);

    if ((pMutex == NULL) || (pAbstime == NULL) || !TV_VALID(*pAbstime))
        {
        return (EINVAL);
        }

    /*
     * make sure that if anyone else is blocked on this condition variable
     * that the same mutex was used to guard it. Also verify that mutex is
     * locked and that we are the owner
     */

    if (((pCond->condMutex != NULL) && (pCond->condMutex != pMutex)) ||
        (!(mutexOwner = _pthreadSemOwnerGet (pMutex->mutexSemId))) ||
        (mutexOwner != taskIdSelf()))
        {
        return (EINVAL);
        }
    else
        pCond->condMutex = pMutex;

    pCond->condMutex->mutexCondRefCount++;
    pCond->condRefCount++;

    /*
     * The guard mutex might be using the PTHREAD_PRIO_PROTECT (i.e. ceiling)
     * protocol. Since this routine is going to release the guard mutex which
     * will then be acquired by another thread, we must first keep a local
     * copy of the original thread's priority as recorded by
     * pthread_mutex_lock() so that we can restore it back in the mutex
     * structure when the guard mutex is acquired again by this routine (this
     * is addressing the defect 130350).
     *
     * Note: we don't need to know here whether the guard mutex is actually
     *       using the PTHREAD_PRIO_PROTECT protocol. We choose to blindly save
     *       and restore the content of 'mutexSavPriority'.  The
     *       pthread_mutex_unlock() routine will know what to do with it.
     */

    callerSavedPrio = pMutex->mutexSavPriority;

    /*
     * From now on allow for immediate cancellation and check whether there
     * may be pending cancellation requests.
     */

    if (pthread_testandset_canceltype (PTHREAD_CANCEL_ASYNCHRONOUS, 
				       &savtype) != 0)
	{
	return (EINVAL);
	}

    /*
     * convert to number of ticks (relative time) or return if time has
     * already passed
     */

    if (clock_gettime(CLOCK_REALTIME, &now) != 0)
        {
        pthread_testandset_canceltype (savtype, NULL);
        return (errno);
        }

    /* SPR# 118167 - wrong check for "too late" condition */

    if (TV_GT(now, *pAbstime))    /* overflow, e.g. too late */
        {
        /* drop and reacquire mutex as per POSIX */

        if (semGive(pMutex->mutexSemId) == ERROR)
            {
            pthread_testandset_canceltype (savtype, NULL);
            return (EINVAL);        /* internal error */
            }

        if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
            {
            pthread_testandset_canceltype (savtype, NULL);
            return (EINVAL);        /* internal error */
            }

        if (pthread_testandset_canceltype (savtype, NULL) != 0)
	    {
	    return (EINVAL);
	    }

        /*
         * Restore the original content of the guard mutex's 'mutexSavPriority'
         * field, i.e. the original priority of the thread in case a priority
         * ceiling protocol is used by the guard mutex (this is part of the fix
         * for the defect 130350).
         */

        pMutex->mutexSavPriority = callerSavedPrio;

        /*
         * Let's decrement the reference counts. If no one is waiting on the
         * condition variable anymore, then let's unlink it from the guard
         * mutex (defect 130210).
         */

        pCond->condMutex->mutexCondRefCount--;

        if (--pCond->condRefCount == 0)
            pCond->condMutex = NULL;

        return (ETIMEDOUT);
        }

    TV_SET(tmp, *pAbstime);
    TV_SUB(tmp, now);

    /*
     * The TV_CONVERT_TO_TICK() macro adds an additional tick to make up for
     * the partial tick lost while executing the call.  Because this routine
     * uses an absolute time it is incorrect to add this tick, so
     * TV_CONVER_TO_TICK_NO_PARTIAL() is used instead.
     */

    TV_CONVERT_TO_TICK_NO_PARTIAL(nTicks, tmp);

#ifdef _WRS_CONFIG_SMP

    if (selfPthreadId)
        selfPthreadId->cvcond = pCond;

    if (semExchange (pMutex->mutexSemId, pCond->condSemId,
                     (_Vx_ticks_t) nTicks) == ERROR)
        {
        /*
         * If errno is EINTR the caller was interrupted by a signal.  In this
         * case the caller returns '0' indicating a spurious wakeup.
         */

        if (errno != EINTR)
            {
            if (selfPthreadId)
                selfPthreadId->cvcond = NULL;

            if (pthread_testandset_canceltype(savtype, NULL) != 0)
		{
		return (EINVAL);
		}

            if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
                {
                return (EINVAL);
                }

            /*
             * Restore the original content of the guard mutex's
             * 'mutexSavPriority' field, i.e. the original priority of
             * the thread in case a priority ceiling protocol is used by
             * the guard mutex (this is part of the fix
             * for the defect 130350).
             */

            pMutex->mutexSavPriority = callerSavedPrio;

            /*
             * Let's decrement the reference counts. If no one is waiting on the
             * condition variable anymore, then let's unlink it from the guard
             * mutex.
             */

            pCond->condMutex->mutexCondRefCount--;

            if (--pCond->condRefCount == 0)
                pCond->condMutex = NULL;

            return (ETIMEDOUT);
            }
        }

#else /* !_WRS_CONFIG_SMP */
    TASK_LOCK();

    if (semGive(pMutex->mutexSemId) == ERROR)
        {
        TASK_UNLOCK();
        pthread_testandset_canceltype (savtype, NULL);
        return (EINVAL);
        }

    if (selfPthreadId)
        selfPthreadId->cvcond = pCond;

    if (semTake(pCond->condSemId, (_Vx_ticks_t) nTicks) == ERROR)
        {
        /*
         * If errno is EINTR the caller was interrupted by a signal.  In this
         * case caller returns '0' indicating spurious wakeup.
         */

        if (errno != EINTR)
            {
            if (selfPthreadId)
                selfPthreadId->cvcond = NULL;

            TASK_UNLOCK();

            if (pthread_testandset_canceltype (savtype, NULL) != 0)
		{
		return (EINVAL);
		}

            if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
                {
                return (EINVAL);
                }

            /*
             * Restore the original content of the guard mutex's
             * 'mutexSavPriority' field, i.e. the original priority of
             * the thread in case a priority ceiling protocol is used by
             * the guard mutex (this is part of the fix for the defect 130350).
             */

            pMutex->mutexSavPriority = callerSavedPrio;

            /*
             * Let's decrement the reference counts. If no one is waiting on the
             * condition variable anymore, then let's unlink it from the guard
             * mutex.
             */

            pCond->condMutex->mutexCondRefCount--;

            if (--pCond->condRefCount == 0)
                pCond->condMutex = NULL;

            return (ETIMEDOUT);
            }
        }

    TASK_UNLOCK();
#endif /* _WRS_CONFIG_SMP */

    if (pthread_testandset_canceltype (savtype, NULL) != 0)
	{
	return (EINVAL);
	}

    if (semTake(pMutex->mutexSemId, WAIT_FOREVER) == ERROR)
        {
        return (EINVAL);
        }

    if (selfPthreadId)
        selfPthreadId->cvcond = NULL;

    /*
     * Restore the original content of the guard mutex's 'mutexSavPriority'
     * field, i.e. the original priority of the thread in case a priority
     * ceiling protocol is used by the guard mutex (this is part of the fix
     * for the defect 130350).
     */

    pMutex->mutexSavPriority = callerSavedPrio;

    /*
     * Let's decrement the reference counts. If no one is waiting on the
     * condition variable anymore, then let's unlink it from the guard mutex.
     */

    pCond->condMutex->mutexCondRefCount--;

    if (--pCond->condRefCount == 0)
        pCond->condMutex = NULL;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*
 * Section 13.5 - Thread Scheduling
 */

/*******************************************************************************
*
* pthread_attr_setscope - set contention scope for thread attributes (POSIX)
*
* For VxWorks PTHREAD_SCOPE_SYSTEM is the only supported contention scope.
* If the PTHREAD_SCOPE_PROCESS value is passed to this function this will
* result in 'ENOTSUP' being returned.
*
* RETURNS: On success zero; on failure the 'EINVAL' or 'ENOTSUP' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_getscope(), pthread_attr_init()
*/

int pthread_attr_setscope
    (
    pthread_attr_t *    pAttr,          /* thread attributes object     */
    int                 contentionScope /* new contention scope         */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (contentionScope != PTHREAD_SCOPE_PROCESS &&
        contentionScope != PTHREAD_SCOPE_SYSTEM))
        {
        return (EINVAL);
        }

    if (contentionScope == PTHREAD_SCOPE_PROCESS)
        return (ENOTSUP);

#if 0
    /*
     * Restriction on VxWorks: no multiprocessor, so all threads
     * compete for resources, PTHREAD_SCOPE_SYSTEM always in effect
     */

    pAttr->threadAttrContentionscope = contentionScope;
#endif

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getscope - get contention scope from thread attributes (POSIX)
*
* Reads the current contention scope setting from a thread attributes object.
* For VxWorks this is always PTHREAD_SCOPE_SYSTEM. If the thread attributes
* object is uninitialized then 'EINVAL' will be returned. The contention
* scope is returned in the location pointed to by <pContentionScope>.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_init(), pthread_attr_setscope()
*/

int pthread_attr_getscope
    (
    const pthread_attr_t * pAttr,               /* thread attributes object */
    int *                  pContentionScope     /* contention scope (out) */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

#if 0

    *pContentionScope = pAttr->threadAttrContentionscope;

#else

    /*
     * Restriction on VxWorks: so all threads
     * compete for resources, PTHREAD_SCOPE_SYSTEM always in effect
     */

    *pContentionScope = PTHREAD_SCOPE_SYSTEM;

#endif

    return (_RETURN_PTHREAD_SUCCESS);
}

/*******************************************************************************
*
* pthread_attr_setinheritsched - set inheritsched attribute in thread attribute object (POSIX)
*
* This routine sets the scheduling inheritance to be used when creating a
* thread with the thread attributes object specified by <pAttr>.
*
* Possible values are:
* \is
* \i PTHREAD_INHERIT_SCHED
* Inherit scheduling parameters from parent thread.
* \i PTHREAD_EXPLICIT_SCHED
* Use explicitly provided scheduling parameters (i.e. those specified in the
* thread attributes object).
* \ie
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_getinheritsched(), pthread_attr_init(),
* pthread_attr_setschedparam(), pthread_attr_setschedpolicy()
*/

int pthread_attr_setinheritsched
    (
    pthread_attr_t *    pAttr,          /* thread attributes object     */
    int                 inheritsched    /* inheritance mode             */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (inheritsched != PTHREAD_INHERIT_SCHED &&
        inheritsched != PTHREAD_EXPLICIT_SCHED))
        {
        return (EINVAL);
        }

    pAttr->threadAttrInheritsched = inheritsched;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getinheritsched - get current value if inheritsched attribute in thread attributes object (POSIX)
*
* This routine gets the scheduling inheritance value from the thread
* attributes object <pAttr>.
*
* Possible values are:
* \is
* \i PTHREAD_INHERIT_SCHED
* Inherit scheduling parameters from parent thread.
* \i PTHREAD_EXPLICIT_SCHED
* Use explicitly provided scheduling parameters (i.e. those specified in the
* thread attributes object).
* \ie
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_init(), pthread_attr_getschedparam(),
* pthread_attr_getschedpolicy() pthread_attr_setinheritsched()
*/

int pthread_attr_getinheritsched
    (
    const pthread_attr_t * pAttr,               /* thread attributes object */
    int *                  pInheritsched        /* inheritance mode (out) */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    *pInheritsched = pAttr->threadAttrInheritsched;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setschedpolicy - set schedpolicy attribute in thread attributes object (POSIX)
*
* Select the thread scheduling policy. The default scheduling policy is
* to inherit the current system setting. Unlike the POSIX model,
* scheduling policies under VxWorks are global. If a scheduling policy is
* being set explicitly, the PTHREAD_EXPLICIT_SCHED mode must be set (see
* pthread_attr_setinheritsched() for information), and the selected scheduling
* policy must match the global scheduling policy in place at
* the time; failure to do so will result in pthread_create() failing with
* the error 'EPERM'.
*
* POSIX defines the following policies:
* \is
* \i SCHED_RR
* Realtime, round-robin scheduling.
* \i SCHED_FIFO
* Realtime, first-in first-out scheduling.
* \i SCHED_OTHER
* Other, active VxWorks native scheduling policy.
* \ie
*
* Although the SCHED_RR and SCHED_FIFO policies can be set when the precaution
* described above is respected, using the SCHED_OTHER policy instead is always
* ensured to be successful.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_getschedpolicy(), pthread_attr_init(),
* pthread_attr_setinheritsched(), pthread_getschedparam(),
* pthread_setschedparam(), sched_setscheduler(), sched_getscheduler()
*/

int pthread_attr_setschedpolicy
    (
    pthread_attr_t *    pAttr,          /* thread attributes    */
    int                 policy          /* new policy           */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (policy != SCHED_OTHER &&
        policy != SCHED_FIFO &&
        policy != SCHED_RR))
        {
        return (EINVAL);
        }

    pAttr->threadAttrSchedpolicy = policy;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getschedpolicy - get schedpolicy attribute from thread attributes object (POSIX)
*
* This routine returns, via the pointer <pPolicy>, the current scheduling
* policy in the thread attributes object specified by <pAttr>. Possible values
* for VxWorks systems are SCHED_RR, SCHED_FIFO and SCHED_OTHER.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_init(), pthread_attr_setschedpolicy(),
* pthread_getschedparam(), pthread_setschedparam(), sched_setscheduler(),
* sched_getscheduler()
*/

int pthread_attr_getschedpolicy
    (
    const pthread_attr_t *      pAttr,          /* thread attributes    */
    int *                       pPolicy         /* current policy (out) */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    *pPolicy = pAttr->threadAttrSchedpolicy;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setschedparam - set schedparam attribute in thread attributes object (POSIX)
*
* Set the scheduling parameters in the thread attributes object <pAttr>.
* The scheduling parameters are essentially the thread's priority. Note that
* the PTHREAD_EXPLICIT_SCHED mode must be set
* (see pthread_attr_setinheritsched() for information) for the priority to take
* effect.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_getschedparam(), pthread_attr_init(),
* pthread_getschedparam(), pthread_setschedparam(),
* pthread_attr_setinheritsched(), sched_getparam(), sched_setparam()
*/

int pthread_attr_setschedparam
    (
    pthread_attr_t *            pAttr,  /* thread attributes    */
    const struct sched_param *  pParam  /* new parameters       */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (pParam == NULL) || !VALID_PRIORITY (pParam->sched_priority))
        return (EINVAL);

    pAttr->threadAttrSchedparam.sched_priority = pParam->sched_priority;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getschedparam - get value of schedparam attribute from thread attributes object (POSIX)
*
* Return, via the pointer <pParam>, the current scheduling parameters from the
* thread attributes object <pAttr>.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_init(), pthread_attr_setschedparam(),
* pthread_getschedparam(), pthread_setschedparam(), sched_getparam(),
* sched_setparam()
*/

int pthread_attr_getschedparam
    (
    const pthread_attr_t *pAttr,        /* thread attributes            */
    struct sched_param *pParam          /* current parameters (out)     */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (pParam == NULL))
        return (EINVAL);

    pParam->sched_priority = pAttr->threadAttrSchedparam.sched_priority;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_getschedparam - get value of schedparam attribute from a thread (POSIX)
*
* This routine reads the current scheduling parameters and policy of the thread
* specified by <thread>. The information is returned via <pPolicy> and
* <pParam>.
*
* Note that this routine actually always maps the current VxWorks scheduling
* policy on one of the two following POSIX scheduling policies: SCHED_FIFO or
* SCHED_RR. The SCHED_OTHER policy can therefore never be returned even if it
* has been set via pthread_setschedparam().
*
* RETURNS: On success zero; on failure the 'ESRCH' error code.
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_getschedparam() pthread_attr_getschedpolicy()
* pthread_attr_setschedparam() pthread_attr_setschedpolicy()
* pthread_setschedparam(), sched_getparam(), sched_setparam()
*/

int pthread_getschedparam
    (
    pthread_t            thread,        /* thread                       */
    int *                pPolicy,       /* current policy (out)         */
    struct sched_param * pParam         /* current parameters (out)     */
    )
    {
    pthreadCB * pThread         = (pthreadCB *)thread;
    BOOL        roundRobinOn;
    WIND_TCB *  pTcb;

    /*
     * Let's simply validate the targeted pthread. Locking it would be
     * overkill since we do not need to modify its control block.
     * There is a possibility that the targeted pthread is canceled or
     * deleted during or after the validation. However the sched_getparam()
     * API verifies the task ID for us.
     */

    if (VALID_PTHREAD (pThread, pTcb) == FALSE)
        return (ESRCH);

    roundRobinOn = _schedPxKernelIsTimeSlicing (NULL);

    if (roundRobinOn == TRUE)
        *pPolicy = SCHED_RR;
    else
        *pPolicy = SCHED_FIFO;

    return (sched_getparam ((pid_t)pTcb, pParam) != 0 ? errno : 0);
    }

/*******************************************************************************
*
* pthread_setschedparam - dynamically set schedparam attribute for a thread (POSIX)
*
* This routine will set the scheduling parameters (<pParam>) and policy
* (<policy>) for the thread specified by <thread>.
*
* In VxWorks the scheduling policy is global and not set on a per-thread basis;
* if the selected policy is one of SCHED_FIFO or SCHED_RR and this does not
* match the current VxWorks scheduling policy then this function will return
* an error ('EPERM'). If the <policy> parameter is set to SCHED_OTHER, which
* always matches the active scheduling policy, only the thread's priority
* will be changed.
*
* RETURNS: On success zero; on failure one of the following non-zero error
* codes: 'EPERM', 'ESRCH' (invalid task ID), 'EINVAL' (scheduling priority is
* outside valid range)
*
* ERRNO: N/A
*
* SEE ALSO: pthread_attr_getschedparam(), pthread_attr_getschedpolicy(),
* pthread_attr_setschedparam(), pthread_attr_setschedpolicy(),
* pthread_getschedparam(), sched_getparam(), sched_setparam()
*/

int pthread_setschedparam
    (
    pthread_t   thread,                 /* thread               */
    int         policy,                 /* new policy           */
    const struct sched_param * pParam   /* new parameters       */
    )
    {
    pthreadCB * pThread         = (pthreadCB *)thread;
    WIND_TCB *  pTcb;

    /*
     * Let's simply validate the targeted pthread. Locking it would be
     * overkill since we do not need to modify its control block.
     * There is a possibility that the targeted pthread is canceled or
     * deleted during or after the validation. However the sched_setparam()
     * and sched_setscheduler() API verify the task ID for us.
     */

    if (VALID_PTHREAD (pThread, pTcb) == FALSE)
        return (ESRCH);

    if (policy == SCHED_OTHER)
        {
        if (sched_setparam ((pid_t)pTcb, pParam) == ERROR)
            {
            if ((errno == EINVAL) || (errno == ESRCH))
                return errno;
            else
                return (EPERM);
            }

        return (_RETURN_PTHREAD_SUCCESS);
        }

    /*
     * For the other policies, let's see whether this matches with the
     * current VxWorks scheduling policy.
     */

    if (sched_setscheduler((pid_t)pTcb, policy, pParam) == ERROR)
        {
        if ((errno == EINVAL) || (errno == ESRCH))
            return errno;
        else
            return (EPERM);
        }

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*
 * Section 16 - Thread Management
 */

/*******************************************************************************
*
* pthread_attr_init - initialize thread attributes object (POSIX)
*
* DESCRIPTION
*
* This routine initializes a thread attributes object. If <pAttr> is NULL
* then this function will return EINVAL.
*
* The attributes that are set by default are as follows:
* \is
* \i 'Stack Address'
* NULL - allow the system to allocate the stack.
* \i 'Stack Size'
* 0 - use the VxWorks taskLib default stack size.
* \i 'Detach State'
* PTHREAD_CREATE_JOINABLE
* \i 'Contention Scope'
* PTHREAD_SCOPE_SYSTEM
* \i 'Scheduling Inheritance'
* PTHREAD_INHERIT_SCHED
* \i 'Scheduling Policy'
* SCHED_OTHER (i.e. active VxWorks native scheduling policy).
* \i 'Scheduling Priority'
* Use pthreadLib default priority
* \ie
*
* Note that the scheduling policy and priority values are only used if the
* scheduling inheritance mode is changed to PTHREAD_EXPLICIT_SCHED - see
* pthread_attr_setinheritsched() for information.
*
* Additionally, VxWorks-specific attributes are being set as follows:
* \is
* \i 'Task Name'
* NULL - the task name is automatically generated.
* \i 'Task Options'
* VX_FP_TASK
* \ie
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_attr_destroy(),
* pthread_attr_getdetachstate(),
* pthread_attr_getinheritsched(),
* pthread_attr_getschedparam(),
* pthread_attr_getschedpolicy(),
* pthread_attr_getscope(),
* pthread_attr_getstackaddr(),
* pthread_attr_getstacksize(),
* pthread_attr_setdetachstate(),
* pthread_attr_setinheritsched(),
* pthread_attr_setschedparam(),
* pthread_attr_setschedpolicy(),
* pthread_attr_setscope(),
* pthread_attr_setstackaddr(),
* pthread_attr_setstacksize(),
* pthread_attr_setname() (VxWorks extension),
* pthread_attr_setopt() (VxWorks extension)
*
*/

int pthread_attr_init
    (
    pthread_attr_t * pAttr              /* thread attributes */
    )
    {
    if (pAttr == NULL)
        return (EINVAL);    /* not a POSIX error return */

    pAttr->threadAttrStackaddr                  = NULL;
    pAttr->threadAttrStacksize                  = ((size_t) 0);
    pAttr->threadAttrDetachstate                = PTHREAD_CREATE_JOINABLE;
    pAttr->threadAttrContentionscope            = PTHREAD_SCOPE_SYSTEM;
    pAttr->threadAttrInheritsched               = PTHREAD_INHERIT_SCHED;
    pAttr->threadAttrSchedpolicy                = SCHED_OTHER;
    pAttr->threadAttrSchedparam.sched_priority  = DEF_PRIORITY;
    pAttr->threadAttrStatus                     = PTHREAD_INITIALIZED_OBJ;
    pAttr->threadAttrName                       = NULL;
    pAttr->threadAttrOptions                    = VX_FP_TASK;
    pAttr->threadAttrGuardsize                  = ((size_t) 0);

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_destroy - destroy a thread attributes object (POSIX)
*
* Destroy the thread attributes object <pAttr>. It should not be re-used until
* it has been reinitialized.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_attr_init()
*/

int pthread_attr_destroy
    (
    pthread_attr_t * pAttr              /* thread attributes */
    )
    {
    if (pAttr == NULL)
        return (EINVAL);    /* not a POSIX specified error return */

    pAttr->threadAttrStatus = PTHREAD_DESTROYED_OBJ;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setopt - set options in thread attribute object
*
* This non-POSIX routine sets options in the specified thread attributes
* object, <pAttr>. This allows for specifying a non-default set of options for
* the VxWorks task acting as a thread. Additional options may be applied to
* the task once the thread has been created via the taskOptionsSet() API.
*
* Note that the task options provided through this routine will supersede the
* default options otherwise applied at thread creation.
*
* See <taskLib.h> for definitions of valid task options.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_getopt(), taskOptionsSet()
*/

int pthread_attr_setopt
    (
    pthread_attr_t *    pAttr,          /* thread attributes */
    int                 options         /* VxWorks task options */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    pAttr->threadAttrOptions = options;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getopt - get options from thread attribute object
*
* This non-POSIX routine gets options from the specified thread attributes
* object, <pAttr>. To see the options actually applied to the VxWorks task
* under thread, use taskOptionsGet().
*
* This routine expects the <pOptions> parameter to be a valid storage space.
*
* See <taskLib.h> for definitions of task options.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed
* or if <pOptions> is NULL.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_setopt(), taskOptionsGet()
*/

int pthread_attr_getopt
    (
    pthread_attr_t *    pAttr,          /* thread attributes */
    int *               pOptions        /* VxWorks task options (out) */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (pOptions == NULL))
        return (EINVAL);

    *pOptions = pAttr->threadAttrOptions;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setname - set name in thread attribute object
*
* This routine sets the name in the specified thread attributes object, <pAttr>.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_getname()
*/

int pthread_attr_setname
    (
    pthread_attr_t *    pAttr,          /* thread attributes */
    char *              name            /* VxWorks task name */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    pAttr->threadAttrName = name;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getname - get name of thread attribute object
*
* This routine gets the name in the specified thread attributes object, <pAttr>.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed
* or if <name> is NULL.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_setname()
*/

int pthread_attr_getname
    (
    pthread_attr_t *    pAttr,          /* thread attributes */
    char **             name            /* VxWorks task name (out) */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) || (name == NULL))
        return (EINVAL);

    if (pAttr->threadAttrName == NULL)
        *name = "";
    else
        *name = pAttr->threadAttrName;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setstacksize - set stacksize attribute in thread attributes object (POSIX)
*
* This routine sets the thread stack size (in bytes) in the specified thread
* attributes object, <pAttr>.
*
* The stack address is set using the routine pthread_attr_setstackaddr(). Note
* that failure to set the stack size when a stack address is provided will
* result in an 'EINVAL' error status returned by pthread_create().
*
* RETURNS: 'EINVAL' if the stack size is lower than PTHREAD_STACK_MIN or if
* an invalid thread attribute is passed. Zero otherwise.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_getstacksize(),
* pthread_attr_setstackaddr(),
* pthread_attr_init(),
* pthread_create()
*/

int pthread_attr_setstacksize
    (
    pthread_attr_t *    pAttr,          /* thread attributes    */
    size_t              stacksize       /* new stack size       */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    if (stacksize < PTHREAD_STACK_MIN)
        return (EINVAL);

    pAttr->threadAttrStacksize = stacksize;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getstacksize - get stack value of stacksize attribute from thread attributes object (POSIX)
*
* This routine gets the current stack size from the thread attributes object
* <pAttr> and places it in the location pointed to by <pStacksize>.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed
* or if <pStackSize> is NULL.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_init(),
* pthread_attr_setstacksize(),
* pthread_attr_getstackaddr()
*/

int pthread_attr_getstacksize
    (
    const pthread_attr_t *      pAttr,          /* thread attributes    */
    size_t *                    pStacksize      /* current stack size (out) */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
          (pStacksize == NULL))
         return (EINVAL);

    *pStacksize = pAttr->threadAttrStacksize;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setstackaddr - set stackaddr attribute in thread attributes object (POSIX)
*
* This routine sets the stack address in the thread attributes object <pAttr>
* to be <pStackaddr>. On VxWorks this address must be the lowest address of
* the stack regardless of what the thread considers as the stack base or the
* stack end.
*
* No alignment constraints are imposed by the pthread library so the thread's
* stack can be obtained via a simple call to malloc() or memPartAlloc().
*
* The memory area used a stack is not automatically freed when the thread
* exits. This operation cannot be done via the exiting thread's cleanup stack
* since the cleanup handler routines use the same stack as the thread.
* Therefore freeing the stack space must be done by the code which allocated
* the thread's stack once the thread's task no longer exists in the system.
*
* The stack size is set using the routine pthread_attr_setstacksize(). Note that
* failure to set the stack size when a stack address is provided will result
* in an 'EINVAL' error status returned by pthread_create().
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_getstacksize(),
* pthread_attr_setstacksize(),
* pthread_attr_init()
*
* INTERNAL
* This routine only sets the address of the execution stack.
*/

int pthread_attr_setstackaddr
    (
    pthread_attr_t *    pAttr,          /* thread attributes            */
    void *              pStackaddr      /* new stack address            */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
        return (EINVAL);

    pAttr->threadAttrStackaddr = pStackaddr;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getstackaddr - get value of stackaddr attribute from thread attributes object (POSIX)
*
* This routine returns the stack address from the thread attributes object
* <pAttr> in the location pointed to by <ppStackaddr>.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed
* or if <ppStackaddr> is NULL.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_init(),
* pthread_attr_getstacksize(),
* pthread_attr_setstackaddr()
*/

int pthread_attr_getstackaddr
    (
    const pthread_attr_t *      pAttr,          /* thread attributes    */
    void **                     ppStackaddr     /* current stack address (out)*/
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (ppStackaddr == NULL))
         return (EINVAL);

    *ppStackaddr = pAttr->threadAttrStackaddr;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_setdetachstate - set detachstate attribute in thread attributes object (POSIX)
*
* This routine sets the detach state in the thread attributes object <pAttr>.
* The new detach state specified by <detachstate> must be one of
* PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE. Any other values will
* cause an error to be returned ('EINVAL').
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_attr_getdetachstate(),
* pthread_attr_init()
*/

int pthread_attr_setdetachstate
    (
    pthread_attr_t *    pAttr,          /* thread attributes    */
    int                 detachstate     /* new detach state     */
    )
    {
    if ((pAttr == NULL) || (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ))
         return (EINVAL);

    if (detachstate != PTHREAD_CREATE_DETACHED &&
        detachstate != PTHREAD_CREATE_JOINABLE)
        {
        return (EINVAL);
        }

    pAttr->threadAttrDetachstate = detachstate;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_attr_getdetachstate - get value of detachstate attribute from thread attributes object (POSIX)
*
* This routine returns the current detach state specified in the thread
* attributes object <pAttr>. The value is stored in the location pointed to
* by <pDetachstate>. Possible values for the detach state are:
* PTHREAD_CREATE_DETACHED and 'PTHREAD_CREATE_JOINABLE'.
*
* RETURNS: zero on success, 'EINVAL' if an invalid thread attribute is passed
* or if <pDetachState> is NULL.
*
* ERRNO: None.
*
* SEE ALSO:
* pthread_attr_init(),
* pthread_attr_setdetachstate()
*/

int pthread_attr_getdetachstate
    (
    const pthread_attr_t *      pAttr,          /* thread attributes    */
    int *                       pDetachstate    /* current detach state (out) */
    )
    {
    if ((pAttr == NULL) ||
        (pAttr->threadAttrStatus != PTHREAD_INITIALIZED_OBJ) ||
        (pDetachstate == NULL))
         return (EINVAL);

    *pDetachstate = pAttr->threadAttrDetachstate;

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_create - create a thread (POSIX)
*
* This routine creates a new thread and if successful writes its ID into the
* location pointed to by <pThread>. If <pAttr> is NULL then default attributes
* are used. The new thread executes <startRoutine> with <arg> as its argument.
*
* The new thread's cancelability state and cancelability type are respectively
* set to PTHREAD_CANCEL_ENABLE and PTHREAD_CANCEL_DEFERRED.
*
* RETURNS: On success zero; on failure one of the following non-zero error
* codes:
* \is
* \i EINVAL
* can be returned when the value specified by <pAttr> is invalid, when a
* user-supplied stack address is provided but the stack size is invalid, and
* when the <pThread> parameter is null.
* \i EAGAIN
* can be returned when not enough memory is available to either create the
* thread or create a resource required for the thread.
* \i EPERM
* the explicit scheduling policy does not match the VxWorks scheduling policy
* currently in effect.
* \ie
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_exit(),
* pthread_join(),
* pthread_detach()
*
* \INTERNAL:
* This routine must take care of not involving anything that cannot be safely
* used by a native VxWorks task, and must take care of not turning a native
* VxWorks task into a POSIX thread.
*/

int pthread_create
    (
    pthread_t * pThread,                /* Thread ID (out) */
    const pthread_attr_t * pAttr,       /* Thread attributes object */
    void * (*startRoutine)(void *),     /* Entry function */
    void * arg                          /* Entry function argument */
    )
    {
    return (_pthreadCreate (pThread,
                            pAttr,
                            (void * (*)(void *)) wrapperFunc,
                            startRoutine,
                            arg,
                            (int) posixPriorityNumbering));
    }

/*******************************************************************************
*
* pthread_detach - dynamically detach a thread (POSIX)
*
* This routine puts the thread <thread> into the detached state. This prevents
* other threads from synchronizing on the termination of the thread using
* pthread_join().
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i ESRCH
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: pthread_join()
*/

int pthread_detach
    (
    pthread_t thread            /* thread to detach */
    )
    {
    pthreadCB * pThread         = (pthreadCB *) thread;
    pthreadCB * selfPthreadId   = NULL;
    int ret;

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    selfPthreadId = _pthread_id_self();

    /*
     * We don't want the thread's flags to be changed or read while we
     * manipulate them, so let's lock the thread's control block.
     */

    if (PTHREAD_VALIDATE_AND_LOCK (pThread, selfPthreadId) != OK)
        return (ESRCH);

    if (pThread->flags & JOINABLE)
        {
#if DETACHED_PTHREADS_MAY_BE_JOINED
        pThread->flags |= DETACHED;
#else
        pThread->flags &= ~JOINABLE;
#endif
        ret = _RETURN_PTHREAD_SUCCESS;
        }
    else
        ret = EINVAL;

    PTHREAD_VALIDATED_UNLOCK (pThread, selfPthreadId);

    return (ret);
    }

/*******************************************************************************
*
* pthread_join - wait for a thread to terminate (POSIX)
*
* This routine will block the calling thread until the thread specified by
* <thread> terminates, or is canceled. The thread must be in the joinable
* state, i.e. it cannot have been detached by a call to pthread_detach(), or
* created in the detached state.
*
* If <ppStatus> is not NULL and pthread_join() returns successfully, when
* <thread> terminates its exit status will be stored in the specified location.
* The exit status will be either the value passed to pthread_exit(), or
* PTHREAD_CANCELED if the thread was canceled or the thread was deleted by a
* VxWorks task.
*
* Only one thread can wait for the termination of a given thread. If another
* thread is already waiting when this function is called an error will be
* returned ('EINVAL').
*
* If the calling thread passes its own ID in <thread>, the call will fail
* with the error 'EDEADLK'.
*
* If the joined thread no longer exists or is about to exit, the error 'ESRCH'
* is returned.
*
* NOTE
* All threads that remain <joinable> at the time they exit should ensure that
* pthread_join() is called on their behalf by another thread to reclaim the
* resources that they hold.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i ESRCH
* \i EDEADLK
* \ie
*
* ERRNO: N/A
*
* SEE ALSO: pthread_detach(), pthread_exit()
*/

int pthread_join
    (
    pthread_t   thread,         /* thread to wait for           */
    void **     ppStatus        /* exit status of thread (out)  */
    )
    {
    pthreadCB *         pThread         = (pthreadCB *) thread;
    pthreadCB *         selfPthreadId   = _pthread_id_self();
    int                 prevCnclType;   /* thread's previous cancel. type */
    TASK_ID             thTaskId;       /* ID of Vx task behind the thread */

    /*
     * Switch to deferred cancelability type as we do not want to be
     * canceled after having locked the joined pthread's control block.
     */

    _pthread_setcanceltype_inline (PTHREAD_CANCEL_DEFERRED, &prevCnclType,
                                   selfPthreadId);

    /*
     * We have to acquire the library's mutex prior to locking the caller in
     * case somebody else is attempting to validate and lock the
     * about-to-be-joined pthread.
     */

    if (semTake (pthreadLibSemId, WAIT_FOREVER) == ERROR)
        return EINVAL;  /* POSIX in shape but not in spirit... */

    /*
     * pthread_join() may not use the VALID_PTHREAD macro since one is allowed
     * to call this routine for a thread that has already exited (therefore the
     * thread ID stored in the TCB has been reset to NULL). However we still
     * have to release the thread's resources then and return its exit status
     * to the caller.
     */

    if (!(pThread && ((pThread->flags & VALID) == VALID)))
        {
        (void) semGive (pthreadLibSemId);
        pthread_testandset_canceltype (prevCnclType, NULL);
        return ESRCH;
        }

    /* Also a thread cannot wait for its own termination... */

    if (pThread == selfPthreadId)
        {
        (void) semGive (pthreadLibSemId);
        pthread_testandset_canceltype (prevCnclType, NULL);
        return (EDEADLK);
        }

    /*
     * Let's lock the joined thread so that it does not go away while we are
     * preparing to wait for it. We check for a possible error status coming
     * back from the lock operation as we might be attempting to join a
     * thread that is already too far gone into its exit sequence to be
     * joined.
     */

    selfPthreadId->lockedPthread = pThread;
    if (semExchange (pthreadLibSemId, pThread->lock, WAIT_FOREVER) == ERROR)
        {
        selfPthreadId->lockedPthread = 0;
        pthread_testandset_canceltype (prevCnclType, NULL);
        return ESRCH;
        }

    /*
     * If the thread has been detached or already has someone waiting for
     * its termination, then it may not be joined by the caller thread.
     */

    if (!(pThread->flags & JOINABLE) || pThread->flags & JOINER_WAITING)
        {
        PTHREAD_UNLOCK(pThread);
        selfPthreadId->lockedPthread = 0;

        pthread_testandset_canceltype (prevCnclType, NULL);

        return (EINVAL);
        }

    /*
     * If the thread has already exited and was joinable, provide its exit
     * status to the caller, release the control block on its behalf, and
     * return success.
     */

#if DETACHED_PTHREADS_MAY_BE_JOINED
    if ((pThread->flags & JOINABLE) && !(pThread->flags & DETACHED) &&
        (pThread->flags & TASK_EXITED))
#else
    if ((pThread->flags & JOINABLE) && (pThread->flags & TASK_EXITED))
#endif
        {
        if (ppStatus != NULL)
            *ppStatus = pThread->exitStatus;

        /*
         * No call to PTHREAD_UNLOCK is necessary since the lock mutex is
         * going to be destroyed.
         */

        pthreadCtrlBlockFree (pThread);

        selfPthreadId->lockedPthread = 0;
        if (pthread_testandset_canceltype (prevCnclType, NULL) != 0)
	    {
	    return (EINVAL);
	    }

        return (_RETURN_PTHREAD_SUCCESS);
        }

    /*
     * Let's indicate that someone is waiting for the thread's termination
     * and let's register which pthread the calling pthread has joined (in
     * case of cancellation of the caller).
     */

    pThread->flags |= JOINER_WAITING;
    selfPthreadId->joinedPthread = pThread;

    /* let's not depend on the thread data */

    thTaskId = PTHREAD_TO_NATIVE_TASK (pThread);

    PTHREAD_UNLOCK(pThread);
    selfPthreadId->lockedPthread = 0;

    /*
     * From now on allow for immediate cancellation and let's check whether
     * there are pending cancellation requests.
     * Note: if the caller is cancelled at this point this leads to a memory
     *       leak in case nobody else joins the targeted pthread.
     *       This is an unclear area of the POSIX.1 specs: on the one hand
     *       POSIX.1 states that pthread_join() should be called with the
     *       caller's cancelability type set to PTHREAD_CANCEL_DEFERRED, but
     *       on the other hand it lists pthread_join() as a compulsory
     *       cancellation point.
     */

    if (pthread_testandset_canceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0)
	{
	return (EINVAL);
	}

    /* Then let's wait for the thread's termination */

    if (semTake (pThread->exitJoinSemId, WAIT_FOREVER) != OK)
        return EINVAL;        /* POSIX in shape but not in spirit... */

    /* Re-instate the calling thread's previous cancelability type */

    _pthread_setcanceltype_inline (prevCnclType, NULL, selfPthreadId);

    if (ppStatus != NULL)
        *ppStatus = pThread->exitStatus;

    /*
     * Re-acquire the exited pthread's lock. Any potential concurrent joiner
     * should release the lock quickly since the JOINER_WAITING flag has been
     * set earlier. The pthread's lock is never unlocked after that point
     * since the mutex is going to be deleted.
     *
     * Note: the order of the acquisition of the pthread's lock and the
     * library's mutex is reversed here; this is not a problem since the use
     * of semExchange() guarantees that no deadlocks can occur.
     */

    PTHREAD_LOCK (pThread);

    /* Free the joined pthread's control block */

    pthreadCtrlBlockFree(pThread);

    selfPthreadId->joinedPthread = NULL;

    /*
     * Perform a wait on the VxWorks task and continue only when the
     * task is considered removed from the system.
     */

    if (taskWait (thTaskId, WAIT_FOREVER) == ERROR)
        {
        if (errno != S_objLib_OBJ_ID_ERROR)
            {
            /*
             * if errno == S_objLib_OBJ_ID_ERROR assume thread is already
             * gone and continue.
             */

            return (EINVAL);  /* POSIX in shape but not in spirit... */
            }
        }

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*******************************************************************************
*
* pthread_exit - terminate a thread (POSIX)
*
* This function terminates the calling thread. All cleanup handlers that have
* been set for the calling thread with pthread_cleanup_push() are executed
* in reverse order (the most recently added handler is executed first).
* Termination functions for thread-specific data are then called for all
* keys that have non-NULL values associated with them in the calling thread
* (see pthread_key_create() for more details). Finally, execution of the
* calling thread is stopped.
*
* The <status> argument is the return value of the thread and can be
* consulted from another thread using pthread_join() unless this thread was
* detached (i.e. a call to pthread_detach() had been made for it, or it was
* created in the detached state).
*
* All threads that remain <joinable> at the time they exit should ensure that
* pthread_join() is called on their behalf by another thread to reclaim the
* resources that they hold.
*
* If the calling task is not a POSIX thread, it will exit immediately.
*
* RETURNS: Does not return.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_cleanup_push(),
* pthread_detach(),
* pthread_join(),
* pthread_key_create()
*/

void pthread_exit
    (
    void * status               /* exit status */
    )
    {
    WIND_TCB * pTcb = (WIND_TCB *)taskIdSelf();
    pthreadCB * pThread = NULL;

    /* Get the caller's pthread control block address */

    if ((pThread = (pthreadCB *)pTcb->pPthread) == NULL)
        exit (EXIT_FAILURE);                            /* not a POSIX thread */

    /*
     * Change the thread's cancelability type and state to prevent
     * any possibility for cancellation. This ensures that no deadlock occurs
     * while the library's mutex or the pthread's control block's mutex are
     * acquired. this also ensures that the cleanup handlers can be safe from
     * triggering a cancellation in case they happen to be or call cancellation
     * points.
     * Note that there is really no reason to check whether the calling
     * pthread is still valid...
     */

    pThread->canceltype = PTHREAD_CANCEL_DEFERRED;
    pThread->cancelstate = PTHREAD_CANCEL_DISABLE;

    /*
     * Let's not go away just yet if somebody is attempting to do something
     * on the calling pthread. We have to acquire the library's mutex
     * prior to locking the caller in case somebody else is attempting to
     * validate and lock the about-to-exit pthread
     * Note also that there is no way to handle semTake() or semGive() failure
     * here...
     */

    (void)semTake (pthreadLibSemId, WAIT_FOREVER);
    (void)semExchange (pthreadLibSemId, pThread->lock, WAIT_FOREVER);

    /*
     * Execute the bulk of the exit sequence: cleanup operations, resource
     * freeing operations.
     */

    _pthread_onexit_cleanup (pThread, FALSE);

    /*
     * If the thread is not joinable let's do the resource clean up now then
     * let the underlying VxWorks task exit.
     */

#if DETACHED_PTHREADS_MAY_BE_JOINED
    if (!(pThread->flags & JOINABLE) ||
        ((pThread->flags & DETACHED) && !(pThread->flags & JOINER_WAITING)))
#else
    if (!(pThread->flags & JOINABLE))
#endif
        {
        /*
         * Note: the exiting thread is not unlocked since the mutex is going to
         * be deleted by pthreadCtrlBlockFree(). The resulting error coming from
         * pthreadValidateAndLock() will tell any thread pended for an operation
         * on this exiting thread that this operation cannot proceed.
         */

        pthreadCtrlBlockFree(pThread);

        /*
         * We must reset the pthread's ID to NULL in the TCB so that the pthread
         * delete hook does not attempt to release resources already released
         * when it kicks in.
         */

        pTcb->pPthread = NULL;

        taskExit ((_Vx_exit_code_t) status);

        /* Never comes here */
        }

    /*
     * The thread should have someone waiting for it, let pthread_join() do
     * the resource cleaning work.
     * Note that if for some reason the thread is joinable but no one joined
     * it then the pthread data structure will remain allocated, generating a
     * memory leak. This seems to be a known issue with POSIX threads ("shadow
     * threads"). Also, although the POSIX standard does not say explicitly
     * that pthread_join() or pthread_detach() must be called after a thread is
     * created, its rationale section does say that one of these two routines
     * "should eventually be called for every thread that is created so that
     * the storage associated with the thread may be reclaimed".
     */

    pThread->exitStatus = status;

    /*
     * Release joining thread if any. Note that the pthread may be unlocked
     * only _after_ the JOINER_WAITING flag has been checked otherwise it
     * would be possible for the joiner thread to free the exiting thread's
     * control block _before_ the exiting thread has had the time to verify
     * the flags. Also it is not possible to unlock the thread after the
     * joiner thread has been released since the exiting thread's control
     * block might have already been freed by the joiner thread when the
     * exiting thread comes to unlocking itself...
     */

    if (pThread->flags & JOINER_WAITING)
        {
        PTHREAD_UNLOCK (pThread);
        (void) semGive (pThread->exitJoinSemId);
        }
    else
        PTHREAD_UNLOCK (pThread);

    /*
     * We must reset the pthread's ID to NULL in the TCB so that the pthread
     * delete hook does not attempt to release resources already released
     * when it kicks in.
     */

    pTcb->pPthread = NULL;

    taskExit ((_Vx_exit_code_t) status);

    /* Never returns */
    }

/*******************************************************************************
*
* pthread_equal - compare thread IDs (POSIX)
*
* Tests the equality of the two threads <t1> and <t2>.
*
* RETURNS: Non-zero if <t1> and <t2> refer to the same thread, otherwise zero.
*/

int pthread_equal
    (
    pthread_t t1,                       /* thread one */
    pthread_t t2                        /* thread two */
    )
    {
    return (t1 == t2);
    }

/*******************************************************************************
*
* pthread_self - get the calling thread's ID (POSIX)
*
* This function returns the calling thread's ID.
*
* If the caller is a native VxWorks task it will be given a POSIX thread
* persona.
*
*
* RETURNS: Calling thread's ID.
*/

pthread_t pthread_self (void)
    {
    WIND_TCB * pTcb = (WIND_TCB *)taskIdSelf();

    /*
     * In case the caller is a native VxWorks task let's give it a thread
     * persona.
     */

    self_become_pthread();

    return ((pthread_t)pTcb->pPthread);
    }

/*******************************************************************************
*
* pthread_once - dynamic package initialization (POSIX)
*
* This routine provides a mechanism to ensure that one, and only one call
* to a user specified initialization function will occur. This allows all
* threads in a system to attempt initialization of some feature they need
* to use, without any need for the application to explicitly prevent multiple
* calls.
*
* When a thread makes a call to pthread_once(), the first thread to call it
* with the specified control variable, <pOnceControl>, will result in a call to
* <initFunc>, but subsequent calls will not. The <pOnceControl> parameter
* determines whether the associated initialization routine has been called.
* The <initFunc> function is complete when pthread_once() returns.
*
* The function pthread_once() is not a cancellation point; however, if the
* function <initFunc> is a cancellation point, and the thread is canceled
* while executing it, the effect on <pOnceControl> is the same as if
* pthread_once() had never been called.
*
* CAVEAT
* If the initialization function does not return then all threads calling
* pthread_once() with the same control variable will stay blocked as well.
* It is therefore imperative that the initialization function always returns.
*
* WARNING
* If <pOnceControl> has automatic storage duration or is not initialized to
* the value PTHREAD_ONCE_INIT, the behavior of pthread_once() is undefined.
*
* The constant PTHREAD_ONCE_INIT is defined in the 'pthread.h' header file.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: None
*
* INTERNAL
* The usage of semaphores in a two-level fashion is for fear that the user's
* initialization function does not return. If we were to use only a
* library-level semaphore for the mutual exclusion we would risk blocking all
* the threads calling pthread_once() for any control object in case one of the
* initialization routine does not return... Therefore the library-level
* semaphore is only used to protect the initialization of the control object,
* while the release of the created resource is protected via taskLock() (see
* below) since POSIX does not provide a cleaning API for this purpose. The
* uniqueness of the call to the initialization function is done by an other
* semaphore created when the control object is initialized and acting as a
* synchronization mean. This semaphore is deleted once the initialization
* function has run to completion. The seemingly redundant flag verifications
* are required to prevent race conditions.
*/

int pthread_once
    (
    pthread_once_t * pOnceControl,      /* once control location        */
    void (*initFunc)(void)              /* function to call             */
    )
    {
    pthreadCB *   pThread;

    if (initFunc == NULL)
        return EINVAL;  /* agree with IEEE 1003.1-2003 */

    if (pOnceControl == NULL)
        return (EINVAL);

startAgain:

    /*
     * Let's return immediately if the initialization function has already
     * been called for this control object.
     */

    if (pOnceControl->onceDone)
        return (_RETURN_PTHREAD_SUCCESS);

    if (semTake (pthreadLibOnceSemId, WAIT_FOREVER) == ERROR)
        return EINVAL;  /* POSIX in shape but not in spirit... */

    /*
     * If we come here, the call has not been done yet (or is not yet finished)
     * or the control object hasn't been initialized yet. Let's check all this.
     */

    if (!pOnceControl->onceInitialized)
        {
        /*
         * If we come here, no one else has initialized this control
         * object yet. Let's do it.
         */

        if ((pOnceControl->onceMutex = semBCreate (SEM_Q_PRIORITY,
                                                   SEM_EMPTY)) == SEM_ID_NULL)
            {
            (void)semGive (pthreadLibOnceSemId);
            return EINVAL;      /* POSIX in shape but not in spirit... */
            }

        /*
         * Let's flag the control object as initialized and let's
         * register who initialized it in order to prevent a possible
         * recursive call to pthread_once() with the same control object
         * by the initialization function (SPR #98589).
         */

        pOnceControl->onceInitialized = TRUE;
        pOnceControl->onceMyTid = taskIdSelf();

        /*
         * The field storing whether we will be cancelled in the init
         * routine is setup here.
         */

        pOnceControl->onceCanceled = FALSE;

        if (semGive (pthreadLibOnceSemId) == ERROR)
            return EINVAL;      /* POSIX in shape but not in spirit... */

        /*
         * We also store the address of the control variable in our thread's
         * private data so that we can do the necessary cleanup if we
         * are canceled in the initialization routine. Magic spell
         * summoning is needed however before we can access the thread
         * internals. This will make the caller a POSIX thread if needed.
         */

        pThread = _pthread_id_self();
        pThread->pOnceControl = pOnceControl;

        /*
         * We are now ready to call the initialization function. Once it has
         * returned the 'onceDone' state variable is immediately set to TRUE.
         * XXX PAD - this still leaves the possibility of a cancellation
         *           happening just before the state variable is set. This
         *           makes possible for the initialization function to
         *           execute more than once...
         */

        initFunc ();
        pOnceControl->onceDone = TRUE;

        /*
         * Make ready to unblock any concurrent threads that might be
         * waiting for the initialization routine to be executed and then
         * delete the onceMutex semaphore since it is not required
         * anymore. The flushing operation is done under the protection
         * of a semaphore in order to make sure that no late incoming
         * concurrent thread might "miss the train" and call semTake()
         * with a NULL semaphore ID.
         */

        if (semTake (pthreadLibOnceSemId, WAIT_FOREVER) == ERROR)
            return EINVAL;      /* POSIX in shape but not in spirit... */

        /*
         * Let's unblock all possible pending threads on this control
         * object.
         */

        (void)semFlush (pOnceControl->onceMutex);

        /*
         * Now delete the semaphore since it is no longer needed. Like
         * the creation of the object, its deletion is protected.
         */

        (void)semDelete (pOnceControl->onceMutex);

        pOnceControl->onceMutex = NULL;

        if (semGive (pthreadLibOnceSemId) == ERROR)
            return EINVAL;      /* POSIX in shape but not in spirit... */

        /*
         * We have to wait till everything is settled before resetting the
         * pthread's reference to the control variable to NULL. This way, in
         * case the pthread is cancelled _after_ the initialization routine
         * has been called, we give a chance to pthread_exit() to simply flush
         * the pending pthreads instead of completely resetting the control
         * variable (which would make possible for the the initialization
         * routine to be executed more than once).
         */

        pThread->pOnceControl = NULL;

        return (_RETURN_PTHREAD_SUCCESS);
        }

    /*
     * The control object has been initialized by someone else. Let's
     * make sure of that and check that the call to pthread_once() has not
     * been made by the initialization routine itself using the same
     * control object in a recursive fashion (SPR #98589). If it is the
     * case we just return now.
     */

    if (pOnceControl->onceMyTid == taskIdSelf ())
        {
        if (semGive (pthreadLibOnceSemId) != OK)
            return EINVAL;      /* POSIX in shape but not in spirit... */
        return (_RETURN_PTHREAD_SUCCESS);
        }

    /* Let's check whether the initialization function has returned already */

    if (pOnceControl->onceDone)
        {
        if (semGive (pthreadLibOnceSemId) != OK)
            return EINVAL;      /* POSIX in shape but not in spirit... */
        return (_RETURN_PTHREAD_SUCCESS);
        }

    /*
     * The initialization function appears to have not returned yet. Let's wait
     * for it by pending on the once control's mutex.
     * Note that, since the semaphore associated to the control object will
     * be deleted after the initialization function has run to completion, we
     * need to make sure this semaphore is still valid before it is taken
     * (semTake() will crash if it is passed a null semaphore ID...).
     * The verification and the count of pended concurrent threads is also
     * protected by a mutex so that no thread should pass the verification of
     * the mutex ID while the first thread is already preparing to flush and
     * delete the said mutex.
     */

    /*
     * Let's pend on the once control variable's mutex till it is flushed.
     *
     * CAVEAT: the caller may be blocked forever if the initialization
     * function does not return. If the init routine was cancelled however,
     * there is no problem (pthreadOnce returned to a pristine state).
     */

    /* coverity[var_deref_model : FALSE] */

    if (semExchange (pthreadLibOnceSemId, pOnceControl->onceMutex,
                     WAIT_FOREVER) == ERROR)
        {
        /*
         * Just to be safe, let's check whether semTake() returned with
         * an error because the semaphore has been destroyed by another
         * thread executing pthread_once(). If this is the case then the
         * onceMutex member of the control object will be null and we can
         * safely continue since the initialization function has been
         * executed then. Otherwise, it is possible there is a real problem
         * and we need to report it.
         */

        if (pOnceControl->onceMutex != NULL)
            return EINVAL;      /* POSIX in shape but not in spirit... */
        }

    /*
     * POSIX does not specifies what happens for other threads waiting for
     * the initialization routine to complete when it was canceled; we chose
     * to make them return to square one and redo the initialization step
     * again. This will also restore the onceCanceled field to a pristine
     * state.
     */

    if (pOnceControl->onceCanceled)
        goto startAgain;

    /* The initialization function has returned. Let's continue */

    return (_RETURN_PTHREAD_SUCCESS);
    }

/*
 * Section 17 - Thread-Specific Data
 */

/*******************************************************************************
*
* pthread_key_create - create a thread specific data key (POSIX)
*
* This routine allocates a new thread specific data key. The key is stored in
* the location pointed to by <key>. The value initially associated with the
* returned key is NULL in all currently executing threads. If the maximum
* number of keys are already allocated, the function returns an error
* ('EAGAIN').
*
* The <destructor> parameter specifies a destructor function associated with
* the key. When a thread terminates via pthread_exit(), or by cancellation,
* <destructor> is called with the value associated with the key in that
* thread as an argument. The destructor function is 'not' called if that value
* is NULL. The order in which destructor functions are called at thread
* termination time is unspecified.
*
* It is the user's responsibility to call pthread_key_delete() when the memory
* associated with the key is no longer required, and to ensure that no threads
* access the key after it has been deleted. Failure to do this can return
* unexpected results, and can cause memory leaks.
*
* RETURNS: On success zero; on failure the 'EAGAIN' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_getspecific(),
* pthread_key_delete(),
* pthread_setspecific()
*/

int pthread_key_create
    (
    pthread_key_t * pKey,               /* thread specific data key     */
    void (*destructor)(void *)          /* destructor function          */
    )
    {
    self_become_pthread ();

    (void)semTake (key_mutex, WAIT_FOREVER);    /* can't return on error */

    for ((*pKey) = 0; (*pKey) < _POSIX_THREAD_KEYS_MAX; (*pKey)++)
        {
        if (key_table[(*pKey)].keyStatus == KEY_FREE)
            {
            key_table[(*pKey)].keyStatus = KEY_IN_USE;
            key_table[(*pKey)].destructor = destructor;
            (void)semGive (key_mutex);
            return (_RETURN_PTHREAD_SUCCESS);
            }
        }

    (void)semGive (key_mutex);
    return(EAGAIN);
    }

/*******************************************************************************
*
* pthread_setspecific - set thread specific data (POSIX)
*
* Sets the value of the thread specific data associated with <key> to <value>
* for the calling thread.
*
* Setting the <value> to NULL will cause the destructor not to be called during
* cleanup of the thread.
*
* RETURNS: On success zero; on failure a non-zero error code:
* \is
* \i EINVAL
* \i ENOMEM
* \ie
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_getspecific(),
* pthread_key_create(),
* pthread_key_delete()
*/

int pthread_setspecific
    (
    pthread_key_t       key,            /* thread specific data key     */
    const void *        value           /* new value                    */
    )
    {
    int         ret;
    pthreadCB * pThread;

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    pThread = _pthread_id_self();

    if ((pThread->privateData) ||
        (pThread->privateData = pthread_key_allocate_data()))
        {
        if ((key < _POSIX_THREAD_KEYS_MAX) &&
            (key_table[key].keyStatus != KEY_FREE))
            {
            if (pThread->privateData[key] == NULL)
                {
                if (value != NULL)
                    pThread->privateDataCount++;
                }
            else
                {
                if (value == NULL)
                    {

                    /*
                     * Decrement the data count here because during cleanup we
                     * do not want to call the destructor for this associated
                     * key since the value is NULL.
                     */

                    pThread->privateDataCount--;
                    }
                }

            pThread->privateData[key] = value;
            ret = OK;
            }
        else
            {
            ret = EINVAL;
            }
        }
    else
        {
        ret = ENOMEM;
        }

    return(ret);
    }

/*******************************************************************************
*
* pthread_getspecific - get thread specific data (POSIX)
*
* This routine returns the value associated with the thread specific data
* key <key> for the calling thread.
*
* RETURNS: The value associated with <key>, or NULL.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_key_create(),
* pthread_key_delete(),
* pthread_setspecific()
*/

void * pthread_getspecific
    (
    pthread_key_t key           /* thread specific data key */
    )
    {
    void * ret = NULL;
    pthreadCB * pThread;

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    pThread = _pthread_id_self();

    if ((pThread->privateData) && (key < _POSIX_THREAD_KEYS_MAX))
        {
	if (key_table[key].keyStatus == KEY_IN_USE)
	    {
	    ret = (void *)pThread->privateData[key];
	    }
        }

    return(ret);
    }

/*******************************************************************************
*
* pthread_key_delete - delete a thread specific data key (POSIX)
*
* This routine deletes the thread specific data associated with <key>, and
* deallocates the key itself. It does not call any destructor associated
* with the key.
*
* Any attempt to use key following the call to pthread_key_delete() results
* in undefined behavior.
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_key_create()
*/

int pthread_key_delete
    (
    pthread_key_t key           /* thread specific data key to delete */
    )
    {
    if (key < _POSIX_THREAD_KEYS_MAX)
        {
        if (semTake (key_mutex, WAIT_FOREVER) == ERROR)
            return EINVAL;      /* POSIX in shape but not in spirit... */

        switch (key_table[key].keyStatus)
            {
            case KEY_IN_USE:

                key_table[key].destructor = NULL;
                key_table[key].keyStatus = KEY_FREE;

                (void)semGive (key_mutex);
                return (_RETURN_PTHREAD_SUCCESS);

            case KEY_FREE:
                (void)semGive (key_mutex);
                return (EINVAL);

            default:
                (void)semGive (key_mutex);
                return (EBUSY);
            }
        }
    else
        {
        return (EINVAL);
        }
    }

/*
 * Section 18 - Thread Cancellation
 */

/*******************************************************************************
*
* pthread_cancel - cancel execution of a thread (POSIX)
*
* This routine sends a cancellation request to the thread specified by
* <thread>. Depending on the settings of that thread, it may ignore the
* request, terminate immediately or defer termination until it reaches a
* cancellation point.
*
* When the thread terminates it performs as if pthread_exit() had been called
* with the exit status 'PTHREAD_CANCELED'.
*
* See also the list of cancellation points in system calls and library calls
* detailed in the pthreadLib documentation.
*
* IMPLEMENTATION NOTE
* In VxWorks, asynchronous thread cancellation is accomplished using a signal.
* The signal 'SIGCNCL' has been reserved for this purpose. Applications should
* take care not to block or handle this signal.
*
* Please also note that all threads that remain <joinable> at the time they are
* cancelled should ensure that pthread_join() is called on their behalf by
* another thread to reclaim the resources that they hold.
*
* RETURNS: On success zero; on failure the 'ESRCH' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_exit(),
* pthread_setcancelstate(),
* pthread_setcanceltype(),
* pthread_testcancel()
*/

int pthread_cancel
    (
    pthread_t thread                    /* thread to cancel */
    )
    {
    pthreadCB * pThread         = (pthreadCB *)thread;
    pthreadCB * selfPthreadId   = NULL;
    int         status          = _RETURN_PTHREAD_SUCCESS;

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    selfPthreadId = _pthread_id_self();

    /*
     * We validate and lock the to-be-cancelled pthread so that it cannot go
     * away while we check its cancellation properties.
     */

    if (PTHREAD_VALIDATE_AND_LOCK (pThread, selfPthreadId) != OK)
        return (ESRCH);

    /*
     * POSIX mandates that cancellation requests are held pending for threads
     * which are in the PTHREAD_CANCEL_DISABLE state (SPR#114130) as well as
     * for threads whose cancellability is enabled but which are in the
     * PTHREAD_CANCEL_DEFERRED state.
     * This means that unless the thread is enabled and asynchronous (in which
     * case we act immediately), cancellation requests are held pending.
     */

    if (pThread->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS &&
        pThread->cancelstate == PTHREAD_CANCEL_ENABLE)
        {
        /*
         * Release pthread's control block now or pthread_exit() would block.
         */
        PTHREAD_VALIDATED_UNLOCK (pThread, selfPthreadId);
        status = pthread_kill ((pthread_t)pThread, SIGCANCEL);
        }
    else
        {
        pThread->cancelrequest = 1;
        PTHREAD_VALIDATED_UNLOCK (pThread, selfPthreadId);
        }

    return status;
    }

/*******************************************************************************
*
* pthread_setcancelstate - set cancellation state for calling thread (POSIX)
*
* This routine sets the cancellation state for the calling thread to <state>,
* and, if <oldstate> is not NULL, returns the old state in the location
* pointed to by <oldstate>.
*
* The state can be one of the following:
* \is
* \i PTHREAD_CANCEL_ENABLE
* Enable thread cancellation.
* \i PTHREAD_CANCEL_DISABLE
* Disable thread cancellation (i.e. thread cancellation requests are ignored).
* \ie
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_cancel(),
* pthread_setcanceltype(),
* pthread_testcancel()
*/

int pthread_setcancelstate
    (
    int         state,                  /* new state            */
    int *       oldstate                /* old state (out)      */
    )
    {
    pthreadCB * pThread;

    if ((state != PTHREAD_CANCEL_DISABLE) && (state != PTHREAD_CANCEL_ENABLE))
        return (EINVAL);

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    pThread = _pthread_id_self();

    /*
     * Note that no validation and locking of the pthread is necessary since
     * 1) only the calling pthread executes this code and, 2) the unprotected
     * setting of the <cancelstate> field will not adversely affect a pthread
     * calling pthread_cancel() at the same time.
     */

    if (oldstate != NULL)
        *oldstate = pThread->cancelstate;

    pThread->cancelstate = state;

    return (0);
    }

/*******************************************************************************
*
* pthread_setcanceltype - set cancellation type for calling thread (POSIX)
*
* This routine sets the cancellation type for the calling thread to <type>.
* If <oldtype> is not NULL, then the old cancellation type is stored in the
* location pointed to by <oldtype>.
*
* Possible values for <type> are:
* \is
* \i PTHREAD_CANCEL_ASYNCHRONOUS
* Any cancellation request received by this thread will be acted upon as soon
* as it is received.
* \i PTHREAD_CANCEL_DEFERRED
* Cancellation requests received by this thread will be deferred until the
* next cancellation point is reached.
* \ie
*
* RETURNS: On success zero; on failure the 'EINVAL' error code.
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_cancel(),
* pthread_setcancelstate(),
* pthread_testcancel()
*/

int pthread_setcanceltype
    (
    int         type,           /* new type             */
    int *       oldtype         /* old type (out)       */
    )
    {
    pthreadCB * pThread;

    if (type != PTHREAD_CANCEL_ASYNCHRONOUS && type != PTHREAD_CANCEL_DEFERRED)
        {
        return EINVAL;
        }

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    pThread = _pthread_id_self();

    _pthread_setcanceltype_inline (type, oldtype, pThread);

    return 0;
    }

/*******************************************************************************
*
* pthread_testcancel - create a cancellation point in the calling thread (POSIX)
*
* This routine creates a cancellation point in the calling thread. It has
* no effect if cancellation is disabled (i.e. the cancellation state has been
* set to PTHREAD_CANCEL_DISABLE using the pthread_setcancelstate() function).
*
* If cancellation is enabled, the cancellation type is PTHREAD_CANCEL_DEFERRED
* and a cancellation request has been received, then this routine will call
* pthread_exit() with the exit status set to 'PTHREAD_CANCELED'. If any of
* these conditions is not met, then the routine does nothing.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_cancel(),
* pthread_setcancelstate(),
* pthread_setcanceltype()
*/

void pthread_testcancel (void)
    {
    pthreadCB *   pThread;

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    pThread = _pthread_id_self();

    _pthread_testcancel_inline (pThread);
    }

/*******************************************************************************
*
* pthread_cleanup_push - pushes a routine onto the cleanup stack (POSIX)
*
* This routine pushes the specified cancellation cleanup handler routine,
* <routine>, onto the cancellation cleanup stack of the calling thread. When
* a thread exits and its cancellation cleanup stack is not empty, the cleanup
* handlers are invoked with the argument <arg> in LIFO order from the
* cancellation cleanup stack.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_cleanup_pop(),
* pthread_exit()
*/

void pthread_cleanup_push
    (
    void (*routine)(void *),            /* cleanup routine      */
    void * arg                          /* argument             */
    )
    {
    pthreadCB *pThread;
    cleanupHandler *pClean;

    /* Get the ID of the caller; make the caller a POSIX thread if needed */

    pThread = _pthread_id_self();

    pClean = malloc(sizeof (cleanupHandler));
    if (pClean == NULL)
        return;

    pClean->routine     = routine;
    pClean->arg         = arg;
    pClean->next        = pThread->handlerBase;
    pThread->handlerBase= pClean;
    }

/*******************************************************************************
*
* pthread_cleanup_pop - pop a cleanup routine off the top of the stack (POSIX)
*
* This routine removes the cleanup handler routine at the top of the
* cancellation cleanup stack of the calling thread and executes it if
* <run> is non-zero. The routine should have been added using the
* pthread_cleanup_push() function.
*
* Once the routine is removed from the stack it will no longer be called when
* the thread exits.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO:
* pthread_cleanup_push(),
* pthread_exit()
*/

void pthread_cleanup_pop
    (
    int run                     /* execute handler? */
    )
    {
    pthreadCB *pThread = SELF_PTHREAD_ID;
    cleanupHandler *pClean;

    if (pThread == NULL)
        return;                 /* not a POSIX thread */

    pClean = pThread->handlerBase;
    if (pClean)
        {
        pThread->handlerBase = pClean->next;
        if (run != 0)
            pClean->routine(pClean->arg);
        free(pClean);
        }
    }

/*#############################################################################
#
# Support routines section.
#
#############################################################################*/

/********************************************************************************
* _pthread_oncancel_exit - terminate the calling pthread when canceled
*
* This routine handles the first stage of the cancellation process (i.e. prior
* to the exit sequence actually taking place. It is essentially ensuring that
* various aspects of the POSIX specs regarding cancellations are respected.
*
* Note that this routine is invoked in two different ways: 1) when a pthread
* calls pthread_testcancel(), and 2) when a pthread gets asynchronously
* cancelled (i.e. the SIGCANCEL signal handler is the caller).
*
* RETURNS: N/A
*
* \NOMANUAL
*/

_WRS_INLINE void _pthread_oncancel_exit
    (
    pthreadCB * pThread         /* pointer to canceled pthread's ctrl block */
    )
    {
    pthread_cond_t * cvcond;

    /*
     * Change the cancelled thread's cancelability type and state to prevent
     * any possibility for concurrent cancellations.
     */

    pThread->cancelstate = PTHREAD_CANCEL_DISABLE;
    pThread->canceltype = PTHREAD_CANCEL_DEFERRED;

    /*
     * Check whether the outgoing pthread might have locked another pthread.
     * If that's the case, release this other pthread now so to avoid
     * deadlocks.
     * Note that in the cancellation scenario we are assured that the locked
     * pthread has not exited since both the pthread_cancel() and
     * pthread_exit() APIs first acquire the pthread's lock before
     * proceeding. If the outgoing pthread already holds the lock those
     * operations will be deferred until the lock is released.
     */

    if (pThread->lockedPthread)
        {
        PTHREAD_UNLOCK (pThread->lockedPthread);
        pThread->lockedPthread = NULL;
        }

    /*
     * The POSIX specs mandate that a pthread canceled while joining
     * another pthread must not leave this other pthread detached (i.e.
     * yet another pthread should be able to join it and free its resources).
     */

    if (pThread->joinedPthread)
        {
        /*
         * We can't call PTHREAD_VALIDATE_AND_LOCK as the joined pthread
         * might have exited already.
         */

        (void)semTake (pthreadLibSemId, WAIT_FOREVER);

        if ((pThread->joinedPthread->flags & VALID) == VALID)
            {
            (void)semExchange (pthreadLibSemId, pThread->joinedPthread->lock,
                               WAIT_FOREVER);

            pThread->joinedPthread->flags &= ~JOINER_WAITING;
            PTHREAD_UNLOCK (pThread->joinedPthread);
            }
        else
            (void)semGive (pthreadLibSemId);

        pThread->joinedPthread = NULL;
        }

    /*
     * Now let's ensure that, per the POSIX specs for the pthread_cond_wait()
     * and pthread_cond_timedwait() API, a thread canceled while blocked on a
     * cancellation variable re-acquires the guard mutex prior to executing the
     * cancellation cleanup handlers.
     *
     * This routines also makes sure that the guard mutex is not recursively
     * acquired (i.e. the cancelled pthread already owns it) in case the
     * cancellation happened before semExchange() could release the guard mutex.
     *
     * Note: this execution path happens when the cancellation request is
     * issued before the thread actually gets to pend (synchronous path).
     */

    cvcond = pThread->cvcond;
    if (cvcond && cvcond->condMutex)
        {
        if (_pthreadSemOwnerGet (cvcond->condMutex->mutexSemId)
                    != taskIdSelf())
            pthread_mutex_lock (cvcond->condMutex);

        /*
         * Decrement this condition variable's reference count so that
         * it can be deleted.
         */

        --cvcond->condRefCount;
        }

    /* coverity[sleep] */

    pthread_exit (PTHREAD_CANCELED);
    }

/*******************************************************************************
*
* sigCancelHandler - thread cancel signal handler function
*
* Handles thread cancel signals. It is attached as part of the thread creation.
*
* \NOMANUAL
*/

LOCAL void sigCancelHandler
    (
    int dummy   /* not used */
    )
    {
    pthreadCB * pThread = SELF_PTHREAD_ID;      /* can never be NULL as this
                                                   handler applies to pthreads
                                                   only */

    _pthread_oncancel_exit (pThread);
    }

/*******************************************************************************
*
* wrapperFunc - thread entry wrapper function
*
* This function is a wrapper for the thread. It manages the attachment of
* the thread cancel signal handler, set the thread's signal mask to that of its
* creator, calls the thread's entry routine, and cleans up if the thread entry
* routine should just 'return' rather than calling pthread_exit().
*
* This routine is invoked by _pthreadCreate() through taskCreate().
*
* RETURNS: this routine should never return.
*
* \NOMANUAL
*/

LOCAL void wrapperFunc
    (
    void * (*function)(void *),         /* thread's entry routine */
    void * arg,                         /* thread's argument */
    const sigset_t creatorSigMask       /* thread's creator signal mask */
    )
    {
    /* Register the cancellation signal handler */

    signal (SIGCANCEL, sigCancelHandler);

    /*
     * POSIX 1003.1 states that the new thread inherits the signal
     * mask of its creator. Set the mask now.
     */

    (void) sigprocmask (SIG_SETMASK, &creatorSigMask, NULL);

    /*
     * Call the thread's entry routine here, and pass its result to
     * pthread_exit().
     */

    pthread_exit((void *)(function)(arg));

    /* NOTREACHED */

    return;
    }

/********************************************************************************
* _pthread_testcancel_inline - support routine for pthread_testcancel()
*
* This routine simply inlines the code for the pthread_testcancel() API.
*
* Note: the <pThread> parameter is assumed to be valid.
*
* RETURNS: N/A
*
* \NOMANUAL
*/

_WRS_INLINE void _pthread_testcancel_inline
    (
    pthreadCB * pThread /* this call applies to the calling pthread */
    )
    {
    if (pThread->cancelstate == PTHREAD_CANCEL_ENABLE &&
        pThread->cancelrequest == 1)
        {
        /* There is a pending cancellation request. Exit now */

        _pthread_oncancel_exit (pThread);
        }
    }

/*******************************************************************************
*
* _pthread_setcanceltype_inline - support routine for pthread_setcanceltype()
*
* This routine simply inlines the code for the pthread_setcanceltype() API.
* It assumes that the type has been verified by the caller.
*
* Note: the <pThread> parameter is assumed to be valid.
*
* RETURNS: 0 always.
*
* SEE ALSO: pthread_setcanceltype(),
*/

_WRS_INLINE void _pthread_setcanceltype_inline
    (
    int         type,           /* new type             */
    int *       oldtype,        /* old type (out)       */
    pthreadCB * pThread         /* this call applies to the calling pthread */
    )
    {
    /*
     * If the oldtype is required, save it now. Note that no validation and
     * locking of the pthread is necessary since 1) only the calling pthread
     * executes this code and, 2) the unprotected setting of the <canceltype>
     * field will not adversely affect a pthread calling pthread_cancel() at
     * the same time. */

    if (oldtype != NULL)
        *oldtype = pThread->canceltype;

    pThread->canceltype = type;
    }

/********************************************************************************
* pthread_testandset_canceltype - test and set a POSIX thread cancellation type
*
* This routine acts like pthread_setcanceltype() excepts that it will first test
* whether there is a pending cancellation request and honor it accordingly. It
* is used by pthread routines which act as cancellation points. Previous
* implementations used only pthread_setcanceltype() and thus failed to honor
* pending cancellation requests.
*
* The test is done with pthread_testcancel() which will honor any pending
* cancellation requests and exit accordingly.
*
* NOTE : the pthread_setcanceltype() API cannot be modified to include the test
* for pending cancellation requests as this would make it a cancellation point,
* something that POSIX forbids. This is why we use a separate API. This API is
* not local as it is called outside of the pthread library (by aioPxLib for
* instance). Also, for performance reasons, pthread_testandset_canceltype()
* actually uses inline versions of pthread_testcancel() and
* pthread_setcanceltype()
*
* RETURNS: N/A
*
* \NOMANUAL
*/

int pthread_testandset_canceltype
    (
    int   type,                 /* new cancellation type */
    int * oldtype               /* old cancellation type (out) */
    )
    {
    pthreadCB * pThread = SELF_PTHREAD_ID;

    /*
     * The calling task may not be a POSIX thread: it can be a regular VxWorks
     * task using the pthreadLib API to start threads. In this case we simply
     * return.
     */

    if (pThread == NULL)
        return 0;

    /*
     * XXX PAD - this is an internal routine. The type verification may be
     * overkill..
     */

    if (type != PTHREAD_CANCEL_ASYNCHRONOUS && type != PTHREAD_CANCEL_DEFERRED)
        {
        return EINVAL;
        }

    /*
     * _pthread_testcancel_inline() will test for and honor any pending
     * cancellation requests. If there are any, the thread will be canceled
     * accordingly. (SPR# 117697)
     */

    _pthread_testcancel_inline (pThread);

    /*
     * If there is no pending request, we simply call the inline version of
     * pthread_setcanceltype() to do the job.
     */

     _pthread_setcanceltype_inline (type, oldtype, pThread);

    return 0;
    }

/*******************************************************************************
*
* _pthread_id_self - returns the caller's pthread ID
*
* The _pthread_id_self() inline is a fast superset of the POSIX
* pthread_self() API. It returns the calling thread's ID (a pointer to the
* POSIX thread control block) and, if necessary, gives the caller a POSIX
* thread persona.
*
* For performance, the caller's thread ID is first retrieved in the TCB.
* If that value is null then the caller is a VxWorks native task which needs
* being turned into a POSIX thread.
*
* This inline also works around compiler warnings caused by the option
* -Wbad-function-cast: we need to use a temporary variable to hold the value
* returned by pthread_self() and then cast it to a pthread CB pointer.
*
* RETURNS: a pointer to the POSIX thread's control block
*
* \NOMANUAL
*/

_WRS_INLINE pthreadCB * _pthread_id_self (void)
    {
    pthreadCB * pThread = SELF_PTHREAD_ID;

    if (pThread == NULL)
        {
        pthread_t threadId;

        threadId = pthread_self();
        pThread = (pthreadCB *)threadId;
        }

    return pThread;
    }

/*******************************************************************************
*
* pthread_key_allocate_data - allocate key data
*
* Internal function to allocate the key storage internally for the thread.
*
* RETURNS: pointer to the new key data
*
* \NOMANUAL
*/

static const void ** pthread_key_allocate_data (void)
    {
    const void ** new_data;

    if ((new_data =
              (const void**)malloc(sizeof(void *) * _POSIX_THREAD_KEYS_MAX)))
        {
        memset((void *)new_data, 0, sizeof(void *) * _POSIX_THREAD_KEYS_MAX);
        }

    return(new_data);
    }

/*******************************************************************************
*
* cleanupPrivateData - release a thread's private data
*
* This is called during the cleanup of a thread (i.e. pthread_exit).  It will
* check to see if BOTH the destructor is a non-NULL value and that the data
* value is a non-NULL value.  If both of these are non-NULL values, the
* destructor will be called.
*
* The privateDataCount is used so we do not need to go through all the keys to
* determine if the destructor should be called.
*
* This routine may NOT manipulate the TCB of the executing task and it may
* NOT call exit(). See the manual of pthread_abnormal_exit() for the
* rationales.
*
* RETURNS:
*
* \NOMANUAL
*/

LOCAL void cleanupPrivateData
    (
    pthreadCB * pThread
    )
    {
    void *              data;
    int                 key;
    int                 itr;

    (void)semTake (key_mutex, WAIT_FOREVER);    /* can't return error */

    for (itr = 0; itr < _POSIX_THREAD_DESTRUCTOR_ITERATIONS; itr++)
        {
        for (key = 0; key < _POSIX_THREAD_KEYS_MAX; key++)
            {
            if (pThread->privateDataCount)
                {
                if (pThread->privateData[key])
                    {
                    data = (void *)pThread->privateData[key];
                    pThread->privateData[key] = NULL;
                    pThread->privateDataCount--;
                    if (key_table[key].destructor)
                        {

                        /*
                         * Both the data and destructor are non-NULL values
                         * so call the destructor.
                         */

                        (void)semGive (key_mutex);
                        key_table[key].destructor(data);
                        (void)semTake (key_mutex, WAIT_FOREVER);
                        }
                    }
                }
            else
                {
                if (pThread->privateData != NULL)
                    {
                    free (pThread->privateData);
                    pThread->privateData = NULL;
                    }
                (void)semGive (key_mutex);
                return;
                }
            }
        }
    free (pThread->privateData);
    (void)semGive (key_mutex);
    }

/*******************************************************************************
*
* _pthread_onexit_cleanup - do common cleanup operations on exit
*
* This routine does most of the cleanup and resource freeing operations common
* between pthread_exit() and pthread_abnormal_exit().
*
* It may NOT manipulate the TCB of the task underlying the exiting pthread and
* it may NOT call taskExit(). See the manual of pthread_abnormal_exit() for
* the rationales.
*
* RETURNS: N/A
*
* \NOMANUAL
*/

_WRS_INLINE void _pthread_onexit_cleanup
    (
    pthreadCB * pThread,                /* exiting pthread's control block */
    BOOL        abnormalTermination     /* TRUE if abnormal termination */
    )
    {
    cleanupHandler * tmp;
    pthread_cond_t * cvcond = pThread->cvcond;

    /*
     * Let's indicate now the underlying VxWorks task is considered exited.
     * Even though the pthread control block is still around we want other
     * pthreads to get an error from the pthread validation.
     */

    pThread->flags |= TASK_EXITED;

    /*
     * If we were canceled during the initialization routine of pthread_once(),
     * we need to do some cleanup on the control variable to leave it "as if
     * pthread_once() was never called". The cleanup consists of restoring the
     * control variable to a pristine state as well as unblocking all threads
     * waiting on the semaphore and then destroying it.
     *
     * This needs to be protected because other threads might just be running
     * pthread_once() at the same time and the PTHREAD_VALIDATE_AND_LOCK will
     * protect this process.
     */

    if (pThread->pOnceControl != NULL)
        {
        /*
         * See comments in pthread_once() for an explanation of the logic
         * below.
         */

        (void)semTake (pthreadLibOnceSemId, WAIT_FOREVER);

        if (pThread->pOnceControl->onceDone)
            {
            (void)semFlush (pThread->pOnceControl->onceMutex);
            (void) semDelete (pThread->pOnceControl->onceMutex);
            pThread->pOnceControl->onceMutex = NULL;
            }
        else
            {
            pThread->pOnceControl->onceCanceled = TRUE;
            pThread->pOnceControl->onceMyTid = 0;
            pThread->pOnceControl->onceInitialized = FALSE;
            pThread->pOnceControl->onceDone = FALSE;
            (void)semFlush (pThread->pOnceControl->onceMutex);
            (void) semDelete (pThread->pOnceControl->onceMutex);
            pThread->pOnceControl->onceMutex = NULL;
            }

        (void)semGive (pthreadLibOnceSemId);
        }

    /*
     * Call the cancellation cleanup handlers.
     *
     * In the situation of an abnormally terminated thread, some of the
     * operations done by the cleanup handlers may be unable to complete
     * properly since the code is executed in the context of the task that
     * terminated the POSIX thread.
     */

    while (pThread->handlerBase)
        {
        tmp = pThread->handlerBase;
        pThread->handlerBase->routine (pThread->handlerBase->arg);
        pThread->handlerBase = pThread->handlerBase->next;
        free (tmp);
        }

    /*
     * The POSIX specs for pthread_cond_wait() and pthread_cond_timedwait()
     * require that a thread cancelled while blocked on a cancellation
     * variable re-acquire the guard mutex prior to executing the
     * cancellation cleanup handlers. However once the cleanup handlers have
     * been executed it is possible for the guard mutex to stay unavailable
     * for ever if none of the cleanup handlers releases it. In order to
     * prevent this we check whether the thread is existing while still
     * holding the mutex and we forcefully release this mutex if this is the
     * case.
     *
     * In the situation of an abnormally terminated thread, the guard mutex will
     * always be released (otherwise the task is not deleted), so the task
     * that terminated the thread re-acquired that guard mutex.
     */

    if (cvcond && cvcond->condMutex)
        {
        cvcond->condMutex->mutexCondRefCount--;
        (void)pthread_mutex_unlock (cvcond->condMutex);
        }

    cleanupPrivateData (pThread);
    }

/*******************************************************************************
*
* pthread_abnormal_exit - handles pthread's abnormal exit
*
* This function is invoked from a task delete hook to clean up after a thread
* that has been deleted by a call to taskDelete() or for the case of native
* VxWorks tasks turned pthreads, has exited without calling pthread_exit().
*
* It does essentially the same a pthread_exit() so the two routines should be
* maintained in sync, However it cannot manipulate the TCB of the executing
* task, including using SELF_PTHREAD_ID, since the delete hook can be executed
* in the context of the task that killed the task underlying the pthread.
*
* This routine assumes its <pThread> parameter to point to a valid area of
* memory.
*
* \NOMANUAL
*/

LOCAL void pthread_abnormal_exit
    (
    pthreadCB * pThread
    )
    {
    WIND_TCB *  pTcb;
    pthread_cond_t * cvcond = NULL;

    /*
     * Change the exited thread's cancelability type and state to prevent
     * any possibility for cancellation. Although this is a moot operation for
     * a terminated pthread, this still ensures that the cleanup handlers
     * can be safe from triggering a cancellation in case they happen to be
     * or call cancellation points.
     */

    pThread->canceltype = PTHREAD_CANCEL_DEFERRED;
    pThread->cancelstate = PTHREAD_CANCEL_DISABLE;

    /*
     * Let's not go away just yet if somebody is attempting to do something
     * on the abnormally terminated pthread. We have to acquire the
     * library's mutex prior to locking the thread in case somebody else is
     * attempting to validate and lock the abnormally terminated pthread
     * Note also that there is no way to handle semTake() or semGive() failure
     * here...
     */

    (void)semTake (pthreadLibSemId, WAIT_FOREVER);

    /*
     * If the pthread validation fails here, we can not call exit() but
     * instead we just return.  This is because pthread_abnormal_exit() is
     * registered as a task delete hook and if we exit, we would be exiting
     * out of the task that triggered the hook and would then destroy itself.
     * We acknowledge that returning prematurely will cause a memory leak but,
     * being unable to validate the thread, there is no way to know whether
     * freeing resources might trigger exceptions or other problems.
     *
     * IMPORTANT NOTE: we cannot use PTHREAD_VALIDATE_AND_LOCK here as this
     * would cause a segmentation fault for native VxWorks task.
     */

    if (!VALID_PTHREAD (pThread, pTcb))
        {
        (void) semGive (pthreadLibSemId);
        return;
        }

    /*
     * While not officially supported, if this is a RTP task that somehow
     * acquired a kernel pthread context, and it is the last task of the RTP,
     * then RTP resource reclamation will have already occurred. In that case,
     * the pthread lock semaphore will have already been reclaimed, so just
     * invalidate the pthread and keep going. It's not worth potentially
     * locking up pthreadLibSemId if someone made a mistake.
     */

    if (OBJ_VERIFY (pThread->lock, semClassId) != OK)
	{
	pThread->flags |= TASK_EXITED;
        (void) semGive (pthreadLibSemId);
	}
    else
	{
	(void) semExchange (pthreadLibSemId, pThread->lock, WAIT_FOREVER);
	}

    /*
     * In case the thread got abnormally terminated while pending on a condition
     * variable, the deleted task re-acquires the guard mutex. This is
     * mimicking the cancellation scenario but we don't have to check whether
     * the mutex might be already acquired by the terminated thread here since
     * we know it cannot.
     *
     * Note: this has to be done before the call to _pthread_onexit_cleanup().
     */

    cvcond = pThread->cvcond;
    if (cvcond && cvcond->condMutex)
        {
        (void)pthread_mutex_lock (cvcond->condMutex);

        /*
         * Decrement this condition variable's reference count so that
         * it can be deleted.
         */
        --cvcond->condRefCount;
        }

    /*
     * Execute the bulk of the exit sequence: cleanup operations, resource
     * freeing operations.
     * Note that the execution of the cleanup handlers may turn the executing
     * task into a POSIX thread (depending on the pthread API being used).
     */

    /* coverity[sleep] */

    _pthread_onexit_cleanup (pThread, TRUE);

    /*
     * Now handle the case of a possible joined thread and perform additional
     * resource freeing as necessary.
     */

#if DETACHED_PTHREADS_MAY_BE_JOINED
    if (!(pThread->flags & JOINABLE) || ((pThread->flags & DETACHED)))
#else
    if (!(pThread->flags & JOINABLE))
#endif
        {
        /*
         * Note: the exiting thread is not unlocked since the mutex is going
         * to be deleted by pthreadCtrlBlockFree(). The resulting error coming
         * from pthreadValidateAndLock() will tell any thread pended for an
         * operation on this exiting thread that this operation cannot
         * proceed.
         */

        /* coverity[sleep] */

        pthreadCtrlBlockFree(pThread);
        return;
        }

    /*
     * We do not have an exit status to provide, so we use PTHREAD_CANCELED
     * since that's really the only status that a conforming application
     * might be looking for.
     * XXX PAD - should we introduce something like PTHREAD_ABORT?
     */

    pThread->exitStatus = PTHREAD_CANCELED;

    /*
     * Release joiner thread if any. Note that the pthread may be unlocked
     * only _after_ the JOINER_WAITING flag has been checked otherwise it
     * would be possible for the joiner thread to free the exiting thread's
     * control block _before_ the exiting thread has had the time to verify
     * the flags. Also it is not possible to unlock the thread after the
     * joiner thread has been released since the exiting thread's control
     *  block might have already been freed by the joiner thread when the
     *  exiting thread comes to unlocking itself...
     */
    if (pThread->flags & JOINER_WAITING)
        {
        PTHREAD_UNLOCK(pThread);
        (void) semGive (pThread->exitJoinSemId);
        }
    else
        {
        /*
         * In a normal situation the completion of the resource freeing is done
         * by the joiner pthread. Since this is an abnormal exit situation and
         * no joiner thread has been detected, we free the pthread's resources
         * without unlocking the pthread's control block so that a possible
         * late joiner get an error from PTHREAD_LOCK.
         */

        /* coverity[sleep] */

        pthreadCtrlBlockFree (pThread);
        }
    }

/*******************************************************************************
*
* pthreadCtrlBlockFree - free the exiting pthread's control block
*
* This routine releases all resources associated with the exiting thread's
* control block. When it returns the thread is considered gone from the
* system.
*
* This routine may NOT manipulate the TCB of the executing task and it may
* NOT call exit(). See the manual of pthread_abnormal_exit() for the
* rationales.
*
* RETURNS:
*
* \NOMANUAL
*/

LOCAL void pthreadCtrlBlockFree
    (
    pthreadCB *pThread
    )
    {
    /*
     * We have to acquire the library's mutex prior to freeing the memory
     * used for the exiting/exited pthread's control block in order to
     * guarantee that anyone having acquired the library's mutex and
     * validated the pthread can rely on the pthread's control block to be
     * still valid when they attempt to lock it.
     */

    (void)semTake (pthreadLibSemId, WAIT_FOREVER);

    pThread->vxTaskId = 0;
    pThread->flags = 0;         /* mark the pthread as invalid */

    /* Sanity verification to make sure private data have been freed */

    if (pThread->privateData)
        {
        cleanupPrivateData(pThread);
        }

     /*
      * Always delete the join synchronization semaphore if it has been
      * created (defect 130517).
      */

    if (pThread->exitJoinSemId != NULL)
        (void) semDelete(pThread->exitJoinSemId);

    (void) semDelete(pThread->lock);

    free (pThread);

    /*
     * Release the library's mutex only after the full resource freeing
     * operation is completed.
     */

    (void) semGive (pthreadLibSemId);

    /* At this point the POSIX thread no longer exists in the system */
    }

/*******************************************************************************
*
* self_become_pthread - convert a native task into a POSIX thread
*
* This routine makes possible for native VxWorks tasks (i.e. tasks that are
* not created via a call to pthread_create()) to use POSIX thread API. This
* requires giving the native VxWorks task the minimum set of POSIX thread
* attributes required to have the POSIX thread API function correctly.
*
* Unlike with pthreads created via pthread_create(), nobody will usually
* join a native VxWorks task turned POSIX thread. It is therefore more
* appropriate to not set the attachment state of the new thread to
* "joinable" so that resource reclamation occurs naturally when the
* newly formed thread calls pthread_exit().
*
* Note that if the newly formed thread does not call pthread_exit() the
* resource reclamation will be handle via the library's task delete hook
* routine.
*
* RETURNS: N/A
*
* \NOMANUAL
*/

LOCAL void self_become_pthread (void)
    {
    pthreadCB * pThread = NULL;
    WIND_TCB *  pTcb = (WIND_TCB *)taskIdSelf();
    int         vxTaskPrio;

    /* If the caller is already a POSIX thread, then there is nothing to do */

    if ((pThread = (pthreadCB *)pTcb->pPthread) != NULL)
        return;

    /* Allocation of the pthread control block for the native task */

    if (!(pThread = malloc(sizeof (pthreadCB))))
        return;

    bzero((char *)pThread, sizeof (pthreadCB));
    pThread->privateData = NULL;
    pThread->handlerBase = NULL;
    pThread->vxTaskId = (TASK_ID)pTcb;
    pThread->joinedPthread = NULL;

    /*
     * Create the mutex used to lock the pthread's control block as a "delete
     * safe" mutex. This will address the abnormal termination scenario by
     * preventing a pthread to go away while holding the lock of another
     * pthread's CB.
     */

    pThread->lock = semMCreate (SEM_Q_PRIORITY | SEM_INVERSION_SAFE |
                                SEM_DELETE_SAFE);

    if (pThread->lock == SEM_ID_NULL)
        {
        free (pThread);
        return;
        }

    pThread->lockedPthread = 0;

    /*
     * Get the priority from the associated task. The thread internal priority
     * contains the POSIX version so we need to convert it.
     */

    if (taskPriorityGet (PTHREAD_TO_NATIVE_TASK (pThread), &vxTaskPrio) != OK)
        {
        free (pThread);
        return;
        }

    pThread->priority = PX_VX_PRI_CONV (posixPriorityNumbering, vxTaskPrio);

    pThread->cancelstate = PTHREAD_CANCEL_ENABLE;
    pThread->canceltype = PTHREAD_CANCEL_DEFERRED;

    pThread->cancelrequest = 0;
    pThread->flags = VALID;

    /* Register the cancellation signal handler */

    signal (SIGCANCEL, sigCancelHandler);

    /*
     * Record the thread ID. At this point the native task has been turned
     * into a POSIX thread.
     */

    pTcb->pPthread = pThread;
    }

/*******************************************************************************
*
* pthreadValidateAndLock - validate and lock a pthread
*
* This routine makes possible for the caller to validate a pthread and lock it
* without risking inconsistencies if ever the caller gets preempted in the
* middle of this operation.
*
* By convention this routine is to be called by all pthreadLib functions that
* need to have exclusive access to the targeted pthread's control block.
*
* All the mutexes involves here are inversion safe so as to minimize the
* impact on concurrent pthreads.
*
* RETURNS: ERROR if acquiring semaphore fails or if thread is invalid, otherwise
*          OK
*
* \NOMANUAL
*/

LOCAL STATUS pthreadValidateAndLock
    (
    pthreadCB * pThread,        /* pthread to validate and lock */
    pthreadCB * selfPthreadId   /* pointer to caller's control block */
    )
    {
    WIND_TCB *  pTcb;
    STATUS      status = OK;
    int         prevCnclType;

    /*
     * Switch to deferred cancelability type as we do not want to be
     * canceled after having acquired either of the library's mutex or the
     * pthread's control block's mutex.
     */

    _pthread_setcanceltype_inline (PTHREAD_CANCEL_DEFERRED, &prevCnclType,
                                   selfPthreadId);

    /* Acquire the library's mutex prior to doing the validation */

    if (semTake (pthreadLibSemId, WAIT_FOREVER) == ERROR)
        {
        pthread_testandset_canceltype (prevCnclType, NULL);

        return (ERROR);
        }

    /*
     * Prior to locking the targeted thread its ID is registered in the
     * caller's control block in case the caller gets cancelled while holding
     * the lock to the targeted thread (this would result in a situation where
     * the targeted thread would no longer be able to function properly or exit
     * cleanly).
     */

    selfPthreadId->lockedPthread = pThread;

    if (VALID_PTHREAD (pThread, pTcb) == FALSE)
        {
        (void) semGive (pthreadLibSemId);
        selfPthreadId->lockedPthread = 0;
        pthread_testandset_canceltype (prevCnclType, NULL);

        return (ERROR);
        }

    /*
     * Lock the pthread.
     *
     * Use semExchange() to make sure the library's mutex is released once the
     * pthread's lock is acquired or pended on.
     * Note that the pthread lock might fail if the pthread is deleted. The
     * caller should be allowed to continue but should be made aware of the
     * problem.
     */

    if (semExchange (pthreadLibSemId, pThread->lock, WAIT_FOREVER) == ERROR)
        {
        selfPthreadId->lockedPthread = 0;
        status = ERROR;
        }

    /*
     * Restore original cancelability type and check whether there may be
     * pending cancellation that have to be honoured. If the caller has been
     * canceled this routine does not return and the lock on the targeted
     * pthread is be released by _pthread_oncancel_exit().
     */

    if (pthread_testandset_canceltype (prevCnclType, NULL) != 0)
	{
	status = ERROR;
	}

    return status;
    }

/*******************************************************************************
*
* deadlock - simulate a deadlocked mutex
*
* Unlike mutex semaphores, POSIX mutexes deadlock if the owner tries to acquire
* a mutex more than once. This routine ensures that the calling thread will
* come to a grinding halt. This routine will only get called if there is an
* application programming error.
*
* \NOMANUAL
*/

LOCAL void deadlock (void)
    {
    taskSuspend(0);
    }

