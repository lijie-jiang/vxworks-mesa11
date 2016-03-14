/* gfxEs2GearDemo.inl - Standard OpenGL ES2 Gears Demo */

/*
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Ported to GLES2.
 * Kristian Høgsberg <krh@bitplanet.net>
 * May 3, 2010
 *
 * Improve GLES2 port:
 *   * Refactor gear drawing.
 *   * Use correct normals for surfaces.
 *   * Improve shader.
 *   * Use perspective projection transformation.
 *   * Add FPS count.
 *   * Add comments.
 * Alexandros Frantzis <alexandros.frantzis@linaro.org>
 * Jul 13, 2010
 */

/*
 * Copyright (c) 2011-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
14sep15,yat  Add support for Mesa GPU DRI (US24710)
10sep15,yat  Add missing numConfigs check after eglChooseConfig (V7GFX-279)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18sep14,yat  Add support for dynamic RTP demos (US40486)
24jan14,mgc  Modified for VxWorks 7 release
05feb11,jlb  Created, based on work from Brian Paul
*/

/*

DESCRIPTION

This example program provides the standard OpenGL gear program.

*/


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* includes */

#if defined(__vxworks)
#include <vxWorks.h>
#include <ioLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
#if defined(_WRS_KERNEL)
#include <envLib.h>
#endif
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#if defined(GFX_USE_GBM)
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#define GFX_KMS_GET_CONN_STR
#define GFX_KMS_FIND_CONN_CRTC
#define GFX_KMS_FIND_CRTC_FB
#include <gfxKmsHelper.inl>
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
#include <evdevLib.h>
#endif

/* defines */

#define VERTICES_PER_TOOTH 34
#define GEAR_VERTEX_STRIDE 6

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#if defined(_FSLVIV)
/*
 * Non-WR EGL implementations may allow additional flexibility in
 * choosing a window size. If these are non-zero, the render surface is clamped.
 */
#define WIN_WIDTH       0
#define WIN_HEIGHT      0
#endif

#if !defined(GFX_USE_EGL_FBDEV)
#define GFX_USE_EGL_FBDEV 0
#define GFX_USE_EGL_DRI   1
#endif

/* typedefs */

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

/* Each vertex consist of GEAR_VERTEX_STRIDE GLfloat attributes */
typedef GLfloat GearVertex[GEAR_VERTEX_STRIDE];

/**
 * Struct representing a gear.
 */
typedef struct
    {
    /** The array of vertices comprising the gear */
    GearVertex *vertices;
    /** The number of vertices comprising the gear */
    long nvertices;
    /** The Vertex Buffer Object holding the vertices in the graphics card */
    GLuint vbo;
    } ES2_GEAR_DEF;

typedef struct
    {
    EGLContext context;     /* EGL context */
    EGLDisplay display;     /* EGL display */
    EGLSurface surface;     /* EGL draw surface */
    EGLint surfWidth;       /* EGL surface width */
    EGLint surfHeight;      /* EGL surface height */
    EGLint surfBuffer;      /* EGL surface render buffer */
#if defined(_FSLVIV)
    EGLNativeDisplayType eglNativeDisplay;  /* Used for non-WR EGL */
    EGLNativeWindowType  eglNativeWindow;   /* Used for non-WR EGL */
#endif
#if defined(GFX_USE_GBM)
    int                  drmDevFd;
    struct gbm_device*   gbm_dev;
    struct gbm_surface*  gbm_surf;
    drmModeModeInfo      mode;
    uint32_t             connId;
    uint32_t             crtcId;
    uint32_t             fbId;
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
    int                  evDevFd;
#endif
    GLuint               vertexShader;
    GLuint               fragmentShader;
    GLuint               shaderProgram;
    GLuint               positionLoc;
    GLuint               normalLoc;
    GLuint               projLoc;
    GLuint               lightLoc;
    GLuint               colorLoc;
    long                 tRate0;
    long                 frames;
    long                 tRot0;
    GLfloat              angle;
    GLfloat              view_rotx, view_roty, view_rotz;
    ES2_GEAR_DEF         gear[3];
    GLfloat              transform[16];
    } ES2GEAR_SCENE;

/**************************************************************************
 *
 * es2GearVertexShader - vector shader program
 *
 */
static const char *es2GearVertexShader =
    "attribute vec4 position;\n"
    "attribute vec4 normal;\n"
    "uniform mat4 proj;\n"
    "varying vec3 rotated_normal;\n"
    "varying vec3 rotated_position;\n"
    "vec4 tmp;\n"
    "void main ()\n"
    "{\n"
    "   gl_Position = proj * position;\n"
    "   rotated_position = gl_Position.xyz;\n"
    "   tmp = proj * normal;\n"
    "   rotated_normal = tmp.xyz;\n"
    "}\n";

/**************************************************************************
 *
 * es2GearFragmentShader - fragment shader program
 *
 */
static const char *es2GearFragmentShader =
    "precision highp float;\n"
    "uniform vec4 color;\n"
    "uniform vec3 light;\n"
    "varying vec3 rotated_normal;\n"
    "varying vec3 rotated_position;\n"
    "vec3 light_direction;\n"
    "vec4 white = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "void main ()\n"
    "{\n"
    "   light_direction = normalize (light - rotated_position);\n"
    "   gl_FragColor = color + white * dot (light_direction, rotated_normal);\n"
    "}\n";

