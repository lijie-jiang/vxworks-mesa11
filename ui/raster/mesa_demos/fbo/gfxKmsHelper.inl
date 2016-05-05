/* gfxKmsHelper.inl - KMS helper routine */

/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01mar16,yat  Add GFX_KMS_PAGE_FLIP (US76256)
08jan16,qsn  Fix missing framebuffer problem (US72757)
20oct15,qsn  Add support for select (US68597)
09oct15,yat  Written
*/

#ifndef _GFX_KMS_HELPER_H
#define _GFX_KMS_HELPER_H

/* includes */

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <i915_drm.h>
#include <time.h>
#if defined(__vxworks)
#include <string.h>
#include <strings.h>
#include <selectLib.h>
#else
#include <sys/select.h>
#endif /* __vxworks */

/* defines */

#ifndef DRM_MAX_STR_CHARS
#define DRM_MAX_STR_CHARS 128
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC    1000L
#endif

#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC   1000L
#endif

#ifndef NSEC_PER_MSEC
#define NSEC_PER_MSEC   1000000L
#endif

#if defined(GFX_KMS_GET_CONN_STR)
/*******************************************************************************
*
* gfxKmsGetConnTypeStr - get display connector type string
*
* This routine gets the display connector type string.
*
* RETURNS: display connector type string
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsGetConnModeStr
*/

static const char *gfxKmsGetConnTypeStr
    (
    uint32_t connector_type
    )
    {
    switch (connector_type)
        {
        case DRM_MODE_CONNECTOR_VGA:
            return "VGA";
        case DRM_MODE_CONNECTOR_DVII:
            return "DVI";
        case DRM_MODE_CONNECTOR_DVID:
            return "DVI";
        case DRM_MODE_CONNECTOR_DVIA:
            return "DVI";
        case DRM_MODE_CONNECTOR_Composite:
            return "composite";
        case DRM_MODE_CONNECTOR_SVIDEO:
            return "s-video";
        case DRM_MODE_CONNECTOR_LVDS:
            return "LVDS panel";
        case DRM_MODE_CONNECTOR_Component:
            return "component";
        case DRM_MODE_CONNECTOR_9PinDIN:
            return "9-pin DIN";
        case DRM_MODE_CONNECTOR_DisplayPort:
            return "DP";
        case DRM_MODE_CONNECTOR_HDMIA:
            return "HDMI";
        case DRM_MODE_CONNECTOR_HDMIB:
            return "HDMI";
        case DRM_MODE_CONNECTOR_TV:
            return "TV";
        case DRM_MODE_CONNECTOR_eDP:
            return "eDP";
        default:
            return "unknown";
        }
    }

/*******************************************************************************
*
* gfxKmsGetConnModeStr - get display connector mode string
*
* This routine gets the display connector mode string.
*
* RETURNS: display connector mode string
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsGetConnTypeStr
*/

static const char *gfxKmsGetConnModeStr
    (
    drmModeConnection connection
    )
    {
    switch (connection)
        {
        case DRM_MODE_CONNECTED:
            return "connected";
        case DRM_MODE_DISCONNECTED:
            return "disconnected";
        default:
            return "unknown";
        }
    }
#endif /* GFX_KMS_GET_CONN_STR */

#if defined(GFX_KMS_FIND_CONN_TYPE)
/*******************************************************************************
*
* gfxKmsFindConnType - find matching display connector type
*
* This routine finds the matching display connector type.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsFindConnDisp
*/

static int gfxKmsFindConnType
    (
    uint32_t connector_type,
    uint32_t disp
    )
    {
    switch (connector_type)
        {
        case DRM_MODE_CONNECTOR_VGA:
            if (disp == DRM_MODE_CONNECTOR_VGA) return 0;
            return 1;
        case DRM_MODE_CONNECTOR_DVII:
        case DRM_MODE_CONNECTOR_DVID:
        case DRM_MODE_CONNECTOR_DVIA:
            if (disp == DRM_MODE_CONNECTOR_DVII) return 0;
            if (disp == DRM_MODE_CONNECTOR_DVID) return 0;
            if (disp == DRM_MODE_CONNECTOR_DVIA) return 0;
            return 1;
        case DRM_MODE_CONNECTOR_Composite:
            return 1;
        case DRM_MODE_CONNECTOR_SVIDEO:
            return 1;
        case DRM_MODE_CONNECTOR_LVDS:
            if (disp == DRM_MODE_CONNECTOR_LVDS) return 0;
            return 1;
        case DRM_MODE_CONNECTOR_Component:
            return 1;
        case DRM_MODE_CONNECTOR_9PinDIN:
            return 1;
        case DRM_MODE_CONNECTOR_DisplayPort:
        case DRM_MODE_CONNECTOR_eDP:
            if (disp == DRM_MODE_CONNECTOR_DisplayPort) return 0;
            if (disp == DRM_MODE_CONNECTOR_eDP) return 0;
            return 1;
        case DRM_MODE_CONNECTOR_HDMIA:
        case DRM_MODE_CONNECTOR_HDMIB:
            if (disp == DRM_MODE_CONNECTOR_HDMIA) return 0;
            if (disp == DRM_MODE_CONNECTOR_HDMIB) return 0;
            return 1;
        case DRM_MODE_CONNECTOR_TV:
            return 1;
        default:
            return 1;
        }
    }

