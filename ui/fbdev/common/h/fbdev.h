/* fbdev.h - VxWorks Graphics fbdev Driver Interface */

/*
 * Copyright (c) 2009-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
12jan16,rpc  Fix typos and misc doc changes (US73457)
08oct15,yat  Add GFX_FIXED_FBMODE_OFFSET (US67554)
12jun15,yat  Add offset for xlnxlcvc (US58560)
16apr15,yat  Fix ioctl typo (V7GFX-245)
06sep13,mgc  Prepared for VxWorks 7 release
03may10,m_c  Removed bit field in FB_PIXEL_FORMAT
04aug09,m_c  Added FB_INFO
01aug09,m_c  Written
*/

/*
DESCRIPTION

The fbdev driver model supplies an interface for interacting with display
controller devices.  This interface is required by higher level graphics
software in order to render pixel data onto a display.

Fbdev uses the traditional open(), close(), write() and ioctl() routines to
provide a standard interface for graphics development.  These routines are part
of the VxWorks I/O interface library.

\h FBDEV OPEN
RTP applications and kernel modules can open a display controller device by
using the open() routine as follows:

\cs

    FILE    fd;

    fd = open("/dev/fb0", O_RDWR, 0x644);

\ce

The name of a display controller device ("/dev/fb0" in the above example) will
typically be of the form "/dev/fb\<x\>", where \<x\> is a device index
indicating a
particular device frame-buffer configuration.  This configuration is based on
the parameters specified in the VxWorks Image Project.  Developers can modify
frame-buffer configuration parameters using the ioctl() routine.

\h FBDEV CLOSE
RTP applications and kernel modules can close a display controller's file
descriptor using the close() routine as follows:

\cs

    close(fd);

\ce

Closing a display controller's file descriptor will cause the fbdev driver to
free and clean-up resources associated to the application.  For the safety of
other applications that may be accessing the display device, the close() routine
will not shut down the display controller.

\h FBDEV IOCTL
A display controller device can also be manipulated using supported IOCTLs.
The ioctl() routine can be used as follows to manipulate a display controller
device:

\cs

    FB_IOCTL_ARG    arg;
    FILE            fd;

    fd = open("/dev/fb0", O_RDWR, 0x644);
    ioctl(fd, FB_IOCTL_DEV_SHOW, 10);

\ce

Applications must provide a file descriptor to a display controller device,
specify a valid IOCTL identifier, and an argument tailored to that IOCTL.  The
fbdev driver model provides the following IOCTLs:

\sh Required IOCTLs:

\is
\i 'FB_IOCTL_GET_VIDEO_MODE'

Allows applications to query the current video mode of the display controller.
The IOCTL must be called by setting the 'getvideoMode' member of an FB_IOCTL_ARG
variable.  The 'getVideoMode' member holds 'pBuf', a character pointer that
contains the video mode description.

The 'getVideoMode.pBuf' variable must be allocated within the calling
application's context.  The maximum string size is defined with the
'FB_MAX_VIDEO_MODE_LEN' macro. Valid video mode string formats are as follows:

    \<width\>x\<height\>-\<bitDepth\>@\<refresh\>
    \<width\>x\<height\>-\<bitDepth\>
    \<width\>x\<height\>@\<refresh\>
    \<width\>x\<height\>

    where \<width\>x\<height\>  is the display resolution,
          \<bitDepth\>        is the number of bits per pixel, and
          \<refresh\>         is the display refresh rate

The video mode string "default" can also be specified to configure the display
controller using its default video mode. The ioctl() will return with OK if a
video mode has been configured.  If the driver cannot find an exact match for
the video mode string and the string is correctly formated, the closest matching
mode will be used.  If the device has not been initialized, NULL is passed in
place of 'pBuf', or the video mode string is not in a correct format the routine
returns ERROR.

\i 'FB_IOCTL_SET_VIDEO_MODE'

Allows applications to reconfigure the display controller to a new video mode.
The IOTCL must be called by setting the 'setVideoMode' member of an FB_IOCTL_ARG
variable.  The 'setVideoMode' member holds 'videoMode' a constant character
pointer that must contain the desired video mode parameters (for specifics on
the string format, see 'FB_IOCTL_GET_VIDEO_MODE').

Fbdev drivers will attempt to match the provided string with one of the
supported configurations.  If a matching configuration is found, video mode
reconfigure will proceed and ioctl() returns with OK.  Otherwise, the ioctl()
returns with ERROR.

\i 'FB_IOCTL_GET_FB_INFO'

Allows applications to query details of the current frame-buffer.  The IOCTL
must be called with an FB_IOCTL_ARG argument.  If successful, the ioctl() will
return with OK, and populate the 'getFbInfo' member.  In the case of an error,
the ioctl() will return ERROR.

The 'getFbInfo' member will contain all relevant information required to render
data onto a display.  This includes resolution, bit depth, pixel format,
frame-buffer configuration, and addressable memory.

\i 'FB_IOCTL_SET_FB'

Allows applications to specify a memory buffer to be used as frame-buffer. The
IOCTL must be called by setting the 'setFb' member of an FB_IOCTL_ARG variable.
The
'setFb' member holds the pointer 'pFb' that allows applications to provide the
location of the desired frame-buffer.

The behavior of this IOCTL is driver specific, most implementations will require
that the application have allocated memory in a specific format or location.  In
typically situations, the IOCTL is called with an offset of the original
frame-buffer address obtained from FB_IOCTL_GET_FB_INFO.  Using memory allocated
through other means may result in undefined and potentially hazardous driver
behavior, particularly if operated from an RTP.

If supported and enabled, FB_IOCTL_VSYNC_ENABLE, will cause the driver to delay
frame-buffer swaps to coincide with the vertical blanking interrupt.

\i 'FB_IOCTL_GET_CONFIGS'

Allows applications to query all supported configurations for the display
controller.  The IOCTL must be called by setting the 'getConfigs' member of an
FB_IOCTL_ARG variable.  The 'getConfigs' member holds the pointer 'pConfigs'
that must point to a pre-allocated FB_CONFIG array that can contain
FB_MAX_CONFIGS elements.

\ie

\sh Optional IOCTLs:

\is

\i 'FB_IOCTL_CLEAR_SCREEN'

Allows applications to quickly clear the screen to black or to splash screen.
The IOCTL must be called by setting the 'clearScreen' member of an FB_IOCTL_ARG
variable. The 'clearScreen' member holds 'setSplash' a FALSE or TRUE flag
to indicate whether to clear the screen to black or to splash screen.

\i 'FB_IOCTL_DEV_SHOW'

Allows applications to request that the driver display configuration information
to the console.  The amount of information displayed is controlled by the level
of verbosity which is passed as an integer into the IOCTL.  The information
displayed can be driver specific.

\i 'FB_IOCTL_VSYNC_ENABLE'

Allows application to enable vsync (if supported) on a particular display
controller.  Enabling vsync will synchronize frame-buffer swaps, performed via
FB_IOCTL_SET_FB, to coincide with the vertical blanking interrupt.

\i 'FB_IOCTL_VSYNC_DISABLE'

Allows application to disable vsync on a particular display controller.
Disabling vsync will no longer synchronize frame-buffer swaps, instead these
will occur immediately when calling FB_IOCTL_SET_FB.

\i 'FB_IOCTL_VSYNC'

Allows application to synchronize their execution to a vertical blank interrupt.
When calling this IOCTL, applications will pend until the next next interrupt.

\ie

\h FBDEV WRITE

Fbdev drivers implement the write() routine to allow users to have console
capabilities on a graphical display.  The write() routine is used when VxWorks
has been configured with INCLUDE_FBDEV_CONSOLE.

INCLUDE FILES: fbdev.h

*/

