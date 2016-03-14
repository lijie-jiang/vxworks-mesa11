/* system.c - system management functions */

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
14sep15,yat  Clean up code (US66034)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides compatibility functions for the Linux system operations.

NOMANUAL

*/

#include <vxoal/krnl/system.h>

struct task_struct current_thread_info;
struct atomic_notifier_head panic_notifier_list;

