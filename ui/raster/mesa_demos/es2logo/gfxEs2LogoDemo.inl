/* gfxEs2LogoDemo.inl - OpenGL ES Logo Demo */

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
24feb16,yat  Fix static analysis defects (US75033)
14sep15,yat  Add support for Mesa GPU DRI (US24710)
10sep15,yat  Add missing numConfigs check after eglChooseConfig (V7GFX-279)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18sep14,yat  Add support for dynamic RTP demos (US40486)
10mar14,yat  Modified for VxWorks 7 release from demo provided by kka
*/

/*

DESCRIPTION

This example program provides the OpenGL ES logo program.

*/

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
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#if defined(GFX_USE_GBM)
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#define GFX_KMS_GET_CONN_STR
#define GFX_KMS_FIND_CONN_CRTC
#define GFX_KMS_FIND_CRTC_FB
#include "gfxKmsHelper.inl"
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
#include <evdevLib.h>
#endif
#include "gfxMathHelper.inl"

/* defines */

#if defined(_FSLVIV)
/*
 * Non-WR EGL implementations may allow additional flexibility in
 * choosing a window size. If these are non-zero, the render surface is clamped.
 */
#define WIN_WIDTH       0
#define WIN_HEIGHT      0
#endif

#define VPOSITIONNORMAL_ELEMENTS 7
#define GL_BIND_ATTRIB_LOCATION_VPOSITION 0
#define GL_BIND_ATTRIB_LOCATION_SOURCECOLOR 1

#if !defined(GFX_USE_EGL_FBDEV)
#define GFX_USE_EGL_FBDEV 0
#define GFX_USE_EGL_DRI   1
#endif

/* typedefs */

typedef struct
    {
    GLuint program;
    GLuint rotation_index;
    GLuint perspective_index;
    GLuint pushaway_index;
    GLuint perspectiveTranslate_index;
    GLuint light_index;
    } LogoProgram;

typedef struct
    {
    GLuint program;
    GLuint rotation_index;
    GLuint perspective_index;
    GLuint pushaway_index;
    GLuint shadow_index;
    GLuint shadowTranslate_index;
    GLuint perspectiveTranslate_index;
    } ShadowProgram;

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

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
    long tRate0;
    long frames;
    GLfloat proj[16];
    GLuint indicesBufferIndex;
    GLuint verticesBufferIndex;
    LogoProgram logoProgram;
    ShadowProgram shadowProgram;
    GLfloat angle;
    } ES2LOGO_SCENE;

/* locals */

static GLfloat lightVector[] =
    {
    -0.2f, 1.0f, 1.0f
    };

static const GLfloat vVertices[] =
    {
    0.0f, 0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 14.75f,
    -14.0f, 0.0f, 11.5f, -19.0f, 0.0f, 16.75f, 0.0f, 0.0f, 23.25f, 0.0f,
    0.0f, 16.0f, -12.0f, 0.0f, 12.75f, -6.75f, 0.0f, 15.75f, -15.5f, 0.0f,
    18.0f, -19.0f, 0.0f, 13.5f, -19.0f, 0.0f
    };

/*
 * Indices give 2 4-sided figures and 1 3-sided figure.
 * For each 4-sided figure, the 0-2 side is the internal side. 
 */
static const GLubyte indices[] =
    {
    2, 1, 0, 0, 3, 2, 6, 5, 4, 4, 7, 6, 10, 9, 8
    };

/* Indicates that VxWorks logo has 2 4-sided shapes and one 3-sided shape. */
static const unsigned int logoShapes[] =
    {
    4, 4, 3
    };

static const GLubyte indicesTriangles[] = { 2, 1, 0 };

static const unsigned int shapesTriangles[] = { 3 };

static GLfloat *vVerticesExtruded = NULL;
static GLubyte *indicesExtruded = NULL;

static GLfloat perspectiveMatrix[16];
static GLfloat shadowMatrix[16];
static GLfloat pushawayVector[4];
static GLfloat shadowTranslate[4];
static GLfloat perspectiveTranslateVec[4];

