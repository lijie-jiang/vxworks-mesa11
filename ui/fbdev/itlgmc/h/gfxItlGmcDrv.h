/* gfxItlGmcDrv.h - Intel Graphics and Memory Controller frame buffer driver header file */

/*
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
29jul15,rpc  Add DVI enum (US50615)
27mar15,qsn  Support VSYNC (not enabled) (US54417)
25feb15,tlu  Enable LVDS/VGA displays (US50700, US50616) 
11mar15,yat  Add show and FBDEV task support (US55410)
16feb15,qsn  Support RTP mode (US50617)
06jan15,qsn  Modified for itl_64_vx7 bsp (US50612)
22dec14,yat  Rework pmap of frame buffer for RTP restart (US46449)
20dec14,qsn  Initial VxWorks 7 release (US48907)
*/

#ifndef __INCgfxItlGmcDrvH
#define __INCgfxItlGmcDrvH

/* includes */

#include <vxWorks.h>
#include <ioLib.h>
#include <semLib.h>
#include <fbdev.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#if defined(GFX_USE_FBDEV_TASK)
#include <taskLib.h>
#endif

/* defines */

#undef GFX_USE_PMAP
#define GFX_VSYNC_POLL

/* For FB_IOCTL_GET_VIDEO_MODES to return current video mode instead of
   all video modes so that FB_IOCTL_SET_VIDEO_MODE that can fail
   will not be called during FB demos and tests runs */
#define GFX_USE_CURRENT_VIDEO_MODES

#if defined(GFX_USE_PMAP)
#define GFX_PMAP_ATTR               (MMU_ATTR_VALID |         \
                                     MMU_ATTR_SUP_RW |        \
                                     MMU_ATTR_USR_RW |        \
                                     MMU_ATTR_SPL_1)
#endif

#define GFX_VM_STATE                (VM_STATE_VALID |         \
                                     VM_STATE_WRITABLE |      \
                                     MMU_ATTR_SPL_MSK)

#define GFX_VM_STATE_MASK           (VM_STATE_MASK_VALID |    \
                                     VM_STATE_MASK_WRITABLE | \
                                     MMU_ATTR_SPL_1)

/* typedefs */

typedef enum gfxDisp
    {
    GFX_DISP_ITL_VGA      = DRM_MODE_CONNECTOR_VGA,
    GFX_DISP_ITL_DVI      = DRM_MODE_CONNECTOR_DVID,
    GFX_DISP_ITL_LVDS     = DRM_MODE_CONNECTOR_LVDS,
    GFX_DISP_ITL_DP       = DRM_MODE_CONNECTOR_DisplayPort,
    GFX_DISP_ITL_HDMI     = DRM_MODE_CONNECTOR_HDMIA
    } GFX_DISP;

#if defined(GFX_USE_FBDEV_TASK)
typedef enum gfxTaskCmdType
    {
    GFX_TASK_OPEN,
    GFX_TASK_SET_VIDEO_MODE,
    GFX_TASK_SET_FB_ADDR
    } GFX_TASK_CMD_TYPE;
#endif

/* Frame buffer display device */
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
    void*                           firstPhysAddr;  /* address of first buffer   */
    void*                           firstVirtAddr;  /* address of first buffer   */
    void*                           secondVirtAddr; /* address of second buffer  */
    void*                           thirdVirtAddr;  /* address of third buffer   */
    void*                           frontPhysAddr;  /* front buffer address      */
    void*                           frontVirtAddr;  /* front buffer address      */
    int                             drmDevFd;       /* drm device file ID        */
    uint32_t                        connId;         /* drm display connector     */
    drmModeModeInfo                 mode;           /* drm display mode          */
    uint32_t                        crtcId;         /* drm crtc                  */
    uint32_t                        firstFbId;      /* first drm framebuffer ID  */
    uint32_t                        secondFbId;     /* second drm framebuffer ID */
    uint32_t                        thirdFbId;      /* third drm framebuffer ID  */
    uint32_t                        firstHandle;    /* first dumb buffer handle  */
    uint32_t                        secondHandle;   /* second dumb buffer handle */
    uint32_t                        thirdHandle;    /* third dumb buffer handle  */
    atomic32_t                      isVideoModeSet; /* flag setting video mode   */
    int                             isMaster;       /* flag to set master */
    unsigned int                    pageFlipTimeout;/* flag to use drmModePageFlip() instead of drmModeSetrtc() */
#if defined(GFX_USE_FBDEV_TASK)
    TASK_ID                         taskId;
    SEM_ID                          cmdAvail;
    SEM_ID                          retAvail;
    GFX_TASK_CMD_TYPE               cmdType;
    void*                           cmdData;
    STATUS                          retStat;
#endif
    FUNCPTR                         setVideoModeExFuncPtr;
    FUNCPTR                         setFbAddrFuncPtr;
    char                            deviceName[FB_MAX_STR_CHARS];
    char                            displayName[FB_MAX_STR_CHARS];
    char                            modeStr[FB_MAX_VIDEO_MODE_LEN];
#if defined(GFX_VXBUS_GEN2)
    VXB_DEV_ID                      vxbDev;
#endif
    } GFX_FBDEV;

#endif /* __INCgfxItlGmcDrvH */