/**************************************************************************
***************************************************************************
*
*  Support Routines code
*
***************************************************************************
**************************************************************************/

/** 
 * Fills a gear vertex.
 * 
 * @param v the vertex to fill
 * @param x the x coordinate
 * @param y the y coordinate
 * @param z the z coortinate
 * @param n pointer to the normal table 
 * 
 * @return the operation error code
 */
static GearVertex *vert
    (
    GearVertex *v,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat *n
    )
    {
    v[0][0] = x;
    v[0][1] = y;
    v[0][2] = z;
    v[0][3] = n[0];
    v[0][4] = n[1];
    v[0][5] = n[2];

    return v + 1;
    }

/** 
 * Multiplies two 4x4 matrices.
 * 
 * The result is stored in matrix m.
 * 
 * @param m the first matrix to multiply
 * @param n the second matrix to multiply
 */
static void multiply
    (
    GLfloat *m,
    const GLfloat *n
    )
    {
    GLfloat tmp[16];
    const GLfloat *row, *column;
    div_t d;
    int i, j;

    for (i = 0; i < 16; i++)
        {
        tmp[i] = 0;
        d = div (i, 4);
        row = n + d.quot * 4;
        column = m + d.rem;
        for (j = 0; j < 4; j++)
            tmp[i] += row[j] * column[j * 4];
        }
    bcopy ((char *)&tmp, (char *)m, sizeof (tmp));
    }

/** 
 * Rotates a 4x4 matrix.
 * 
 * @param[in,out] m the matrix to rotate
 * @param angle the angle to rotate
 * @param x the x component of the direction to rotate to
 * @param y the y component of the direction to rotate to
 * @param z the z component of the direction to rotate to
 */
static void rotate
    (
    GLfloat *m,
    GLfloat angle,
    GLfloat x,
    GLfloat y,
    GLfloat z
    )
    {
    GLfloat s, c;

    s = (GLfloat)sin ((double)angle);
    c = (GLfloat)cos ((double)angle);
    GLfloat r[16];

    r[0] = x * x * (1.0f - c) + c;
    r[1] = y * x * (1.0f - c) + z * s;
    r[2] = x * z * (1.0f - c) - y * s;
    r[3] = 0.0f;
    r[4] = x * y * (1.0f - c) - z * s;
    r[5] = y * y * (1.0f - c) + c;
    r[6] = y * z * (1.0f - c) + x * s;
    r[7] = 0.0f;
    r[8] = x * z * (1.0f - c) + y * s;
    r[9] = y * z * (1.0f - c) - x * s;
    r[10] = z * z * (1.0f - c) + c;
    r[11] = 0.0f;
    r[12] = 0.0f;
    r[13] = 0.0f;
    r[14] = 0.0f;
    r[15] = 1.0f;

    multiply (m, r);
    }

/** 
 * Translates a 4x4 matrix.
 * 
 * @param[in,out] m the matrix to translate
 * @param x the x component of the direction to translate to
 * @param y the y component of the direction to translate to
 * @param z the z component of the direction to translate to
 */
static void translate
    (
    GLfloat *m,
    GLfloat x,
    GLfloat y,
    GLfloat z
    )
    {
    GLfloat t[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f
        };

    multiply (m, t);
    }

/**************************************************************************
***************************************************************************
*
*  Initialization code
*
***************************************************************************
**************************************************************************/

/*******************************************************************************
 *
 * es2GearInitOne - draw a gear wheel
 *
 *  Input:  inner_radius - radius of hole at center
 *          outer_radius - radius at center of teeth
 *          width - width of gear
 *          teeth - number of teeth
 *          tooth_depth - depth of tooth
 *
 * RETURNS: 0 if successful, or 1 otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearInit
 *
 */
