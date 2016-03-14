/* gfxEs2MultiShaderTest.inl - OpenGL ES2 Shaders Test */

/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
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
28jan15,yat  Create shader test (US46449)
*/

/*

DESCRIPTION

This example program provides the shader program.

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
#if defined(_FSLVIV)
#include <gc_hal_base.h>
#endif
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
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
#include <gfxKmsHelper.inl>
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
#include <evdevLib.h>
#endif

/* defines */

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

#if 0
#define PRINTF          (void)printf
#else
#define PRINTF(...)     do {} while (0)
#endif

/* typedefs */

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

typedef struct
    {
    GLfloat              *vertices;
    GLushort             *indices;
    GLuint               vbo;
    GLint                count;
    } ES2_MULTISHADER_DEF;

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
    GLint                idTime;
    long                 tRate0;
    long                 frames;
    } ES2_MULTISHADER_SCENE;

static const char *es2MultiShaderFragmentShader = "#pragma debug(on)\n\n#ifdef GL_ES\nprecision highp float;\n#endif\nconst vec2 resolution = vec2(800.0, 480.0);\nuniform float glmm_Time;\nuniform vec2 mouse;\n\n//Declare functions\nvec2 ObjUnion(in vec2 d1,in vec2 d2);\nvec2 floorPlane(in vec3 p);\nvec3 color_checkers(in vec4\n                    p);\nvec2 roundBox(in vec3 p);\nvec2 sdBox( vec3 p, vec3 b );\nvec3 color_white(in vec3 p);\nvec2 distanceField(in vec3 p);\nvec2 simpleBuilding (vec3 p, vec3 b );\nvec4 applyFog (in vec4 currColor, in vec3 ray);\nfloat maxcomp(in vec3 p );\nvec2 infiniteBuildings(in vec3 p);\nfloat sdCross( in vec3 p );\nvec2 sidewalk(vec3 p);\nvec2 tallBuilding (vec3 p, vec3 b );\nvec2 infiniteTallBuildings(in vec3 p);\nvec3 color_brick(in vec3 p);\n\n\n\n#define EPS 0.01\n#define INF 100000.0\n\n#define PHONG_SHADING 1\n#define RAYMARCH_SHADING 1\n#define TEST_SHADING 0\n\n#define SPINNING_CAMERA 0\n#define MOUSE_CAMERA 1\n#define PAN_CAMERA 2\n#define STILL_CAMERA 3\n#define AUTOPAN_CAMERA 4\n\n// mode selection\nconst int SHADING_MODE = TEST_SHADING;\nconst int CAMERA_MODE = AUTOPAN_CAMERA;\nvec3 E;\n\n// some simple colors\nconst vec3 COLOR_GREY = vec3(0.2,0.2,0.2);\nconst vec3 COLOR_WHITE = vec3(1.0,1.0,1.0);\nconst vec3 COLOR_BLACK = vec3(0);\nconst vec3 COLOR_WINDOW = vec3(0,0.4,0.55);\n\n//============================== UTILS ====================================//\nvec2 distanceField(in vec3 p){\n        return ObjUnion(floorPlane(p),infiniteBuildings(p)); // infinite boxes\n}\n\nvec2 ObjUnion(in vec2 d1,in vec2 d2){\n        if (d1.x<d2.x)\n        {\n            return d1;\n        }\n        else\n        return d2;\n}\n\n// http://www.ozone3d.net/blogs/lab/20110427/glsl-random-generator/\nfloat rand(vec2 n)\n{\n        return 0.5 + 0.5 *\n        fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);\n}\n\n// from IQ\nfloat maxcomp(in vec3 p ) { return max(p.x,max(p.y,p.z));}\n\n\n\n// =============================== OBJECTS =======================================//\n// CREDIT: http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm //\n\n//floorPlane (color is determined by y-component, ie 0.0)\nvec2 floorPlane(in vec3 p){\n        return vec2(p.y+2.0,0);\n}\n\n// ROUNDBOX (try other objects )\n//(color is determined by y-component, ie 1.0)\nvec2 roundBox(in vec3 p){\n        return vec2(length(max(abs(p)-vec3(1,1,1),0.0))-0.25,1);\n}\n\n// SIGNED BOX\nvec2 sdBox( vec3 p, vec3 b ){\n  vec3  di = abs(p) - b;\n  float mc = maxcomp(di);\n  return vec2(min(mc,length(max(di,0.0))), 1);\n}\n\n\n// INFINITE SIMPLE BUILDINGS RANDOm (CREDIT: H3R3)\nvec2 infiniteBuildings(in vec3 pentry2)\n{\n        vec3 cValue = vec3(5.0, 5.0, 5.0); // how close cubes are to each other\n\n        vec3 q = pentry2;\n        //repetition in x and z direction\n        q.x = mod(pentry2.x,cValue.x)-0.5*cValue.x;\n        q.z = mod(pentry2.z,cValue.z)-0.5*cValue.z;\n\n        vec2 pos = vec2(ceil(pentry2.x/cValue.x), ceil(pentry2.z/cValue.z));\n\n        float height = rand(pos)*5.0;//* 10.0 - 4.0;\n        float width1 = rand(pos + 100.0) + 0.5;\n        float width2 = rand(pos + 1500.0) + 0.5;\n\n        //building height\n        //vec3 k = vec3(1,height,1);\n        height = height + height * 0.1;//snoise(vec2((width1 * cos(glmm_Time/5.)), width2 * sin(glmm_Time/4.) ) );\n        vec3 k = vec3(max(0.0, width1), max(0.0, height), max(0.0, width2));\n        return simpleBuilding(q,k);\n}\n\n\n// SIMPLE BUILDING (white)\nvec2 simpleBuilding (in vec3 pentry, in vec3 b ){\n        float body = sdBox(pentry,b).x;\n\n        vec3 q = pentry;\n        vec3 cEntry = vec3(0.5, 0.5, 0.5); //0.5\n\n        q = mod(pentry,cEntry)-0.5*cEntry;\n\n        float cr = sdCross(q*3.0)/3.0;\n        body = max( body, -cr );\n\n        // top \"cap\" of building\n        float top = sdBox(pentry-vec3(0,b.y,0),vec3(b.x, b.y/25.0, b.z)).x;\n\n        // some buildings have an additional top that's slightly smaller\n        if(fract(b.y/2.0) < 0.5){\n                float c = fract(b.y)<0.2? 10.0:fract(b.y)*30.0; //if top portion is small enough, make it longer\n                float toptop = sdBox(pentry-vec3(0,b.y+b.y/25.0,0),vec3(b.x*fract(b.y), b.y/c, b.z*fract(b.y))).x;\n                body = min(body,toptop);\n        }\n\n        // make a box inside to look like windows\n        float inside = sdBox(pentry,vec3(b.x*0.9, b.y, b.z*0.9)).x;\n        body = min(inside,min(body,top));\n\n        float outputColor = b.y;\n        if(body==inside)\n                outputColor = 99.0; // if inside is hit, shade with window color (99.0 is just placeholder value)\n\n\n  return vec2(body,outputColor);\n}\n\n\n\n// INFINITE TALL BUILDINGS RANDOM\nvec2 infiniteTallBuildings(in vec3 pentry3){\n\n        vec3 cValue = vec3(23,0,23); // how close cubes are to each other\n\n        vec3 q = pentry3;\n        //repetition in x and z direction\n        q.x = mod(pentry3.x,cValue.x)-0.5*cValue.x;\n        q.z = mod(pentry3.z,cValue.z)-0.5*cValue.z;\n\n        vec2 pos = vec2(ceil(pentry3.x/cValue.x), ceil(pentry3.z/cValue.z));\n\n        float height = rand(pos)*20.0;\n        if (fract(height)/2.0 < 0.4) //adding some variation in height\n                height = height/2.0;\n\n        float width1 = rand(pos + 80.0) + 0.5;\n        float width2 = rand(pos + 500.0) + 0.5;\n\n        //building height\n\n        vec3 k = vec3(max(0.0, width1), max(0.0, height), max(0.0, width2));\n        return tallBuilding(q,k);\n}\n\n\n\n//TALL BUILDING (WHITE)\nvec2 tallBuilding (vec3 p, vec3 b ){\n        float body = sdBox(p,b).x;\n\n        vec3 q = p;\n        vec3 c = vec3(0.5);\n        q = mod(p,c)-0.5*c;\n        float vert_bars = sdBox(q,vec3(0.1,INF,0.1)).x;\n        body = max(body, -vert_bars);\n\n        //dividng ledge\n        const float ledgeheight = 0.2;\n        float ledge = sdBox(p-vec3(0,b.y,0),vec3(b.x,ledgeheight,b.z)).x;\n        body = min(body,ledge);\n\n        //2nd portion\n        float body2 = sdBox(p-vec3(0,b.y+ledgeheight,0),b*vec3(0.8,0.2,0.8)).x;\n        body = min(body,body2);\n\n        //3rd portion\n        float body3 = sdBox(p-vec3(0,b.y+ledgeheight+b.y*0.2, 0), b*vec3(0.6,0.2,0.6)).x;\n        body = min(body3, body);\n\n\n        return vec2(body,1);\n}\n\n// SD_CROSS (modified from IQ's original)\nfloat sdCross( in vec3 p ){\n        const float w = 0.4;\n  float da = sdBox(p.xyz,vec3(INF,w,w)).x;\n  float db = sdBox(p.yzx,vec3(w,INF,w)).x;\n  float dc = sdBox(p.zxy,vec3(w,w,INF)).x;\n  return min(da,db);\n}\n\n\n\n// ============COLORS============= //\n// Checkerboard Color\nvec3 color_checkers(in vec3 p){\n        if (fract(p.x*.5)>.5)\n        if (fract(p.z*.5)>.5)\n        return COLOR_GREY;\n        else\n        return vec3(1,1,1);\n        else\n        if (fract(p.z*.5)>.5)\n        return vec3(1,1,1);\n        else\n        return COLOR_GREY;\n}\n\n//Brick Color\nvec3 color_brick(in vec3 p){\n        const vec3 brickColor = vec3(0.2,0.2,0.2);\n        const vec3 mortarColor = vec3(0.8);\n        const vec2 brickSize = vec2(0.3,0.15);\n        const vec2 brickPct = vec2(0.9,0.85);\n\n        vec2 position = (p.zy)/brickSize;\n        vec2 useBrick = vec2(0);\n\n        if(fract(position.y*0.5) > 0.5)\n                position.x += 0.5;\n\n        position = fract(position);\n        useBrick = step(position, brickPct);\n\n        vec3 color =  mix(mortarColor, brickColor, useBrick.x*useBrick.y);\n\n\n        position = p.xy/brickSize;\n        if(fract(position.y*0.5) > 0.5)\n                position.x += 0.5;\n        position = fract(position);\n        useBrick = step(position, brickPct);\n        color = (color+mix(mortarColor, brickColor, useBrick.x*useBrick.y))/2.0;\n\n        return color;\n\n}\n\n// Fog (credit: http://www.mazapan.se/news/2010/07/15/gpu-ray-marching-with-distance-fields/)\nvec4 applyFog (in vec4 currColor, in vec3 ray){\n        float rayLength = length(ray);\n        vec3 nRay = ray/rayLength;\n\n        float fogAmount = 1.0-exp(-rayLength * 0.02); //0.008\n        float sunAmount = 0.0;//pow( max( dot (nRay, sunDir), 0.0), 8.0);\n\n        vec4 fogColor = mix(vec4(0.5,0.6,0.7,1.0), vec4(1.0,0.9,0.7,1.0), sunAmount);\n        return mix(currColor, fogColor, fogAmount);\n}\n\nvarying vec2 texUV;\nvoid main(void){\n/*    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n    return;*/\n    //Camera animation\n    vec3 U=vec3(0,1,0);//Camera Up Vector\n    vec3 viewDest=vec3(0,0,0); //Change camere view vector here\n    //vec3 E; //moved to global space\n    E=vec3(-sin(1.0)*10.0,7,cos(1.0)*10.0);\n    vec3 moveCamDir = normalize(vec3(E.x,0.0,E.y));\n    E+=moveCamDir*glmm_Time;\n\n\n    //Camera setup\n    vec3 C=normalize(viewDest-E);\n    vec3 A=cross(C, U);\n    vec3 B=cross(A, C);\n    vec3 M=(E+C);\n\n    //vec2 vPos=2.0*gl_FragCoord.xy/resolution.xy - 1.0; // = (2*Sx-1) where Sx = x in screen space (between 0 and 1)\n\tvec2 vPos=texUV * 0.5;\n    vec3 P=M + vPos.x*A*resolution.x/resolution.y + vPos.y*B; //normalize resolution in either x or y direction (ie resolution.x/resolution.y)\n    vec3 rayDir=normalize(P-E); //normalized direction vector from Eye to point on screen\n\n    //Colors\n    const vec4 skyColor = vec4(0.7, 0.8, 1.0, 1.0);\n    const vec4 sunColor = vec4 (1.0, 0.9, 0.7, 1.0);\n\n    //Raymarching\n    const vec3 e=vec3(0.1,0.0,0.0);\n    const float MAX_DEPTH=170.0; //Max depth use 500\n    const int MAX_STEPS = 100; // max number of steps use 150\n    const float MIN_DIST = 0.01;\n\n    vec2 dist=vec2(0.0,0.0);\n    float totalDist=0.0;\n    vec3 c,p,n; //c=color (used in PHONG and RAYMARCH modes), p=ray position, n=normal at any point on the surface\n\n    int steps = 0;\n    for(int i=0;i<MAX_STEPS;i++){\n            steps++;\n            totalDist+=dist.x*0.7; //use smoothing constant\n            p=E+rayDir*totalDist; // p = eye + total_t*rayDir\n            dist=distanceField(p);\n            if (abs(dist.x)<MIN_DIST) break; // break when p gets sufficiently close to object or exceeds max dist\n    }\n\n    vec4 finalColor = skyColor;\n\n    if (totalDist<MAX_DEPTH){\n            // check which color to use via the y-component\n            if (dist.y==0.0) // floorPlane color\n            c=color_checkers(p);\n            else if(dist.y==1.0) // building color\n            c=COLOR_WHITE;\n\n            {\n                    vec3 sunDir = vec3(normalize(viewDest-E)); //sun comes from the camera\n\n                    vec3 N = normalize(vec3(\n                    distanceField(p).x-distanceField(p-e.xyy).x,\n                    distanceField(p).x-distanceField(p-e.yxy).x,\n                    distanceField(p).x-distanceField(p-e.yyx).x)); //normal at point\n\n                    vec3 L = sunDir;\n                    vec3 V = normalize(E-p);\n\n                    // color info is stored in y component\n\n                    if(fract(dist.y) < 0.5) // building color (half of the buildings are brick)\n                            finalColor=vec4(color_brick(p),1.0);\n                    if(fract(dist.y) >= 0.5)\n                            finalColor = vec4(COLOR_GREY,1.0);\n                    if (dist.y==0.0) // floorPlane color\n                            finalColor=vec4(COLOR_BLACK,1.0);\n                    if (dist.y == 99.0)\n                            finalColor = vec4(COLOR_WINDOW,1);\n\n\n                    //calculate lighting: diffuse + sunlight\n                    float diffuseTerm = clamp(dot(V,N), 0.0, 1.0);\n                    finalColor = mix(finalColor, sunColor, diffuseTerm*0.55);\n\n            }\n    }\n    //gl_FragColor.rgba = finalColor;\n\n    //apply fog\n    vec3 r = p-E;\n    finalColor = applyFog(finalColor, r);\n    gl_FragColor = finalColor;\n\n\n}\n\n";