#ifndef __INC_fbdev_h
#define __INC_fbdev_h

/* includes */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* defines */

/* Driver control commands */
/* - required IOCTLs */
#define FB_IOCTL_GET_VIDEO_MODE     0x1000
#define FB_IOCTL_SET_VIDEO_MODE     0x1001
#define FB_IOCTL_GET_FB_INFO        0x1002
#define FB_IOCTL_GET_CONFIGS        0x1003
#define FB_IOCTL_SET_FB             0x1004
/* - optional IOCTLs */
#define FB_IOCTL_CLEAR_SCREEN       0x9000
#define FB_IOCTL_VSYNC              0x9001
#define FB_IOCTL_DEV_SHOW           0x9002
#define FB_IOCTL_VSYNC_ENABLE       0x9003
#define FB_IOCTL_VSYNC_DISABLE      0x9004
#define FB_IOCTL_GET_VIDEO_MODES    0x9005

/* Default video mode name */
#define FB_DEFAULT_VIDEO_MODE       "default"

/* Max length of the string returned by FB_IOCTL_GET_VIDEO_MODE */
#define FB_MAX_VIDEO_MODE_LEN       20

/* Max number of video modes that can be returned by FB_IOCTL_GET_VIDEO_MODES */
#define FB_MAX_VIDEO_MODES          10

/* Max number of configurations that can be returned by FB_IOCTL_GET_CONFIGS */
#define FB_MAX_CONFIGS              10