/*******************************************************************************
*
* gfxKmsFindConnDisp - find display connector
*
* This routine finds the display connector.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsFindConnType
*/

static int gfxKmsFindConnDisp
    (
    int                 fd,
    uint32_t            disp,
    uint32_t            width,
    uint32_t            height,
    drmModeConnector**  pConn,
    drmModeModeInfo*    pMode
    )
    {
    drmModeRes*         res;
    unsigned int        i;

    if (!pConn)
        {
        (void)fprintf (stderr, "Bad connector\n");
        return 1;
        }

    res = drmModeGetResources (fd);
    if (!res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM resources\n");
        return 1;
        }

    for (i = 0; i < res->count_connectors; i++)
        {
        *pConn = drmModeGetConnector (fd, res->connectors[i]);
        if (!(*pConn))
            {
            continue;
            }

        if (((*pConn)->connection == DRM_MODE_CONNECTED) &&
            ((*pConn)->count_modes > 0) &&
            (!gfxKmsFindConnType ((*pConn)->connector_type, disp)))
            {
            unsigned int j;

            for (j = 0; j < (*pConn)->count_modes; j++)
                {
                if (((*pConn)->modes[j].hdisplay == width) &&
                    ((*pConn)->modes[j].vdisplay == height))
                    {
                    *pMode = (*pConn)->modes[j];
                    drmModeFreeResources (res);
                    return 0;
                    }
                }
            }

        drmModeFreeConnector (*pConn);
        }

    *pConn = NULL;
    pMode = NULL;
    drmModeFreeResources (res);
    return 1;
    }
#endif /* GFX_KMS_FIND_CONN_TYPE */

#if defined(GFX_KMS_FIND_CONN)
/*******************************************************************************
*
* gfxKmsFindConn - find any connected display connector
*
* This routine finds any connected display connector.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxKmsFindConn
    (
    int                 fd,
    drmModeConnector**  pConn,
    drmModeModeInfo*    pMode
    )
    {
    drmModeRes*         res;
    unsigned int        i;

    if (!pConn)
        {
        (void)fprintf (stderr, "Bad connector\n");
        return 1;
        }

    res = drmModeGetResources (fd);
    if (!res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM resources\n");
        return 1;
        }

    for (i = 0; i < res->count_connectors; i++)
        {
        *pConn = drmModeGetConnector (fd, res->connectors[i]);
        if (!(*pConn))
            {
            continue;
            }

        if (((*pConn)->connection == DRM_MODE_CONNECTED) &&
            ((*pConn)->count_modes > 0))
            {
            *pMode = (*pConn)->modes[0];
            drmModeFreeResources (res);
            return 0;
            }

        drmModeFreeConnector (*pConn);
        }

    *pConn = NULL;
    pMode = NULL;
    drmModeFreeResources (res);
    return 1;
    }
#endif /* GFX_KMS_FIND_CONN */

#if defined(GFX_KMS_FIND_CRTC_CONN)
/*******************************************************************************
*
* gfxKmsFindCrtcConn - find display crtc based on connector
*
* This routine finds the display crtc based on connector.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxKmsFindCrtcConn
    (
    int                 fd,
    drmModeConnector*   conn,
    drmModeCrtc**       pCrtc
    )
    {
    drmModeRes*         res;
    drmModeEncoder*     enc;
    unsigned int        i;

    if (!conn)
        {
        (void)fprintf (stderr, "Bad connector\n");
        return 1;
        }

    if (!pCrtc)
        {
        (void)fprintf (stderr, "Bad crtc\n");
        return 1;
        }

    if (conn->encoder_id)
        {
        enc = drmModeGetEncoder (fd, conn->encoder_id);
        if (enc && (enc->crtc_id))
            {
            *pCrtc = drmModeGetCrtc (fd, enc->crtc_id);
            if (*pCrtc)
                {
                drmModeFreeEncoder (enc);
                return 0;
                }
            }

        drmModeFreeEncoder (enc);
        }

    res = drmModeGetResources (fd);
    if (!res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM resources\n");
        return 1;
        }

    for (i = 0; i < res->count_encoders; i++)
        {
        unsigned int j;

        enc = drmModeGetEncoder (fd, res->encoders[i]);
        if (!enc)
            {
            continue;
            }

        for (j = 0; j < res->count_crtcs; j++)
            {
            if (!(enc->possible_crtcs & (1 << j)))
                {
                continue;
                }

            if (res->crtcs[j])
                {
                *pCrtc = drmModeGetCrtc (fd, res->crtcs[j]);
                if (*pCrtc)
                    {
                    drmModeFreeEncoder (enc);
                    drmModeFreeResources (res);
                    return 0;
                    }
                }
            }

        drmModeFreeEncoder (enc);
        }

    *pCrtc = NULL;
    drmModeFreeResources (res);
    return 1;
    }
#endif /* GFX_KMS_FIND_CRTC_CONN */

