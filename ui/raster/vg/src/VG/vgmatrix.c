/* vgmatrix.c - Wind River VG Matrix Functionality */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22dec14,yat  Fix build warning for LP64 (US50456)
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
20jun09,m_c  Written
*/

/*
DESCRIPTION
These routines provide matrix functionality that support the OpenVG
implementation.
*/

/* includes */

#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* forward declarations */

LOCAL void matMultiply(float*, const float*);

/*******************************************************************************
 *
 *  matMultiply - right-multiply the specified matrixes
 *
 */
LOCAL void matMultiply
    (
    float* M,
    const float* N
    )
    {
    int                             i;
    float                           e00, e01, e02;
    float                           e10, e11, e12;
    float                           e20, e21, e22;

    e00 = M[0], e01 = M[1], e02 = M[2];
    e10 = M[3], e11 = M[4], e12 = M[5];
    e20 = M[6], e21 = M[7], e22 = M[8];
    for (i = 0; i < 9; i += 3)
        {
        M[i + 0] = (N[i + 0] * e00) + (N[i + 1] * e10) + (N[i + 2] * e20);
        M[i + 1] = (N[i + 0] * e01) + (N[i + 1] * e11) + (N[i + 2] * e21);
        M[i + 2] = (N[i + 0] * e02) + (N[i + 1] * e12) + (N[i + 2] * e22);
        }
    }

/*******************************************************************************
 *
 *  vgGetMatrix
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetMatrix
    (
    VGfloat* matrix
    ) VG_API_EXIT
    {
    float*                          M;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPtr(matrix, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Return the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        matrix[0] = M[0];
        matrix[1] = M[1];
        matrix[2] = M[2];
        matrix[3] = M[3];
        matrix[4] = M[4];
        matrix[5] = M[5];
        matrix[6] = M[6];
        matrix[7] = M[7];
        matrix[8] = M[8];
        }
    }

/*******************************************************************************
 *
 *  vgLoadIdentity
 *
 */
VG_API_CALL void VG_API_ENTRY vgLoadIdentity
    (
    ) VG_API_EXIT
    {
    float*                          M;

    GET_GC();
    if (pGc != NULL)
        {
        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        M[0] = M[4] = M[8] = 1.0;
        M[1] = M[2] = M[3] = M[5] = M[6] = M[7] = 0.0;
        }
    }

/*******************************************************************************
 *
 *  vgLoadMatrix
 *
 */
VG_API_CALL void VG_API_ENTRY vgLoadMatrix
    (
    const VGfloat* matrix
    ) VG_API_EXIT
    {
    float*                          M;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPtr(matrix, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        M[0] = matrix[0];
        M[1] = matrix[1];
        M[3] = matrix[3];
        M[4] = matrix[4];
        M[6] = matrix[6];
        M[7] = matrix[7];
        if (pGc->vg.matrixMode == VG_MATRIX_IMAGE_USER_TO_SURFACE)
            {
            M[2] = matrix[2];
            M[5] = matrix[5];
            M[8] = matrix[8];
            }
        else
            {
            M[2] = M[5] = 0.0;
            M[8] = 1.0;
            }
        }
    }

/*******************************************************************************
 *
 *  vgMultMatrix
 *
 */
VG_API_CALL void VG_API_ENTRY vgMultMatrix
    (
    const VGfloat* matrix
    ) VG_API_EXIT
    {
    float*                          M;
    float                           N[9];

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPtr(matrix, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Create a local copy of the specified matrix, ignoring the last row if the targeted matrix is affine */
        N[0] = matrix[0];
        N[1] = matrix[1];
        N[3] = matrix[3];
        N[4] = matrix[4];
        N[6] = matrix[6];
        N[7] = matrix[7];
        if (pGc->vg.matrixMode == VG_MATRIX_IMAGE_USER_TO_SURFACE)
            {
            N[2] = matrix[2];
            N[5] = matrix[5];
            N[8] = matrix[8];
            }
        else
            {
            N[2] = N[5] = 0.0;
            N[8] = 1.0;
            }

        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        matMultiply(M, N);
        }
    }

/*******************************************************************************
 *
 *  vgRotate
 *
 */
VG_API_CALL void VG_API_ENTRY vgRotate
    (
    VGfloat angle
    ) VG_API_EXIT
    {
    float                           ca, sa;
    float*                          M;
    float                           N[9];

    GET_GC();
    if (pGc != NULL)
        {
        /* Set a temporary rotation matrix */
        ca = (float)cos((double)DEG_TO_RAD(angle));
        sa = (float)sin((double)DEG_TO_RAD(angle));
        N[0] = N[4] = ca;
        N[1] = sa;
        N[2] = N[5] = N[6] = N[7] = 0.0;
        N[3] = -sa;
        N[8] = 1.0;

        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        matMultiply(M, N);
        }
    }

/*******************************************************************************
 *
 *  vgScale
 */
VG_API_CALL void VG_API_ENTRY vgScale
    (
    VGfloat sx,
    VGfloat sy
    ) VG_API_EXIT
    {
    float*                          M;
    float                           N[9];

    GET_GC();
    if (pGc != NULL)
        {
        /* Set a temporary scale matrix */
        N[0] = sx;
        N[1] = N[2] = N[3] = N[5] = N[6] = N[7] = 0.0;
        N[4] = sy;
        N[8] = 1.0;

        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        matMultiply(M, N);
        }
    }

/*******************************************************************************
 *
 *  vgShear
 *
 */
VG_API_CALL void VG_API_ENTRY vgShear
    (
    VGfloat shx,
    VGfloat shy
    ) VG_API_EXIT
    {
    float*                          M;
    float                           N[9];

    GET_GC();
    if (pGc != NULL)
        {
        /* Set a temporary shear matrix */
        N[0] = N[4] = N[8] = 1.0;
        N[1] = shy;
        N[2] = N[5] = N[6] = N[7] = 0.0;
        N[3] = shx;

        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        matMultiply(M, N);
        }
    }

/*******************************************************************************
 *
 *  vgTranslate
 */
VG_API_CALL void VG_API_ENTRY vgTranslate
    (
    VGfloat tx,
    VGfloat ty
    ) VG_API_EXIT
    {
    float*                          M;
    float                           N[9];

    GET_GC();
    if (pGc != NULL)
        {
        /* Set a temporary translation matrix */
        N[0] = N[4] = N[8] = 1.0;
        N[1] = N[2] = N[3] = N[5] = 0.0;
        N[6] = tx;
        N[7] = ty;

        /* Update the current transformation matrix */
        M = pGc->vg.pCurrentMatrix;
        matMultiply(M, N);
        }
    }
