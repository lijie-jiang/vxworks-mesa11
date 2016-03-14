/* evdevLibKbdCfg.c - evdev keyboard library configlette file */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
08nov13,j_x  create
*/

#ifndef __INCevdevLibKbdCfgc
#define __INCevdevLibKbdCfgc

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include <tyLib.h>

/* externals */

extern DEV_HDR *    iosDevMatch (const char * name);

/* functions */

/*******************************************************************************
*
* evdevKbdGetConsole - return console instance for keyboard attach
*
* This routine returns console instance for keyboard attach. The return value
* depends on configuration. If PC console component is configured, it
* will return PC console instance. If PC console component is not configured,
* it will return tty device instance; If no shell attach is configured, it will
* return NULL.
*
* RETURNS: device instance.
*
* ERRNO: N/A
*
* \NOMANUAL
*/

TY_DEV_ID evdevKbdGetConsole (void)
    {
#if defined (INCLUDE_EVDEV_LIB_KBD_SHELL_ATTACH) 
#if !defined (INCLUDE_PC_CONSOLE)
    char ttyDevName[20];

    snprintf (ttyDevName, sizeof(ttyDevName), "/tyCo/%d", CONSOLE_TTY);

    return (TY_DEV_ID)iosDevMatch(ttyDevName);
#else
    return (TY_DEV_ID)pcConDevBind(PC_CONSOLE, NULL, NULL);
#endif
#else
    return NULL;
#endif
    }

/*******************************************************************************
*
* evdevKbdIsRedirectTty - return if shell attach is configured
*
* This routine returns a boolean value if INCLUDE_EVDEV_LIB_KBD_SHELL_ATTACH
* is defined.
*
* RETURNS: TRUE, or FALSE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

BOOL evdevKbdIsRedirectTty (void)
    {
#if defined (INCLUDE_EVDEV_LIB_KBD_SHELL_ATTACH)
#if !defined (INCLUDE_PC_CONSOLE)
    return TRUE;
#else
    return FALSE;
#endif
#else 
    return FALSE;
#endif
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibKbdCfgc */
