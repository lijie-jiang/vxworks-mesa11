/* pthread.h - header for POSIX threads (pthreads) */
 
/*
 * Copyright (c) 1984-2004, 2006-2007, 2010-2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. 
 */
 
/*
modification history
--------------------
01p,10jan11,pad  Moved internal flags into pthreadLibP.h. Structure
		 pthread_once_t is now declared in base/b_pthread_once_t.h.
01o,11nov10,pad  Moved internal types out into pthreadLibP.h
01n,14apr10,jpb  Updated for LP64 adaptation.
01m,02may07,jpb  SMP audit.  Removed unused structure fields.
01l,30aug06,pad  Removed direct pthread_t type definition and include
                 base/b_pthread_t.h instead (defect 63706 / fix 63777).
01k,27mar06,jpb  Fixed a couple of function prototypes for conformance (SPR
		 117227). Added PTHREAD_VALID_OBJ and PTHREAD_INVALID. Renamed
		 PTHREAD_INITIALIZED and PTHREAD_DESTROYED into
		 PTHREAD_INITIALIZED_OBJ and PTHREAD_DESTROYED_OBJ.
01j,22feb06,pes  Add base/b_pthread_attr_t.h include
01i,06jan06,gls  moved pthread_attr_t to pthreadCommon.h
01h,27sep04,pad  Added onceDone and onceMyTid fields to the pthread_once_t
                 type (SPR #98589).
01g,18jun04,pad  Changed value of PTHREAD_CANCELED constant so as to avoid
                 confusion with VxWorks ERROR.
01f,04jun04,pad  Added a threadAttrOptions field to the pthread_attr_t
                 structure so that task options can be set for threads (SPR
                 92417).
01e,07may04,pad  Re-instated the _POSIX_THREAD_PRIO_INHERIT and
                 _POSIX_THREAD_PRIO_PROTECT macros since they are part of the
                 POSIX standard.
01d,19mar04,pad  Provided macro definitions to make all supported routine
                 prototypes visible (SPR 94829). Fixed PTHREAD_COND_INITIALIZER
		 macro (SPR 96603).
01c,22oct01,jgn  add definition of PTHREAD_STACK_MIN (SPR #71110)
01b,11sep00,jgn  split into kernel & user level parts (SPR #33375)
01a,17jul00,jgn  created from DOT-4 version
*/
 
#ifndef __INCpthreadh
#define __INCpthreadh

/* includes */

#include <vxWorks.h>
#include <pthreadCommon.h>
#include <base/b_pthread_attr_t.h>
#include <base/b_pthread_t.h>
#include <base/b_pthread_once_t.h>
#include <semLib.h>
#include <signal.h>
#include <timers.h>
#include <sched.h>
#include <taskLib.h>		/* WIND_TCB */

#if defined(__cplusplus)
extern "C" {
#endif	/* __cplusplus */

/*
 * The following set of macros ("Compile-Time Symbolic Constants") corresponds
 * to the supported set of POSIX set features supported for kernel code.
 * What's not listed below is not supported (namely
 * _POSIX_THREAD_PROCESS_SHARED and _POSIX_THREAD_SAFE_FUNCTIONS).
 */

#define _POSIX_THREADS				1
#define _POSIX_THREAD_PRIORITY_SCHEDULING	1
#define _POSIX_THREAD_PRIO_INHERIT		1
#define _POSIX_THREAD_PRIO_PROTECT		1
#define _POSIX_THREAD_ATTR_STACKSIZE		1
#define _POSIX_THREAD_ATTR_STACKADDR		1

/* Other defines */

#ifdef _POSIX_THREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_PRIVATE	0
#define PTHREAD_PROCESS_SHARED	1
#endif /* _POSIX_THREAD_PROCESS_SHARED */

#define PTHREAD_STACK_MIN		4096	/* suggested minimum */

#define	PTHREAD_INHERIT_SCHED		0	/* implementation default */
#define	PTHREAD_EXPLICIT_SCHED		1
#define PTHREAD_SCOPE_PROCESS		2
#define PTHREAD_SCOPE_SYSTEM		3	/* implementation default */


#define PTHREAD_ONCE_INIT		{FALSE, FALSE, 0, NULL, FALSE}

#define PTHREAD_INITIALIZED_OBJ		0xF70990EF	/* object can be used */
#define PTHREAD_DESTROYED_OBJ		-1	/* object status */
#define PTHREAD_VALID_OBJ		0xEC542A37
#define PTHREAD_INVALID_OBJ		-1
#define PTHREAD_UNUSED_YET_OBJ		-1

#define PTHREAD_MUTEX_INITIALIZER	{NULL, PTHREAD_VALID_OBJ,	\
					PTHREAD_UNUSED_YET_OBJ, 0, 0,   \
					{PTHREAD_INITIALIZED_OBJ,	\
					PTHREAD_PRIO_INHERIT, 0}}
#define PTHREAD_COND_INITIALIZER	{NULL, PTHREAD_VALID_OBJ,	\
					PTHREAD_UNUSED_YET_OBJ, 0, NULL}