static int es2GearInitOne
    (
    ES2GEAR_SCENE* pDemo,
    GLint gearNum,
    GLfloat inner_radius,
    GLfloat outer_radius,
    GLfloat width,
    GLint teeth,
    GLfloat tooth_depth
    )
    {
    GLint i;
    GLfloat r0, r1, r2;
    GLfloat a0, da;
    GearVertex *v;
    GLfloat s[5], c[5];
    GLfloat normal[3];
    ES2_GEAR_DEF* pGear = &pDemo->gear[gearNum];

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.0f;
    r2 = outer_radius + tooth_depth / 2.0f;

    a0 = (GLfloat)(2.0 * M_PI / teeth);
    da = a0 / 4.0f;

    /* Allocate memory for the vertices */
    pGear->vertices = (GearVertex *)calloc (VERTICES_PER_TOOTH * teeth, sizeof (*pGear->vertices));
    if (pGear->vertices == NULL)
        {
        return 1;
        }

    s[4] = 0.0f;
    c[4] = 1.0f;
    v = pGear->vertices;
    for (i = 0; i < teeth; i++)
        {
        s[0] = s[4];
        c[0] = c[4];
        s[1] = (GLfloat)sin ((double)((GLfloat)i * a0 + 1.0f * da));
        c[1] = (GLfloat)cos ((double)((GLfloat)i * a0 + 1.0f * da));
        s[2] = (GLfloat)sin ((double)((GLfloat)i * a0 + 2.0f * da));
        c[2] = (GLfloat)cos ((double)((GLfloat)i * a0 + 2.0f * da));
        s[3] = (GLfloat)sin ((double)((GLfloat)i * a0 + 3.0f * da));
        c[3] = (GLfloat)cos ((double)((GLfloat)i * a0 + 3.0f * da));
        s[4] = (GLfloat)sin ((double)((GLfloat)i * a0 + 4.0f * da));
        c[4] = (GLfloat)cos ((double)((GLfloat)i * a0 + 4.0f * da));

        normal[0] = 0.0f;
        normal[1] = 0.0f;
        normal[2] = 1.0f;

        v = vert (v, r2 * c[1], r2 * s[1], width * 0.5f, normal);

        v = vert (v, r2 * c[1], r2 * s[1], width * 0.5f, normal);
        v = vert (v, r2 * c[2], r2 * s[2], width * 0.5f, normal);
        v = vert (v, r1 * c[0], r1 * s[0], width * 0.5f, normal);
        v = vert (v, r1 * c[3], r1 * s[3], width * 0.5f, normal);
        v = vert (v, r0 * c[0], r0 * s[0], width * 0.5f, normal);
        v = vert (v, r1 * c[4], r1 * s[4], width * 0.5f, normal);
        v = vert (v, r0 * c[4], r0 * s[4], width * 0.5f, normal);

        v = vert (v, r0 * c[4], r0 * s[4], width * 0.5f, normal);
        v = vert (v, r0 * c[0], r0 * s[0], width * 0.5f, normal);
        v = vert (v, r0 * c[4], r0 * s[4], -width * 0.5f, normal);
        v = vert (v, r0 * c[0], r0 * s[0], -width * 0.5f, normal);

        normal[0] = 0.0f;
        normal[1] = 0.0f;
        normal[2] = -1.0f;

        v = vert (v, r0 * c[4], r0 * s[4], -width * 0.5f, normal);

        v = vert (v, r0 * c[4], r0 * s[4], -width * 0.5f, normal);
        v = vert (v, r1 * c[4], r1 * s[4], -width * 0.5f, normal);
        v = vert (v, r0 * c[0], r0 * s[0], -width * 0.5f, normal);
        v = vert (v, r1 * c[3], r1 * s[3], -width * 0.5f, normal);
        v = vert (v, r1 * c[0], r1 * s[0], -width * 0.5f, normal);
        v = vert (v, r2 * c[2], r2 * s[2], -width * 0.5f, normal);
        v = vert (v, r2 * c[1], r2 * s[1], -width * 0.5f, normal);

        v = vert (v, r1 * c[0], r1 * s[0], width * 0.5f, normal);

        v = vert (v, r1 * c[0], r1 * s[0], width * 0.5f, normal);
        v = vert (v, r1 * c[0], r1 * s[0], -width * 0.5f, normal);
        v = vert (v, r2 * c[1], r2 * s[1], width * 0.5f, normal);
        v = vert (v, r2 * c[1], r2 * s[1], -width * 0.5f, normal);
        v = vert (v, r2 * c[2], r2 * s[2], width * 0.5f, normal);
        v = vert (v, r2 * c[2], r2 * s[2], -width * 0.5f, normal);
        v = vert (v, r1 * c[3], r1 * s[3], width * 0.5f, normal);
        v = vert (v, r1 * c[3], r1 * s[3], -width * 0.5f, normal);
        v = vert (v, r1 * c[4], r1 * s[4], width * 0.5f, normal);
        v = vert (v, r1 * c[4], r1 * s[4], -width * 0.5f, normal);

        v = vert (v, r1 * c[4], r1 * s[4], -width * 0.5f, normal);
        }

    pGear->nvertices = (v - pGear->vertices);

    /* Store the vertices in a vertex buffer object (VBO) */
    glGenBuffers (1, &pGear->vbo);
    glBindBuffer (GL_ARRAY_BUFFER, pGear->vbo);
    glBufferData (GL_ARRAY_BUFFER, pGear->nvertices * sizeof (GearVertex),
        pGear->vertices, GL_STATIC_DRAW);

    return 0;
    }

/**************************************************************************
***************************************************************************
*
*  Gear/Window update Routines
*
***************************************************************************
**************************************************************************/

/*******************************************************************************
 *
 * es2GearRenderOne - draw a single gear to the surface
 *
 * This routine draws a single gear
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearRender
 *
 */