#define VPOSITION_ELEMENTS 3
#define VCOLOR_ELEMENTS 4

static const unsigned int numVertices = sizeof (vVertices) / sizeof (vVertices[0]) / VPOSITION_ELEMENTS;
static const unsigned int numIndices = sizeof (indices) / sizeof (indices[0]);

static unsigned int numVerticesExtruded = 0;
static unsigned int numIndicesExtruded = 0;

static const char g_strVertexShaderLogo[] =
    "uniform mat4 rotation;\n"
    "uniform mat4 perspective;\n"
    "uniform vec4 pushaway;\n"
    "uniform vec3 u_LightPos;\n"
    "attribute vec4 position;\n"
    "attribute vec4 normal;\n"
    "vec4 a_Color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "vec4 rotated_position;\n"
    "vec4 rotated_normal;\n"
    "varying vec4 v_Color;\n"
    "void main ()\n"
    "{\n"
    "   rotated_position = rotation * position;\n"
    "   rotated_normal = rotation * normal;\n"
    "   rotated_normal = normal;\n"
    "   vec3 lightVector = u_LightPos - vec3(rotated_position);\n"
    "   float distance = length (lightVector);\n"
    "   float diffuse = max(dot(normalize (lightVector), vec3(rotated_normal)), 0.1);\n"
    "   diffuse = diffuse * (1.0 / (1.0 + (0.05 * distance * distance)));\n"
    "   gl_Position = perspective *  (rotated_position + pushaway);\n"
    "   v_Color = a_Color * diffuse;\n"
    "}\n";

static const char g_strFragmentShaderLogo[] =
    "precision mediump float;\n"
    "varying vec4 v_Color;\n"
    "vec4 u_Color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "void main ()\n"
    "{\n"
    "   gl_FragColor = v_Color;\n"
    "}\n";

static const char g_strVertexShaderShadow[] =
    "uniform mat4 proj;\n"
    "uniform mat4 perspective;\n"
    "uniform vec4 pushaway;\n"
    "uniform mat4 shadowMatrix;\n"
    "uniform vec4 u_shadowTranslation;\n"
    "attribute vec4 position;\n"
    "vec4 a_Color = vec4(0.8, 0.8, 0.8, 1.0);\n"
    "vec4 rotated_position;\n"
    "void main ()\n"
    "{\n"
    "   rotated_position = proj * position;\n"
    "   vec4 shadowPosition =  ((shadowMatrix * rotated_position) + u_shadowTranslation);\n"
    "   vec3 shadowDistance = vec3(shadowPosition) - vec3(rotated_position);\n"
    "   gl_Position = perspective * (shadowPosition + pushaway);\n"
    "}\n";

static const char g_strFragmentShaderShadow[] =
    "precision lowp float;\n"
    "varying vec4 v_Color;\n"
    "vec4 u_Color = vec4(0.75, 0.75, 0.75, 1.0);\n"
    "void main ()\n"
    "{\n"
    "   gl_FragColor = u_Color;\n"
    "}\n";

/**************************************************************************
 *
 * es2LogoPrint - print the configuration of the graphics library
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
static void es2LogoPrint
    (
    ES2LOGO_SCENE* pDemo,
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

/**************************************************************************
 *
 * es2LogoProgramCreate - create the shader program
 *
 * This routine creates the shader program
 *
 */
