/* gfxSampleIosDrv.c - Sample frame buffer ios driver */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
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
#include <gfxFbIosDrv.inl>

/* locals */

/* List of video modes known to the driver */
LOCAL FB_VIDEO_MODE sampleHdmiFbModesDb[] =
    {
        {"1920x1080",
        60, 1920, 1080, 32, 4*1920,
        148500000, 88, 148, 4, 36, 44, 5,
        0,
        FB_VMODE_NONINTERLACED,
        0,
        0,
        3},
        {"1280x1024",
        60, 1280, 1024, 32, 4*1280,
        108000000, 48, 226, 15, 38, 112, 3,
        0,
        FB_VMODE_NONINTERLACED,
        0,
        0,
        3}
    };

LOCAL FB_VIDEO_MODE sampleLvdsFbModesDb[] =
    {
        {"1024x768",
        60, 1024, 768, 32, 4*1024,
        260000000, 220, 40, 21, 7, 60, 10,
        0,
        FB_VMODE_NONINTERLACED,
        0,
        0,
        3}
    };

LOCAL FB_VIDEO_MODE sampleLcdFbModesDb[] =
    {
        {"800x480",
        60, 800, 480, 32, 4*800,
        27300000, 40, 60, 10, 10, 20, 10,
        0,
        FB_VMODE_NONINTERLACED,
        0,
        0,
        3}
    };

/* forward declarations */

IMPORT STATUS gfxSampleHwInit (GFX_FBDEV*);
IMPORT STATUS gfxSampleSetVideoModeEx (GFX_FBDEV*, FB_VIDEO_MODE*);
IMPORT STATUS gfxSampleSetFbAddr (GFX_FBDEV*);
IMPORT void gfxSampleIntHandlerVsync (GFX_FBDEV*);

/*******************************************************************************
 *
 * gfxSampleInit - frame buffer driver initialization
 *
 * RETURNS: N/A
 *
 */
void gfxSampleInit
    (
    const char*     pSysModel,
    FUNCPTR         getCfgFuncPtr
    )
    {
    GFX_FBDEV*      pDev;

    pDev = drvInit(getCfgFuncPtr, 0);
    if (pDev == NULL)
        return;

    if (strstr (pSysModel, "Sample") != NULL)
        {
        if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_HDMI))
            {
            pDev->disp = GFX_DISP_SAMPLE_HDMI;
            pDev->fbModesDb = sampleHdmiFbModesDb;
            pDev->fbModesCount = NELEMENTS(sampleHdmiFbModesDb);
            pDev->setVideoModeExFuncPtr = gfxSampleSetVideoModeEx;
            pDev->setFbAddrFuncPtr = gfxSampleSetFbAddr;
#if defined(GFX_VSYNC_IRQ)
            pDev->intHandlerVsyncFuncPtr = gfxSampleIntHandlerVsync;
#endif
            }
        else if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_LVDS_PANEL))
            {
            pDev->disp = GFX_DISP_SAMPLE_LVDS_PANEL;
            pDev->fbModesDb = sampleLvdsFbModesDb;
            pDev->fbModesCount = NELEMENTS(sampleLvdsFbModesDb);
            pDev->setVideoModeExFuncPtr = gfxSampleSetVideoModeEx;
            pDev->setFbAddrFuncPtr = gfxSampleSetFbAddr;
            }
        else if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_FLAT_PANEL))
            {
            pDev->disp = GFX_DISP_SAMPLE_FLAT_PANEL;
            pDev->fbModesDb = sampleLcdFbModesDb;
            pDev->fbModesCount = NELEMENTS(sampleLcdFbModesDb);
            pDev->setVideoModeExFuncPtr = gfxSampleSetVideoModeEx;
            pDev->setFbAddrFuncPtr = gfxSampleSetFbAddr;
            }
        else
            {
            (void)fprintf (stderr, "Invalid display %s\n", pDev->displayName);
            drvCleanup (pDev);
            return;
            }
        if (ERROR == gfxSampleHwInit(pDev))
            {
            drvCleanup (pDev);
            return;
            }
        }
    else
        {
        (void)fprintf (stderr, "Invalid SYS_MODEL\n");
        drvCleanup (pDev);
        return;
        }

    if (ERROR == drvInstall ())
        {
        (void)fprintf (stderr, "Unable to install driver\n");
        drvCleanup (pDev);
        return;
        }

    if (ERROR == devAdd (pDev))
        {
        (void)fprintf (stderr, "Unable to add driver\n");
        drvCleanup (pDev);
        return;
        }

    pDev->enabled = TRUE;
    }
