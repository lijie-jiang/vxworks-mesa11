/* gfxItlGmcShow.c - Intel Graphics and Memory Controller graphics driver */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
14sep15,yat  Add more variables to show (US66034)
27mar15,rpc  Static analysis fixes (US50633)
11mar15,yat  Add show support (US55410)
*/

/* includes */

#include <gfxItlGmcDrv.h>

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
* SEE ALSO:
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
            return "DVI-I";
        case DRM_MODE_CONNECTOR_DVID:
            return "DVI-D";
        case DRM_MODE_CONNECTOR_DVIA:
            return "DVI-A";
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
            return "HDMI-A";
        case DRM_MODE_CONNECTOR_HDMIB:
            return "HDMI-B";
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
* SEE ALSO:
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

/*******************************************************************************
*
* gfxKmsGetEncoderStr - get display encoder string
*
* This routine gets the display encoder string.
*
* RETURNS: display connector mode string
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static const char *gfxKmsGetEncoderStr
    (
    uint32_t encoder_type
    )
    {
    switch (encoder_type)
        {
        case DRM_MODE_ENCODER_NONE:
            return "none";
        case DRM_MODE_ENCODER_DAC:
            return "DAC";
        case DRM_MODE_ENCODER_TMDS:
            return "TMDS";
        case DRM_MODE_ENCODER_LVDS:
            return "LVDS";
        case DRM_MODE_ENCODER_TVDAC:
            return "TVDAC";
        default:
            return "unknown";
        }
    }

/*******************************************************************************
*
* gfxItlGmcdCmdShow - show driver information
*
* This routine shows driver information
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void gfxItlGmcdShow
    (
    GFX_FBDEV*      pDev
    )
    {
    int i, j;
    drmModeRes*       res;
    drmModePlaneRes*  plane_res;

    if (pDev->drmDevFd < 0)
        {
        return;
        }

    res = drmModeGetResources(pDev->drmDevFd);
    if (!res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM resources\n");
        return;
        }

    (void)printf ("\ncount_fbs=%d\n", res->count_fbs);
    (void)printf ("count_crtcs=%d\n", res->count_crtcs);
    (void)printf ("count_connectors=%d\n", res->count_connectors);
    (void)printf ("count_encoders=%d\n", res->count_encoders);
    (void)printf ("min_width=%d\n", res->min_width);
    (void)printf ("max_width=%d\n", res->max_width);
    (void)printf ("min_height=%d\n", res->min_height);
    (void)printf ("max_height=%d\n", res->max_height);

    for (i = 0; i < res->count_fbs; i++)
        {
        drmModeFB *fb = drmModeGetFB(pDev->drmDevFd, res->fbs[i]);
        (void)printf ("framebuffer=%d-%d\n", i, res->fbs[i]);
        if (!fb)
            {
            continue;
            }
        (void)printf ("\tfb_id=%d\n", fb->fb_id);
        (void)printf ("\twidth=%d\n", fb->width);
        (void)printf ("\theight=%d\n", fb->height);
        (void)printf ("\tpitch=%d\n", fb->pitch);
        (void)printf ("\tbpp=%d\n", fb->bpp);
        (void)printf ("\tdepth=%d\n", fb->depth);
        (void)printf ("\thandle=%d\n", fb->handle);
        drmModeFreeFB(fb);
        }

    for (i = 0; i < res->count_crtcs; i++)
        {
        drmModeCrtc *crtc = drmModeGetCrtc(pDev->drmDevFd, res->crtcs[i]);
        (void)printf ("crtc=%d-%d\n", i, res->crtcs[i]);
        if (!crtc)
            {
            continue;
            }
        (void)printf ("\tcrtc_id=%d\n", crtc->crtc_id);
        (void)printf ("\tbuffer_id=%d\n", crtc->buffer_id);
        (void)printf ("\tx=%d\n", crtc->x);
        (void)printf ("\ty=%d\n", crtc->y);
        (void)printf ("\twidth=%d\n", crtc->width);
        (void)printf ("\theight=%d\n", crtc->height);
        (void)printf ("\tmode_valid=%d\n", crtc->mode_valid);
        (void)printf ("\tgamma_size=%d\n", crtc->gamma_size);
        (void)printf ("\tclock=%d\n", crtc->mode.clock);
        (void)printf ("\thdisplay=%d\n", crtc->mode.hdisplay);
        (void)printf ("\thsync_start=%d\n", crtc->mode.hsync_start);
        (void)printf ("\thsync_end=%d\n", crtc->mode.hsync_end);
        (void)printf ("\thtotal=%d\n", crtc->mode.htotal);
        (void)printf ("\thskew=%d\n", crtc->mode.hskew);
        (void)printf ("\tvdisplay=%d\n", crtc->mode.vdisplay);
        (void)printf ("\tvsync_start=%d\n", crtc->mode.vsync_start);
        (void)printf ("\tvsync_end=%d\n", crtc->mode.vsync_end);
        (void)printf ("\tvtotal=%d\n", crtc->mode.vtotal);
        (void)printf ("\tvscan=%d\n", crtc->mode.vscan);
        (void)printf ("\tvrefresh=%d\n", crtc->mode.vrefresh);
        (void)printf ("\tflags=%x\n", crtc->mode.flags);
        (void)printf ("\ttype=%d\n", crtc->mode.type);
        (void)printf ("\tname=%s\n", crtc->mode.name);
        drmModeFreeCrtc(crtc);
        }

    for (i = 0; i < res->count_connectors; i++)
        {
        drmModeConnector *conn = drmModeGetConnector(pDev->drmDevFd, res->connectors[i]);
        (void)printf ("connector=%d-%d\n", i, res->connectors[i]);
        if (!conn)
            {
            continue;
            }
        (void)printf ("\tconnector_id=%d\n", conn->connector_id);
        (void)printf ("\tencoder_id=%d\n", conn->encoder_id);
        (void)printf ("\tconnector_type=%d-%s\n", conn->connector_type, gfxKmsGetConnTypeStr(conn->connector_type));
        (void)printf ("\tconnector_type_id=%d\n", conn->connector_type_id);
        (void)printf ("\tconnection=%d-%s\n", conn->connection, gfxKmsGetConnModeStr(conn->connection));
        (void)printf ("\tmmWidth=%d\n", conn->mmWidth);
        (void)printf ("\tmmHeight=%d\n", conn->mmHeight);
        (void)printf ("\tcount_modes=%d\n", conn->count_modes);
        for (j = 0; j < conn->count_modes; j++)
            {
            struct drm_mode_modeinfo *mode = (struct drm_mode_modeinfo *)&conn->modes[j];
            (void)printf ("\tmode=%d\n", j);
            (void)printf ("\t\tname=%s\n", mode->name);
            (void)printf ("\t\tclock=%d\n", mode->clock);
            (void)printf ("\t\thdisplay=%d\n", mode->hdisplay);
            (void)printf ("\t\thsync_start=%d\n", mode->hsync_start);
            (void)printf ("\t\thsync_end=%d\n", mode->hsync_end);
            (void)printf ("\t\thtotal=%d\n", mode->htotal);
            (void)printf ("\t\thskew=%d\n", mode->hskew);
            (void)printf ("\t\tvdisplay=%d\n", mode->vdisplay);
            (void)printf ("\t\tvsync_start=%d\n", mode->vsync_start);
            (void)printf ("\t\tvsync_end=%d\n", mode->vsync_end);
            (void)printf ("\t\tvtotal=%d\n", mode->vtotal);
            (void)printf ("\t\tvscan=%d\n", mode->vscan);
            (void)printf ("\t\tvrefresh=%d\n", mode->vrefresh);
            (void)printf ("\t\tflags=%x\n", mode->flags);
            (void)printf ("\t\ttype=%d\n", mode->type);
            (void)printf ("\t\tname=%s\n", mode->name);
            }
        (void)printf ("\tcount_props=%d\n", conn->count_props);
        for (j = 0; j < conn->count_props; j++)
            {
            drmModePropertyRes *props = drmModeGetProperty(pDev->drmDevFd, conn->props[j]);
            (void)printf ("\tprops=%d-%d\n", j, conn->props[j]);
            if (!props)
                {
                continue;
                }
            (void)printf ("\t\tprop_id=%d\n", props->prop_id);
            (void)printf ("\t\tflags=%x\n", props->flags);
            (void)printf ("\t\tname=%s\n", props->name);
            (void)printf ("\t\tcount_values=%d\n", props->count_values);
            (void)printf ("\t\tcount_enums=%d\n", props->count_enums);
            (void)printf ("\t\tcount_blobs=%d\n", props->count_blobs);
            drmModeFreeProperty(props);
            }
        (void)printf ("\tcount_encoders=%d\n", conn->count_encoders);
        for (j = 0; j < conn->count_encoders; j++)
            {
            drmModeEncoder *enc = drmModeGetEncoder(pDev->drmDevFd, conn->encoders[j]);
            (void)printf ("\tencoders=%d-%d\n", j, conn->encoders[j]);
            if (!enc)
                {
                continue;
                }
            (void)printf ("\t\tencoder_id=%d\n", enc->encoder_id);
            (void)printf ("\t\ttype=%d-%s\n", enc->encoder_type, gfxKmsGetEncoderStr(enc->encoder_type));
            (void)printf ("\t\tcrtc_id=%d\n", enc->crtc_id);
            (void)printf ("\t\tpossible_crtcs=0x%x\n", enc->possible_crtcs);
            (void)printf ("\t\tpossible_clones=0x%x\n", enc->possible_clones);
            drmModeFreeEncoder(enc);
            }
        drmModeFreeConnector(conn);
        }

    for (i = 0; i < res->count_encoders; i++)
        {
        drmModeEncoder *enc = drmModeGetEncoder(pDev->drmDevFd, res->encoders[i]);
        (void)printf ("encoder=%d-%d\n", i, res->encoders[i]);
        if (!enc)
            {
            continue;
            }
        (void)printf ("\tencoder_id=%d\n", enc->encoder_id);
        (void)printf ("\ttype=%d-%s\n", enc->encoder_type, gfxKmsGetEncoderStr(enc->encoder_type));
        (void)printf ("\tcrtc_id=%d\n", enc->crtc_id);
        (void)printf ("\tpossible_crtcs=0x%x\n", enc->possible_crtcs);
        (void)printf ("\tpossible_clones=0x%x\n", enc->possible_clones);
        drmModeFreeEncoder(enc);
        }

    drmModeFreeResources(res);

    plane_res = drmModeGetPlaneResources(pDev->drmDevFd);
    if (!plane_res)
        {
        (void)fprintf (stderr, "Failed to retrieve DRM plane resources\n");
        return;
        }

    for (i = 0; i < plane_res->count_planes; i++)
        {
        drmModePlane *ovr = drmModeGetPlane(pDev->drmDevFd, plane_res->planes[i]);
        (void)printf ("plane=%d-%d\n", i, plane_res->planes[i]);
        if (!ovr)
            {
            continue;
            }
        (void)printf ("\tplane_id=%d\n", ovr->plane_id);
        (void)printf ("\tcount_formats=%d\n", ovr->count_formats);
        (void)printf ("\tcrtc_id=%d\n", ovr->crtc_id);
        (void)printf ("\tfb_id=%d\n", ovr->fb_id);
        (void)printf ("\tcrtc_x=%d\n", ovr->crtc_x);
        (void)printf ("\tcrtc_y=%d\n", ovr->crtc_y);
        (void)printf ("\tx=%d\n", ovr->x);
        (void)printf ("\ty=%d\n", ovr->y);
        (void)printf ("\tpossible_crtcs=%d\n", ovr->possible_crtcs);
        (void)printf ("\tgamma_size=%d\n", ovr->gamma_size);
        drmModeFreePlane(ovr);
        }

    drmModeFreePlaneResources(plane_res);
    }