#if defined(GFX_KMS_FIND_CRTC_FB)
/*******************************************************************************
*
* gfxKmsFindCrtcFb - find display crtc and framebuffer
*
* This routine finds the display crtc and framebuffer.
*
* RETURNS: 0 if successful found crtc, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxKmsFindCrtcFb
    (
    int                 fd,
    drmModeCrtc**       pCrtc,
    drmModeFB**         pFb
    )
    {
    drmModeRes*         res;
    unsigned int        i;

    if (!pCrtc)
        {
        (void)fprintf (stderr, "Bad crtc\n");
        return 1;
        }

    if (!pFb)
        {
        (void)fprintf (stderr, "Bad fb\n");
        return 1;
        }

    res = drmModeGetResources (fd);
    if (!res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM resources\n");
        return 1;
        }

    for (i = 0; i < res->count_crtcs; i++)
        {
        *pCrtc = drmModeGetCrtc (fd, res->crtcs[i]);
        if (!(*pCrtc))
            {
            continue;
            }

        *pFb = drmModeGetFB (fd, (*pCrtc)->buffer_id);

        drmModeFreeResources (res);
        return 0;
        }

    *pCrtc = NULL;
    *pFb = NULL;
    drmModeFreeResources (res);
    return 1;
    }
#endif /* GFX_KMS_FIND_CRTC_FB */

#if defined(GFX_KMS_FIND_CONN_CRTC)
/*******************************************************************************
*
* gfxKmsFindConnCrtc - find display connector based on crtc
*
* This routine finds the display connector based on crtc.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxKmsFindConnCrtc
    (
    int                 fd,
    drmModeCrtc*        crtc,
    drmModeConnector**  pConn
    )
    {
    drmModeRes*         res;
    drmModeEncoder*     enc;
    unsigned int        i;

    if (!pConn)
        {
        (void)fprintf (stderr, "Bad connector\n");
        return 1;
        }

    res = drmModeGetResources (fd);
    if (!res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM resources\n");
        return 1;
        }

    for (i = 0; i < res->count_connectors; i++)
        {
        *pConn = drmModeGetConnector (fd, res->connectors[i]);
        if (!(*pConn))
            {
            continue;
            }

        if (((*pConn)->connection == DRM_MODE_CONNECTED) &&
            ((*pConn)->count_modes > 0) &&
             (*pConn)->encoder_id)
            {
            enc = drmModeGetEncoder (fd, (*pConn)->encoder_id);
            if (enc && (enc->crtc_id == crtc->crtc_id))
                {
                drmModeFreeEncoder (enc);
                drmModeFreeResources (res);
                return 0;
                }

            drmModeFreeEncoder (enc);
            }

        drmModeFreeConnector (*pConn);
        }

    *pConn = NULL;
    drmModeFreeResources (res);
    return 1;
    }
#endif /* GFX_KMS_FIND_CONN_CRTC */

/*******************************************************************************
*
* gfxKmsOpenDrmDev - open DRM device driver
*
* This routine opens the DRM device driver.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsCloseDrmDev
*/

static int gfxKmsOpenDrmDev
    (
    int*                pFd
    )
    {
    char deviceName[DRM_MAX_STR_CHARS];
    unsigned int i;

    for (i = 0; i < DRM_MAX_MINOR; i++)
        {
        (void)snprintf (deviceName, DRM_MAX_STR_CHARS, DRM_DEV_NAME, DRM_DIR_NAME, i);
        if ((*pFd = open (deviceName, O_RDWR, 0666)) != -1)
            {
            break;
            }
        }
    if (i >= DRM_MAX_MINOR)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", deviceName, errno);
        return 1;
        }

    if (drmSetMaster (*pFd))
        {
        (void)fprintf (stderr, "drmSetMaster errno:%08x\n", errno);
        return 1;
        }

    return 0;
    }