/* Pixel format flags */
#define FB_PIX_ALPHA_IS_PAD         0x00000001  /* alpha is used as padding */
#define FB_PIX_NATIVE               0x00000002  /* indicate frame buffer native format */
#define FB_PIX_RGBA                 0x00000004  /* format is RGBA (as opposed to ARGB) */
#define FB_PIX_SWAP                 0x00000008  /* red and blue are swapped */

/* Sync flags */
#define FB_SYNC_HOR_HIGH_ACT        0x00000001  /* horizontal sync high active */
#define FB_SYNC_VERT_HIGH_ACT       0x00000002  /* vertical sync high active */
#define FB_SYNC_EXT                 0x00000004  /* external sync */
#define FB_SYNC_COMP_HIGH_ACT       0x00000008  /* composite sync high active */
#define FB_SYNC_BROADCAST           0x00000010  /* broadcast video timings */
#define FB_SYNC_ON_GREEN            0x00000020  /* sync on green */

/* Mode flags */
#define FB_VMODE_NONINTERLACED      0x00000000  /* non-interlaced */
#define FB_VMODE_INTERLACED         0x00000001  /* interlaced */
#define FB_VMODE_DOUBLE             0x00000002  /* double scan */
#define FB_VMODE_ODD_FLD_FIRST      0x00000004  /* interlaced: top line first */
#define FB_VMODE_MASK               0x000000ff

#define FB_MAX_STR_CHARS            128

#define FB_MAX_BUFFERS              3

#define FB_DEVICE_PREFIX            "/dev/fb"
#define FB_MAX_DEVICE               5

#if defined(_WRS_CONFIG_FBDEV_XLNXLCVCFB)
#define GFX_FIXED_FBMODE_OFFSET
#endif

/* typedefs */

/* Pixel format */
typedef struct fbPixelFormat
    {
    unsigned int    flags;          /* flags */
    char            alphaBits;      /* size of alpha/padding channel */
    char            redBits;        /* size of red channel */
    char            greenBits;      /* size of green channel */
    char            blueBits;       /* size of blue channel */
    } FB_PIXEL_FORMAT;

typedef struct fbInfo
    {
    void*           pFirstFb;       /* first virtual address in memory */
    void*           pFirstFbPhys;   /* first physical address in memory */
    void*           pFb;            /* virtual address in memory */
    void*           pFbPhys;        /* physical address in memory */
    unsigned int    bpp;            /* bits per pixel */
    FB_PIXEL_FORMAT pixelFormat;    /* pixel format */
    unsigned int    width, stride;  /* width in pixels, stride in bytes */
    unsigned int    height;         /* height in lines */
#if defined(GFX_FIXED_FBMODE_OFFSET)
    unsigned int    offset;         /* buffer offset in bytes */
#endif
    unsigned int    vsync;          /* vsync enable or disable */
    unsigned int    buffers;        /* number of buffers */
    } FB_INFO;

/* Frame buffer configuration */
typedef struct fbConfig
    {
    int             id;             /* unique id (0 = invalid) */
    FB_PIXEL_FORMAT pixelFormat;    /* pixel format */
    } FB_CONFIG;

/* Video mode */
typedef struct fbVideoMode
    {
    char            name[FB_MAX_STR_CHARS];
    unsigned int    refresh;
    unsigned int    xres;
    unsigned int    yres;
    unsigned int    bpp;
    unsigned int    stride;
#if defined(GFX_FIXED_FBMODE_OFFSET)
    unsigned int    offset;         /* buffer offset in bytes */
#endif
    unsigned int    pixclock;
    unsigned int    left_margin;
    unsigned int    right_margin;
    unsigned int    upper_margin;
    unsigned int    lower_margin;
    unsigned int    hsync_len;
    unsigned int    vsync_len;
    unsigned int    sync;           /* see FB_SYNC_ */
    unsigned int    vmode;          /* see FB_VMODE_ */
    uintptr_t       flag;
    unsigned int    vsync;
    unsigned int    buffers;
    } FB_VIDEO_MODE;

/* Driver control request argument */
typedef union fbIoctlArg
    {
    struct
        {
        char*           pBuf;           /* buffer */
        } getVideoMode;

    struct
        {
        const char*     videoMode;      /* video mode */
        } setVideoMode;

    FB_INFO             getFbInfo;

    struct
        {
        FB_CONFIG*      pConfigs;
        } getConfigs;

    struct
        {
        void*           pFb;            /* frame buffer address */
        int             when;           /* number of frames to wait */
        } setFb;

    struct
        {
        unsigned int    setSplash;      /* set clear screen to splash screen */
        } clearScreen;

    struct
        {
        FB_VIDEO_MODE*  pVideoModes;
        } getVideoModes;

    } FB_IOCTL_ARG;

#ifdef  __cplusplus
}
#endif

#endif  /* __INC_fbdev_h */