static const char *es2MultiShaderVertexShader = "#pragma debug(on)\n\n#ifdef GL_ES\nprecision highp float;\n#endif\nattribute vec2 glmm_attTexCoord0;\nattribute vec4 glmm_attVertex;\nvarying vec2 texUV;\n\nvoid main(void)\n{ \n\ttexUV = glmm_attTexCoord0.xy;\n\tgl_Position = glmm_attVertex;\n}";

/*******************************************************************************
 *
 * es2MultiShaderCheckGL0 - check GL status
 *
 * This routine checks the GL status.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void es2MultiShaderCheckGL0
    (
    char const *file,
    int line
    )
    {
    GLenum status = glGetError();
    const char *str;

    if (status == GL_NO_ERROR)
        {
        return;
        }

    switch (status)
        {
        case GL_INVALID_ENUM:
            str = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            str = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            str = "GL_INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            str = "GL_OUT_OF_MEMORY";
            break;
        default:
            str = "Unknown";
            break;
        }

    (void)fprintf (stderr, "%s:%d:%s\n", file, line, str);
    }

#define es2MultiShaderCheckGL() es2MultiShaderCheckGL0(__FILE__, __LINE__)

/*******************************************************************************
 *
 * es2MultiShaderPrint - print the configuration of the graphics library
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
static void es2MultiShaderPrint
    (
    ES2_MULTISHADER_SCENE* pDemo,
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
 * es2MultiShaderCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2MultiShaderEglDeinit
 *
 */