static int es2LogoProgramCreate
    (
    const char* vertexShader,
    const char* fragmentShader,
    GLuint*     attribIndexArray,
    GLchar**    attribNameArray,
    GLubyte     numAttribs,
    GLuint*     programPtr
    )
    {
    GLuint vShader, fShader;
    int i;

    (void)printf ("Compiling vertex shader\n");

    /* Compile the shaders*/
    vShader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vShader, 1, &vertexShader, NULL);
    glCompileShader (vShader);

    /* Check for compile success*/
    GLint nCompileResult = 0;
    glGetShaderiv (vShader, GL_COMPILE_STATUS, &nCompileResult);
    if (0 == nCompileResult)
        {
        char  strLog[1024];
        GLint nLength;
        glGetShaderInfoLog (vShader, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        glDeleteShader (vShader);
        return 1;
        }

    (void)printf ("Compiling fragment shader\n");

    fShader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fShader, 1, &fragmentShader, NULL);
    glCompileShader (fShader);

    /* Check for compile success*/
    glGetShaderiv (fShader, GL_COMPILE_STATUS, &nCompileResult);
    if (0 == nCompileResult)
        {
        char  strLog[1024];
        GLint nLength;
        glGetShaderInfoLog (fShader, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        glDeleteShader (vShader);
        glDeleteShader (fShader);
        return 1;
        }

    /* Attach the individual shaders to the common shader program*/
    *programPtr = glCreateProgram ();
    glAttachShader (*programPtr, vShader);
    glAttachShader (*programPtr, fShader);

    (void)printf ("Linking shader\n");

    /* Link the vertex shader and fragment shader together*/
    glLinkProgram (*programPtr);

    /* Check for link success*/
    GLint nLinkResult = 0;
    glGetProgramiv (*programPtr, GL_LINK_STATUS, &nLinkResult);
    if (0 == nLinkResult)
        {
        char strLog[1024];
        GLint nLength;
        glGetProgramInfoLog (*programPtr, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        glDeleteShader (vShader);
        glDeleteShader (fShader);
        glDeleteProgram (*programPtr);
        return 1;
        }

    /* Bind the attribute names */
    for (i = 0; i < numAttribs; i++)
        {
        glBindAttribLocation (*programPtr, attribIndexArray[i], attribNameArray[i]);
        }

    return 0;
    }

/**************************************************************************
 *
 * es2ShadowProgramInit - initialize the shadow shader program
 *
 * This routine initializes the shadow shader program
 *
 */
static int es2ShadowProgramInit
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    /* Set up so that back faces are culled and depth testing is performed 
     * from -1.0. 
     */
    GLuint attribIndexArray[] = { 0 };
    GLchar* attribNameArray[] = { (GLchar*)"vPosition" };

    if (es2LogoProgramCreate (g_strVertexShaderShadow,
            g_strFragmentShaderShadow, attribIndexArray, attribNameArray, 1,
            &(pDemo->shadowProgram.program)))
        {
        return 1; 
        }

    glEnableVertexAttribArray (GL_BIND_ATTRIB_LOCATION_VPOSITION);
    glEnableVertexAttribArray (1);

    pDemo->shadowProgram.rotation_index = glGetUniformLocation (pDemo->shadowProgram.program, "proj");
    pDemo->shadowProgram.shadow_index = glGetUniformLocation (pDemo->shadowProgram.program, "shadowMatrix");
    pDemo->shadowProgram.shadowTranslate_index = glGetUniformLocation (pDemo->shadowProgram.program, "u_shadowTranslation");
    pDemo->shadowProgram.perspective_index = glGetUniformLocation (pDemo->shadowProgram.program, "perspective");
    pDemo->shadowProgram.perspectiveTranslate_index = glGetUniformLocation (pDemo->shadowProgram.program, "perspectiveTranslate");
    pDemo->shadowProgram.pushaway_index = glGetUniformLocation (pDemo->shadowProgram.program, "pushaway");

    mathHelperShadowCastMatrix(shadowMatrix, lightVector, -1.2f);
    /* The right column of the shadow matrix is the required translation.
     * However, this column doesn't seem to translate the images.
     * So we're sending in an extra size 4 vector for the translation.
     */
    shadowTranslate[0] = shadowMatrix[3];
    shadowTranslate[1] = shadowMatrix[7];
    shadowTranslate[2] = shadowMatrix[11];
    shadowTranslate[3] = 0.0f;
    shadowMatrix[3] = 0.0f;
    shadowMatrix[7] = 0.0f;
    shadowMatrix[11] = 0.0f;
    shadowMatrix[15] = 1.0f;

    return 0;
    }

