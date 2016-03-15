/****************************************************************************
*
*    Copyright 2012 - 2013 Vivante Corporation, Sunnyvale, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/


/*
 * Vivante specific definitions and declarations for EGL library.
 */

#ifndef __eglvivante_h_
#define __eglvivante_h_

#ifdef __cplusplus
extern "C" {
#endif

#define EGLAPIENTRY

#if defined(_WIN32) || defined(__VC32__) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
/* Win32 and Windows CE platforms. */
#include <windows.h>
typedef HDC             EGLNativeDisplayType;
typedef HWND            EGLNativeWindowType;
typedef HBITMAP         EGLNativePixmapType;

#elif (defined(LINUX) || defined(__vxworks)) && defined(EGL_API_DFB) && !defined(__APPLE__)
#include <directfb.h>
typedef struct _DFBDisplay * EGLNativeDisplayType;
typedef IDirectFBWindow *  EGLNativeWindowType;
typedef struct _DFBPixmap *  EGLNativePixmapType;

EGLNativePixmapType
dfbCreatePixmap(
    EGLNativeDisplayType Display,
    int Width,
    int Height
    );

EGLNativePixmapType
dfbCreatePixmapWithBpp(
    EGLNativeDisplayType Display,
    int Width,
    int Height,
	int BitsPerPixel
    );

void
dfbGetPixmapInfo(
    EGLNativePixmapType Pixmap,
    int * Width,
    int * Height,
    int * BitsPerPixel,
    int * Stride,
    void* * Bits
    );

void
dfbDestroyPixmap(
    EGLNativePixmapType Pixmap
    );

#elif (defined(LINUX) || defined(__vxworks)) && defined(EGL_API_FB) && !defined(__APPLE__)

#if defined(WL_EGL_PLATFORM)
/* Wayland types for client apps. */
typedef struct wl_display *      EGLNativeDisplayType;
typedef struct wl_egl_window *   EGLNativeWindowType;
typedef struct wl_egl_pixmap *   EGLNativePixmapType;

#else
/* Linux platform for FBDEV. */
typedef struct _FBDisplay * EGLNativeDisplayType;
typedef struct _FBWindow *  EGLNativeWindowType;
typedef struct _FBPixmap *  EGLNativePixmapType;
#endif

EGLNativeDisplayType
fbGetDisplay(
    void *context
    );

EGLNativeDisplayType
fbGetDisplayByIndex(
    int DisplayIndex
    );

void
fbGetDisplayGeometry(
    EGLNativeDisplayType Display,
    int * Width,
    int * Height
    );

void
fbGetDisplayInfo(
    EGLNativeDisplayType Display,
    int * Width,
    int * Height,
    unsigned long * Physical,
    int * Stride,
    int * BitsPerPixel
    );

void
fbDestroyDisplay(
    EGLNativeDisplayType Display
    );

EGLNativeWindowType
fbCreateWindow(
    EGLNativeDisplayType Display,
    int X,
    int Y,
    int Width,
    int Height
    );

void
fbGetWindowGeometry(
    EGLNativeWindowType Window,
    int * X,
    int * Y,
    int * Width,
    int * Height
    );

void
fbGetWindowInfo(
    EGLNativeWindowType Window,
    int * X,
    int * Y,
    int * Width,
    int * Height,
    int * BitsPerPixel,
    unsigned int * Offset
    );

void
fbDestroyWindow(
    EGLNativeWindowType Window
    );

EGLNativePixmapType
fbCreatePixmap(
    EGLNativeDisplayType Display,
    int Width,
    int Height
    );

EGLNativePixmapType
fbCreatePixmapWithBpp(
    EGLNativeDisplayType Display,
    int Width,
    int Height,
	int BitsPerPixel
    );

void
fbGetPixmapGeometry(
    EGLNativePixmapType Pixmap,
    int * Width,
    int * Height
    );

void
fbGetPixmapInfo(
    EGLNativePixmapType Pixmap,
    int * Width,
    int * Height,
    int * BitsPerPixel,
    int * Stride,
    void ** Bits
    );

void
fbDestroyPixmap(
    EGLNativePixmapType Pixmap
    );

#elif defined(__ANDROID__) || defined(ANDROID)

struct egl_native_pixmap_t;

#if ANDROID_SDK_VERSION >= 9
    #include <android/native_window.h>

    typedef struct ANativeWindow*           EGLNativeWindowType;
    typedef struct egl_native_pixmap_t*     EGLNativePixmapType;
    typedef void*                           EGLNativeDisplayType;
#else
    struct android_native_window_t;
    typedef struct android_native_window_t*    EGLNativeWindowType;
    typedef struct egl_native_pixmap_t *        EGLNativePixmapType;
    typedef void*                               EGLNativeDisplayType;
#endif

#elif defined(LINUX) || defined(__APPLE__)
/* X11 platform. */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef Display *   EGLNativeDisplayType;
typedef Window      EGLNativeWindowType;

#ifdef CUSTOM_PIXMAP
typedef void *      EGLNativePixmapType;
#else
typedef Pixmap      EGLNativePixmapType;
#endif /* CUSTOM_PIXMAP */

#elif defined(__QNXNTO__)
#include <screen/screen.h>

/* VOID */
typedef int              EGLNativeDisplayType;
typedef screen_window_t  EGLNativeWindowType;
typedef screen_pixmap_t  EGLNativePixmapType;

#elif defined(__vxworks)

typedef struct _FBDisplay * EGLNativeDisplayType;
typedef struct _FBWindow *  EGLNativeWindowType;
typedef struct _FBPixmap *  EGLNativePixmapType;

#else

#error "Platform not recognized"

/* VOID */
typedef void *  EGLNativeDisplayType;
typedef void *  EGLNativeWindowType;
typedef void *  EGLNativePixmapType;

#endif

#if defined(__EGL_EXPORTS) && !defined(EGLAPI)
#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#  define EGLAPI    __declspec(dllexport)
# else
#  define EGLAPI
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __eglvivante_h_ */
