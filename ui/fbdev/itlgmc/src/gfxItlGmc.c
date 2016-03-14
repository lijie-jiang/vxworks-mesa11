/* gfxItlGmc.c - Intel Graphics and Memory Controller graphics driver */

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
08jan16,qsn  Add delay after crtc connection in setting video mode (US72757)
14sep15,yat  Use GEM to create framebuffer (US66034)
31mar15,rpc  Static analysis fixes (US50633) crtc init as -1
27mar15,rpc  Static analysis fixes (US50633)
27mar15,qsn  Support VSYNC (US54417)
25feb15,tlu  Enable LVDS/VGA displays (US50700, US50616)
11mar15,yat  Add show and FBDEV task support (US55410)
16feb15,qsn  Support RTP mode (US50617)
22jan15,qsn  Make this code initiate I915 driver initialization (US50612)
06jan15,qsn  Modified for itl_64_vx7 bsp (US50612)
20dec14,qsn  Initial VxWorks 7 release (US48907)
*/

/* includes */

#include <gfxItlGmcDrv.h>
#define GFX_KMS_GET_CONN_STR
#define GFX_KMS_FIND_CONN_TYPE
#define GFX_KMS_FIND_CONN
#define GFX_KMS_FIND_CRTC_CONN
#define GFX_KMS_CREATE_DRM_FB
#define GFX_KMS_DESTROY_DRM_FB
#include <gfxKmsHelper.inl>
#include <timerDev.h> /* for sysClkRateGet */

/* forward declarations */

IMPORT int gfxDrmDevInit (void);
IMPORT void gfxDrmDevDeinit (void);
IMPORT unsigned int gfxItlGmcPageFlipTimeout (void);
IMPORT unsigned int gfxItlGmcTaskPriority (void);
IMPORT unsigned int gfxItlGmcStackSize (void);
IMPORT VOIDFUNCPTR gfxItlGmcShowFuncPtr (void);