/**************************************************************************
 *
 * es2LogoProgramInit - initialize the logo shader program
 *
 * This routine initializes the logo shader program
 *
 */
static int es2LogoProgramInit
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    GLuint attribIndexArray[] = { 0, 1 };
    GLchar* attribNameArray[] = { (GLchar*)"vPosition", (GLchar*)"normal" };

    if (es2LogoProgramCreate (g_strVertexShaderLogo,
            g_strFragmentShaderLogo, attribIndexArray, attribNameArray, 2,
            &(pDemo->logoProgram.program)))
        {
        return 1; 
        }

    glEnableVertexAttribArray (GL_BIND_ATTRIB_LOCATION_VPOSITION);
    glEnableVertexAttribArray (1);

    pDemo->logoProgram.rotation_index = glGetUniformLocation (pDemo->logoProgram.program, "rotation");
    pDemo->logoProgram.perspective_index = glGetUniformLocation (pDemo->logoProgram.program, "perspective");
    pDemo->logoProgram.pushaway_index = glGetUniformLocation (pDemo->logoProgram.program, "pushaway");
    pDemo->logoProgram.light_index = glGetUniformLocation (pDemo->logoProgram.program, "u_LightPos");

    return 0;
    }

/**************************************************************************
 *
 * es2LogoVxWorksInit - initialize the logo
 *
 * This routine initializes the logo
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 */
static int es2LogoVxWorksInit
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    /* Extrude the shape. */
    if (mathHelperExtrudeShape (vVertices, indices, logoShapes, 
            3, /* 3 pieces in the VxWorks logo */ 
            3.0f, /* Extrusion length */
            &vVerticesExtruded, &indicesExtruded, &numVerticesExtruded,
            &numIndicesExtruded))
        {
        return 1;
        }

    glGenBuffers (1, &(pDemo->indicesBufferIndex));
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, pDemo->indicesBufferIndex);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, numIndicesExtruded * sizeof (GLubyte),
            indicesExtruded, GL_STATIC_DRAW);
    
    /* The logo is set to fit from 0,0 to 23, -23.  
     * Scale it to fit inside -1,1 to 1,1. */
    mathHelperScaleStride (vVerticesExtruded, numVerticesExtruded, 7, 1.0f / 11.5f,
            1.0f / 11.5f, 1.0f / 11.5f);
    mathHelperTranslateStride (vVerticesExtruded, numVerticesExtruded, 7, -1.0f,
            1.0f, 0.0f);

#define RESHAPE    
#ifdef RESHAPE
    /* Reshape the logo according to the screen dimensions */
    if (pDemo->surfWidth < pDemo->surfHeight)
        {
        GLfloat scaleY = (GLfloat) (pDemo->surfWidth)
                / (GLfloat) (pDemo->surfHeight);
        mathHelperScaleStride (vVerticesExtruded, numVerticesExtruded, 7, 1.0f,
                scaleY, 1.0f);
        }
    else
        {
        /*width is more than height, so scale the x value */
        GLfloat scaleX = (GLfloat) (pDemo->surfHeight)
                / (GLfloat) (pDemo->surfWidth);
        mathHelperScaleStride (vVerticesExtruded, numVerticesExtruded, 7, scaleX,
                1.0f, 1.0f);
        }
#endif /*RESHAPE */

    glGenBuffers (1, &(pDemo->verticesBufferIndex));
    glBindBuffer (GL_ARRAY_BUFFER, pDemo->verticesBufferIndex);
    glBufferData (GL_ARRAY_BUFFER,
            numVerticesExtruded * VPOSITIONNORMAL_ELEMENTS * sizeof (GLfloat),
            vVerticesExtruded, GL_STATIC_DRAW);
    /* Position attribute */
    glVertexAttribPointer (GL_BIND_ATTRIB_LOCATION_VPOSITION, 4,
            GL_FLOAT, GL_FALSE, VPOSITIONNORMAL_ELEMENTS * sizeof (GLfloat), NULL);
    /* Normal attribute */
    glVertexAttribPointer (1, 3,
            GL_FLOAT, GL_FALSE, VPOSITIONNORMAL_ELEMENTS * sizeof (GLfloat),
            (GLfloat *) 0 + 4);

    return 0;
    }