/*******************************************************************************
*
* gfxKmsCloseDrmDev - close DRM device driver
*
* This routine closes the DRM device driver.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsOpenDrmDev
*/

static void gfxKmsCloseDrmDev
    (
    int                 fd
    )
    {
    if (fd > 0)
        {
        (void)drmDropMaster (fd);
        (void)close (fd);
        }
    }

#if defined(GFX_KMS_DESTROY_DRM_FB)
/*******************************************************************************
*
* gfxKmsDestroyDrmFb - destroy a DRM framebuffer
*
* This routine destroys a DRM framebuffer and frees relevant resources.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static void gfxKmsDestroyDrmFb
    (
    int                 fd,
    uint32_t            fbId,
    uint32_t            handle
    )
    {
    if (fbId)
        {
        if (drmModeRmFB(fd, fbId))
            {
            (void)fprintf (stderr, "Failed to destroy framebuffer\n");
            }
        }

    if (handle)
        {
        struct drm_gem_close close_bo;

        close_bo.handle = handle;
        if (drmIoctl(fd, DRM_IOCTL_GEM_CLOSE, &close_bo))
            {
            (void)fprintf (stderr, "DRM_IOCTL_GEM_CLOSE error\n");
            }
        }
    }
#endif /* GFX_KMS_DESTROY_DRM_FB */

#if defined(GFX_KMS_OPEN_DRM_FB)
/*******************************************************************************
*
* gfxKmsOpenDrmFb - open a DRM framebuffer
*
* This routine opens a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsCreateDrmFb
*/

static int gfxKmsOpenDrmFb
    (
    int                 fd,
    drmModeFB*          fb,
    uint32_t*           pHandle
    )
    {
    struct drm_gem_flink flink;
    struct drm_gem_open open_arg;

    flink.handle = fb->handle;
    if (drmIoctl (fd, DRM_IOCTL_GEM_FLINK, &flink))
        {
        (void)fprintf (stderr, "DRM_IOCTL_GEM_FLINK error\n");
        return 1;
        }

    open_arg.name = flink.name;
    if (drmIoctl (fd, DRM_IOCTL_GEM_OPEN, &open_arg))
        {
        (void)fprintf (stderr, "DRM_IOCTL_GEM_OPEN error\n");
        return 1;
        }

    *pHandle = open_arg.handle;

    return 0;
    }
#endif /* GFX_KMS_OPEN_DRM_FB */

#if defined(GFX_KMS_CREATE_DRM_FB)
/*******************************************************************************
*
* gfxKmsCreateDrmFb - create a DRM framebuffer
*
* This routine creates a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsOpenDrmFb
*/

static int gfxKmsCreateDrmFb
    (
    int                 fd,
    uint32_t            width,
    uint32_t            height,
    uint32_t*           pFbId,
    uint32_t*           pHandle,
    uint64_t            size,
    void*               virtAddr,
    void*               physAddr
    )
    {
    struct drm_i915_gem_create create_bo;

    create_bo.size = size;
    create_bo.handle = 0;
#if defined(__vxworks)
    create_bo.virtAddr = (unsigned long)virtAddr;
    create_bo.physAddr = (unsigned long)physAddr;
#endif /* __vxworks */
    if (drmIoctl(fd, DRM_IOCTL_I915_GEM_CREATE, &create_bo))
        {
        (void)fprintf (stderr, "DRM_IOCTL_I915_GEM_CREATE error\n");
        return 1;
        }

    if (create_bo.handle == 0)
        {
        (void)fprintf (stderr, "Bad handle\n");
        return 1;
        }

    /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
    if (drmModeAddFB(fd, width, height, 24, 32, (((width << 2) + 511) & ~511),
                     create_bo.handle, pFbId))
        {
        (void)fprintf (stderr, "drmModeAddFB failed\n");
        return 1;
        }

    *pHandle = create_bo.handle;

    return 0;
    }
#endif /* GFX_KMS_CREATE_DRM_FB */

#if defined(GFX_KMS_MMAP_DRM_FB)
/*******************************************************************************
*
* gfxKmsMmapDrmFb - memory map a DRM framebuffer
*
* This routine memory maps a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gfxKmsMmapDrmFb
    (
    int                 fd,
    uint32_t            fbId,
    uint32_t            handle,
    uint64_t            size,
    void**              pPtr
    )
    {
    struct drm_i915_gem_mmap_gtt mmap_arg;
#if defined(__vxworks)
    struct drm_vxmmap mapreq;
#endif /* __vxworks */

    mmap_arg.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_I915_GEM_MMAP_GTT, &mmap_arg))
        {
        (void)fprintf (stderr, "DRM_IOCTL_I915_GEM_MMAP_GTT error\n");
        return 1;
        }
