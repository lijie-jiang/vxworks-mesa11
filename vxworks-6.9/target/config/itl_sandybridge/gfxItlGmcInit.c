/* gfxItlGmcInit.c - Intel Graphics and Memory Controller frame buffer driver initialization */

/*
 * Copyright (c) 2014-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
04feb16,yat  Remove system model check for Intel (US73271)
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
29jul15,rpc  Add entries for LVDS and DVI (US50700,US50615)
25feb15,tlu  Enable LVDS/VGA displays (US50700, US50616)
11mar15,yat  Add show and FBDEV task support (US55410)
06jan15,qsn  Modified for itl_64_vx7 bsp (US50612)
20dec14,qsn  Initial VxWorks 7 release (US48907)
*/

#ifndef __INCgfxItlGmcInitc
#define __INCgfxItlGmcInitc

#include <vxWorks.h>

#ifndef FBDEV_MEMORY_SIZE
#define FBDEV_MEMORY_SIZE           (1920*1080*4*3)
#endif
#ifndef FBDEV_PREEMPTIVE_RTP_MEMORY_FREE
#define FBDEV_PREEMPTIVE_RTP_MEMORY_FREE       TRUE
#endif

#ifdef INCLUDE_FBDEV_ITLGMC_0
#ifndef ITLGMC_FBDEV_NAME_0
#define ITLGMC_FBDEV_NAME_0         "/dev/fb0"
#endif
#ifndef ITLGMC_FBDEV_DISPLAY_0
#define ITLGMC_FBDEV_DISPLAY_0      "VGA"
#endif
#ifndef ITLGMC_FBDEV_RESOLUTION_0
#define ITLGMC_FBDEV_RESOLUTION_0   "1920x1080-32"
#endif
#ifndef ITLGMC_FBDEV_BUFFERS_0
#define ITLGMC_FBDEV_BUFFERS_0      3
#endif
#ifndef ITLGMC_FBDEV_VSYNC_0
#define ITLGMC_FBDEV_VSYNC_0        TRUE
#endif
#endif

#ifdef INCLUDE_FBDEV_ITLGMC_1
#ifndef ITLGMC_FBDEV_NAME_1
#define ITLGMC_FBDEV_NAME_1         "/dev/fb1"
#endif
#ifndef ITLGMC_FBDEV_DISPLAY_1
#define ITLGMC_FBDEV_DISPLAY_1      "DP"
#endif
#ifndef ITLGMC_FBDEV_RESOLUTION_1
#define ITLGMC_FBDEV_RESOLUTION_1   "1920x1080-32"
#endif
#ifndef ITLGMC_FBDEV_BUFFERS_1
#define ITLGMC_FBDEV_BUFFERS_1      3
#endif
#ifndef ITLGMC_FBDEV_VSYNC_1
#define ITLGMC_FBDEV_VSYNC_1        TRUE
#endif
#endif

#ifdef INCLUDE_FBDEV_ITLGMC_2
#ifndef ITLGMC_FBDEV_NAME_2
#define ITLGMC_FBDEV_NAME_2         "/dev/fb2"
#endif
#ifndef ITLGMC_FBDEV_DISPLAY_2
#define ITLGMC_FBDEV_DISPLAY_2      "HDMI"
#endif
#ifndef ITLGMC_FBDEV_RESOLUTION_2
#define ITLGMC_FBDEV_RESOLUTION_2   "1920x1080-32"
#endif
#ifndef ITLGMC_FBDEV_BUFFERS_2
#define ITLGMC_FBDEV_BUFFERS_2      3
#endif
#ifndef ITLGMC_FBDEV_VSYNC_2
#define ITLGMC_FBDEV_VSYNC_2        TRUE
#endif
#endif

#ifdef INCLUDE_FBDEV_ITLGMC_3
#ifndef ITLGMC_FBDEV_NAME_3
#define ITLGMC_FBDEV_NAME_3         "/dev/fb3"
#endif
#ifndef ITLGMC_FBDEV_DISPLAY_3
#define ITLGMC_FBDEV_DISPLAY_3      "DVI"
#endif
#ifndef ITLGMC_FBDEV_RESOLUTION_3
#define ITLGMC_FBDEV_RESOLUTION_3   "1920x1080-32"
#endif
#ifndef ITLGMC_FBDEV_BUFFERS_3
#define ITLGMC_FBDEV_BUFFERS_3      3
#endif
#ifndef ITLGMC_FBDEV_VSYNC_3
#define ITLGMC_FBDEV_VSYNC_3        TRUE
#endif
#endif

