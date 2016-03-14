/* gfxSampleInit.c - Sample frame buffer driver initialization */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24jan14,mgc  Modified for VxWorks 7 release
*/

#ifndef __INCgfxSampleInitc
#define __INCgfxSampleInitc

#include <vxWorks.h>

#ifdef SYS_MODEL
#undef SYS_MODEL
#endif
#define SYS_MODEL                   "Sample"

#ifndef FBDEV_MEMORY_SIZE
#define FBDEV_MEMORY_SIZE           (16*1024*1024)
#endif
#ifndef FBDEV_PREEMPTIVE_RTP_MEMORY_FREE
#define FBDEV_PREEMPTIVE_RTP_MEMORY_FREE       TRUE
#endif

#if defined(INCLUDE_FBDEV_SAMPLE_0)
#ifndef SAMPLE_FBDEV_NAME_0
#define SAMPLE_FBDEV_NAME_0         "/dev/fb0"
#endif
#ifndef SAMPLE_FBDEV_DISPLAY_0
#define SAMPLE_FBDEV_DISPLAY_0      "HDMI"
#endif
#ifndef SAMPLE_FBDEV_RESOLUTION_0
#define SAMPLE_FBDEV_RESOLUTION_0   "default"
#endif
#ifndef SAMPLE_FBDEV_BUFFERS_0
#define SAMPLE_FBDEV_BUFFERS_0      3
#endif
#ifndef SAMPLE_FBDEV_VSYNC_0
#define SAMPLE_FBDEV_VSYNC_0        FALSE
#endif
#endif

#if defined(INCLUDE_FBDEV_SAMPLE_1)
#ifndef SAMPLE_FBDEV_NAME_1
#define SAMPLE_FBDEV_NAME_1         "/dev/fb1"
#endif
#ifndef SAMPLE_FBDEV_DISPLAY_1
#define SAMPLE_FBDEV_DISPLAY_1      "LVDS panel"
#endif
#ifndef SAMPLE_FBDEV_RESOLUTION_1
#define SAMPLE_FBDEV_RESOLUTION_1   "default"
#endif
#ifndef SAMPLE_FBDEV_BUFFERS_1
#define SAMPLE_FBDEV_BUFFERS_1      3
#endif
#ifndef SAMPLE_FBDEV_VSYNC_1
#define SAMPLE_FBDEV_VSYNC_1        FALSE
#endif
#endif

#if defined(INCLUDE_FBDEV_SAMPLE_2)
#ifndef SAMPLE_FBDEV_NAME_2
#define SAMPLE_FBDEV_NAME_2         "/dev/fb2"
#endif
#ifndef SAMPLE_FBDEV_DISPLAY_2
#define SAMPLE_FBDEV_DISPLAY_2      "Flat panel"
#endif
#ifndef SAMPLE_FBDEV_RESOLUTION_2
#define SAMPLE_FBDEV_RESOLUTION_2   "default"
#endif
#ifndef SAMPLE_FBDEV_BUFFERS_2
#define SAMPLE_FBDEV_BUFFERS_2      3
#endif
#ifndef SAMPLE_FBDEV_VSYNC_2
#define SAMPLE_FBDEV_VSYNC_2        FALSE
#endif
#endif

#ifdef INCLUDE_FBDEV_SPLASH
#ifndef FB_SPLASH_BLIT
#define FB_SPLASH_BLIT              gfxFbSplashBlit
#endif
IMPORT STATUS FB_SPLASH_BLIT (void*, int, int, int, int);
#endif

IMPORT void gfxSampleInit (const char*, FUNCPTR);

#if defined(INCLUDE_FBDEV_SAMPLE_0)
/*******************************************************************************
 *
 * gfxSampleGetCfg0 - Get frame buffer configuration
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
LOCAL STATUS gfxSampleGetCfg0
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
    *pDeviceName = SAMPLE_FBDEV_NAME_0;
    *pDisplayName = SAMPLE_FBDEV_DISPLAY_0;
    *pResolution = SAMPLE_FBDEV_RESOLUTION_0;
    *pBuffers = SAMPLE_FBDEV_BUFFERS_0;
    *pVsync = SAMPLE_FBDEV_VSYNC_0;
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

/*******************************************************************************
*
* gfxSampleInit0 - Frame buffer driver initialization
*
* RETURNS: N/A
*
*/
void gfxSampleInit0
    (
    void
    )
    {
    gfxSampleInit (SYS_MODEL, gfxSampleGetCfg0);
#if defined (INCLUDE_FBDEV_CONSOLE)
    gfxFbConsoleInit ();
#endif
    }
#endif

#if defined(INCLUDE_FBDEV_SAMPLE_1)
/*******************************************************************************
 *
 * gfxSampleGetCfg1 - Get frame buffer configuration
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
LOCAL STATUS gfxSampleGetCfg1
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
    *pDeviceName = SAMPLE_FBDEV_NAME_1;
    *pDisplayName = SAMPLE_FBDEV_DISPLAY_1;
    *pResolution = SAMPLE_FBDEV_RESOLUTION_1;
    *pBuffers = SAMPLE_FBDEV_BUFFERS_1;
    *pVsync = SAMPLE_FBDEV_VSYNC_1;
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

/*******************************************************************************
*
* gfxSampleInit1 - Frame buffer driver initialization
*
* RETURNS: N/A
*
*/
void gfxSampleInit1
    (
    void
    )
    {
    gfxSampleInit (SYS_MODEL, gfxSampleGetCfg1);
#if defined (INCLUDE_FBDEV_CONSOLE)
    gfxFbConsoleInit ();
#endif
    }
#endif

#if defined(INCLUDE_FBDEV_SAMPLE_2)
/*******************************************************************************
 *
 * gfxSampleGetCfg2 - Get frame buffer configuration
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
LOCAL STATUS gfxSampleGetCfg2
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
    *pDeviceName = SAMPLE_FBDEV_NAME_2;
    *pDisplayName = SAMPLE_FBDEV_DISPLAY_2;
    *pResolution = SAMPLE_FBDEV_RESOLUTION_2;
    *pBuffers = SAMPLE_FBDEV_BUFFERS_2;
    *pVsync = SAMPLE_FBDEV_VSYNC_2;
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

/*******************************************************************************
*
* gfxSampleInit2 - Frame buffer driver initialization
*
* RETURNS: N/A
*
*/
void gfxSampleInit2
    (
    void
    )
    {
    gfxSampleInit (SYS_MODEL, gfxSampleGetCfg2);
#if defined (INCLUDE_FBDEV_CONSOLE)
    gfxFbConsoleInit ();
#endif
    }
#endif

#endif /* __INCgfxSampleInitc */
