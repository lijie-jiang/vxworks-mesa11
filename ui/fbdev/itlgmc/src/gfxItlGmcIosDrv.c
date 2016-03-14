/* gfxItlGmcDrv.c - Intel Graphics and Memory Controller frame buffer ios driver */

/*
 * Copyright (c) 2014-2016, Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
04feb16,yat  Remove system model check for Intel (US73271)
29jul15,rpc  Add modes for LVDS and DVI (US50700,US50615)
24jul15,rpc  Add DP modes for kontron target (US50700)
27mar15,qsn  Support VSYNC (US54417)
11mar15,yat  Add show and FBDEV task support (US55410)
02feb15,qsn  Allow boot time video resolution configuration (US53391)
22jan15,qsn  Updated display port video resolution (US50612)
20dec14,qsn  Initial VxWorks 7 release (US48907)
*/

/* includes */

#include <gfxItlGmcDrv.h>
#include <gfxFbIosDrv.inl>

/* locals */

/* List of video modes known to the driver */
LOCAL FB_VIDEO_MODE gfxItlGmcFbModesDb[] =
    {
        {"1920x1080", 60, 1920, 1080, 32, 4*1920,
        0, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, 0, 1, 3},
        {"1280x1024", 60, 1280, 1024, 32, 4*1280,
        0, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, 0, 1, 3},
        {"1024x768", 60, 1024, 768, 32, 4*1024,
        0, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, 0, 1, 3},
        {"640x480", 60, 640, 480, 32, 4*640,
        0, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, 0, 1, 3}
    };

LOCAL FB_VIDEO_MODE gfxItlGmcLVDSFbModesDb[] =
    {
        {"1024x768", 60, 1024, 768, 32, 4*1024,
        0, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, 0, 1, 3},
        {"640x480", 60, 640, 480, 32, 4*640,
        0, 0, 0, 0, 0, 0, 0, 0, FB_VMODE_NONINTERLACED, 0, 1, 3}
    };

#if !defined(GFX_VXBUS_GEN2)
LOCAL GFX_FBDEV *pDevShow = NULL;
#endif

/* forward declarations */

IMPORT void drm_fb_set_options(char*, char*);
IMPORT STATUS gfxItlGmcdInit (GFX_FBDEV*);
IMPORT STATUS gfxItlGmcSetVideoModeEx (GFX_FBDEV*, FB_VIDEO_MODE*);
IMPORT STATUS gfxItlGmcSetFbAddr (GFX_FBDEV*);
IMPORT VOIDFUNCPTR gfxItlGmcShowFuncPtr (void);

/*******************************************************************************
*
* gfxItlGmcInit - frame buffer driver initialization
*
* This routine initializes the frame buffer driver.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void gfxItlGmcInit
    (
    FUNCPTR         getCfgFuncPtr
    )
    {
    GFX_FBDEV*      pDev;
    FB_VIDEO_MODE   fbMode;

    pDev = drvInit(getCfgFuncPtr, 0);
    if (pDev == NULL)
        return;

    if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_VGA))
        {
        pDev->disp = GFX_DISP_ITL_VGA;
        pDev->fbModesDb = gfxItlGmcFbModesDb;
        pDev->fbModesCount = NELEMENTS(gfxItlGmcFbModesDb);
        }
    else if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_DP))
        {
        pDev->disp = GFX_DISP_ITL_DP;
        pDev->fbModesDb = gfxItlGmcFbModesDb;
        pDev->fbModesCount = NELEMENTS(gfxItlGmcFbModesDb);
        }
    else if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_HDMI))
        {
        pDev->disp = GFX_DISP_ITL_HDMI;
        pDev->fbModesDb = gfxItlGmcFbModesDb;
        pDev->fbModesCount = NELEMENTS(gfxItlGmcFbModesDb);
        }
    else if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_DVI))
        {
        pDev->disp = GFX_DISP_ITL_DVI;
        pDev->fbModesDb = gfxItlGmcFbModesDb;
        pDev->fbModesCount = NELEMENTS(gfxItlGmcFbModesDb);
        }
    else if (OK == gfxStrcmp (pDev->displayName, GFX_DISP_LVDS_PANEL))
        {
        pDev->disp = GFX_DISP_ITL_LVDS;
        pDev->fbModesDb = gfxItlGmcLVDSFbModesDb;
        pDev->fbModesCount = NELEMENTS(gfxItlGmcLVDSFbModesDb);
        }
    else
        {
        (void)fprintf (stderr, "Invalid display %s\n", pDev->displayName);
        drvCleanup (pDev);
        return;
        }
    pDev->setVideoModeExFuncPtr = gfxItlGmcSetVideoModeEx;
    pDev->setFbAddrFuncPtr = gfxItlGmcSetFbAddr;
    bzero ((void*)&fbMode, sizeof (FB_VIDEO_MODE));
    if (ERROR == getFbMode (pDev, pDev->modeStr, &fbMode))
        {
        drvCleanup (pDev);
        return;
        }
    drm_fb_set_options(pDev->displayName, fbMode.name);
    if (ERROR == gfxItlGmcdInit (pDev))
        {
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
#if !defined(GFX_VXBUS_GEN2)
    pDevShow = pDev;
#endif
    }

/*******************************************************************************
*
* gfxItlGmcShow - show driver information
*
* RETURNS: N/A
*
*/
void gfxItlGmcShow
    (
    void
    )
    {
#if defined(GFX_VXBUS_GEN2)
    GFX_FBDEV*      pDev = vxbDevSoftcGet(gfxComp.vxbDev);
#else
    GFX_FBDEV*      pDev = pDevShow;
#endif
    VOIDFUNCPTR     showFuncPtr = gfxItlGmcShowFuncPtr();

    if (pDev && showFuncPtr)
        {
        showFuncPtr (pDev);
        }
    }