/*******************************************************************************
 *
 * es2LogoCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2LogoEglDeinit
 *
 */
static void es2LogoCleanup
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    if (vVerticesExtruded != NULL)
        {
        free (vVerticesExtruded);
        vVerticesExtruded = NULL;
        }
    if (indicesExtruded != NULL)
        {
        free (indicesExtruded);
        indicesExtruded = NULL;
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
 * es2LogoEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the ES2LOGO_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2LogoEglInit
 *
 */
static void es2LogoEglDeinit
    (
    ES2LOGO_SCENE* pDemo
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

    es2LogoCleanup (pDemo);
    }

#if defined(GFX_USE_GBM)
/*******************************************************************************
*
* es2LogoFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int es2LogoFindDrmFb
    (
    ES2LOGO_SCENE* pDemo
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
 * es2LogoEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to ES2LOGO_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2LogoEglDeinit
 *
 */
static ES2LOGO_SCENE* es2LogoEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    ES2LOGO_SCENE* pDemo;
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
    pDemo = (ES2LOGO_SCENE*)calloc (1, sizeof (ES2LOGO_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;
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
            es2LogoEglDeinit (pDemo);
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
        es2LogoEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        es2LogoEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        if (es2LogoFindDrmFb (pDemo))
            {
            es2LogoEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_ES_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        es2LogoEglDeinit (pDemo);
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
        es2LogoEglDeinit (pDemo);
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
            es2LogoEglDeinit (pDemo);
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
        es2LogoEglDeinit (pDemo);
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
        es2LogoEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        es2LogoEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        es2LogoPrint (pDemo, eglMajor, eglMinor);
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
            es2LogoEglDeinit (pDemo);
            return (NULL);
            }

        handle = gbm_bo_get_handle (gbm_bo).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            es2LogoEglDeinit (pDemo);
            return (NULL);
            }

        /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
        if (drmModeAddFB (pDemo->drmDevFd,
                          pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                          24, 32, (((pDemo->mode.hdisplay << 2) + 511) & ~511),
                          handle, &(pDemo->fbId)))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            es2LogoEglDeinit (pDemo);
            return (NULL);
            }

        if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                            0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
            {
            es2LogoEglDeinit (pDemo);
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
 * es2LogoDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2LogoInit
 *
 */
static void es2LogoDeinit
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    if (pDemo->indicesBufferIndex)
        {
        glDeleteBuffers (1, &pDemo->indicesBufferIndex);
        }
    if (pDemo->verticesBufferIndex)
        {
        glDeleteBuffers (1, &pDemo->verticesBufferIndex);
        }

    if (pDemo->shadowProgram.program)
        {
        glDeleteProgram (pDemo->shadowProgram.program);
        glUseProgram (0);
        }
    if (pDemo->logoProgram.program)
        {
        glDeleteProgram (pDemo->logoProgram.program);
        glUseProgram (0);
        }
    }

/*******************************************************************************
 *
 * es2LogoInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2LogoDeinit
 *
 */
static int es2LogoInit
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    if (es2LogoVxWorksInit (pDemo))
        {
        return 1; 
        }

    if (es2LogoProgramInit (pDemo))
        {
        return 1;
        }

    if (es2ShadowProgramInit (pDemo))
        {
        return 1;
        }

    glClearColor (0.8f, 0.8f, 0.8f, 0.0f);
    glEnable (GL_DEPTH_TEST);

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    /* Set up for perspective */
    mathHelperPerspectiveMatrix(perspectiveMatrix,
            1.5f, 20.0f, 0.8f, 0.8f);

    pushawayVector[0] = 0.0f;
    pushawayVector[1] = -0.0f;
    pushawayVector[2] = -3.5f;
    pushawayVector[3] = 0.0f;

    return 0; 
    }