static void es2GearRenderOne
    (
    ES2GEAR_SCENE* pDemo,
    GLint gearNum,
    GLfloat *transform,
    GLfloat x,
    GLfloat y,
    GLfloat angle,
    const GLfloat *color
    )
    {
    GLfloat model_view[16];
    ES2_GEAR_DEF* gear = &pDemo->gear[gearNum];

    /* Translate and rotate the gear */
    bcopy ((char *)transform, (char *)model_view, sizeof (model_view));
    translate (model_view, x, y, 0);
    rotate (model_view, (GLfloat)(2.0 * M_PI * angle / 360.0), 0.0f, 0.0f, 1.0f);

    /* Create and set the ModelViewProjectionMatrix */
    glUniformMatrix4fv (pDemo->projLoc, 1, GL_FALSE, model_view);

    /* Set the gear color */
    glUniform4fv (pDemo->colorLoc, 1, color);

    /* Set the vertex buffer object to use */
    glBindBuffer (GL_ARRAY_BUFFER, gear->vbo);

    /* Set up the position of the attributes in the vertex buffer object */
    glVertexAttribPointer (pDemo->positionLoc, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof (GLfloat), NULL);
    glVertexAttribPointer (pDemo->normalLoc, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof (GLfloat), (GLfloat *) 0 + 3);

    /* Enable the attributes */
    glEnableVertexAttribArray (pDemo->positionLoc);
    glEnableVertexAttribArray (pDemo->normalLoc);

    /* Draw the triangle strips that comprise the gear */
    glDrawArrays (GL_TRIANGLE_STRIP, 0, (GLsizei)gear->nvertices);

    /* Disable the attributes */
    glDisableVertexAttribArray (pDemo->positionLoc);
    glDisableVertexAttribArray (pDemo->normalLoc);
    }

/*******************************************************************************
 *
 * es2GearPrint - print the configuration of the graphics library
 *
 * This routine prints the configuration for the EGL and OpenGL libraries.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void es2GearPrint
    (
    ES2GEAR_SCENE* pDemo,
    EGLint eglMajor,
    EGLint eglMinor
    )
    {
#if defined(__vxworks)
#if defined(_MESA)
    (void)printf ("EGL_DRIVER:%s\n", getenv("EGL_DRIVER"));
#endif
#endif /* __vxworks */
    (void)printf ("EGL API version: %d.%d\n", eglMajor, eglMinor);
    (void)printf ("EGL_VERSION: %s\n", eglQueryString (pDemo->display, EGL_VERSION));
    (void)printf ("EGL_VENDOR: %s\n", eglQueryString (pDemo->display, EGL_VENDOR));
    (void)printf ("EGL_CLIENT_APIS: %s\n", eglQueryString (pDemo->display, EGL_CLIENT_APIS));
    (void)printf ("EGL_EXTENSIONS: %s\n", eglQueryString (pDemo->display, EGL_EXTENSIONS));
    (void)printf ("EGL_WIDTH: %d\n", pDemo->surfWidth);
    (void)printf ("EGL_HEIGHT: %d\n", pDemo->surfHeight);
    (void)printf ("EGL_RENDER_BUFFER: 0x%x(%s)\n", pDemo->surfBuffer,
                  (pDemo->surfBuffer == EGL_BACK_BUFFER) ?
                  "EGL_BACK_BUFFER" : "EGL_SINGLE_BUFFER");
    (void)printf ("GL_RENDERER: %s\n", (char *) glGetString (GL_RENDERER));
    (void)printf ("GL_VERSION: %s\n", (char *) glGetString (GL_VERSION));
    (void)printf ("GL_VENDOR: %s\n", (char *) glGetString (GL_VENDOR));
    (void)printf ("GL_EXTENSIONS: %s\n", (char *) glGetString (GL_EXTENSIONS));
    }

/*******************************************************************************
 *
 * es2GearCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearEglDeinit
 *
 */
static void es2GearCleanup
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    GLint i;

    for (i = 0; i < sizeof (pDemo->gear) / sizeof (pDemo->gear[0]); i++)
        {
        ES2_GEAR_DEF* gear = &pDemo->gear[i];
        if (gear->vertices)
            {
            free (gear->vertices);
            gear->vertices = NULL;
            }
        }
#if defined(GFX_USE_EVDEV)
    if (pDemo->evDevFd > 0)
        {
        (void)close (pDemo->evDevFd);
        }
#endif
    free (pDemo);
    }

/*******************************************************************************
 *
 * es2GearEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the ES2GEAR_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearEglInit
 *
 */
static void es2GearEglDeinit
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    glFinish ();
    if (pDemo->display != EGL_NO_DISPLAY)
        {
        /* Release the rendering context */
        if (pDemo->context != EGL_NO_CONTEXT)
            {
            (void)eglDestroyContext (pDemo->display, pDemo->context);
            }

        /* Release the surface */
        if (pDemo->surface != EGL_NO_SURFACE)
            {
            (void)eglDestroySurface (pDemo->display, pDemo->surface);
            }

#if defined(GFX_USE_GBM)
        if (pDemo->gbm_surf)
            {
            gbm_surface_destroy (pDemo->gbm_surf);
            }
#endif /* GFX_USE_GBM */

        /* Close the display */
        (void)eglMakeCurrent (pDemo->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        (void)eglTerminate (pDemo->display);
        }
    (void)eglReleaseThread ();

#if defined(_FSLVIV)
    /* Release non-WR EGL resources */
    fbDestroyWindow (pDemo->eglNativeWindow);
    fbDestroyDisplay (pDemo->eglNativeDisplay);
#endif
#if defined(GFX_USE_GBM)
    gfxKmsCloseDrmDev (pDemo->drmDevFd);
#endif /* GFX_USE_GBM */

    es2GearCleanup (pDemo);
    }

