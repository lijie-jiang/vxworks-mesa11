/* gfxGbmHelper.inl - GBM helper routine */

/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01mar16,yat  Written (US76256)
*/

#ifndef _GFX_GBM_HELPER_H
#define _GFX_GBM_HELPER_H

/* includes */

#define GFX_KMS_GET_CONN_STR
#define GFX_KMS_FIND_CONN_CRTC
#define GFX_KMS_FIND_CRTC_FB
#include "gfxKmsHelper.inl"
#include <gbm.h>

/* typedefs */

typedef struct
    {
    int                  drmDevFd;
    struct gbm_device*   gbm_dev;
    struct gbm_surface*  gbm_surf;
    drmModeModeInfo      mode;
    uint32_t             connId;
    uint32_t             crtcId;
    uint32_t             fbId[2];
    int                  fbIndex;
    int                  setCrtc;
    struct gbm_bo*       bo_current;
    struct gbm_bo*       bo_next;
    } GFX_GBM;

/*******************************************************************************
*
* gfxGbmCreateDevice - open a DRM device and create a GBM device
*
* This routine opens a DRM device and create a GBM device.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxGbmDestroyDevice
*/

static int gfxGbmCreateDevice
    (
    GFX_GBM *pGbm
    )
    {
    /* Open a DRM device to use Mesa GPU DRI driver */
    if (gfxKmsOpenDrmDev (&(pGbm->drmDevFd)))
        {
        return 1;
        }

    /* Create a GBM device */
    pGbm->gbm_dev = gbm_create_device (pGbm->drmDevFd);
    if (pGbm->gbm_dev == NULL)
        {
        (void)fprintf (stderr, "gbm_create_device failed\n");
        return 1;
        }

    return 0;
    }

/*******************************************************************************
*
* gfxGbmDestroyDevice - destroy the GBM device
*
* This routine destroys the GBM device
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: gfxGbmCreateDevice
*/

static void gfxGbmDestroyDevice
    (
    GFX_GBM *pGbm
    )
    {
    if (pGbm->gbm_dev)
        {
        gbm_device_destroy (pGbm->gbm_dev);
        }

    gfxKmsCloseDrmDev (pGbm->drmDevFd);
    }

/*******************************************************************************
*
* gfxGbmFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxGbmFindDrmFb
    (
    GFX_GBM *pGbm
    )
    {
    drmModeCrtc*         crtc;
    drmModeFB*           fb;
    drmModeConnector*    conn;

    if (gfxKmsFindCrtcFb (pGbm->drmDevFd, &crtc, &fb))
        {
        (void)fprintf (stderr, "No crtc found\n");
        return 1;
        }

    if (gfxKmsFindConnCrtc (pGbm->drmDevFd, crtc, &conn))
        {
        (void)fprintf (stderr, "No connector found\n");
        drmModeFreeCrtc (crtc);
        return 1;
        }

    if (!fb)
        {
        /* Use the first available mode */
        crtc->mode = conn->modes[0];
        }

    (void)fprintf (stderr, "Found %s %s %dx%d\n",
                   gfxKmsGetConnModeStr(DRM_MODE_CONNECTED),
                   gfxKmsGetConnTypeStr (conn->connector_type),
                   crtc->mode.hdisplay, crtc->mode.vdisplay);

    pGbm->mode = crtc->mode;
    pGbm->connId = conn->connector_id;
    pGbm->crtcId = crtc->crtc_id;
    drmModeFreeCrtc (crtc);
    drmModeFreeConnector (conn);

    return 0;
    }

/*******************************************************************************
*
* gfxGbmCreateSurface - create a GBM surface
*
* This routine creates a GBM surface.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxGbmDestroySurface
*/

static int gfxGbmCreateSurface
    (
    GFX_GBM *pGbm
    )
    {
    pGbm->gbm_surf = gbm_surface_create (pGbm->gbm_dev,
                                         pGbm->mode.hdisplay,
                                         pGbm->mode.vdisplay,
                                         GBM_BO_FORMAT_XRGB8888,
                     GBM_BO_USE_RENDERING|GBM_BO_USE_SCANOUT);
   if (pGbm->gbm_surf == NULL)
        {
        (void)fprintf (stderr, "gbm_surface_create failed\n");
        return 1;
        }

   return 0;
   }

/*******************************************************************************
*
* gfxGbmDestroySurface - destroy the GBM surface
*
* This routine destroys the GBM surface
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: gfxGbmCreateSurface
*/

static void gfxGbmDestroySurface
    (
    GFX_GBM *pGbm
    )
    {
    if (pGbm->gbm_surf)
        {
        gbm_surface_destroy (pGbm->gbm_surf);
        }
    }

/*******************************************************************************
*
* gfxGbmRmFB - remove the frame buffer
*
* This routine removes the frame buffer
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: gfxGbmAddFB
*/