/*******************************************************************************
 *
 * es2LogoRender - render the application
 *
 * This routine renders the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static int es2LogoRender
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    GLfloat rotationmatrix[] =
        {
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0
        };
    static GLfloat rotationAngle = 0.0;
    static int rotationDirection = 1;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Rotate around the Y axis. */
    mathHelperRotateRollPitch (rotationmatrix, 0, rotationAngle);

    /* Rotate around the Y axis. */
    mathHelperRotateRollPitch (rotationmatrix, rotationAngle, 0);

    if (rotationDirection)
        {
        rotationAngle += pDemo->angle;
        }
    else
        {
        rotationAngle -= pDemo->angle;
        }

    if (rotationAngle >= 20.0)
        {
        rotationDirection = 0;
        }
    else if (rotationAngle <= 0.0)
        {
        rotationDirection = 1;
        }

    /* Draw the shadow. */
    glUseProgram (pDemo->shadowProgram.program);
    glUniformMatrix4fv (pDemo->shadowProgram.rotation_index, 1, GL_FALSE, rotationmatrix);
    glUniformMatrix4fv (pDemo->shadowProgram.perspective_index, 1, GL_FALSE, perspectiveMatrix);
    glUniform4fv (pDemo->shadowProgram.pushaway_index, 1, pushawayVector);
    glUniformMatrix4fv (pDemo->shadowProgram.shadow_index, 1, GL_FALSE, shadowMatrix);
    glUniform4fv (pDemo->shadowProgram.shadowTranslate_index, 1, shadowTranslate);
    glUniform4fv (pDemo->shadowProgram.perspectiveTranslate_index, 1, perspectiveTranslateVec);
    glDrawElements (GL_TRIANGLES, numIndicesExtruded, GL_UNSIGNED_BYTE, 0);

    /* Draw the actual logo. */
    glUseProgram (pDemo->logoProgram.program);
    glUniformMatrix4fv (pDemo->logoProgram.rotation_index, 1, GL_FALSE, rotationmatrix);
    glUniformMatrix4fv (pDemo->logoProgram.perspective_index, 1, GL_FALSE, perspectiveMatrix);
    glUniform4fv (pDemo->logoProgram.pushaway_index, 1, pushawayVector);
    glUniform3fv (pDemo->logoProgram.light_index, 1, lightVector);
    glDrawElements (GL_TRIANGLES, numIndicesExtruded, GL_UNSIGNED_BYTE, 0);

    return 0;
    }

/*******************************************************************************
 *
 * es2LogoPerfCount - calculate and show performance
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
static void es2LogoPerfCount
    (
    ES2LOGO_SCENE* pDemo
    )
    {
    long t;
    struct timeval curtime;

    (void)gettimeofday (&curtime, NULL);
    t = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);

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
 * es2LogoGetChar - gets an ASCII character from keyboard
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
static unsigned short es2LogoGetChar
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
 * gfxEs2LogoDemo - core functionality of the program
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
static int gfxEs2LogoDemo
    (
    unsigned int device,
    int runTime,
    unsigned int eglDriver,
    GLfloat angle
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    ES2LOGO_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = es2LogoEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "es2LogoEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (es2LogoInit (pDemo))
        {
        (void)fprintf (stderr, "es2LogoInit failed\n");
        es2LogoDeinit (pDemo);
        es2LogoEglDeinit (pDemo);
        return (EXIT_FAILURE);
        }

    pDemo->angle = angle? (angle/1000.0f) : 0.01f;

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
            unsigned short character = es2LogoGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        es2LogoPerfCount (pDemo);

        if (es2LogoRender (pDemo))
            {
            (void)fprintf (stderr, "es2LogoRender failed\n");
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
    es2LogoDeinit (pDemo);
    es2LogoEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