#if defined(GFX_USE_GBM)
/*******************************************************************************
*
* es2GearFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int es2GearFindDrmFb
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    drmModeCrtc*         crtc;
    drmModeFB*           fb;
    drmModeConnector*    conn;

    if (gfxKmsFindCrtcFb (pDemo->drmDevFd, &crtc, &fb))
        {
        (void)fprintf (stderr, "No crtc found\n");
        return 1;
        }

    if (gfxKmsFindConnCrtc (pDemo->drmDevFd, crtc, &conn))
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

    pDemo->mode = crtc->mode;
    pDemo->connId = conn->connector_id;
    pDemo->crtcId = crtc->crtc_id;
    drmModeFreeCrtc (crtc);
    drmModeFreeConnector (conn);

    return 0;
    }
#endif /* GFX_USE_GBM */

/*******************************************************************************
 *
 * es2GearEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to ES2GEAR_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearEglDeinit
 *
 */
static ES2GEAR_SCENE* es2GearEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    ES2GEAR_SCENE* pDemo;
    GLboolean   printInfo = GL_TRUE;
    EGLint      eglMajor, eglMinor;
    EGLint      configAttribs[] =
        {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BUFFER_SIZE, 32,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
        };
    EGLint      contextAttribs[] =
        {
        EGL_CONTEXT_CLIENT_VERSION,
        2,
        EGL_NONE
        };
    EGLConfig eglConfig;                        /* EGL configuration */
    EGLint numConfigs = 0;                      /* Number of EGL configurations */
#if defined(_FSLVIV)
    EGLint displayWidth = 0;                    /* Display width */
    EGLint displayHeight = 0;                   /* Display height */
    EGLint reservedWidth = 0;                   /* Limit for surface width */
    EGLint reservedHeight = 0;                  /* Limit for surface height */
    unsigned int i;
#elif defined(_TIIMGT)
#elif defined(_MESA)
#else
    int fd;
    char deviceName[FB_MAX_STR_CHARS];
    unsigned int i;
#endif
#if defined(GFX_USE_EVDEV)
    char evDevName[EV_DEV_NAME_LEN + 1];
    unsigned int e;
