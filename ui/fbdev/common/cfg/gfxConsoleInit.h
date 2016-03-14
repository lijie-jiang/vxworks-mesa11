/* gfxConsoleInit.h - Frame buffer driver definitions */

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24jan14,mgc  Modified for VxWorks 7 release
01a,17may12,rfm  Written
*/

#ifndef __INC_gfxConsoleInit_h
#define __INC_gfxConsoleInit_h

/* includes */

#include <vxWorks.h>
#include <fbdev.h>

/* defines */

#define FB_DEFAULT_CONSOLE_WRITE        gfxFbConsoleWrite

/* forward declarations */

IMPORT void gfxFbConsoleInit (void);
IMPORT int gfxFbConsoleWrite (FB_INFO*, const char*, size_t);

#endif /* __INC_gfxConsoleInit_h */