/*******************************************************************************
*
* gfxItlGmcCmdSetVideoModeEx - set video mode
*
* This routine is executed by the created task to set video mode.
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/

LOCAL STATUS gfxItlGmcCmdSetVideoModeEx
    (
    GFX_FBDEV*      pDev,
    FB_VIDEO_MODE*  pFbMode
    )
    {
    drmModeModeInfo mode;
    uint32_t        connId;
    uint32_t        crtcId;
    uint64_t        fbSize;
    uint32_t        firstFbId = 0;
    uint32_t        secondFbId = 0;
    uint32_t        thirdFbId = 0;
    uint32_t        firstHandle = 0;
    uint32_t        secondHandle = 0;
    uint32_t        thirdHandle = 0;
    void*           secondVirtAddr = NULL;
    void*           secondPhysAddr;
    void*           thirdVirtAddr = NULL;
    void*           thirdPhysAddr;
    drmModeCrtc*    crtc;
    drmModeConnector* conn;

    if (gfxKmsFindConnDisp (pDev->drmDevFd, pDev->disp,
                            pFbMode->xres, pFbMode->yres, &conn, &mode))
        {
        (void)fprintf (stderr, "No %s %s found\n",
                       gfxKmsGetConnModeStr(DRM_MODE_CONNECTED),
                       gfxKmsGetConnTypeStr (pDev->disp));
        if (!gfxKmsFindConn (pDev->drmDevFd, &conn, &mode))
            {
            (void)fprintf (stderr, "Found %s %s %dx%d\nplease reconfigure the display device in the kernel\n",
                           gfxKmsGetConnModeStr(DRM_MODE_CONNECTED),
                           gfxKmsGetConnTypeStr (conn->connector_type),
                           mode.hdisplay, mode.vdisplay);
            drmModeFreeConnector (conn);
            }
        return ERROR;
        }

    connId = conn->connector_id;

    if (gfxKmsFindCrtcConn (pDev->drmDevFd, conn, &crtc))
        {
        (void)fprintf (stderr, "No crtc found\n");
        drmModeFreeConnector (conn);
        return ERROR;
        }

    crtcId = crtc->crtc_id;
    drmModeFreeCrtc (crtc);
    drmModeFreeConnector (conn);
    fbSize = pFbMode->stride * pFbMode->yres;

    /* create first buffer */
    if (gfxKmsCreateDrmFb (pDev->drmDevFd,
                           pFbMode->xres, pFbMode->yres,
                           &firstFbId, &firstHandle, fbSize,
                           pDev->firstVirtAddr, pDev->firstPhysAddr))
        {
        goto ErrOut;
        }

    /* create second buffer */
    if (pDev->fbMode.buffers > 1)
        {
        secondVirtAddr = pDev->firstVirtAddr + fbSize;
        secondPhysAddr = pDev->firstPhysAddr + fbSize;
        if (gfxKmsCreateDrmFb (pDev->drmDevFd,
                               pFbMode->xres, pFbMode->yres,
                               &secondFbId, &secondHandle, fbSize,
                               secondVirtAddr, secondPhysAddr))
            {
            goto ErrOut;
            }
        }

    /* create third buffer */
    if (pDev->fbMode.buffers > 2)
        {
        thirdVirtAddr = secondVirtAddr + fbSize;
        thirdPhysAddr = secondPhysAddr + fbSize;
        if (gfxKmsCreateDrmFb (pDev->drmDevFd,
                               pFbMode->xres, pFbMode->yres,
                               &thirdFbId, &thirdHandle, fbSize,
                               thirdVirtAddr, thirdPhysAddr))
            {
            goto ErrOut;
            }
        }

    /* connect first framebuffer to crtc */
    if (drmModeSetCrtc (pDev->drmDevFd, crtcId, firstFbId,
                        0, 0, &connId, 1, &mode))
        {
        (void)fprintf (stderr, "Failed to set DRM CRTC.\n");
        goto ErrOut;
        }

    /* delay for 100 msecs after crtc connection */
    (void)taskDelay ((100 * sysClkRateGet ()) / 1000);

    /* destroy previous framebuffer(s) */
    gfxKmsDestroyDrmFb (pDev->drmDevFd, pDev->firstFbId, pDev->firstHandle);
    gfxKmsDestroyDrmFb (pDev->drmDevFd, pDev->secondFbId, pDev->secondHandle);
    gfxKmsDestroyDrmFb (pDev->drmDevFd, pDev->thirdFbId, pDev->thirdHandle);

    /* store newly-created framebuffer info */
    pDev->mode          = mode;
    pDev->connId        = connId;
    pDev->crtcId        = crtcId;
    pDev->firstFbId     = firstFbId;
    pDev->firstHandle   = firstHandle;
    if (pDev->fbMode.buffers > 1)
        {
        pDev->secondVirtAddr = secondVirtAddr;
        pDev->secondFbId     = secondFbId;
        pDev->secondHandle   = secondHandle;
        }
    if (pDev->fbMode.buffers > 2)
        {
        pDev->thirdVirtAddr = thirdVirtAddr;
        pDev->thirdFbId     = thirdFbId;
        pDev->thirdHandle   = thirdHandle;
        }

    (void)drmDropMaster (pDev->drmDevFd);
    pDev->isMaster = FALSE;

    if (vxAtomic32Get(&pDev->isVideoModeSet) == FALSE)
        {
        (void)vxAtomic32Set (&pDev->isVideoModeSet, TRUE);
        }

    return OK;

ErrOut:
    gfxKmsDestroyDrmFb (pDev->drmDevFd, thirdFbId, thirdHandle);
    gfxKmsDestroyDrmFb (pDev->drmDevFd, secondFbId, secondHandle);
    gfxKmsDestroyDrmFb (pDev->drmDevFd, firstFbId, firstHandle);

    return ERROR;
    }

/*******************************************************************************
*
* gfxItlGmcSetVideoModeEx - set video mode
*
* This routine sets video mode.
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/

STATUS gfxItlGmcSetVideoModeEx
    (
    GFX_FBDEV*      pDev,
    FB_VIDEO_MODE*  pFbMode
    )
    {
    STATUS ret;
#if defined(GFX_USE_FBDEV_TASK)
    /* set parameters for this command */
    pDev->cmdData = (void *)pFbMode;
    pDev->cmdType = GFX_TASK_SET_VIDEO_MODE;

    /* inform that command is ready */
    (void)semGive (pDev->cmdAvail);

    /* wait for the result */
    (void)semTake (pDev->retAvail, WAIT_FOREVER);
    ret = pDev->retStat;