#endif
    /* Allocate basic structure to carry demo data */
    pDemo = (ES2GEAR_SCENE*)calloc (1, sizeof (ES2GEAR_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;
    pDemo->tRot0 = -1;
#if defined(__vxworks)
#if defined(_MESA)
    switch (eglDriver)
        {
        case GFX_USE_EGL_DRI:
            putenv ("EGL_DRIVER=egl_dri2");
            break;
        default:
            putenv ("EGL_DRIVER=egl_fbdev");
            break;
        }
#endif
#endif /* __vxworks */
    /* EGL Initialization */

    /*
     * EGL Initialization Step 1:  Initialize display
     *
     * The following lines take care of opening the default display
     * (typically /dev/fb0) and setting up the OpenGL library.
     */
#if defined(_FSLVIV)
    /* Use vendor-specific display routine for non-WindRiver EGL */
    for (i = device; i < FB_MAX_DEVICE; i++)
        {
        pDemo->eglNativeDisplay = (EGLNativeDisplayType) fbGetDisplayByIndex (i);
        if (pDemo->eglNativeDisplay)
            {
            break;
            }
        }
    if (i >= FB_MAX_DEVICE)
        {
        free (pDemo);
        (void)fprintf (stderr, "fbGetDisplayByIndex failed\n");
        return (NULL);
        }
    fbGetDisplayGeometry (pDemo->eglNativeDisplay, &displayWidth, &displayHeight);

    reservedWidth = WIN_WIDTH ? WIN_WIDTH : displayWidth;
    reservedHeight = WIN_HEIGHT ? WIN_HEIGHT : displayHeight;
    pDemo->display = eglGetDisplay ((EGLNativeDisplayType) pDemo->eglNativeDisplay);
#elif defined(_TIIMGT)
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
#elif defined(_MESA)
#if defined(GFX_USE_GBM)
    if (eglDriver == GFX_USE_EGL_FBDEV)
        {
        /* Mesa EGL FBDEV driver */
        pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
        }
    else
        {
        /* Open a DRM device to use Mesa GPU DRI driver */
        if (gfxKmsOpenDrmDev (&(pDemo->drmDevFd)))
            {
            free (pDemo);
            return (NULL);
            }

        pDemo->gbm_dev = gbm_create_device (pDemo->drmDevFd);
        if (pDemo->gbm_dev == NULL)
            {
            (void)fprintf (stderr, "gbm_create_device failed\n");
            es2GearEglDeinit (pDemo);
            return (NULL);
            }

        pDemo->display = eglGetDisplay ((EGLNativeDisplayType)pDemo->gbm_dev);
        }
#else
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
#endif /* GFX_USE_GBM */
#else
    for (i = device; i < FB_MAX_DEVICE; i++)
        {
        (void)snprintf (deviceName, FB_MAX_STR_CHARS, "%s%d", FB_DEVICE_PREFIX, i);
        if ((fd = open (deviceName, O_RDWR, 0666)) != -1)
            {
            break;
            }
        }
    if (i >= FB_MAX_DEVICE)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", deviceName, errno);
        free (pDemo);
        return (NULL);
        }
    (void)close (fd);
    pDemo->display = eglGetDisplay ((EGLNativeDisplayType) deviceName);
#endif
    if (pDemo->display == EGL_NO_DISPLAY)
        {
        (void)fprintf (stderr, "eglGetDisplay failed - possibly driver is not compatible\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        if (es2GearFindDrmFb (pDemo))
            {
            es2GearEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_ES_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 2: Choose an EGL configuration
     * Query the OpenGL ES library (and fbdev driver) to find a compatible
     * configuration.
     */
    if (!eglChooseConfig (pDemo->display, configAttribs, &eglConfig, 1, &numConfigs) || (numConfigs <= 0))
        {
        (void)fprintf (stderr, "Couldn't get an EGL visual config\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 3: Create the back buffer
     * Create a window surface using the selected configuration and query its
     * dimensions
     */
#if defined(_FSLVIV)
    /* Use vendor-specific window creation routine for non-WindRiver EGL */
    pDemo->eglNativeWindow = (EGLNativeWindowType) fbCreateWindow (pDemo->eglNativeDisplay,
                                                  (displayWidth - reservedWidth) / 2,
                                                  (displayHeight - reservedHeight) / 2,
                                                  reservedWidth, reservedHeight);

    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                                             pDemo->eglNativeWindow, NULL);
#elif defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        pDemo->gbm_surf = gbm_surface_create (pDemo->gbm_dev,
                                              pDemo->mode.hdisplay,
                                              pDemo->mode.vdisplay,
                                              GBM_BO_FORMAT_XRGB8888,
                          GBM_BO_USE_RENDERING|GBM_BO_USE_SCANOUT);
        if (pDemo->gbm_surf == NULL)
            {
            (void)fprintf (stderr, "gbm_surface_create failed\n");
            es2GearEglDeinit (pDemo);
            return (NULL);
            }
        }
    else
        {
        pDemo->gbm_surf = NULL;
        }
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                        (EGLNativeWindowType)pDemo->gbm_surf, NULL);
#else
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                                             NULL, NULL);
#endif
    if (pDemo->surface == EGL_NO_SURFACE)
        {
        (void)fprintf (stderr, "eglCreateWindowSurface failed\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                           EGL_WIDTH, &(pDemo->surfWidth));
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                           EGL_HEIGHT, &(pDemo->surfHeight));
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                           EGL_RENDER_BUFFER, &(pDemo->surfBuffer));

    /*
     * EGL Initialization Step 4: Setup the context
     * Create a rendering context and set it as the current context being used
     */
    pDemo->context = eglCreateContext (pDemo->display, eglConfig,
                                       EGL_NO_CONTEXT, contextAttribs);
    if (pDemo->context == EGL_NO_CONTEXT)
        {
        (void)fprintf (stderr, "eglCreateContext failed\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        es2GearEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        es2GearPrint (pDemo, eglMajor, eglMinor);
        }

    (void)eglSwapInterval (pDemo->display, 1);

    /*
     * EGL Initialization Step 5: Clear screen
     * Clear the front, back, third buffers to begin with a clean slate
     */
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
    glClear (GL_COLOR_BUFFER_BIT);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
    glClear (GL_COLOR_BUFFER_BIT);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        struct gbm_bo* gbm_bo;
        uint32_t handle;

        gbm_bo = gbm_surface_lock_front_buffer (pDemo->gbm_surf);
        if (gbm_bo == NULL)
            {
            (void)fprintf (stderr, "gbm_surface_lock_front_buffer failed\n");
            es2GearEglDeinit (pDemo);
            return (NULL);
            }

        handle = gbm_bo_get_handle (gbm_bo).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            es2GearEglDeinit (pDemo);
            return (NULL);
            }

        /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
        if (drmModeAddFB (pDemo->drmDevFd,
                          pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                          24, 32, (((pDemo->mode.hdisplay << 2) + 511) & ~511),
                          handle, &(pDemo->fbId)))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            es2GearEglDeinit (pDemo);
            return (NULL);
            }

        if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                            0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
            {
            es2GearEglDeinit (pDemo);
            return (NULL);
            }

        gbm_surface_release_buffer (pDemo->gbm_surf, gbm_bo);
        }
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
    /* Open evdev device */
    for (e = 0; e < EV_DEV_DEVICE_MAX; e++)
        {
        (void)snprintf (evDevName, EV_DEV_NAME_LEN, "%s%d", EV_DEV_NAME_PREFIX, e);
        if ((pDemo->evDevFd = open (evDevName, O_RDONLY|O_NONBLOCK, 0)) != -1)
            {
            break;
            }
        }
    if (e >= EV_DEV_DEVICE_MAX)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", evDevName, errno);
        }
    else
        {
        int evMsgCount = 0;
        EV_DEV_EVENT evDevEvent;

        /* Clear event */
        (void) ioctl (pDemo->evDevFd, FIONREAD, &evMsgCount);
        while (evMsgCount >= sizeof (EV_DEV_EVENT))
            {
            if (read (pDemo->evDevFd, (char *)&evDevEvent, sizeof (EV_DEV_EVENT)) < sizeof (EV_DEV_EVENT))
                {
                break;
                }
            (void) ioctl (pDemo->evDevFd, FIONREAD, &evMsgCount);
            }
        }
#endif
    return (pDemo);
    }

/*******************************************************************************
 *
 * es2GearDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearInit
 *
 */
