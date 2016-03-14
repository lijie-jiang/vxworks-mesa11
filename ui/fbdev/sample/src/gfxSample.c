/* gfxSampleIosDrv.c - Sample frame buffer driver */

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
01apr15,yat  Resolve defects from static analysis run (US50633)
24jan14,mgc  Modified for VxWorks 7 release
*/

/* includes */

#include <gfxSampleDrv.h>

/*******************************************************************************
 *
 * gfxSampleHwInit - initialize the hardware
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
STATUS gfxSampleHwInit
    (
    GFX_FBDEV*      pDev
    )
    {
    (void)printf ("Sample driver for %s: initialize hardware\n",
           pDev->deviceName);
    return OK;
    }

/*******************************************************************************
 *
 * gfxSampleSetVideoModeEx - set the display mode
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
STATUS gfxSampleSetVideoModeEx
    (
    GFX_FBDEV*      pDev,
    FB_VIDEO_MODE*  pFbMode
    )
    {
    (void)printf ("Sample driver for %s: set video mode to %s\n",
           pDev->deviceName, pDev->modeStr);
    return OK;
    }

/*******************************************************************************
 *
 * gfxSampleSetFbAddr - set frame buffer address
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
STATUS gfxSampleSetFbAddr
    (
    GFX_FBDEV*      pDev
    )
    {
    static int count = 0;

    if (++count < 5)
        (void)printf ("Sample driver for %s: set frame buffer address to %p\n",
               pDev->deviceName, (pDev->frontVirtAddr));
    return OK;
    }

#if defined(GFX_VSYNC_IRQ)
/*******************************************************************************
 *
 * gfxSampleIntHandlerVsync - vsync interrupt handler
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
void gfxSampleIntHandlerVsync
    (
    GFX_FBDEV*      pDev
    )
    {
    /* Swap frame buffer as needed */
    if (pDev->needSwap)
        {
        pDev->whenSwap--;
        if (pDev->whenSwap <= 0)
            {
            pDev->needSwap = FALSE;
            pDev->setFbAddrFuncPtr (pDev);
            (void)semGive ((SEM_ID)pDev->vsync);
            }
        }
    else if (pDev->needVsync)
        {
        pDev->needVsync = FALSE;
        (void)semGive ((SEM_ID)pDev->vsync);
        }
    }
#endif