#ifdef INCLUDE_FBDEV_ITLGMC_4
#ifndef ITLGMC_FBDEV_NAME_4
#define ITLGMC_FBDEV_NAME_4         "/dev/fb4"
#endif
#ifndef ITLGMC_FBDEV_DISPLAY_4
#define ITLGMC_FBDEV_DISPLAY_4      "LVDS panel"
#endif
#ifndef ITLGMC_FBDEV_RESOLUTION_4
#define ITLGMC_FBDEV_RESOLUTION_4   "1024x768-32"
#endif
#ifndef ITLGMC_FBDEV_BUFFERS_4
#define ITLGMC_FBDEV_BUFFERS_4      3
#endif
#ifndef ITLGMC_FBDEV_VSYNC_4
#define ITLGMC_FBDEV_VSYNC_4        TRUE
#endif
#endif

#ifdef INCLUDE_FBDEV_SPLASH
#ifndef FB_SPLASH_BLIT
#define FB_SPLASH_BLIT              gfxFbSplashBlit
#endif
IMPORT STATUS FB_SPLASH_BLIT (void*, int, int, int, int);
#endif

#if defined(INCLUDE_PC_CONSOLE)
#define GFX_PC_CONSOLE_CHECK                                                   \
    {                                                                          \
    (void)printf ("The Intel HD Graphics driver will be initialized but\nthe driver may not work properly with the PC console,\nplease remove INCLUDE_PC_CONSOLE from the kernel\nif the graphics does not work properly\n"); \
    }
#else
#define GFX_PC_CONSOLE_CHECK
#endif

#if defined(INCLUDE_SMP_INIT)
#define GFX_SMP_CHECK
#else
#define GFX_SMP_CHECK                                                          \
    {                                                                          \
    (void)printf ("The Intel HD Graphics driver will be initialized but\nthe driver may not work properly on some non-SMP configuration,\nplease use SMP configuration with VX_SMP_NUM_CPUS >= 1\nif the graphics does not work properly\n"); \
    }
#endif

IMPORT void gfxItlGmcInit (FUNCPTR);

