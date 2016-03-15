/* gfxSampleDrv.h - Sample frame buffer driver header file */

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
22dec14,yat  Rework pmap of frame buffer for RTP restart (US46449)
24jan14,mgc  Modified for VxWorks 7 release
*/

#ifndef __INCgfxSampleDrvH
#define __INCgfxSampleDrvH

/* includes */

#include <vxWorks.h>
#include <ioLib.h>
#include <semLib.h>
#include <fbdev.h>

/* defines */


#undef GFX_USE_PMAP

/* typedefs */

/*
 * Define the display type even if there is only one
 */
typedef enum gfxDisp
    {
    GFX_DISP_SAMPLE_HDMI,
    GFX_DISP_SAMPLE_LVDS_PANEL,
    GFX_DISP_SAMPLE_FLAT_PANEL
    } GFX_DISP;

/*
 * Frame buffer display device
 *
 * This structure is required with the following fields,
 * additional field can be added for driver specific use
 */
typedef struct
    {
    DEV_HDR                         header;         /* I/O subsystem header */
    int                             enabled;
    /* Video mode */
    GFX_DISP                        disp;           /* display */
    FB_VIDEO_MODE                   fbMode;         /* video mode */
    FB_VIDEO_MODE*                  fbModesDb;
    ULONG                           fbModesCount;
    ULONG                           fbSize;
#if !defined(GFX_USE_PMAP)
    BOOL                            fbRtpFreeMem;
#endif
    /* Internal data */
    void*                           firstPhysAddr;  /* first buffer address */
    void*                           firstVirtAddr;  /* first buffer address */
    void*                           frontPhysAddr;  /* front buffer address */
    void*                           frontVirtAddr;  /* front buffer address */
#if defined(GFX_VSYNC_IRQ)
    BOOL                            needSwap;
    BOOL                            whenSwap;
    VX_BINARY_SEMAPHORE             (vsync);        /* VBL sync semaphore */
    BOOL                            needVsync;
    VOIDFUNCPTR                     intHandlerVsyncFuncPtr;
#endif
    FUNCPTR                         setVideoModeExFuncPtr;
    FUNCPTR                         setFbAddrFuncPtr;
    char                            deviceName[FB_MAX_STR_CHARS];
    char                            displayName[FB_MAX_STR_CHARS];
    char                            modeStr[FB_MAX_VIDEO_MODE_LEN];
    } GFX_FBDEV;

#endif /* __INCgfxSampleDrvH */
