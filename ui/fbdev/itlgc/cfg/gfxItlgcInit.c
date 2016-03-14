/* gfxItlgcInit.c - Intel Graphics Controller frame buffer driver initialization */

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
17may12,rfm  Written
*/

#include <vxWorks.h>
#include <vxBusLib.h>
#include <hwif/vxbus/vxBus.h>

#ifndef FBDEV_MEMORY_SIZE
#define FBDEV_MEMORY_SIZE           (16*1024*1024)
#endif
#ifndef FBDEV_PREEMPTIVE_RTP_MEMORY_FREE
#define FBDEV_PREEMPTIVE_RTP_MEMORY_FREE       TRUE
#endif

#ifdef INCLUDE_FBDEV_ITLGC_0
#ifndef ITLGC_FBDEV_NAME_0
#define ITLGC_FBDEV_NAME_0          "/dev/fb0"
#endif
#ifndef ITLGC_FBDEV_DISPLAY_0
#define ITLGC_FBDEV_DISPLAY_0       "default"
#endif
#ifndef ITLGC_FBDEV_RESOLUTION_0
#define ITLGC_FBDEV_RESOLUTION_0    "800x600"
#endif

#ifndef ITLGC_FBDEV_BUFFERS_0
#define ITLGC_FBDEV_BUFFERS_0       3
#endif
#ifndef ITLGC_FBDEV_VSYNC_0
#define ITLGC_FBDEV_VSYNC_0         FALSE
#endif
#endif

#ifdef INCLUDE_FBDEV_SPLASH
#ifndef FB_SPLASH_BLIT
#define FB_SPLASH_BLIT              gfxFbSplashBlit
#endif
IMPORT STATUS FB_SPLASH_BLIT (void*, int, int, int, int);
#endif

#ifdef INCLUDE_FBDEV_ITLGC_0
IMPORT BOOL gfxItlgcVxbInitialized;
IMPORT struct vxbDevRegInfo* gfxItlgcPciDevRegistration0;

METHOD_DECL(gfxItlgcVxbDrvInstall);
METHOD_DECL(gfxItlgcVxbDevAdd);

/*******************************************************************************
*
* gfxItlgcInit - Intel kernel frame buffer driver initialization
*
* RETURNS: N/A
*
*/
void gfxItlgcInit0
    (
    void
    )
    {
    /* Register and initialize VxBus driver with VxBus */ 
    vxbDevRegister((struct vxbDevRegInfo *)&gfxItlgcPciDevRegistration0);

    if (gfxItlgcVxbInitialized == TRUE)
        {
        vxbDevMethodRun (VXB_DRIVER_METHOD(gfxItlgcVxbDrvInstall), NULL);
        vxbDevMethodRun (VXB_DRIVER_METHOD(gfxItlgcVxbDevAdd), NULL);
        }
#if defined (INCLUDE_FBDEV_CONSOLE)
    gfxFbConsoleInit ();
#endif
    }

/*******************************************************************************
 *
 * gfxItlgcGetCfg - Get frame buffer configuration
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
STATUS gfxItlgcGetCfg0
    (
    size_t*             pVidMemSize,
    unsigned int*       pRtpFreeMem,
    char**              pDeviceName,
    char**              pDisplayName,
    char**              pResolution,
    unsigned int*       pBuffers,
    unsigned int*       pVsync,
    FUNCPTR*            pConsoleFuncPtr,
    FUNCPTR*            pSplashFuncPtr
    )
    {
    *pVidMemSize = FBDEV_MEMORY_SIZE;
    *pRtpFreeMem = FBDEV_PREEMPTIVE_RTP_MEMORY_FREE;
    *pDeviceName = ITLGC_FBDEV_NAME_0;
    *pDisplayName = ITLGC_FBDEV_DISPLAY_0;
    *pResolution = ITLGC_FBDEV_RESOLUTION_0;
    *pBuffers = ITLGC_FBDEV_BUFFERS_0;
    *pVsync = ITLGC_FBDEV_VSYNC_0;
#if defined(INCLUDE_FBDEV_CONSOLE)
    *pConsoleFuncPtr = FB_CONSOLE_WRITE;
#else
    *pConsoleFuncPtr = NULL;
#endif
#if defined(INCLUDE_FBDEV_SPLASH)
    *pSplashFuncPtr = FB_SPLASH_BLIT;
#else
    *pSplashFuncPtr = NULL;
#endif
    return OK;
    }
#endif