#ifdef INCLUDE_FBDEV_ITLGMC_0
/*******************************************************************************
*
* gfxItlGmcGetCfg0 - Get frame buffer configuration
*
* This routine returns the frame buffer configuration
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/
LOCAL STATUS gfxItlGmcGetCfg0
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
    *pDeviceName = ITLGMC_FBDEV_NAME_0;
    *pDisplayName = ITLGMC_FBDEV_DISPLAY_0;
    *pResolution = ITLGMC_FBDEV_RESOLUTION_0;
    *pBuffers = ITLGMC_FBDEV_BUFFERS_0;
    *pVsync = ITLGMC_FBDEV_VSYNC_0;
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

#ifdef INCLUDE_FBDEV_ITLGMC_1
/*******************************************************************************
*
* gfxItlGmcGetCfg1 - Get frame buffer configuration
*
* This routine returns the frame buffer configuration
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/
LOCAL STATUS gfxItlGmcGetCfg1
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
    *pDeviceName = ITLGMC_FBDEV_NAME_1;
    *pDisplayName = ITLGMC_FBDEV_DISPLAY_1;
    *pResolution = ITLGMC_FBDEV_RESOLUTION_1;
    *pBuffers = ITLGMC_FBDEV_BUFFERS_1;
    *pVsync = ITLGMC_FBDEV_VSYNC_1;
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

#ifdef INCLUDE_FBDEV_ITLGMC_2
/*******************************************************************************
*
* gfxItlGmcGetCfg2 - Get frame buffer configuration
*
* This routine returns the frame buffer configuration
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/
LOCAL STATUS gfxItlGmcGetCfg2
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
    *pDeviceName = ITLGMC_FBDEV_NAME_2;
    *pDisplayName = ITLGMC_FBDEV_DISPLAY_2;
    *pResolution = ITLGMC_FBDEV_RESOLUTION_2;
    *pBuffers = ITLGMC_FBDEV_BUFFERS_2;
    *pVsync = ITLGMC_FBDEV_VSYNC_2;
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

#ifdef INCLUDE_FBDEV_ITLGMC_3
/*******************************************************************************
*
* gfxItlGmcGetCfg3 - Get frame buffer configuration
*
* This routine returns the frame buffer configuration
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/
LOCAL STATUS gfxItlGmcGetCfg3
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
    *pDeviceName = ITLGMC_FBDEV_NAME_3;
    *pDisplayName = ITLGMC_FBDEV_DISPLAY_3;
    *pResolution = ITLGMC_FBDEV_RESOLUTION_3;
    *pBuffers = ITLGMC_FBDEV_BUFFERS_3;
    *pVsync = ITLGMC_FBDEV_VSYNC_3;
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

#ifdef INCLUDE_FBDEV_ITLGMC_4
/*******************************************************************************
*
* gfxItlGmcGetCfg4 - Get frame buffer configuration
*
* This routine returns the frame buffer configuration
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/
LOCAL STATUS gfxItlGmcGetCfg4
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
    *pDeviceName = ITLGMC_FBDEV_NAME_4;
    *pDisplayName = ITLGMC_FBDEV_DISPLAY_4;
    *pResolution = ITLGMC_FBDEV_RESOLUTION_4;
    *pBuffers = ITLGMC_FBDEV_BUFFERS_4;
    *pVsync = ITLGMC_FBDEV_VSYNC_4;
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

/*******************************************************************************
*
* gfxItlGmcInit0 - Frame buffer driver initialization
*
* This routine initializes the frame buffer driver
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void gfxItlGmcInit0
    (
    void
    )
    {
    GFX_PC_CONSOLE_CHECK;
    GFX_SMP_CHECK;
#if defined (INCLUDE_FBDEV_ITLGMC_0)
    gfxItlGmcInit (gfxItlGmcGetCfg0);
#elif defined (INCLUDE_FBDEV_ITLGMC_1)
    gfxItlGmcInit (gfxItlGmcGetCfg1);
#elif defined (INCLUDE_FBDEV_ITLGMC_2)
    gfxItlGmcInit (gfxItlGmcGetCfg2);
#elif defined (INCLUDE_FBDEV_ITLGMC_3)
    gfxItlGmcInit (gfxItlGmcGetCfg3);
#elif defined (INCLUDE_FBDEV_ITLGMC_4)
    gfxItlGmcInit (gfxItlGmcGetCfg4);
#endif
#if defined (INCLUDE_FBDEV_CONSOLE)
    gfxFbConsoleInit ();
#endif
    }

/*******************************************************************************
*
* gfxItlGmcPageFlipTimeout - get driver page flip timeout configuration
*
* This routine returns the configured driver page flip timeout
*
* RETURNS: driver page flip timeout configuration
*
* ERRNO: N/A
*
*/
unsigned int gfxItlGmcPageFlipTimeout
    (
    void
    )
    {
    return FBDEV_PAGE_FLIP_TIMEOUT;
    }

#if defined(INCLUDE_FBDEV_TASK)
/*******************************************************************************
*
* gfxItlGmcTaskPriority - get driver task priority configuration
*
* This routine returns the configured driver task priority
*
* RETURNS: driver task priority configuration
*
* ERRNO: N/A
*
*/
unsigned int gfxItlGmcTaskPriority
    (
    void
    )
    {
    return FBDEV_TASK_PRIORITY;
    }

/*******************************************************************************
*
* gfxItlGmcStackSize - get driver stack size configuration
*
* This routine returns the configured driver stack size
*
* RETURNS: driver stack size configuration
*
* ERRNO: N/A
*
*/
unsigned int gfxItlGmcStackSize
    (
    void
    )
    {
    return FBDEV_STACK_SIZE;
    }
#endif

/*******************************************************************************
*
* gfxItlGmcShowFuncPtr - get driver show function pointer
*
* This routine returns the driver show function pointer
*
* RETURNS: driver show function pointer
*
* ERRNO: N/A
*
*/
VOIDFUNCPTR gfxItlGmcShowFuncPtr
    (
    void
    )
    {
#if defined(INCLUDE_FBDEV_SHOW)
    extern void gfxItlGmcdShow (void *);

    return gfxItlGmcdShow;
#else
    return NULL;
#endif
    }

#endif /* __INCgfxItlGmcInitc */