#else
    ret = gfxItlGmcCmdSetVideoModeEx(pDev, pFbMode);
#endif
    return ret;
    }

/*******************************************************************************
*
* gfxItlGmcCmdSetFbAddr - set the frame buffer address
*
* This routine is executed by the created task to set framebuffer address.
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/

LOCAL STATUS gfxItlGmcCmdSetFbAddr
    (
    GFX_FBDEV*      pDev
    )
    {
    uint32_t   fbId;

    /* determine the correct framebuffer */
    if (pDev->frontVirtAddr == pDev->firstVirtAddr)
        {
        fbId = pDev->firstFbId;
        }
    else if (pDev->frontVirtAddr == pDev->secondVirtAddr)
        {
        fbId = pDev->secondFbId;
        }
    else if (pDev->frontVirtAddr == pDev->thirdVirtAddr)
        {
        fbId = pDev->thirdFbId;
        }
    else
        {
        (void)fprintf (stderr, "Invalid framebuffer address.\n");
        return ERROR;
        }

    if (gfxKmsPageFlip (pDev->drmDevFd, pDev->crtcId, fbId,
                        &pDev->connId, &pDev->mode,
                        pDev->pageFlipTimeout, pDev->fbMode.vsync))
        {
        pDev->isMaster = FALSE;
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* gfxItlGmcSetFbAddr - set the frame buffer address
*
* This routine sets the address of the framebuffer.
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/

STATUS gfxItlGmcSetFbAddr
    (
    GFX_FBDEV*      pDev
    )
    {
    STATUS ret;

#if defined(GFX_USE_FBDEV_TASK)
    pDev->cmdType = GFX_TASK_SET_FB_ADDR;

    /* inform that command is ready */
    (void)semGive (pDev->cmdAvail);

    /* wait for the result */
    (void)semTake (pDev->retAvail, WAIT_FOREVER);
    ret = pDev->retStat;
#else
    ret = gfxItlGmcCmdSetFbAddr(pDev);
#endif
    return ret;
    }

/*******************************************************************************
*
* gfxItlGmcOpen - open DRM device driver
*
* This routine opens the DRM device driver.
*
* RETURNS: OK on success, ERROR otherwise.
*
* ERRNO: N/A
*/

STATUS gfxItlGmcOpen
    (
    GFX_FBDEV*      pDev
    )
    {
    if (pDev->drmDevFd > 0)
        {
        return ERROR;
        }

    /* open DRM device driver */
#if defined(GFX_USE_FBDEV_TASK)
    pDev->cmdType = GFX_TASK_OPEN;

    /* allow the created task to take over the job */
    (void)semGive (pDev->cmdAvail);

    /* wait for DRM device driver to be opened */
    (void)semTake (pDev->retAvail, WAIT_FOREVER);
#else
    (void)gfxKmsOpenDrmDev (&(pDev->drmDevFd));
#endif
    if (pDev->drmDevFd < 0)
        {
        return ERROR;
        }

    return OK;
    }

#if defined(GFX_USE_FBDEV_TASK)
/*******************************************************************************
*
* gfxItlGmcTask - task to perform DRM device driver-related functions
*
* This task performs DRM device driver-related functions.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void gfxItlGmcTask
    (
    GFX_FBDEV*      pDev
    )
    {
    /* wait until the first video mode is set */
    while (vxAtomic32Get(&pDev->isVideoModeSet) == FALSE)
        {
        if (OK == semBTake (pDev->cmdAvail, WAIT_FOREVER))
            {
            if (pDev->cmdType == GFX_TASK_OPEN)
                {
                /* open DRM device driver */
                pDev->retStat = gfxKmsOpenDrmDev (&(pDev->drmDevFd));
                }
            else if (pDev->cmdType == GFX_TASK_SET_VIDEO_MODE)
                {
                /* set video mode */
                pDev->retStat = gfxItlGmcCmdSetVideoModeEx(pDev, pDev->cmdData);
                }
            else
                {
                pDev->retStat = ERROR;
                }
            (void)semGive (pDev->retAvail);
            }
        }

    for (;;)
        {
        /* 
         * wait for setting FB address or setting video mode.
         */
        if (OK == semBTake (pDev->cmdAvail, WAIT_FOREVER))
            {
            if (pDev->isMaster == FALSE)
                {
                if (!drmSetMaster (pDev->drmDevFd))
                    {
                    pDev->isMaster = TRUE;
                    }
                }

            switch (pDev->cmdType)
                {
                case GFX_TASK_SET_VIDEO_MODE:
                    /* set video mode */
                    pDev->retStat = gfxItlGmcCmdSetVideoModeEx(pDev, pDev->cmdData);
                    break;
                case GFX_TASK_SET_FB_ADDR:
                    /* set frame buffer address */
                    pDev->retStat = gfxItlGmcCmdSetFbAddr(pDev);
                    break;
                default:
                    pDev->retStat = ERROR;
                    break;
                }
            (void)semGive (pDev->retAvail);
            }
        }
    }