static void es2MultiShaderCleanup
    (
    ES2_MULTISHADER_SCENE* pDemo
    )
    {
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
 * es2MultiShaderEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the ES2_MULTISHADER_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2MultiShaderEglInit
 *
 */
static void es2MultiShaderEglDeinit
    (
    ES2_MULTISHADER_SCENE* pDemo
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

    es2MultiShaderCleanup (pDemo);
    }

#if defined(GFX_USE_GBM)
/*******************************************************************************
*
* es2MultiShaderFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int es2MultiShaderFindDrmFb
    (
    ES2_MULTISHADER_SCENE* pDemo
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
 * es2MultiShaderEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to ES2_MULTISHADER_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2MultiShaderEglDeinit
 *
 */
static ES2_MULTISHADER_SCENE* es2MultiShaderEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    ES2_MULTISHADER_SCENE* pDemo;
    GLboolean   printInfo = GL_TRUE;
    EGLint      eglMajor, eglMinor;
    EGLint      configAttribs[] =
        {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BUFFER_SIZE, 32,
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
    pDemo = (ES2_MULTISHADER_SCENE*)calloc (1, sizeof (ES2_MULTISHADER_SCENE));
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
            es2MultiShaderEglDeinit (pDemo);
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
        es2MultiShaderEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        es2MultiShaderEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        if (es2MultiShaderFindDrmFb (pDemo))
            {
            es2MultiShaderEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_ES_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        es2MultiShaderEglDeinit (pDemo);
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
        es2MultiShaderEglDeinit (pDemo);
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
            es2MultiShaderEglDeinit (pDemo);
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
        es2MultiShaderEglDeinit (pDemo);
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
        es2MultiShaderEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        es2MultiShaderEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        es2MultiShaderPrint (pDemo, eglMajor, eglMinor);
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
            es2MultiShaderEglDeinit (pDemo);
            return (NULL);
            }

        handle = gbm_bo_get_handle (gbm_bo).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            es2MultiShaderEglDeinit (pDemo);
            return (NULL);
            }

        /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
        if (drmModeAddFB (pDemo->drmDevFd,
                          pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                          24, 32, (((pDemo->mode.hdisplay << 2) + 511) & ~511),
                          handle, &(pDemo->fbId)))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            es2MultiShaderEglDeinit (pDemo);
            return (NULL);
            }

        if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                            0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
            {
            es2MultiShaderEglDeinit (pDemo);
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
 * es2MultiShaderDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2MultiShaderInit
 *
 */
static void es2MultiShaderDeinit
    (
    ES2_MULTISHADER_SCENE* pDemo
    )
    {
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
 * es2MultiShaderInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2MultiShaderDeinit
 *
 */
static int es2MultiShaderInit
    (
    ES2_MULTISHADER_SCENE* pDemo
    )
    {
    PRINTF ("Compiling vertex shader\n");

    /* Compile the shaders*/
    pDemo->vertexShader = glCreateShader (GL_VERTEX_SHADER);
    es2MultiShaderCheckGL ();
    glShaderSource (pDemo->vertexShader, 1, &es2MultiShaderVertexShader, NULL);
    es2MultiShaderCheckGL ();
    glCompileShader (pDemo->vertexShader);
    es2MultiShaderCheckGL ();

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

    PRINTF ("Compiling fragment shader\n");

    pDemo->fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
    es2MultiShaderCheckGL ();
    glShaderSource (pDemo->fragmentShader, 1, &es2MultiShaderFragmentShader, NULL);
    es2MultiShaderCheckGL ();
    glCompileShader (pDemo->fragmentShader);
    es2MultiShaderCheckGL ();

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
    es2MultiShaderCheckGL ();
    glAttachShader (pDemo->shaderProgram, pDemo->vertexShader);
    es2MultiShaderCheckGL ();
    glAttachShader (pDemo->shaderProgram, pDemo->fragmentShader);
    es2MultiShaderCheckGL ();

    PRINTF ("Linking shader\n");

    /* Link the vertex shader and fragment shader together*/
    glLinkProgram (pDemo->shaderProgram);
    es2MultiShaderCheckGL ();

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
    es2MultiShaderCheckGL ();

    /* Bind attributes locations */
    glBindAttribLocation (pDemo->shaderProgram, 0, "glmm_attTexCoord0");
    es2MultiShaderCheckGL ();
    glBindAttribLocation (pDemo->shaderProgram, 1, "glmm_attVertex");
    es2MultiShaderCheckGL ();

    /* Get uniform locations */
    pDemo->idTime = glGetUniformLocation (pDemo->shaderProgram, "glmm_Time");
    es2MultiShaderCheckGL ();

    PRINTF ("Shader done\n");

    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    return 0;
    }

/*******************************************************************************
 *
 * es2MultiShaderRender - render the application
 *
 * This routine renders the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2MultiShaderRenderOne
 *
 */
static int es2MultiShaderRender
    (
    ES2_MULTISHADER_SCENE* pDemo
    )
    {
    es2MultiShaderDeinit (pDemo);

    if (es2MultiShaderInit (pDemo))
        {
        return 1;
        }

    GLfloat color = (GLfloat)(pDemo->frames % 2);

    glClearColor (color, 0.0f, 1.0f - color, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);

    return 0;
    }

/*******************************************************************************
 *
 * es2MultiShaderPerfCount - calculate and show performance
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
static void es2MultiShaderPerfCount
    (
    ES2_MULTISHADER_SCENE* pDemo
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
#if defined(_FSLVIV)
        gctSIZE_T contiguousSize = 0;
        gctPHYS_ADDR contiguousAddress = NULL;
        if (gcvSTATUS_OK == gcoHAL_QueryVideoMemory(
                                gcvNULL, gcvNULL, gcvNULL, gcvNULL, gcvNULL,
                                &contiguousAddress, &contiguousSize))
            {
            (void)fprintf (stdout, "contiguousAddress 0x%08x contiguousSize 0x%08x:%d\n",
                           contiguousAddress, contiguousSize, contiguousSize);
            }
#endif
        }
    }

#if defined(GFX_USE_EVDEV)
/*******************************************************************************
 *
 * es2MultiShaderGetChar - gets an ASCII character from keyboard
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
static unsigned short es2MultiShaderGetChar
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
 * gfxEs2MultiShaderTest - core functionality of the program
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
static int gfxEs2MultiShaderTest
    (
    unsigned int device,
    int runTime,
    unsigned int eglDriver
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    ES2_MULTISHADER_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = es2MultiShaderEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "es2MultiShaderEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (es2MultiShaderInit (pDemo))
        {
        (void)fprintf (stderr, "es2MultiShaderInit failed\n");
        es2MultiShaderDeinit (pDemo);
        es2MultiShaderEglDeinit (pDemo);
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
            unsigned short character = es2MultiShaderGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        es2MultiShaderPerfCount (pDemo);

        if (es2MultiShaderRender (pDemo))
            {
            (void)fprintf (stderr, "es2MultiShaderRender failed\n");
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
    es2MultiShaderDeinit (pDemo);
    es2MultiShaderEglDeinit (pDemo);
    (void)fprintf (stdout, "TEST COMPLETED\n");
    return (EXIT_SUCCESS);
    }