static void gfxGbmRmFB
    (
    GFX_GBM *pGbm
    )
    {
    if (pGbm->drmDevFd)
        {
        if (pGbm->fbId[0])
            {
            if (drmModeRmFB (pGbm->drmDevFd, pGbm->fbId[0]))
                {
                (void)fprintf (stderr, "drmModeRmFB failed\n");
                }
            else
                {
                pGbm->fbId[0] = 0;
                }
            }

        if (pGbm->fbId[1])
            {
            if (drmModeRmFB (pGbm->drmDevFd, pGbm->fbId[1]))
                {
                (void)fprintf (stderr, "drmModeRmFB failed\n");
                }
            else
                {
                pGbm->fbId[1] = 0;
                }
            }
        }
    }

/*******************************************************************************
*
* gfxGbmBoDestroyHandler - gbm_bo destroy handler
*
* This routine handles gbm_bo destroy
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static void gfxGbmBoDestroyHandler
    (
    struct gbm_bo*      bo,
    void*               data
    )
    {
    }

/*******************************************************************************
*
* gfxGbmPageFlipHandler - page flip handler
*
* This routine handles page flip
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: gfxGbmPageFlip
*/

static void gfxGbmPageFlipHandler
    (
    int                 fd,
    unsigned int        frame,
    unsigned int        sec,
    unsigned int        usec,
    void*               data
    )
    {
    GFX_GBM *pGbm = (GFX_GBM *)data;

    if (pGbm->bo_current)
        {
        gbm_surface_release_buffer (pGbm->gbm_surf, pGbm->bo_current);
        }

    pGbm->bo_current = pGbm->bo_next;
    pGbm->bo_next = NULL;
    }

/*******************************************************************************
*
* gfxGbmPageFlip - flip page
*
* This routine flips the page
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxGbmPageFlip
    (
    GFX_GBM *pGbm
    )
    {
    pGbm->fbIndex = (pGbm->fbIndex) ? 0 : 1;

    pGbm->bo_next = gbm_surface_lock_front_buffer (pGbm->gbm_surf);
    if (pGbm->bo_next == NULL)
        {
        (void)fprintf (stderr, "gbm_surface_lock_front_buffer failed\n");
        return 1;
        }

    if (!gbm_bo_get_user_data (pGbm->bo_next))
        {
        uint32_t handle;
        uint32_t stride;

        handle = gbm_bo_get_handle (pGbm->bo_next).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            gbm_surface_release_buffer (pGbm->gbm_surf, pGbm->bo_next);
            pGbm->bo_next = NULL;
            return 1;
            }

        stride = gbm_bo_get_stride (pGbm->bo_next);
        if (stride == 0)
            {
            (void)fprintf (stderr, "Bad stride\n");
            gbm_surface_release_buffer (pGbm->gbm_surf, pGbm->bo_next);
            pGbm->bo_next = NULL;
            return 1;
            }

        (void)fprintf (stdout, "width:%d height:%d handle:%d stride:%d\n", pGbm->mode.hdisplay, pGbm->mode.vdisplay, handle, stride);

        if (drmModeAddFB (pGbm->drmDevFd,
                          pGbm->mode.hdisplay, pGbm->mode.vdisplay,
                          24, 32, stride, handle, &(pGbm->fbId[pGbm->fbIndex])))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            gbm_surface_release_buffer (pGbm->gbm_surf, pGbm->bo_next);
            pGbm->bo_next = NULL;
            return 1;
            }

        gbm_bo_set_user_data (pGbm->bo_next, (void*)pGbm, gfxGbmBoDestroyHandler);
        }

    if (!pGbm->setCrtc)
        {
        if (drmModeSetCrtc (pGbm->drmDevFd, pGbm->crtcId, pGbm->fbId[pGbm->fbIndex],
                            0, 0, &(pGbm->connId), 1, &(pGbm->mode)))
            {
            (void)fprintf (stderr, "drmModeSetCrtc failed\n");
            gbm_surface_release_buffer (pGbm->gbm_surf, pGbm->bo_next);
            pGbm->bo_next = NULL;
            return 1;
            }

        pGbm->setCrtc = 1;
        }

    if (drmModePageFlip (pGbm->drmDevFd, pGbm->crtcId, pGbm->fbId[pGbm->fbIndex],
                         DRM_MODE_PAGE_FLIP_EVENT, (void*)pGbm))
        {
        (void)fprintf (stderr, "drmModePageFlip failed\n");
        gbm_surface_release_buffer (pGbm->gbm_surf, pGbm->bo_next);
        pGbm->bo_next = NULL;
        return 1;
        }

    while (pGbm->bo_next)
        {
        drmEventContext evt;
        evt.version = DRM_EVENT_CONTEXT_VERSION;
        evt.vblank_handler = NULL;
        evt.page_flip_handler = gfxGbmPageFlipHandler;
        if (drmHandleEvent (pGbm->drmDevFd, &evt))
            {
            static int print_count = 0;
            if (print_count < 3)
                {
                (void)fprintf (stderr, "drmHandleEvent errno:%08x\n", errno);
                print_count++;
                }
                break;
            }
        }

    return 0;
    }

#endif /* _GFX_GBM_HELPER_H */