#endif

/*******************************************************************************
*
* gfxItlGmcdInit - initialize Intel graphics driver
*
* This routine initializes the Intel graphics driver.
*
* RETURNS: OK on success, ERROR otherwise
*
* ERRNO: N/A
*/

STATUS gfxItlGmcdInit
    (
    GFX_FBDEV*      pDev
    )
    {
    /* initialize DRM device driver */
    if (gfxDrmDevInit() == ERROR)
        {
        (void)fprintf (stderr, "DRM device driver initialization failed\n");
        return ERROR;
        }

    pDev->pageFlipTimeout = gfxItlGmcPageFlipTimeout ();

#if defined(GFX_USE_FBDEV_TASK)
    /* create semaphores */
    pDev->cmdAvail = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    pDev->retAvail = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    if ((pDev->cmdAvail == SEM_ID_NULL) ||
        (pDev->retAvail == SEM_ID_NULL))
        {
        (void)fprintf (stderr, "Failed to create semaphore(s).\n");
        goto ErrOutGmcdInit;
        }

    /* create and run a task to do actual fbdev (DRM device related) work */
    pDev->taskId = taskSpawn("tGfxFbTask",
                    gfxItlGmcTaskPriority(),
                    (VX_SUPERVISOR_MODE | VX_FP_TASK),
                    gfxItlGmcStackSize(),
                    (FUNCPTR)gfxItlGmcTask,
                    (_Vx_usr_arg_t)pDev, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (pDev->taskId == TASK_ID_ERROR)
        {
        (void)fprintf (stderr, "Failed to create fbdev task.\n");
        goto ErrOutGmcdInit;
        }
#endif

    (void)vxAtomic32Set (&pDev->isVideoModeSet, FALSE);

    if (gfxItlGmcOpen(pDev) == OK)
        {
        /* everything is OK now */
        return OK;
        }

    /* initialization failed */
#if defined(GFX_USE_FBDEV_TASK)
    /* delete the created task */
    (void)taskDelete(pDev->taskId);

ErrOutGmcdInit:
    /* delete created semaphore(s) */
    if (pDev->retAvail != NULL)
        {
        (void)semDelete(pDev->retAvail);
        }
    if (pDev->cmdAvail != NULL)
        {
        (void)semDelete(pDev->cmdAvail);
        }
#endif
    gfxDrmDevDeinit();
    gfxKmsDestroyDrmFb (pDev->drmDevFd, pDev->firstFbId, pDev->firstHandle);
    gfxKmsDestroyDrmFb (pDev->drmDevFd, pDev->secondFbId, pDev->secondHandle);
    gfxKmsDestroyDrmFb (pDev->drmDevFd, pDev->thirdFbId, pDev->thirdHandle);
    gfxKmsCloseDrmDev (pDev->drmDevFd);

    return ERROR;
    }