#if defined(__vxworks)
    mapreq.size = size;
    mapreq.offset = mmap_arg.offset;
    mapreq.virtAddr = 0;
    mapreq.physAddr = 0;
    if (drmIoctl(fd, DRM_IOCTL_VXMMAP, &mapreq))
        {
        (void)fprintf (stderr, "DRM_IOCTL_VXMMAP error\n");
        return 1;
        }
    *pPtr = (void*)(mapreq.virtAddr);
#else
    *pPtr = mmap64(0, size, PROT_READ|PROT_WRITE, MAP_SHARED,
                        fd, mmap_arg.offset);
#endif /* __vxworks */
    if (*pPtr == NULL)
        {
        (void)fprintf (stderr, "NULL frame buffer pointer\n");
        return 1;
        }

    return 0;
    }
#endif /* GFX_KMS_MMAP_DRM_FB */

#if defined(GFX_KMS_PAGE_FLIP)
/*******************************************************************************
*
* gfxKmsPageFlipHandler - page flip handler
*
* This routine handles page flip
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsPageFlip
*/

static void gfxKmsPageFlipHandler
    (
    int                 fd,
    unsigned int        frame,
    unsigned int        sec,
    unsigned int        usec,
    void*               data
    )
    {
    *(int*)data = 0;
    }

/*******************************************************************************
*
* gfxKmsPageFlip - flip page
*
* This routine flips the page
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: gfxKmsPageFlipHandler
*/

static int gfxKmsPageFlip
    (
    int                 fd,
    uint32_t            crtcId,
    uint32_t            fbId,
    uint32_t*           pConnId,
    drmModeModeInfo*    pMode,
    unsigned int        pageFlipTimeout,
    unsigned int        vsync
    )
    {
    int                 pending;

    if (pageFlipTimeout)
        {
        int ret;
        int retry = 1;
        while ((ret = drmModePageFlip (fd, crtcId, fbId,
                             DRM_MODE_PAGE_FLIP_EVENT, &pending)) == -EBUSY)
            {
            /* Sleep and retry but if more than 10 times,
               something is wrong with the page flip */
            struct timespec ntp, otp;

            if (retry > 10) break;
            retry++;
            ntp.tv_sec = (time_t)pageFlipTimeout / MSEC_PER_SEC;
            ntp.tv_nsec = (pageFlipTimeout % MSEC_PER_SEC) * NSEC_PER_MSEC;
            (void)nanosleep (&ntp, &otp);
            }
        if (ret)
            {
            /* Without vsync, drmModePageFlip can return error,
               so print error for vsync only */
            if (vsync)
                {
                static int print_count = 0;
                if (print_count < 3)
                    {
                    (void)fprintf (stderr, "drmModePageFlip errno:%d\n", errno);
                    print_count++;
                    }
                }
            return 1;
            }
        else
            {
            if (vsync)
                {
                drmEventContext evt;
                evt.version = DRM_EVENT_CONTEXT_VERSION;
                evt.vblank_handler = NULL;
                evt.page_flip_handler = gfxKmsPageFlipHandler;
                pending = 1;
                while (pending)
                    {
                    struct timeval timeout;
                    fd_set fds;
                    int ret;
                    FD_ZERO (&fds);
                    FD_SET (fd, &fds);
                    timeout.tv_sec = (time_t)pageFlipTimeout / MSEC_PER_SEC;
                    timeout.tv_usec = (pageFlipTimeout % MSEC_PER_SEC) * USEC_PER_MSEC;
                    ret = select (fd + 1, &fds, NULL, NULL, &timeout);
                    if (ret <= 0)
                        {
                        break;
                        }
                    else if (FD_ISSET (fd, &fds))
                        {
                        if (drmHandleEvent (fd, &evt))
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
                    }
                }
            }
        }
    else
        {
        /* drmModeSetCrtc will flip page to vsync but not efficient
           compared to drmModePageFlip */
        if (drmModeSetCrtc (fd, crtcId, fbId, 0, 0, pConnId, 1, pMode))
            {
            (void)fprintf (stderr, "drmModeSetCrtc errno:%08x\n", errno);
            return 1;
            }
        }

    return 0;
    }
#endif /* GFX_KMS_PAGE_FLIP */

#endif /* _GFX_KMS_HELPER_H */
