/* gfxVgRaster.c - Wind River VG Raster Implementation */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
30mar15,rpc  Static analysis fixes (US50633)
22dec14,yat  Fix build warning for LP64 (US50456)
25jun13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
20may09,m_c  Written
*/

/*
DESCRIPTION
These routines provide the unpinning functionality for the software
implementation of Wind River's VG library.
*/

/* includes */

#include <errno.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* locals */

/* forward declaractions */
#ifdef SUPPORT_OPENVG
LOCAL void vgMakeCurrent (gc_t *, surface_t *, surface_t *, context_t *);
LOCAL void vgReleaseThread (gc_t *);
#endif

/* globals */

GC_DECL gc_t gfxVgGc;

/*******************************************************************************
 *
 * deleteGc - delete the graphics context for the current thread
 *
 */
void deleteGc
    (
    void
    )
    {
    register gc_t * pGc = &gfxVgGc; /* graphics context */

    /* Mark the context as invalid */
    pGc->valid = 0;
    }

/*******************************************************************************
 *
 * initGc - initialize the graphics context for the current thread
 *
 */
gc_t * initGc
    (
    void
    )
    {
    int      i, j, k;
    register gc_t * pGc = &gfxVgGc; /* graphics context */

    if (pGc->valid != 918273645)
        {
        /* Mark the context as valid */
        pGc->valid = 918273645;

        /* Bind the context to the current thread */
        pGc->threadId = (unsigned long)CURRENT_THREAD_ID;

        /* Initialize the EGL blob */
        pGc->egl.criteria = NULL;
        pGc->egl.api = API_NONE;
        pGc->egl.error = EGL_SUCCESS;

        for (i = API_OPENGL_ES; i <= API_OPENGL; i++)
            {
            pGc->egl.pContext[i] = NULL;
            pGc->egl.makeCurrent[i] = NULL;
            pGc->egl.releaseThread[i] = NULL;
            pGc->egl.wait[i] = NULL;
            }

#ifdef SUPPORT_OPENVG
        /* Initialize the OpenVG blob */
        pGc->vg.error = VG_NO_ERROR;
        pGc->vg.pDrawSurface = NULL;

        pGc->vg.clearColor[VG_R] = 0.0;
        pGc->vg.clearColor[VG_G] = 0.0;
        pGc->vg.clearColor[VG_B] = 0.0;
        pGc->vg.clearColor[VG_A] = 0.0;
        pGc->vg.pixelLayout = VG_PIXEL_LAYOUT_UNKNOWN;
        pGc->vg.tileFillColor[VG_R] = 0.0;
        pGc->vg.tileFillColor[VG_G] = 0.0;
        pGc->vg.tileFillColor[VG_B] = 0.0;
        pGc->vg.tileFillColor[VG_A] = 0.0;

        pGc->vg.enableScissoring = VG_FALSE;
        pGc->vg.scissoringData = malloc((MAX_SCISSOR_RECTS * sizeof(int)) +
                                        (RECT_POOL_SIZE * sizeof(rectangle_t)));
        pGc->vg.numScissorRects = 0;
        pGc->vg.pScissorRects = NULL;

        pGc->vg.matrixMode = VG_MATRIX_IMAGE_USER_TO_SURFACE;
        for (i = 0; i < 5; i++)
            {
            for (j = k = 0; j < 9; j++, k = (k + 4) % 12)
                pGc->vg.matrix[i][j] = (j == k) ? (1.0f) : (0.0f);
            }
        pGc->vg.pCurrentMatrix = pGc->vg.matrix[MATRIX_INDEX(pGc->vg.matrixMode)];

        pGc->vg.fillRule = VG_EVEN_ODD;
        pGc->vg.flatness = FLATNESS;
        pGc->vg.pPaths = NULL;
        pGc->vg.pTess = gluNewTess();

        pGc->vg.imageMode = VG_DRAW_IMAGE_NORMAL;
        pGc->vg.imageQuality = VG_IMAGE_QUALITY_NONANTIALIASED;
        pGc->vg.pImages = NULL;

        pGc->vg.glyphOrigin[0] = 0.0;
        pGc->vg.glyphOrigin[1] = 0.0;
        pGc->vg.glyphOrigin[2] = 1.0;
        pGc->vg.pFonts = NULL;
        pGc->vg.textMode = FALSE;

        pGc->vg.pFillPaint = NULL;
        pGc->vg.pStrokePaint = NULL;
        pGc->vg.pPaints = NULL;

        pGc->vg.filterChannelMask = VG_RED | VG_GREEN |VG_BLUE | VG_ALPHA;
        pGc->vg.filterLinear = VG_FALSE;
        pGc->vg.filterPremultiplied = VG_FALSE;

        /* Set EGL callbacks */
        pGc->egl.makeCurrent[API_OPENVG] = vgMakeCurrent;
        pGc->egl.releaseThread[API_OPENVG] = vgReleaseThread;
        pGc->egl.wait[API_OPENVG] = NULL;
#endif /* SUPPORT_OPENVG */
    }

    /* Backup errno */
    pGc->error = errno;

    return (pGc);
    }

#ifdef SUPPORT_OPENVG
/*******************************************************************************
 *
 * vgMakeCurrent - Update resources
 *
 */
LOCAL void vgMakeCurrent
    (
    gc_t *      pGc,
    surface_t * pDrawSurface,
    surface_t * pReadSurface,
    context_t * pContext
    )
    {
    /* Save the draw surface */
    pGc->vg.pDrawSurface = pDrawSurface;

    /* Update the scissoring rectangles */
    if (pGc->vg.numScissorRects > 0)
        vgSetiv(VG_SCISSOR_RECTS, pGc->vg.numScissorRects, pGc->vg.scissoringData);

    /* Mark paints as dirty */
    LL_FOREACH(pGc->vg.pPaints, pObj->dirty = TRUE);
    }

/*******************************************************************************
 *
 * vgReleaseThread - Release all allocated resources
 *
 */
LOCAL void vgReleaseThread
    (
    gc_t * pGc
    )
    {
    /* Free scissoring data */
    free(pGc->vg.scissoringData);

    /* Free paths */
    LL_FOREACH(pGc->vg.pPaths, pObj->refCount = 0;
                               pObj->deletePending = FALSE;
                               vgDestroyPath((VGPath)pObj));

    if (pGc->vg.pTess != NULL)
        gluDeleteTess(pGc->vg.pTess);

    /* Free images */
    LL_FOREACH(pGc->vg.pImages, pObj->refCount = 0;
                                pObj->deletePending = FALSE;
                                vgDestroyImage((VGImage)pObj));

    /* Free fonts */
    LL_FOREACH(pGc->vg.pFonts, pObj->refCount = 0;
                               pObj->deletePending = FALSE;
                               vgDestroyFont((VGFont)pObj));

    /* Free paints */
    LL_FOREACH(pGc->vg.pPaints, pObj->refCount = 0;
                                pObj->deletePending = FALSE;
                                vgDestroyPaint((VGPaint)pObj));
    }
#endif	/* SUPPORT_OPENVG */