static void es2GearDeinit
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    GLint i;

    for (i = 0; i < sizeof (pDemo->gear) / sizeof (pDemo->gear[0]); i++)
        {
        ES2_GEAR_DEF* gear = &pDemo->gear[i];
        if (gear->vbo)
            {
            glDeleteBuffers (1, &gear->vbo);
            gear->vbo = 0;
            }
        }

    if (pDemo->shaderProgram)
        {
        glDeleteShader (pDemo->vertexShader);
        glDeleteShader (pDemo->fragmentShader);
        glDeleteProgram (pDemo->shaderProgram);
        glUseProgram (0);
        }
    }

/*******************************************************************************
 *
 * es2GearInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearDeinit
 *
 */
static int es2GearInit
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    GLfloat ar;
    GLfloat transform[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.1f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    const GLfloat light[3] = { 0.0f, 0.2f, -0.8f };

    (void)printf ("Compiling vertex shader\n");

    /* Compile the shaders*/
    pDemo->vertexShader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (pDemo->vertexShader, 1, &es2GearVertexShader, NULL);
    glCompileShader (pDemo->vertexShader);

    /* Check for compile success*/
    GLint nCompileResult = 0;
    glGetShaderiv (pDemo->vertexShader, GL_COMPILE_STATUS, &nCompileResult);
    if (0 == nCompileResult)
        {
        char  strLog[1024];
        GLint nLength;
        glGetShaderInfoLog (pDemo->vertexShader, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        return 1;
        }

    (void)printf ("Compiling fragment shader\n");

    pDemo->fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (pDemo->fragmentShader, 1, &es2GearFragmentShader, NULL);
    glCompileShader (pDemo->fragmentShader);

    /* Check for compile success*/
    glGetShaderiv (pDemo->fragmentShader, GL_COMPILE_STATUS, &nCompileResult);
    if (0 == nCompileResult)
        {
        char  strLog[1024];
        GLint nLength;
        glGetShaderInfoLog (pDemo->fragmentShader, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        return 1;
        }

    /* Attach the individual shaders to the common shader program*/
    pDemo->shaderProgram = glCreateProgram ();
    glAttachShader (pDemo->shaderProgram, pDemo->vertexShader);
    glAttachShader (pDemo->shaderProgram, pDemo->fragmentShader);

    (void)printf ("Linking shader\n");

    /* Link the vertex shader and fragment shader together*/
    glLinkProgram (pDemo->shaderProgram);

    /* Check for link success*/
    GLint nLinkResult = 0;
    glGetProgramiv (pDemo->shaderProgram, GL_LINK_STATUS, &nLinkResult);
    if (0 == nLinkResult)
        {
        char strLog[1024];
        GLint nLength;
        glGetProgramInfoLog (pDemo->shaderProgram, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        return 1;
        }

    /* Set the shader program*/
    glUseProgram (pDemo->shaderProgram);

    /* Grab attributes locations */
    pDemo->positionLoc = glGetAttribLocation (pDemo->shaderProgram, "position");
    pDemo->normalLoc = glGetAttribLocation (pDemo->shaderProgram, "normal");

    /* Get uniform locations */
    pDemo->projLoc = glGetUniformLocation (pDemo->shaderProgram, "proj");
    pDemo->lightLoc = glGetUniformLocation (pDemo->shaderProgram, "light");
    pDemo->colorLoc = glGetUniformLocation (pDemo->shaderProgram, "color");

    /* Set the ligthLoc uniform which is constant throught the program */
    glUniform3fv (pDemo->lightLoc, 1, light);

    /* Initialize the gears */
    pDemo->view_rotx = 20.0f;
    pDemo->view_roty = 30.0f;
    pDemo->view_rotz = 0.0f;
    pDemo->angle = 0.0f;

    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);

    if (es2GearInitOne (pDemo, 0, 1.0f, 4.0f, 1.0f, 20, 0.7f)) return 1;
    if (es2GearInitOne (pDemo, 1, 0.5f, 1.5f, 0.8f, 10, 0.6f)) return 1;
    if (es2GearInitOne (pDemo, 2, 1.3f, 2.0f, 0.5f, 10, 0.7f)) return 1;

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    if (pDemo->surfWidth < pDemo->surfHeight)
        {
        ar = (GLfloat)pDemo->surfWidth;
        }
    else
        {
        ar = (GLfloat)pDemo->surfHeight;
        }

    transform[0] = 0.1f * ar / (GLfloat)pDemo->surfWidth;
    transform[5] = 0.1f * ar / (GLfloat)pDemo->surfHeight;
    bcopy ((char *)transform, (char *)&(pDemo->transform), sizeof (transform));

    return 0;
    }

/*******************************************************************************
 *
 * es2GearRender - render the application
 *
 * This routine renders the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2GearRenderOne
 *
 */
static int es2GearRender
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    const GLfloat red[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
    const GLfloat green[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
    const GLfloat blue[4] = { 0.2f, 0.2f, 1.0f, 1.0f };
    GLfloat transform[16];

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bcopy ((char *)pDemo->transform, (char *)transform, sizeof (transform));

    /* Translate and rotate the view */
    rotate (transform, (GLfloat)(2.0 * M_PI * pDemo->view_rotx / 360.0), 1.0f, 0.0f, 0.0f);
    rotate (transform, (GLfloat)(2.0 * M_PI * pDemo->view_roty / 360.0), 0.0f, 1.0f, 0.0f);
    rotate (transform, (GLfloat)(2.0 * M_PI * pDemo->view_rotz / 360.0), 0.0f, 0.0f, 1.0f);

    /* Draw the gears */
    es2GearRenderOne (pDemo, 0, transform, -3.0f, -2.0f,         pDemo->angle       , red);
    es2GearRenderOne (pDemo, 1, transform,  3.1f, -2.0f, -2.0f * pDemo->angle - 9.0f, green);
    es2GearRenderOne (pDemo, 2, transform, -3.1f,  4.2f, -2.0f * pDemo->angle - 25.0f, blue);

    return 0;
    }

/*******************************************************************************
 *
 * es2GearPerfCount - calculate and show performance
 *
 * This routine calculates and displays the number of frames rendered every
 * 5 seconds.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void es2GearPerfCount
    (
    ES2GEAR_SCENE* pDemo
    )
    {
    long dt;
    long t;
    struct timeval curtime;

    (void)gettimeofday (&curtime, NULL);
    t = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);

    if (pDemo->tRot0 < 0)
        {
        pDemo->tRot0 = t;
        }
    dt = t - pDemo->tRot0;
    pDemo->tRot0 = t;

    pDemo->angle += 70.0f * (GLfloat)dt / 1000.0f;  /* 70 degrees per second */
    if (pDemo->angle > 360.0f)
        {
        pDemo->angle = (GLfloat)fmod ((double)pDemo->angle, 360.0);
        }

    pDemo->frames++;
    if (pDemo->tRate0 < 0)
        {
        pDemo->tRate0 = t;
        pDemo->frames = 0;
        }
    if ((t - pDemo->tRate0) > 5000)
        {
        long seconds = (t - pDemo->tRate0) / 1000;
        long fps = pDemo->frames / seconds;
        (void)fprintf (stdout, "%ld frames in %ld seconds = %ld FPS\n",
                         pDemo->frames, seconds, fps);
        pDemo->tRate0 = t;
        pDemo->frames = 0;
        }
    }

#if defined(GFX_USE_EVDEV)
/*******************************************************************************
 *
 * es2GearGetChar - gets an ASCII character from keyboard
 *
 * This routine gets an ASCII character from keyboard
 *
 * RETURNS: Ascii character, or NULL
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static unsigned short es2GearGetChar
    (
    int evDevFd
    )
    {
    fd_set readFds;
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO (&readFds);
    FD_SET (evDevFd, &readFds);
    while ((select (evDevFd+1, &readFds, NULL, NULL, &timeout) > 0) &&
           FD_ISSET (evDevFd, &readFds))
        {
        EV_DEV_EVENT evDevEvent;
        ssize_t evDevCount;
        evDevCount = read (evDevFd, (char *)&evDevEvent, sizeof (EV_DEV_EVENT));
        if (evDevCount == sizeof (EV_DEV_EVENT))
            {
            if (evDevEvent.type == EV_DEV_KEY)
                {
                (void)printf ("Key [%04x] [%08x]\n", evDevEvent.code, evDevEvent.value);
                return (unsigned short)((evDevEvent.value)? evDevEvent.code:0);
                }
            }
        }

    return 0;
    }
#endif

/*******************************************************************************
 *
 * gfxEs2GearDemo - core functionality of the program
 *
 * This routine contains the core functionality for the program.
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static int gfxEs2GearDemo
    (
    unsigned int device,
    int runTime,
    unsigned int eglDriver
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    ES2GEAR_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = es2GearEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "es2GearEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (es2GearInit (pDemo))
        {
        (void)fprintf (stderr, "es2GearInit failed\n");
        es2GearDeinit (pDemo);
        es2GearEglDeinit (pDemo);
        return (EXIT_FAILURE);
        }

    runTime = runTime * 1000;
    (void)gettimeofday (&curtime, NULL);
    startTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);
    diffTime = 0;
    /* Run the program so long as it hasn't reached its end time */
    while (((diffTime < runTime)||(runTime == 0))
#if defined(GFX_USE_EVDEV)
        && (kbdQuit == 0)
#endif
        )
        {
#if defined(GFX_USE_EVDEV)
        /* Grab a character from keyboard */
        if (pDemo->evDevFd > 0)
            {
            unsigned short character = es2GearGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        es2GearPerfCount (pDemo);

        if (es2GearRender (pDemo))
            {
            (void)fprintf (stderr, "es2GearRender failed\n");
            break;
            }

        if (eglSwapBuffers (pDemo->display, pDemo->surface) != EGL_TRUE)
            {
            (void)fprintf (stderr, "eglSwapBuffers failed\n");
            break;
            }
#if defined(GFX_USE_GBM)
        if (pDemo->gbm_surf)
            {
            struct gbm_bo* gbm_bo;

            gbm_bo = gbm_surface_lock_front_buffer (pDemo->gbm_surf);
            if (gbm_bo == NULL)
                {
                (void)fprintf (stderr, "gbm_surface_lock_front_buffer failed\n");
                break;
                }

            (void)gfxKmsPageFlip (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                                  &(pDemo->connId), &(pDemo->mode), 20, 1);

            gbm_surface_release_buffer (pDemo->gbm_surf, gbm_bo);
            }
#endif /* GFX_USE_GBM */
        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    /* Deinitialize demo */
    es2GearDeinit (pDemo);
    es2GearEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