#define	PTHREAD_CREATE_DETACHED		0
#define	PTHREAD_CREATE_JOINABLE		1 /*.4a and implementation default */

#define PTHREAD_CANCEL_ENABLE		0
#define PTHREAD_CANCEL_DISABLE		1

#define PTHREAD_CANCELED		((void *)-2)

#define _POSIX_THREAD_THREAD_MAX	0	/* unlimited, not checked */
#define _POSIX_THREAD_KEYS_MAX		256     
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS  4


/* typedefs */

typedef struct {
	int	condAttrStatus;			/* status flag		*/
#ifdef _POSIX_THREAD_PROCESS_SHARED
	int	condAttrPshared;		/* process-shared attr	*/
#endif	/* _POSIX_THREAD_PROCESS_SHARED */
	} pthread_condattr_t;


typedef struct {
	int	mutexAttrStatus;		/* status flag		*/
#ifdef _POSIX_THREAD_PROCESS_SHARED
	int	mutexAttrPshared;		/* process-shared attr	*/
#endif	/* _POSIX_THREAD_PROCESS_SHARED */
	int	mutexAttrProtocol;		/* inherit or protect	*/
	int	mutexAttrPrioceiling;		/* priority ceiling	*/
						/* (protect only)	*/
        int     mutexAttrType;                  /* mutex type           */
	} pthread_mutexattr_t;


/* values for mutexAttrProtocol */

#define PTHREAD_PRIO_NONE       0
#define PTHREAD_PRIO_INHERIT	1
#define PTHREAD_PRIO_PROTECT	2

/* values for mutexAttrType */

#define PTHREAD_MUTEX_NORMAL            0
#define PTHREAD_MUTEX_ERRORCHECK        1
#define PTHREAD_MUTEX_RECURSIVE         2
#define PTHREAD_MUTEX_DEFAULT           PTHREAD_MUTEX_NORMAL

typedef struct {
	SEM_ID			mutexSemId;
        int                     mutexValid;
        int                     mutexInitted;
	int			mutexCondRefCount;
	int			mutexSavPriority;
	pthread_mutexattr_t	mutexAttr;
	} pthread_mutex_t;

typedef struct {
	SEM_ID			condSemId;
        int                     condValid;
        int                     condInitted;
	int			condRefCount;
	pthread_mutex_t         *condMutex;
#ifdef _POSIX_THREAD_PROCESS_SHARED
	pthread_condattr_t	condAttr;
#endif	/* _POSIX_THREAD_PROCESS_SHARED */
	} pthread_cond_t;

typedef unsigned long	pthread_key_t;

/*
 * Section 3 Process Primitives	
 */

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
int pthread_kill(pthread_t thread, int sig);

/*
 * Section 11.3 Mutexes
 */

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

#ifdef _POSIX_THREAD_PROCESS_SHARED
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
#endif	/* _POSIX_THREAD_PROCESS_SHARED */

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/*
 * Section 11.4 Condition variables
 */

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);

#ifdef _POSIX_THREAD_PROCESS_SHARED
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
#endif	/* _POSIX_THREAD_PROCESS_SHARED */

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);

int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
			   const struct timespec *abstime);

/*
 * Section 13.5 Thread scheduling
 */

int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);

int pthread_attr_setschedparam(pthread_attr_t *attr,
	const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr,
	struct sched_param *param);

int pthread_getschedparam(pthread_t thread, int *policy,
	struct sched_param *param);
int pthread_setschedparam(pthread_t thread, int policy,
	const struct sched_param *param);

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
int pthread_mutexattr_getprotocol(pthread_mutexattr_t *attr, int *protocol);

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
	int prioceiling);
int pthread_mutexattr_getprioceiling(pthread_mutexattr_t *attr,
	int *prioceiling);

int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_gettype (pthread_mutexattr_t *attr, int *type);

int pthread_mutex_setprioceiling(pthread_mutex_t *attr, int prioceiling,
	int *old_ceiling);
int pthread_mutex_getprioceiling(pthread_mutex_t *attr, int *prioceiling);

/*
 * Section 16 Thread management
 */

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

int pthread_attr_setopt (pthread_attr_t * pAttr, int options);
int pthread_attr_getopt (pthread_attr_t * pAttr, int * pOptions);
int pthread_attr_setname (pthread_attr_t * pAttr, char * name);
int pthread_attr_getname (pthread_attr_t * pAttr, char ** name);

void pthread_exit(void *value_ptr);

int pthread_create (pthread_t *pThread,
		    const pthread_attr_t *pAttr,
		    void * (*start_routine)(void *),
		    void *arg);

int pthread_join(pthread_t thread, void **status);

int pthread_detach(pthread_t thread);

pthread_t pthread_self(void);

int pthread_equal(pthread_t t1, pthread_t t2);

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/*
 * Section 17 Thread-specific data
 */

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));

int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);

int pthread_key_delete(pthread_key_t key);

/*
 * Section 18 Thread cancellation
 */

int pthread_cancel(pthread_t thread);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel(void);
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);

#if defined(__cplusplus)
}
#endif	/* __cplusplus */

#endif /* __INCpthreadh */
