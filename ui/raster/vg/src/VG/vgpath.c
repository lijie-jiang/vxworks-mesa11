/* vgpath.c - Wind River VG Path Functionality */

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
31mar15,rpc  Static analysis fixes (US50633)
22dec14,yat  Fix build warning for LP64 (US50456)
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
12jul09,m_c  Written
*/

/*
DESCRIPTION
These routines provide path functionality that support the OpenVG
implementation.
*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glu.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* defines */

#define DEBUG_LEVEL 0

/* Matrix determinant */
#define DETERMINANT(_M) ((_M[0] * ((_M[4] * _M[8]) - (_M[4] * _M[7]))) +    \
                        (_M[3] * ((_M[2] * _M[7]) - (_M[1] * _M[8]))) +    \
                        (_M[6] * ((_M[1] * _M[4]) - (_M[2] * _M[4]))))

/* Dot product */
#define DOT(_a, _b) ((_a##x * _b##x) + (_a##y * _b##y))

/* Infinity */
#define VG_INFINITY (1.0f / 0.0f)

/* Size of path data */
#define SIZEOF_PATH_DATA(_ns, _nc) ((((_ns) + (_nc)) * sizeof(path_data_t)) + ((1 + (_ns)) * sizeof(int)))

/* Path coordinate transformation */
#define TRANSFORM(_v, _scale, _bias)(((_v) * (_scale)) + (_bias))

/* typdefefs */

/* Ellipse */
typedef struct
    {
    float rh, rv;         /* horizontal and vertical radii */
    float cosPhi, sinPhi; /* rotation angle cosine and sine */
    float xc, yc;         /* center */
    } ellipse_t;

/* forward declarations */

LOCAL int addCubicBezier(vertex_buffer_t*, int, float, float, float, float, float, float, float, float, int, gc_t*);
LOCAL int addEllipticalArc(vertex_buffer_t*, int, float, float, float, float, float, float, const ellipse_t*, int, gc_t*);
LOCAL int addVertex(vertex_buffer_t*, int, char, float, float, gc_t*);
LOCAL void freeVertexBuffer(vertex_buffer_t*);
LOCAL vertex_buffer_t * allocVertexBuffer(gc_t*);
LOCAL float calcDistance(float, float, float, float);
LOCAL void calcEllipseFromMatrix(const float*, float*, float*, float*);
LOCAL void clearPath(path_t*, int);
LOCAL float convertToFloat(const void*, int);
LOCAL void deletePath(path_t*);
LOCAL int findEllipses(int, float, float, float, float, ellipse_t*, float*, float*);
LOCAL int findUnitCircles(int, float, float, float, float, float*, float*);
LOCAL int generatePathGeometry(path_t*);
LOCAL vertex_ptr getVertex(vertex_buffer_t*, int, gc_t*);
LOCAL vertex_ptr getVertex2(vertex_buffer_t*, int, gc_t*);
LOCAL void growBoundingBox(path_t*, float, float);
LOCAL void setVertex(vertex_buffer_t*, int, char, float, float, gc_t*);
LOCAL void tessCombineCallback(GLdouble[3], vertex_ptr[4], GLfloat[4], vertex_ptr*, path_t*);
LOCAL void tessEdgeFlagCallback(GLboolean);
LOCAL void tessFillVertexCallback(vertex_ptr, path_t*);
LOCAL void transformPoint(float*, float*, const float*);

/* locals */

/* Number of bytes for each datatype */
LOCAL const uchar   numBytes[] =     {1,    /* VG_PATH_DATATYPE_S_8 */
                                  2,    /* VG_PATH_DATATYPE_S_16 */
                                  4,    /* VG_PATH_DATATYPE_S_32 */
                                  4    };   /* VG_PATH_DATATYPE_F */

/* Number of coordinates for each command */
LOCAL const uchar numCoords[] =     {0,       /* VG_CLOSE_PATH */
                                 2,       /* VG_MOVE_TO */
                                 2,       /* VG_LINE_TO */
                                 1 + 1,   /* VG_HLINE_TO */
                                 1 + 1,   /* VG_VLINE_TO */
                                 4 + 2,   /* VG_QUAD_TO */
                                 6,       /* VG_CUBIC_TO */
                                 2 + 4,   /* VG_SQUAD_TO */
                                 4 + 2,   /* VG_SCUBIC_TO */
                                 5,       /* VG_SCCWARC_TO */
                                 5,       /* VG_SCWARC_TO */
                                 5,       /* VG_LCCWARC_TO */
                                 5    };      /* VG_LCWARC_TO */

/*******************************************************************************
 *
 * addCubicBezier - add a cubic Bezier curve to the specified vertex array
 *
 * The goal here is to minimize the number of vertices needed to approximate
 * the spline by carefully choosing the "flatness" test. The essential
 * observation is that when the curve is a uniform speed straight line from end
 * to end, the control points are evenly spaced from beginning to end.
 * Therefore, the measure of how far we deviate from the ideal curve uses
 * distance to the middle controls, not from the line itself, but from their ideal arrangement
 *
 * RETURNS: The number of vertices created
 *
 */
LOCAL int addCubicBezier
    (
    vertex_buffer_t* pVertexBuffer,
    int n,
    float x1,
    float y1,
    float x2,
    float y2,
    float x3,
    float y3,
    float x4,
    float y4,
    int level,
    gc_t* pGc
    )
    {
    double  dist;                         /* "taxicab" metric */
    int     k;                            /* number of vertices created through subdivision */
    float   x12, y12, x23, y23, x34, y34; /* mid-points */
    float   x123, y123, x234, y234;
    float   x1234, y1234;

    /* Set number of vertices created */
    k = 0;

    /* Limit recursion to avoid a stack overflow */
    if (level < MAX_SUBDIV_LEVEL)
        {
        /* Calculate the mid-points */
        x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;

        x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;

        x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;

        /* Determine if the curve is flat enough, i.e. if it needs to be further divided */
        dist = fabs(x1 + x3 - x2 - x2) + fabs(y1 + y3 - y2 - y2) +
               fabs(x2 + x4 - x3 - x3) + fabs(y2 + y4 - y3 - y3);
        if (dist < pGc->vg.flatness)
            {
            setVertex(pVertexBuffer, n, VERTEX_TYPE_CALCULATED, x1234, y1234, pGc);
            k += 1;
            }
        else
            {
            k += addCubicBezier(pVertexBuffer, n + k, x1, y1, x12,
                                y12, x123, y123, x1234, y1234, level + 1, pGc);
            k += addCubicBezier(pVertexBuffer, n + k, x1234, y1234,
                                x234, y234, x34, y34, x4, y4, level + 1, pGc);
            }
        }

    return (k);
    }

/*******************************************************************************
 *
 * addEllipticalArc - using adaptive subdivision, add an elliptical arc to the
 *                     specified vertex array
 *
 * RETURNS: The number of vertices created
 *
 */
LOCAL int addEllipticalArc
    (
    vertex_buffer_t* pVertexBuffer,
    int n,
    float x1,
    float y1,
    float theta1,
    float x2,
    float y2,
    float theta2,
    const ellipse_t* pEllipse,
    int level,
    gc_t* pGc
    )
    {
    float   a;                  /* angle */
    float   cosTheta, sinTheta; /* precalculated cosine/sine values */
    float   dist;               /* distance */
    int     k;                  /* number of vertices created during subdivision */
    float   x12, y12;           /* mid-point */

    /* Set number of vertices created */
    k = 0;

    /* Limit recursion to avoid a stack overflow */
    if (level < MAX_SUBDIV_LEVEL)
        {
        /* Calculate the mid-point */
        a = (theta1 + theta2) / 2.0f;
        cosTheta = (float)cos((double)a), sinTheta = (float)sin((double)a);
        x12 = pEllipse->xc + (pEllipse->rh * pEllipse->cosPhi * cosTheta) -
                (pEllipse->rv * pEllipse->sinPhi * sinTheta);
        y12 = pEllipse->yc + (pEllipse->rh * pEllipse->sinPhi * cosTheta) +
                (pEllipse->rv * pEllipse->cosPhi * sinTheta);

        /* Determine if the curve is flat enough, i.e. if it needs to be further divided */
        dist = (float)fabs((double)(((x12 - x1) * (y2 - y1)) - ((y12 - y1) * (x2 - x1))));
        if (dist < pGc->vg.flatness)
            {
            setVertex(pVertexBuffer, n, VERTEX_TYPE_CALCULATED, x12, y12, pGc);
            k += 1;
            }
        else
            {
            k += addEllipticalArc(pVertexBuffer, n + k, x1, y1, theta1,
                                  x12, y12, a, pEllipse, level + 1, pGc);
            k += addEllipticalArc(pVertexBuffer, n + k, x12, y12, a, x2,
                                  y2, theta2, pEllipse, level + 1, pGc);
            }
        }

    return (k);
    }

/*******************************************************************************
 *
 * addVertex - add a vertex at the nth position of the specified vertex buffer
 *
 * RETURNS: One if the vertex was added, zero if it's identical to its
 *          predecessor
 *
 */
LOCAL int addVertex
    (
    vertex_buffer_t* pVertexBuffer,
    int n,
    char type,
    float x,
    float y,
    gc_t* pGc
    )
    {
    vertex_ptr  V;

    if (n > 1)
        {
        V = getVertex(pVertexBuffer, n - 1, pGc);
        if ((V != NULL) && (V->x == x) && (V->y == y))
            return (0);
        }
    setVertex(pVertexBuffer, n, type, x, y, pGc);

    return (1);
    }

/*******************************************************************************
 *
 * allocVertexBuffer - allocate a vertex buffer
 *
 * RETURNS: A pointer to a vertex buffer
 *
 */
LOCAL vertex_buffer_t * allocVertexBuffer
    (
    gc_t* pGc
    )
    {
    vertex_buffer_t *   pVertexBuffer;

    pVertexBuffer = malloc(sizeof(vertex_buffer_t));
    if (pVertexBuffer == NULL)
        {
        #if     (DEBUG_LEVEL > 0)
            {
            (void)printf ("**** Out of memory ****\n");
            }
        #endif

        longjmp(pGc->env, 1);
        }
    pVertexBuffer->pNext = NULL;
    pVertexBuffer->k = 0;

    return (pVertexBuffer);
    }

/*******************************************************************************
 *
 * calcDistance - the distance between the specified points
 *
 */
LOCAL float calcDistance
    (
    float x1,
    float y1,
    float x2,
    float y2
    )
    {
    x1 -= x2, y1 -= y2;
    return ((float)sqrt(SQUARED(x1) + SQUARED(y1)));
    }

/*******************************************************************************
 *
 * calcEllipseFromMatrix - calculate the horizontal and vertical radii and the
 *                          CCW rotation angle of an ellipse defined by the
 *                          specified matrix
 *
 * The calculations are derived from the reference implementation
 *
 */
LOCAL void calcEllipseFromMatrix
    (
    const float* matrix,
    float* pRh,
    float* pRv,
    float* pRot
    )
    {
    double  hx, hy, hl;
    double  px, py, pl;
    double  qx, qy, ql;
    double  t0, t1;

    px = matrix[0], py = matrix[1];
    qx = matrix[4], qy = -matrix[3];
    if (DOT(p,p) < DOT(q,q))
        {
        EXCHANGE(px, qx);
        EXCHANGE(py, qy);
        t0 = PI / 2;
        }
    else
        t0 = 0.0;
    hx = (px + qx) / 2, hy = (py + qy) / 2, hl = sqrt(DOT(h, h));
    px = (px - qx) * 0.5, py = (py - qy) * 0.5, pl = sqrt(DOT(p, p));
    qx = (pl * hx) + (hl * px), qy = (pl * hy) + (hl * py), ql = DOT(q, q);
    if (ql == 0.0)
        t1 = 0.0;
    else
        {
        t1 = acos(qx / sqrt(ql));
        if (qy < 0.0)
            t1 = TWO_PI - t1;
        }

    *pRh = (float)(hl + pl);
    *pRv = (float)(hl - pl);
    *pRot = (float)(t0 + t1);
    }

/*******************************************************************************
 *
 * clearPath - discard all data of the specified path
 *
 */
LOCAL void clearPath
    (
    path_t* pPath,
    int all
    )
    {
    if (all)
        {
        free(pPath->data);
        pPath->numSegs = pPath->numCoords = pPath->numUserCoords = 0;
        pPath->data = NULL;
        }

    freeVertexBuffer(pPath->pVertexBuffer);
    pPath->numVertices = 0;
    pPath->pVertexBuffer = NULL;

    free(pPath->triangles);
    pPath->maxTriangles = pPath->numFillTriangles = pPath->numStrokeTriangles = 0;
    pPath->triangles = NULL;

    pPath->min[0] = pPath->min[1] = 0.0;
    pPath->max[0] = pPath->max[1] = -1.0;
    }

/*******************************************************************************
 *
 * convertToFloat - convert to a float the data of the specified type
 *
 * RETURNS: A floating-point value
 *
 */
LOCAL float convertToFloat
    (
    const void* pData,
    int datatype
    )
    {
    float value = 0.0f;

    switch (datatype)
        {
        case VG_PATH_DATATYPE_S_8:
            value = *((char*)pData);
            break;

        case VG_PATH_DATATYPE_S_16:
            value = *((short*)pData);
            break;

        case VG_PATH_DATATYPE_S_32:
            value = (float)(*((int*)pData));
            break;

        case VG_PATH_DATATYPE_F:
            value = *((float*)pData);
            break;
        }

    return (value);
    }

/*******************************************************************************
 *
 * deletePath - unlink and delete the specified path
 *
 */
LOCAL void deletePath
    (
    path_t* pPath
    )
    {
    /* Unlink */
    LL_REMOVE(pPath->pGc->vg.pPaths, pPath);

    /* Discard data */
    clearPath(pPath, TRUE);

    /* Free allocated memory */
    free(pPath);
    }

/*******************************************************************************
 *
 * findEllipses - find the center of the ellipse passing through the specified
 *                points
 *
 * RETURNS: TRUE if successful, otherwise FALSE
 *
 */
LOCAL int findEllipses
    (
    int seg,
    float x0,
    float y0,
    float x1,
    float y1,
    ellipse_t* pEllipse,
    float* pTheta0,
    float* pTheta1
    )
    {
    float                           q;              /* scaling factor to enable a solution */
    float                           theta0, theta1; /* angular coordinates */
    float                           xc, yc;         /* center of the ellipse */
    float                           xe, ye;         /* end endpoint (in unit space) */
    float                           xs, ys;         /* start endpoint (in unit space) */

    /* Ignore degenerate case */
    if ((pEllipse->rh == 0.0) && (pEllipse->rv == 0.0))
        return (FALSE);

    /* Transform the endpoints into unit space (inverse rotation and inverse scale) */
    xs = ((x0 * pEllipse->cosPhi) + (y0 * pEllipse->sinPhi)) / pEllipse->rh;
    ys = ((y0 * pEllipse->cosPhi) - (x0 * pEllipse->sinPhi)) / pEllipse->rv;
    xe = ((x1 * pEllipse->cosPhi) + (y1 * pEllipse->sinPhi)) / pEllipse->rh;
    ye = ((y1 * pEllipse->cosPhi) - (x1 * pEllipse->sinPhi)) / pEllipse->rv;

    /* Abandon if the arc's endpoints are coincident */
    if ((xs == xe) && (ys == ye))
        return (FALSE);

    if (!findUnitCircles(seg, xs, ys, xe, ye, &xc, &yc))
        {
        /* There are no solutions, scale to find one */
        q = (float)sqrt((double)(SQUARED(xs - xe) + SQUARED(ys - ye))) / 2.0f;

        /* Use a little hack to make up for rounding errors */
        q *= 1.00001f;

        /* Adjust radii... */
        pEllipse->rh *= q;
        pEllipse->rv *= q;
        /* ...and reevaluate the endpoints in unit space */
        xs = ((x0 * pEllipse->cosPhi) + (y0 * pEllipse->sinPhi)) / pEllipse->rh;
        ys = ((y0 * pEllipse->cosPhi) - (x0 * pEllipse->sinPhi)) / pEllipse->rv;
        xe = ((x1 * pEllipse->cosPhi) + (y1 * pEllipse->sinPhi)) / pEllipse->rh;
        ye = ((y1 * pEllipse->cosPhi) - (x1 * pEllipse->sinPhi)) / pEllipse->rv;
        if (!findUnitCircles(seg, xs, ys, xe, ye, &xc, &yc))
            {
            /*
             * In theory, this should not happen. However, it can because of
             * rouding errors in the math
             */
            return (FALSE);
            }
        }

    /* Move center to (0, 0) */
    xs -= xc, ys -= yc;
    xe -= xc, ye -= yc;

    /* Determine the angular positions of the endpoints... */
    theta0 = (float)atan2((double)ys, (double)xs);
    theta1 = (float)atan2((double)ye, (double)xe);
    /*
     * ...and correct upon the direction around the ellipse
     * (counter-clockwise vs. clockwise)
     */
    q = (theta0 + theta1) / 2;
    if ((seg == VG_SCCWARC_TO) || (seg == VG_LCCWARC_TO))
        {
        if (q < theta0)
            theta1 += TWO_PI;
        }
    else
        {
        if (q > theta0)
            theta1 -= TWO_PI;
        }
    *pTheta0 = theta0;
    *pTheta1 = theta1;

    /* Move the center back into user space (scale then rotation) */
    xc *= pEllipse->rh, yc *= pEllipse->rv;
    pEllipse->xc = (xc * pEllipse->cosPhi) - (yc * pEllipse->sinPhi);
    pEllipse->yc = (xc * pEllipse->sinPhi) + (yc * pEllipse->cosPhi);

    return (TRUE);
    }

/*******************************************************************************
 *
 * findUnitCircles - find the center of the unit circle passing through the
 *                   specified points
 *
 * RETURNS: TRUE if successful, otherwise FALSE
 *
 * See specifications for details ("Appendix A: Mathematics of Ellipses")
 *
 */
LOCAL int findUnitCircles
    (
    int seg,
    float x0,
    float y0,
    float x1,
    float y1,
    float* pXc,
    float* pYc
    )
    {
    float                           dx, dy, s;  /* parameters to the solution */
    float                           xm, ym;     /* median point */

    /* Evaluate the differences and determine the median point */
    dx = x0 - x1, dy = y0 - y1;
    xm = (x0 + x1) / 2, ym = (y0 + y1) / 2;

    /* Solve for intersecting unit circles */
    s = (1.0f / (SQUARED(dx) + SQUARED(dy))) - 0.25f;
    if (s < 0.0f)
        {
        /* Points are too far apart for a solution */
        return (FALSE);
        }
    s = (float)sqrt((double)s);
    dx *= s, dy *= s;

    switch (seg)
        {
        /* Small counter-clockwise and large clockwise arcs */
        case VG_SCCWARC_TO:
        case VG_LCWARC_TO:
            *pXc = xm + dy;
            *pYc = ym - dx;
            break;

        /* Small clockwise and large counter-clockwise arcs */
        case VG_SCWARC_TO:
        case VG_LCCWARC_TO:
            *pXc = xm - dy;
            *pYc = ym + dx;
            break;

        /* ??? */
        default:
            *pXc = 0.0;
            *pYc = 0.0;
            break;
        }

    return (TRUE);
    }

/*****************************************************************************************************************
 *
 * generatePathGeometry - generate the geometry for the specified path
 *
 * RETURNS: Zero if successful
 ****************************************************************************************************************/
LOCAL int generatePathGeometry
    (
    path_t* pPath
    )
    {
    GLdouble         coords[3];      /* vertex coordinates fed to the GLU tesselator */
    path_data_t *   data;           /* path data */
    ellipse_t       ellipse;        /* ellipse */
    int             i, j, k;
    float           length;         /* running length */
    int             moveTo;         /* TRUE if a "move to" command needs to be added */
    int             n;              /* number of vertices in the path */
    float           ox, oy;         /* last point of previous segment */
    GLUtesselator * pTess;          /* GLU tesselator */
    float           px, py;         /* last internal control point of previous segment */
    float           rh, rv, rot;    /* horizontal and vertical radii and CCW rotation angle */
    int             seg;            /* segment */
    float           sx, sy;         /* beginning of current subpath */
    float           theta0, theta1; /* angular positions */
    vertex_ptr      V;              /* current vertex */
    float           x0, y0;         /* coordinates */
    float           x1, y1;
    float           x2, y2;

    /* Get the GLU tesselator */
    pTess = pPath->pGc->vg.pTess;
    if (pTess == NULL)
        return (1);
#if defined(_WRS_KERNEL)
    /* Setup the recovery environment */
    if (setjmp(pPath->pGc->env) != 0)
        {
        clearPath(pPath, FALSE);
        return (2);
        }
#endif
    /* Allocate a vertex buffer*/
    if (pPath->pVertexBuffer == NULL)
        pPath->pVertexBuffer = allocVertexBuffer(pPath->pGc);

    /* ## Generate the body's geometry ## */

    /*
     * Generate the vertices and initialize the segment-to-vertex index
     * (which links a segment to its first vertex)
     */
    data = pPath->data;
    moveTo = TRUE;
    n = 0;
    ox = oy = px = py = sx = sy = x0 = y0 = 0.0;
    pPath->index = (int*)&data[pPath->numCoords + pPath->numSegs];
    pPath->min[0] = pPath->min[1] = VG_INFINITY;
    pPath->max[0] = pPath->max[1] = -VG_INFINITY;
    V = NULL;
    for (i = j = 0, pPath->index[0] = -1; i < pPath->numSegs; pPath->index[++i] = n - 1)
        {
        seg = pPath->data[j++].seg & 0x000000fe;
        switch (seg)
            {
            case VG_CLOSE_PATH:
                moveTo = TRUE;
                if (V != NULL)
                    V->type = VERTEX_TYPE_END;
                /**/
                k = n;
                n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_END, sx, sy, pPath->pGc);
                /**/
                V = NULL;
                /**/
                px = ox = sx;
                py = oy = sy;

                #if     (DEBUG_LEVEL > 1)
                    {
                    (void)printf ("VG_CLOSE_PATH [%d]\n", n - k);
                    }
                #endif
                break;

            case VG_MOVE_TO:
                moveTo = FALSE;
                if (V != NULL)
                    V->type = VERTEX_TYPE_END;
                /**/
                x0 = data[j++].coord;
                y0 = data[j++].coord;
                /**/
                k = n;
                /**/
                V = NULL;
                /**/
                sx = px = ox = x0;
                sy = py = oy = y0;

                #if     (DEBUG_LEVEL > 1)
                    {
                    (void)printf ("VG_MOVE_TO %.3f %.3f [%d]\n", x0, y0, n - k);
                    }
                #endif
                break;

            case VG_LINE_TO:
                if (moveTo)
                    {
                    moveTo = FALSE;
                    if (V != NULL)
                        V->type = VERTEX_TYPE_END;
                    }
                /**/
                x0 = data[j++].coord;
                y0 = data[j++].coord;
                /**/
                k = n;
                n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_START, ox, oy, pPath->pGc);
                n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_REGULAR, x0, y0, pPath->pGc);
                /**/
                V = getVertex(pPath->pVertexBuffer, n - 1, pPath->pGc);
                /**/
                px = ox = x0;
                py = oy = y0;

                #if     (DEBUG_LEVEL > 1)
                    {
                    (void)printf ("VG_LINE_TO %.3f %.3f [%d]\n", x0, y0, n - k);
                    }
                #endif
                break;

            case VG_CUBIC_TO:
                if (moveTo)
                    {
                    moveTo = FALSE;
                    if (V != NULL)
                        V->type = VERTEX_TYPE_END;
                    }
                /**/
                x0 = data[j++].coord;
                y0 = data[j++].coord;
                x1 = data[j++].coord;
                y1 = data[j++].coord;
                x2 = data[j++].coord;
                y2 = data[j++].coord;
                /**/
                k = n;
                n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_START, ox, oy, pPath->pGc);
                n += addCubicBezier(pPath->pVertexBuffer, n, ox, oy, x0, y0, x1, y1, x2, y2, 0, pPath->pGc);
                n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_REGULAR, x2, y2, pPath->pGc);
                /**/
                V = getVertex(pPath->pVertexBuffer, n - 1, pPath->pGc);
                /**/
                px = x1, ox = x2;
                py = y1, oy = y2;

                #if     (DEBUG_LEVEL > 1)
                    {
                    (void)printf ("VG_CUBIC_TO %.3f %.3f %.3f %.3f %.3f %.3f [%d]\n", x0, y0, x1, y1, x2, y2, n - k);
                    }
                #endif
                break;

            case VG_SCCWARC_TO:
            case VG_SCWARC_TO:
            case VG_LCCWARC_TO:
            case VG_LCWARC_TO:
                if (moveTo)
                    {
                    moveTo = FALSE;
                    if (V != NULL)
                        V->type = VERTEX_TYPE_END;
                    }
                /**/
                rh = data[j++].coord;
                rv = data[j++].coord;
                rot = data[j++].coord;
                x0 = data[j++].coord;
                y0 = data[j++].coord;
                /**/
                k = n;
                ellipse.rh = rh;
                ellipse.rv = rv;
                ellipse.cosPhi = (float)cos((double)DEG_TO_RAD(rot)), ellipse.sinPhi = (float)sin((double)DEG_TO_RAD(rot));
                if (findEllipses(seg, ox, oy, x0, y0, &ellipse, &theta0, &theta1))
                    {
                    n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_START, ox, oy, pPath->pGc);
                    n += addEllipticalArc(pPath->pVertexBuffer, n, ox, oy, theta0, x0, y0, theta1, &ellipse, 0, pPath->pGc);
                    n += addVertex(pPath->pVertexBuffer, n, VERTEX_TYPE_REGULAR, x0, y0, pPath->pGc);
                    }
                /**/
                V = getVertex(pPath->pVertexBuffer, n - 1, pPath->pGc);
                /**/
                px = ox = x0;
                py = oy = y0;

                #if     (DEBUG_LEVEL > 1)
                    {
                    if (seg == VG_SCCWARC_TO)
                        (void)printf ("VG_SCCWARC_TO");
                    else if (seg == VG_SCWARC_TO)
                        (void)printf ("VG_SCWARC_TO");
                    else if (seg == VG_LCCWARC_TO)
                        (void)printf ("VG_LCCWARC_TO");
                    else
                        (void)printf ("VG_LCWARC_TO");
                    (void)printf (" %.3f %.3f %.3f %.3f %.3f [%d]\n", rh, rv, rot, x1, y1, n - k);
                    }
                #endif
                break;
            }
        }

    /* Close path (in case the VG_CLOSE_PATH is missing) */
    if (V != NULL)
        V->type = VERTEX_TYPE_END;

    /* Reset internal counts */
    pPath->numVertices = n;
    pPath->numFillTriangles = pPath->numStrokeTriangles = 0;

    if (n > 2)
        {
        /* ## Triangulate the body ## */

        /* Allocate memory for the triangles */
        k = iceil((float)n * VERTEX_TO_TRIANGLE_RATIO);
        if (k > pPath->maxTriangles)
            {
            free(pPath->triangles);
            pPath->triangles = malloc(k * 3 * sizeof(vertex_ptr));
            }
        if (pPath->triangles == NULL)
            longjmp(pPath->pGc->env, 1);
        pPath->maxTriangles = k * 3;

        /* Configure the GLU tesselator */
        /* - Set the callbacks */
        gluTessCallback(pTess, GLU_TESS_COMBINE_DATA, (_GLUfuncptr)tessCombineCallback);
        gluTessCallback(pTess, GLU_TESS_EDGE_FLAG_DATA, (_GLUfuncptr)tessEdgeFlagCallback);
        gluTessCallback(pTess, GLU_TESS_VERTEX_DATA, (_GLUfuncptr)tessFillVertexCallback);
        /* - Force the path's normal to point towards the viewer */
        gluTessNormal(pTess, 0.0, 0.0, 1.0);
        /* - Set winding rule */
        switch (pPath->pGc->vg.fillRule)
            {
            case VG_EVEN_ODD:
                pPath->winding = VG_EVEN_ODD;
                gluTessProperty(pPath->pGc->vg.pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
                break;

            case VG_NON_ZERO:
                pPath->winding = VG_NON_ZERO;
                gluTessProperty(pPath->pGc->vg.pTess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
                break;
            }

        /* Feed the vertices to the tesselator, calculate the running length of the path and its bounding box */
        gluTessBeginPolygon(pTess, pPath);
        for (i = 0, length = 0.0; i < n; i++)
            {
            V = getVertex(pPath->pVertexBuffer, i, pPath->pGc);
            if (V == NULL)
                break;

            if (V->type != VERTEX_TYPE_START)
                length += calcDistance(x0, y0, V->x, V->y);
            V->length = length;
            growBoundingBox(pPath, x0 = V->x, y0 = V->y);

            coords[0] = V->x, coords[1] = V->y, coords[2] = V->w;
            if (V->type == VERTEX_TYPE_START)
                {
                if (i > 0)
                    gluTessEndContour(pTess);
                gluTessBeginContour(pTess);
                }
            gluTessVertex(pTess, coords, V);
            }
        gluEndPolygon(pTess);
        }
    else
        {
        /* Calculate the running length of the path and its bounding box */
        for (i = 0, length = 0.0; i < n; i++)
            {
            V = getVertex(pPath->pVertexBuffer, i, pPath->pGc);
            if (V == NULL)
                break;

            if (V->type != VERTEX_TYPE_START)
                length += calcDistance(x0, y0, V->x, V->y);
            V->length = length;
            growBoundingBox(pPath, x0 = V->x, y0 = V->y);
            }
        }

    /* ## Generate the stroke's geometry ## */
    /* :TODO: */

    /* Adjust counts */
    pPath->maxTriangles /= 3;
    pPath->numFillTriangles /= 3;
    pPath->numStrokeTriangles /= 3;

    return (0);
    }

/*******************************************************************************
 *
 * getVertex - find the address of the nth vertex in the specified vertex
 *             buffer
 *
 * RETURNS: A pointer to a vertex_t structure if successful, otherwise NULL
 */
LOCAL vertex_ptr getVertex
    (
    vertex_buffer_t* pVertexBuffer,
    int n,
    gc_t* pGc
    )
    {
    while (n >= BUCKET_SIZE)
        {
        n -= BUCKET_SIZE;
        if (pVertexBuffer->pNext == NULL)
            return (NULL);
        pVertexBuffer = pVertexBuffer->pNext;
        }

    return ((n < pVertexBuffer->k) ? (&pVertexBuffer->bucket[n]) : (NULL));
    }

/*******************************************************************************
 *
 * getVertex2 - find the address of the nth vertex in the specified vertex
 *              buffer, expanding it as needed
 *
 * RETURNS: A pointer to a vertex_t structure
 */
LOCAL vertex_ptr getVertex2
    (
    vertex_buffer_t* pVertexBuffer,
    int n,
    gc_t* pGc
    )
    {
    while (n >= BUCKET_SIZE)
        {
        n -= BUCKET_SIZE;
        if (pVertexBuffer->pNext == NULL)
            pVertexBuffer->pNext = allocVertexBuffer(pGc);
        pVertexBuffer->k = BUCKET_SIZE;
        pVertexBuffer = pVertexBuffer->pNext;
        }
    if (n >= pVertexBuffer->k)
        pVertexBuffer->k = n + 1;

    return (&pVertexBuffer->bucket[n]);
    }

/*******************************************************************************
 *
 * growBoundingBox - grow the bounding box of a path so that it emcompasses the
 *                   specified point
 *
 */
LOCAL void growBoundingBox
    (
    path_t* pPath,
    float x,
    float y
    )
    {
    if (x < pPath->min[0])
        pPath->min[0] = x;
    if (y < pPath->min[1])
        pPath->min[1] = y;
    if (x > pPath->max[0])
        pPath->max[0] = x;
    if (y > pPath->max[1])
        pPath->max[1] = y;
    }

/*******************************************************************************
 *
 * setVertex - initialize the nth vertex in the specified vertex buffer,
 *             expanding it as needed
 */
LOCAL void setVertex
    (
    vertex_buffer_t* pVertexBuffer,
    int n,
    char type,
    float x,
    float y,
    gc_t* pGc
    )
    {
    while (n >= BUCKET_SIZE)
        {
        n -= BUCKET_SIZE;
        if (pVertexBuffer->pNext == NULL)
            pVertexBuffer->pNext = allocVertexBuffer(pGc);
        pVertexBuffer->k = BUCKET_SIZE;
        pVertexBuffer = pVertexBuffer->pNext;
        }
    if (n >= pVertexBuffer->k)
        pVertexBuffer->k = n + 1;
    pVertexBuffer->bucket[n].type = type;
    pVertexBuffer->bucket[n].x = x;
    pVertexBuffer->bucket[n].y = y;
    pVertexBuffer->bucket[n].w = 1.0f;
    pVertexBuffer->bucket[n].u = pVertexBuffer->bucket[n].v = 0.0f;
    pVertexBuffer->bucket[n].length = -VG_INFINITY;
    }

/*******************************************************************************
 *
 * tessCombineCallback - create a new vertex when the tessellation detects an
 *                       intersection (or to merge features)
 */
LOCAL void tessCombineCallback
    (
    GLdouble coords[3],
    vertex_ptr vertices[4],
    GLfloat weight[4],
    vertex_ptr* pV,
    path_t* pPath
    )
    {
    vertex_ptr                      V;

    /* Allocate a new vertex... */
    V = getVertex2(pPath->pVertexBuffer, pPath->numVertices++, pPath->pGc);
    /* ...and initialize it */
    V->type = VERTEX_TYPE_CALCULATED;
    V->x = (GLfloat)coords[0];
    V->y = (GLfloat)coords[1];
    V->w = (GLfloat)coords[2];
    V->u = V->v = 0.0f;

    /* Send the new vertex back to the tesselator */
    *pV = V;
    }

/*******************************************************************************
 *
 * tessEdgeFlagCallback
 *
 */
LOCAL void tessEdgeFlagCallback
    (
    GLboolean flag
    )
    {
    }

/*******************************************************************************
 *
 * tessFillVertexCallback
 *
 */
LOCAL void tessFillVertexCallback
    (
    vertex_ptr V,
    path_t* pPath
    )
    {
    int                             i;

    i = pPath->numFillTriangles;
    if (i < pPath->maxTriangles)
        pPath->triangles[i++] = V;
    else
        {
        #if     (DEBUG_LEVEL > 0)
            {
            (void)printf ("**** Maximum number of triangles reached for path %p (%d/%d) ****\n", pPath, pPath->numVertices, i);
            }
        #endif

        longjmp(pPath->pGc->env, 1);
        }
    pPath->numFillTriangles = i;
    }

/*******************************************************************************
 *
 * transformPoint - apply a transformation matrix (assumed to be affine) to
 *                  the specified point
 */
LOCAL void transformPoint
    (
    float* pX,
    float* pY,
    const float* matrix
    )
    {
    float                           x, y;

    x = *pX, y = *pY;
    *pX = (x * matrix[0]) + (y * matrix[3]) + matrix[6];
    *pY = (x * matrix[1]) + (y * matrix[4]) + matrix[7];
    }

/*******************************************************************************
 *
 * freeVertexBuffer - free the specified vertex buffer
 *
 */
LOCAL void freeVertexBuffer
    (
    vertex_buffer_t * pVertexBuffer
    )
    {
    LL_FOREACH(pVertexBuffer, free(pObj));
    }

/*******************************************************************************
 *
 * vgAppendPath
 *
 */
VG_API_CALL void VG_API_ENTRY  vgAppendPath
    (
    VGPath dstPath,
    VGPath srcPath
    ) VG_API_EXIT
    {
    path_data_t *   data;               /* new destination path data */
    int             numSegs, numCoords; /* number of segments and coordinates in the source path */
    path_t *        pDstPath = (path_t*)dstPath;
    path_t *        pSrcPath = (path_t*)srcPath;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pDstPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidPath(pSrcPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Check capabilities */
        if ((pDstPath->caps & VG_PATH_CAPABILITY_APPEND_TO) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        if ((pSrcPath->caps & VG_PATH_CAPABILITY_APPEND_FROM) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Avoid appending empty paths */
        numSegs = pSrcPath->numSegs, numCoords = pSrcPath->numCoords;
        if (numSegs <= 0)
            return;

        /* Allocate memory */
        data = realloc(pDstPath->data, SIZEOF_PATH_DATA(pDstPath->numSegs + numSegs, pDstPath->numCoords + numCoords));
        if (data == NULL)
            VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);

        /* Clone the source path's data */
        memcpy(data + pDstPath->numSegs + pDstPath->numCoords, pSrcPath->data, (numSegs + numCoords) * sizeof(path_data_t));

        /* Update counters */
        pDstPath->numSegs += pSrcPath->numSegs;
        pDstPath->numCoords += pSrcPath->numCoords;
        pDstPath->numUserCoords += pSrcPath->numUserCoords;

        /* Reassign data */
        pDstPath->data = data;

        /* Mark the destination path as dirty to force a refresh of the geometry */
        pDstPath->dirty = TRUE;
        }
    }

/*******************************************************************************
 *
 * vgAppendPathData
 *
 */
VG_API_CALL void VG_API_ENTRY  vgAppendPathData
    (
    VGPath path,
    VGint numSegs,
    const VGubyte* segments,
    const void* pCoords
    ) VG_API_EXIT
    {
    path_t *        pPath = (path_t*)path;

    path_data_t *   data;               /* new path data */
    int             datatype;           /* coordinates datatype */
    int             hasRelativeCoords;  /* TRUE if the segment has relative coordinates */
    int             i, j;
    int             k;                  /* size in bytes */
    int             n;                  /* number of coordinates in the path */
    float           ox, oy;             /* last point of previous segment */
    float           px, py;             /* last internal control point of previous segment */
    float           rh, rv, rot;        /* horizontal and vertical radii and CCW rotation angle */
    float           scale, bias;        /* path scale and bias */
    int             seg;                /* segment */
    float           sx, sy;             /* beginning of current subpath */
    float           x0, y0;             /* segment coordinates */
    float           x1, y1;
    float           x2, y2;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((numSegs <= 0) || (segments == NULL))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(pCoords, pPath->datasize))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check capabilities */
        if ((pPath->caps & VG_PATH_CAPABILITY_APPEND_TO) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Validate the segments and determine the number of coordinates in the data */
        for (i = n = 0; i < numSegs; i++)
            {
            seg = segments[i];
            if ((seg < VG_CLOSE_PATH) || (seg > VG_LCWARC_TO_REL))
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
            n += numCoords[seg >> 1];
            }

        /* Allocate memory */
        data = realloc(pPath->data, SIZEOF_PATH_DATA(pPath->numSegs + numSegs, pPath->numCoords + n));
        if (data == NULL)
            VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);

        /* Set initial state */
        datatype = pPath->datatype;
        ox = oy = px = py = sx = sy = 0.0;
        scale = pPath->scale, bias = pPath->bias;

        /* Initialize the internal data */
        for (i = 0, j = pPath->numSegs + pPath->numCoords, k = numBytes[datatype]; i < numSegs; i++)
            {
            hasRelativeCoords = segments[i] & 0x01;
            data[j].seg = segments[i] << 8;
            seg = segments[i] & 0xfe;
            switch (seg)
                {
                case VG_CLOSE_PATH:
                    pPath->numSegs++;
                    data[j++].seg += seg;
                    px = ox = sx;
                    py = oy = sy;
                    break;

                case VG_MOVE_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 2;
                    pPath->numUserCoords += 2;
                    x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x0 += ox, y0 += oy;
                    data[j++].seg += seg;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    sx = px = ox = x0;
                    sy = py = oy = y0;
                    break;

                case VG_LINE_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 2;
                    pPath->numUserCoords += 2;
                    x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x0 += ox, y0 += oy;
                    data[j++].seg += seg;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    px = ox = x0;
                    py = oy = y0;
                    break;

                case VG_HLINE_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 2;
                    pPath->numUserCoords += 1;
                    x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y0 = oy;
                    if (hasRelativeCoords)
                        x0 += ox;
                    data[j++].seg += VG_LINE_TO;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    px = ox = x0;
                    py = oy = y0;
                    break;

                case VG_VLINE_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 2;
                    pPath->numUserCoords += 1;
                    x0 = ox;
                    y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        y0 += oy;
                    data[j++].seg += VG_LINE_TO;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    px = ox = x0;
                    py = oy = y0;
                    break;

                case VG_QUAD_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 6;
                    pPath->numUserCoords += 4;
                    x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x0 += ox, y0 += oy, x1 +=ox, y1 += oy;
                    data[j++].seg += VG_CUBIC_TO;
                    data[j++].coord = (ox + (2 * x0)) / 3;
                    data[j++].coord = (oy + (2 * y0)) / 3;
                    data[j++].coord = (x1 + (2 * x0)) / 3;
                    data[j++].coord = (y1 + (2 * y0)) / 3;
                    data[j++].coord = x1;
                    data[j++].coord = y1;
                    px = x0;
                    py = y0;
                    ox = x1;
                    oy = y1;
                    break;

                case VG_CUBIC_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 6;
                    pPath->numUserCoords += 6;
                    x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    x2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x0 += ox, y0 += oy, x1 +=ox, y1 += oy, x2 +=ox, y2 += oy;
                    data[j++].seg += seg;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    data[j++].coord = x1;
                    data[j++].coord = y1;
                    data[j++].coord = x2;
                    data[j++].coord = y2;
                    px = x1;
                    py = y1;
                    ox = x2;
                    oy = y2;
                    break;

                case VG_SQUAD_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 6;
                    pPath->numUserCoords += 2;
                    x0 = (2 * ox) - px;
                    y0 = (2 * oy) - py;
                    x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x1 += ox, y1 += oy;
                    data[j++].seg += VG_CUBIC_TO;
                    data[j++].coord = (ox + (2 * x0)) / 3;
                    data[j++].coord = (oy + (2 * y0)) / 3;
                    data[j++].coord = (x1 + (2 * x0)) / 3;
                    data[j++].coord = (y1 + (2 * y0)) / 3;
                    data[j++].coord = x1;
                    data[j++].coord = y1;
                    px = x0;
                    py = y0;
                    ox = x1;
                    oy = y1;
                    break;

                case VG_SCUBIC_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 6;
                    pPath->numUserCoords += 4;
                    x0 = (2 * ox) - px;
                    y0 = (2 * oy) - py;
                    x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    x2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x1 += ox, y1 += oy, x2 +=ox, y2 += oy;
                    data[j++].seg += VG_CUBIC_TO;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    data[j++].coord = x1;
                    data[j++].coord = y1;
                    data[j++].coord = x2;
                    data[j++].coord = y2;
                    px = x1;
                    py = y1;
                    ox = x2;
                    oy = y2;
                    break;

                case VG_SCCWARC_TO:
                case VG_SCWARC_TO:
                case VG_LCCWARC_TO:
                case VG_LCWARC_TO:
                    pPath->numSegs++;
                    pPath->numCoords += 5;
                    pPath->numUserCoords += 5;
                    rh = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    rv = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    rot = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                    pCoords = (void *)((long)pCoords + k);
                    if (hasRelativeCoords)
                        x0 += ox, y0 += oy;
                    data[j++].seg += seg;
                    data[j++].coord = rh;
                    data[j++].coord = rv;
                    data[j++].coord = rot;
                    data[j++].coord = x0;
                    data[j++].coord = y0;
                    px = ox = x0;
                    py = oy = y0;
                    break;
                }
            }

        /* Reassign data */
        pPath->data = data;

        /* Mark the path as dirty to force a refresh of the geometry */
        pPath->dirty = TRUE;
        }
    }

/*******************************************************************************
 *
 * vgClearPath
 *
 */
VG_API_CALL void VG_API_ENTRY vgClearPath
    (
    VGPath path,
    VGbitfield capabilities
    ) VG_API_EXIT
    {
    path_t*                         pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Discard all data */
        clearPath(pPath, TRUE);

        /* Set new capabilities */
        pPath->caps = capabilities & VG_PATH_CAPABILITY_ALL;
        }
    }

/*******************************************************************************
 *
 * vgCreatePath
 *
 */
VG_API_CALL VGPath VG_API_ENTRY vgCreatePath
    (
    VGint format,
    VGPathDatatype datatype,
    VGfloat scale,
    VGfloat bias,
    VGint segmentCapacityHint,
    VGint coordCapacityHint,
    VGbitfield capabilities
    ) VG_API_EXIT
    {
    path_t*                         pPath = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (format != VG_PATH_FORMAT_STANDARD)
            VG_FAIL(VG_UNSUPPORTED_PATH_FORMAT_ERROR);

        if ((datatype < VG_PATH_DATATYPE_S_8) || (datatype > VG_PATH_DATATYPE_F))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        if (scale == 0.0)
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Allocate memory for the path structure */
        pPath = malloc(sizeof(path_t));
        if (pPath == NULL)
            VG_FAIL(VG_OUT_OF_MEMORY_ERROR);

        /* Initialize to default values */
        LL_ADD_HEAD(pGc->vg.pPaths, pPath);
        pPath->pGc = pGc;
        pPath->refCount = 0;
        pPath->deletePending = FALSE;
        pPath->dirty = FALSE;
        pPath->format = format;
        pPath->datatype = datatype;
        pPath->datasize = (char)((datatype == VG_PATH_DATATYPE_S_8) ? (1) : ((datatype == VG_PATH_DATATYPE_S_16) ? (2) : (4)));
        pPath->scale = scale;
        pPath->bias = bias;
        pPath->caps = capabilities & VG_PATH_CAPABILITY_ALL;
        pPath->numSegs = 0;
        pPath->numCoords = 0;
        pPath->numUserCoords = 0;
        pPath->data = NULL;
        pPath->numVertices = 0;
        pPath->pVertexBuffer = NULL;
        pPath->winding = VG_EVEN_ODD;
        pPath->maxTriangles = 0;
        pPath->numFillTriangles = 0;
        pPath->numStrokeTriangles = 0;
        pPath->triangles = NULL;
        pPath->min[0] = pPath->min[1] = 0.0;
        pPath->max[0] = pPath->max[1] = -1.0;
        }

zz: return ((VGPath)pPath);
    }

/*******************************************************************************
 *
 * vgDestroyPath
 *
 */
VG_API_CALL void VG_API_ENTRY vgDestroyPath
    (
    VGPath path
    ) VG_API_EXIT
    {
    path_t*                         pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Delete the path */
        if (pPath->refCount > 0)
            pPath->deletePending = TRUE;
        else
            deletePath(pPath);
        }
    }

/*******************************************************************************
 *
 * vgGetPathCapabilities
 *
 */
VG_API_CALL VGbitfield VG_API_ENTRY vgGetPathCapabilities
    (
    VGPath path
    ) VG_API_EXIT
    {
    VGbitfield                      caps = 0;
    path_t*                         pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL(VG_BAD_HANDLE_ERROR);

        /* Return current capabilities */
        caps = pPath->caps;
        }

zz: return (caps);
    }

/*******************************************************************************
 *
 * vgInterpolatePath
 *
 */
VG_API_CALL VGboolean VG_API_ENTRY vgInterpolatePath
    (
    VGPath dstPath,
    VGPath startPath,
    VGPath endPath,
    VGfloat amount
    ) VG_API_EXIT
    {

    /* :TODO: */

    return (VG_FALSE);
    }

/*******************************************************************************
 *
 * vgModifyPathCoords
 *
 */
VG_API_CALL void VG_API_ENTRY vgModifyPathCoords
    (
    VGPath path,
    VGint startSeg,
    VGint numSegs,
    const void* pCoords
    ) VG_API_EXIT
    {
    path_data_t *   data;               /* path data */
    int             datatype;           /* coordinates datatype */
    int             hasRelativeCoords;  /* TRUE if the segment has relative coordinates */
    int             j;
    int             k;                  /* size in bytes */
    float           ox, oy;             /* last point of previous segment */
    float           px, py;             /* last internal control point of previous segment */
    float           rh, rv, rot;        /* horizontal and vertical radii and CCW rotation angle */
    float           scale, bias;        /* path scale and bias */
    int             seg;                /* segment */
    float           sx, sy;             /* beginning of current subpath */
    float           x0, y0;             /* segment coordinates */
    float           x1, y1;
    float           x2, y2;
    path_t *        pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((startSeg < 0) || (numSegs <= 0) || ((startSeg + numSegs) > pPath->numSegs))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(pCoords, pPath->datasize))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check capabilities */
        if ((pPath->caps & VG_PATH_CAPABILITY_MODIFY) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Set initial state */
        datatype = pPath->datatype;
        ox = oy = px = py = sx = sy = 0.0;
        scale = pPath->scale, bias = pPath->bias;

        /* Update coordinates assuming the original user-supplied format */
        data = pPath->data;
        j = 0;
        k = numBytes[datatype];
        while (numSegs > 0)
            {
            /* Process segment */
            hasRelativeCoords = data[j].seg & 0x00000100;
            seg = data[j].seg >> 8;
            switch (seg & 0x000000fe)
                {
                case VG_CLOSE_PATH:
                    px = ox = sx;
                    py = oy = sy;
                    /**/
                    j += 1;
                    break;

                case VG_MOVE_TO:
                    if (startSeg > 0)
                        {
                        x0 = data[j + 1].coord;
                        y0 = data[j + 2].coord;
                        }
                    else
                        {
                        x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x0 += ox, y0 += oy;
                        data[j + 1].coord = x0;
                        data[j + 2].coord = y0;
                        }
                    sx = px = ox = x0;
                    sy = py = oy = y0;
                    /**/
                    j += 3;
                    break;

                case VG_LINE_TO:
                    if (startSeg > 0)
                        {
                        x0 = data[j + 1].coord;
                        y0 = data[j + 2].coord;
                        }
                    else
                        {
                        x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x0 += ox, y0 += oy;
                        data[j + 1].coord = x0;
                        data[j + 2].coord = y0;
                        }
                    px = ox = x0;
                    py = oy = y0;
                    /**/
                    j += 3;
                    break;

                case VG_HLINE_TO:
                    if (startSeg > 0)
                        {
                        x0 = data[j + 1].coord;
                        y0 = data[j + 2].coord;
                        }
                    else
                        {
                        x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y0 = oy;
                        if (hasRelativeCoords)
                            x0 += ox;
                        data[j + 1].coord = x0;
                        data[j + 2].coord = y0;
                        }
                    px = ox = x0;
                    py = oy = y0;
                    /**/
                    j += 3;
                    break;

                case VG_VLINE_TO:
                    if (startSeg > 0)
                        {
                        x0 = data[j + 1].coord;
                        y0 = data[j + 2].coord;
                        }
                    else
                        {
                        x0 = ox;
                        y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            y0 += oy;
                        data[j + 1].coord = x0;
                        data[j + 2].coord = y0;
                        }
                    px = ox = x0;
                    py = oy = y0;
                    /**/
                    j += 3;
                    break;

                case VG_QUAD_TO:
                    if (startSeg > 0)
                        {
                        x0 = ((data[j + 1].coord * 3) - ox) / 2;
                        y0 = ((data[j + 2].coord * 3) - oy) / 2;
                        x1 = data[j + 5].coord;
                        y1 = data[j + 6].coord;
                        }
                    else
                        {
                        x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x0 += ox, y0 += oy, x1 +=ox, y1 += oy;
                        data[j + 1].coord = (ox + (2 * x0)) / 3;
                        data[j + 2].coord = (oy + (2 * y0)) / 3;
                        data[j + 3].coord = (x1 + (2 * x0)) / 3;
                        data[j + 4].coord = (y1 + (2 * y0)) / 3;
                        data[j + 5].coord = x1;
                        data[j + 6].coord = y1;
                        }
                    px = x0;
                    py = y0;
                    ox = x1;
                    oy = y1;
                    /**/
                    j += 7;
                    break;

                case VG_CUBIC_TO:
                    if (startSeg > 0)
                        {
                        x1 = data[j + 3].coord;
                        y1 = data[j + 4].coord;
                        x2 = data[j + 5].coord;
                        y2 = data[j + 6].coord;
                        }
                    else
                        {
                        x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        x2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x0 += ox, y0 += oy, x1 +=ox, y1 += oy, x2 +=ox, y2 += oy;
                        data[j + 1].coord = x0;
                        data[j + 2].coord = y0;
                        data[j + 3].coord = x1;
                        data[j + 4].coord = y1;
                        data[j + 5].coord = x2;
                        data[j + 6].coord = y2;
                        }
                    px = x1;
                    py = y1;
                    ox = x2;
                    oy = y2;
                    /**/
                    j += 7;
                    break;

                case VG_SQUAD_TO:
                    if (startSeg > 0)
                        {
                        x0 = (2 * ox) - px;
                        y0 = (2 * oy) - py;
                        x1 = data[j + 5].coord;
                        y1 = data[j + 6].coord;
                        }
                    else
                        {
                        x0 = (2 * ox) - px;
                        y0 = (2 * oy) - py;
                        x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x1 += ox, y1 += oy;
                        data[j + 1].coord = (ox + (2 * x0)) / 3;
                        data[j + 2].coord = (oy + (2 * y0)) / 3;
                        data[j + 3].coord = (x1 + (2 * x0)) / 3;
                        data[j + 4].coord = (y1 + (2 * y0)) / 3;
                        data[j + 5].coord = x1;
                        data[j + 6].coord = y1;
                        }
                    px = x0;
                    py = y0;
                    ox = x1;
                    oy = y1;
                    /**/
                    j += 7;
                    break;

                case VG_SCUBIC_TO:
                    if (startSeg > 0)
                        {
                        x1 = data[j + 3].coord;
                        y1 = data[j + 4].coord;
                        x2 = data[j + 5].coord;
                        y2 = data[j + 6].coord;
                        }
                    else
                        {
                        x0 = (2 * ox) - px;
                        y0 = (2 * oy) - py;
                        x1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y1 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        x2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y2 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x1 += ox, y1 += oy, x2 +=ox, y2 += oy;
                        data[j + 1].coord = x0;
                        data[j + 2].coord = y0;
                        data[j + 3].coord = x1;
                        data[j + 4].coord = y1;
                        data[j + 5].coord = x2;
                        data[j + 6].coord = y2;
                        }
                    px = x1;
                    py = y1;
                    ox = x2;
                    oy = y2;
                    /**/
                    j += 7;
                    break;

                case VG_SCCWARC_TO:
                case VG_SCWARC_TO:
                case VG_LCCWARC_TO:
                case VG_LCWARC_TO:
                    if (startSeg > 0)
                        {
                        x0 = data[j + 4].coord;
                        y0 = data[j + 5].coord;
                        }
                    else
                        {
                        rh = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        rv = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        rot = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        x0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        y0 = TRANSFORM(convertToFloat(pCoords, datatype), scale, bias);
                        pCoords = (void *)((long)pCoords + k);
                        if (hasRelativeCoords)
                            x0 += ox, y0 += oy;
                        data[j + 1].coord = rh;
                        data[j + 2].coord = rv;
                        data[j + 3].coord = rot;
                        data[j + 4].coord = x0;
                        data[j + 5].coord = y0;
                        }
                    px = ox = x0;
                    py = oy = y0;
                    /**/
                    j += 6;
                    break;
                }

            /* Decrement segment counter only when the start segment has been reached */
            if (--startSeg < 0)
                numSegs--;
            }

        /* Mark the path as dirty to force a refresh of the geometry */
        pPath->dirty = TRUE;
        }
    }

/*******************************************************************************
 *
 * vgPathBounds
 *
 */
VG_API_CALL void VG_API_ENTRY vgPathBounds
    (
    VGPath path,
    VGfloat* pMinX,
    VGfloat* pMinY,
    VGfloat* pWidth,
    VGfloat* pHeight
    ) VG_API_EXIT
    {
    path_t*                         pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((isInvalidPtr(pMinX, sizeof(VGfloat))) ||
            (isInvalidPtr(pMinY, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((isInvalidPtr(pWidth, sizeof(VGfloat))) ||
            (isInvalidPtr(pHeight, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check capabilities */
        if ((pPath->caps & VG_PATH_CAPABILITY_PATH_BOUNDS) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Create the path geometry */
        if (pPath->dirty)
            {
            if (generatePathGeometry(pPath))
                VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);
            pPath->dirty = FALSE;
            }

        /* Return the bounding box */
        if (pPath->numVertices > 0)
            {
            *pMinX = pPath->min[0];
            *pMinY = pPath->min[1];
            *pWidth = pPath->max[0] - pPath->min[0];
            *pHeight = pPath->max[1] - pPath->min[1];
            }
        else
            {
            *pMinX = *pMinY = 0.0;
            *pWidth = *pHeight = -1.0;
            }
        }
    }

/*******************************************************************************
 *
 * vgPathLength
 *
 */
VG_API_CALL VGfloat VG_API_ENTRY vgPathLength
    (
    VGPath path,
    VGint startSeg,
    VGint numSegs
    ) VG_API_EXIT
    {
    int                             i;
    VGfloat                         length = -1.0;  /* length of the subpath */
    vertex_ptr                      V;              /* vertex */
    path_t*                         pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL(VG_BAD_HANDLE_ERROR);

        if (startSeg < 0)
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((numSegs <= 0) || ((startSeg + numSegs) > pPath->numSegs))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check capabilities */
        if ((pPath->caps & VG_PATH_CAPABILITY_PATH_LENGTH) == 0)
            VG_FAIL(VG_PATH_CAPABILITY_ERROR);

        /* Create the path geometry */
        if ((pPath->dirty) || (pGc->vg.fillRule != pPath->winding))
            {
            if (generatePathGeometry(pPath))
                VG_FAIL(VG_OUT_OF_MEMORY_ERROR);
            pPath->dirty = FALSE;
            }

        /* Figure out the length of the subpath */
        i = pPath->index[startSeg + numSegs];
        if (i < 0)
            length = 0.0;
        else
            {
            V = getVertex(pPath->pVertexBuffer, i, pGc);
            if (V != NULL)
                {
                length = V->length;
                i = pPath->index[startSeg];
                if (i > 0)
                    {
                    V = getVertex(pPath->pVertexBuffer, i, pGc);
                    if (V != NULL)
                        length -= V->length;
                    }
                }
            }
        }

zz: return (length);
    }

/*******************************************************************************
 *
 * vgPathTransformedBounds
 *
 */
VG_API_CALL void VG_API_ENTRY vgPathTransformedBounds
    (
    VGPath path,
    VGfloat* pMinX,
    VGfloat* pMinY,
    VGfloat* pWidth,
    VGfloat* pHeight
    ) VG_API_EXIT
    {
    const float *   Tu;     /* "path user to surface" transformation matrix */
    float           x0, y0; /* bounding box corners */
    float           x1, y1;
    float           x2, y2;
    float           x3, y3;
    path_t *        pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((isInvalidPtr(pMinX, sizeof(VGfloat))) ||
            (isInvalidPtr(pMinY, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((isInvalidPtr(pWidth, sizeof(VGfloat))) ||
            (isInvalidPtr(pHeight, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check capabilities */
        if ((pPath->caps & VG_PATH_CAPABILITY_PATH_TRANSFORMED_BOUNDS) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Create the path geometry */
        if (pPath->dirty)
            {
            if (generatePathGeometry(pPath))
                VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);
            pPath->dirty = FALSE;
            }

        /* Return the bounding box */
        if (pPath->numVertices > 0)
            {
            /* Get the transformation matrix */
            Tu = pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_PATH_USER_TO_SURFACE)];

            /* Transform the corners  */
            x0 = pPath->min[0], y0 = pPath->min[1], transformPoint(&x0, &y0, Tu);
            x1 = pPath->min[0], y1 = pPath->max[1], transformPoint(&x1, &y1, Tu);
            x2 = pPath->max[0], y2 = pPath->max[1], transformPoint(&x2, &y2, Tu);
            x3 = pPath->max[0], y3 = pPath->min[1], transformPoint(&x3, &y3, Tu);

            /* Produce an axis-aligned bounding box */
            *pMinX = min(min(min(x0, x1), x2), x3);
            *pMinY = min(min(min(y0, y1), y2), y3);
            *pWidth = max(max(max(x0, x1), x2), x3) - *pMinX;
            *pHeight = max(max(max(y0, y1), y2), y3) - *pMinX;
            }
        else
            {
            *pMinX = *pMinY = 0.0;
            *pWidth = *pHeight = -1.0;
            }
        }
    }

/*******************************************************************************
 *
 * vgPointAlongPath
 *
 */
VG_API_CALL void VG_API_ENTRY vgPointAlongPath
    (
    VGPath path,
    VGint startSeg,
    VGint numSegs,
    VGfloat distance,
    VGfloat* pX,
    VGfloat* pY,
    VGfloat* pTangentX,
    VGfloat* pTangentY
    ) VG_API_EXIT
    {
    float       d;      /* linear interpolation coefficient */
    int         i0, i1; /* vertex indexes */
    float       tx, ty; /* tangent vector */
    vertex_ptr  V0, V1; /* vertices */
    float       x, y;   /* point */
    path_t *    pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((startSeg < 0) || (numSegs <= 0) ||
            ((startSeg + numSegs) > pPath->numSegs))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((pX != NULL) && (isInvalidPtr(pX, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((pY != NULL) && (isInvalidPtr(pY, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((pTangentX != NULL) && (isInvalidPtr(pTangentX, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((pTangentY != NULL) && (isInvalidPtr(pTangentY, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check capabilities */
        if ((pX != NULL) && (pY != NULL) &&
            (pPath->caps & VG_PATH_CAPABILITY_POINT_ALONG_PATH) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        if ((pTangentX != NULL) && (pTangentY != NULL) &&
            (pPath->caps & VG_PATH_CAPABILITY_TANGENT_ALONG_PATH) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Set default tangent for all cases without a solution */
        tx = 1.0;
        ty = 0.0;

        /* Ignore empty paths */
        if (pPath->numVertices == 0)
            {
            x = 0.0;
            y = 0.0;
            }
        else
            {
            /* Find the first and last vertices of the subpath */
            i0 = pPath->index[startSeg];
            if (i0 < 0)
                i0 = 0;
            i1 = pPath->index[startSeg + numSegs];

            V0 = getVertex(pPath->pVertexBuffer, i0, pGc);
            V1 = getVertex(pPath->pVertexBuffer, i1, pGc);
            if ((V0 == NULL)||(V1 == NULL))
                {
                /* error */
                return;
                }

            /* Avoid zero-length subpaths */
            if (i0 >= i1)
                {
                x = V0->x;
                y = V0->y;
                }
            else
                {
                /* Map the distance to the whole path's range */
                distance += V0->length;

                if (distance <= V0->length)
                    {
                    /* Use the starting point... */
                    x = V0->x;
                    y = V0->y;
                    /* ...and calculate its tangent vector */
                    if (V0->type == VERTEX_TYPE_START)
                        {
                        V1 = getVertex(pPath->pVertexBuffer, i0 + 1, pGc);
                        if (V1 != NULL)
                            {
                            tx = V1->x - x;
                            ty = V1->y - y;
                            }
                        }
                    else
                        {
                        V0 = getVertex(pPath->pVertexBuffer, i0 - 1, pGc);
                        if (V0 != NULL)
                            {
                            tx = x - V0->x;
                            ty = y - V0->x;
                            }
                        }

                    }
                else if (distance >= V1->length)
                    {
                    /* Use the ending point...*/
                    x = V1->x;
                    y = V1->y;
                    /* ...and calculate its tangent vector */
                    if (V1->type == VERTEX_TYPE_END)
                        {
                        V0 = getVertex(pPath->pVertexBuffer, i1 - 1, pGc);
                        if (V0 != NULL)
                            {
                            tx = x - V0->x;
                            ty = y - V0->y;
                            }
                        }
                    else
                        {
                        V1 = getVertex(pPath->pVertexBuffer, i1 + 1, pGc);
                        if (V1 != NULL)
                            {
                            tx = V1->x - x;
                            ty = V1->y - y;
                            }
                        }
                    }
                else
                    {
                    /* Search for the segment "containing" the distance */
                    while (i0 < i1)
                        {
                        V1 = getVertex(pPath->pVertexBuffer, ++i0, pGc);

                        if (V1 == NULL)
                            {
                            /* error */
                            return;
                            }

                        if (distance < V1->length)
                            break;
                        V0 = V1;
                        }

                    /* Lerp the position... */
                    d = (distance - V0->length) / (V1->length - V0->length);
                    x = ((1.0f - d) * V0->x) + (d * V1->x);
                    y = ((1.0f - d) * V0->y) + (d * V1->y);
                    /* ...and calculate the tangent vector */
                    tx = V1->x - V0->x;
                    ty = V1->y - V0->y;
                    }
                }
            }

        /* Return */
        if ((pX != NULL) && (pY != NULL))
            {
            *pX = x;
            *pY = y;
            }

        if ((pTangentX != NULL) && (pTangentY != NULL))
            {
            d = 1.0f / (float)sqrt((double)(SQUARED(tx) + SQUARED(ty)));
            *pTangentX = tx * d;
            *pTangentY = ty * d;
            }
        }
    }

/*******************************************************************************
 *
 * vgRemovePathCapabilities
 *
 */
VG_API_CALL void VG_API_ENTRY vgRemovePathCapabilities
    (
    VGPath path,
    VGbitfield capabilities
    ) VG_API_EXIT
    {
    path_t*                         pPath = (path_t*)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Disable capabilities */
        pPath->caps &= ~(capabilities & VG_PATH_CAPABILITY_ALL);
        }
    }

/*******************************************************************************
 *
 * vgTransformPath
 *
 */
VG_API_CALL void VG_API_ENTRY vgTransformPath
    (
    VGPath dstPath,
    VGPath srcPath
    ) VG_API_EXIT
    {
    path_data_t*                    data;               /* transformed path data */
    int                             i, j, k;
    float                           M[9];               /* ellipse parameterization matrix */
    int                             numSegs, numCoords; /* number of segments and coordinates in the source path */
    int                             numUserCoords;      /* number of user supplied coordinates in the source path */
    float                           rh, rv, rot;        /* horizontal and vertical radii and CCW rotation angle */
    int                             seg;                /* segment */
    const float*                    Tu;                 /* "path user to surface" transformation matrix */
    float                           x0, y0;             /* segment coordinates */
    float                           x1, y1;
    float                           x2, y2;
    path_t*                         pDstPath = (path_t*)dstPath;
    path_t*                         pSrcPath = (path_t*)srcPath;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pDstPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidPath(pSrcPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Check capabilities */
        if ((pDstPath->caps & VG_PATH_CAPABILITY_TRANSFORM_TO) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        if ((pSrcPath->caps & VG_PATH_CAPABILITY_TRANSFORM_FROM) == 0)
            VG_FAIL_VOID(VG_PATH_CAPABILITY_ERROR);

        /* Avoid transforming empty paths */
        numSegs = pSrcPath->numSegs, numCoords = pSrcPath->numCoords;
        if (numSegs <= 0)
            return;

        /* Allocate memory */
        data = realloc(pDstPath->data, SIZEOF_PATH_DATA(pDstPath->numSegs + numSegs, pDstPath->numCoords + numCoords));
        if (data == NULL)
            VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);

        /* Get the transformation matrix */
        Tu = pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_PATH_USER_TO_SURFACE)];

        /* Transform the source path's internal data */
        numUserCoords = pSrcPath->numUserCoords;
        for (i = pDstPath->numSegs + pDstPath->numCoords, j = k = 0; j < pSrcPath->numSegs; j++)
            {
            seg = pSrcPath->data[k++].seg;
            switch ((seg >> 8) & 0x000000fe)
                {
                case VG_CLOSE_PATH:
                    data[i++].seg = seg;
                    break;

                case VG_MOVE_TO:
                case VG_LINE_TO:
                    x0 = pSrcPath->data[k++].coord;
                    y0 = pSrcPath->data[k++].coord;
                    /**/
                    transformPoint(&x0, &y0, Tu);
                    /**/
                    data[i++].seg = seg;
                    data[i++].coord = x0;
                    data[i++].coord = y0;
                    break;

                case VG_HLINE_TO:
                case VG_VLINE_TO:
                    x0 = pSrcPath->data[k++].coord;
                    y0 = pSrcPath->data[k++].coord;
                    /**/
                    transformPoint(&x0, &y0, Tu);
                    /**/
                    data[i++].seg = (seg & 0x000001ff) + (VG_LINE_TO << 8);
                    data[i++].coord = x0;
                    data[i++].coord = y0;
                    /**/
                    numUserCoords++;
                    break;

                case VG_QUAD_TO:
                case VG_CUBIC_TO:
                case VG_SQUAD_TO:
                case VG_SCUBIC_TO:
                    x0 = pSrcPath->data[k++].coord;
                    y0 = pSrcPath->data[k++].coord;
                    x1 = pSrcPath->data[k++].coord;
                    y1 = pSrcPath->data[k++].coord;
                    x2 = pSrcPath->data[k++].coord;
                    y2 = pSrcPath->data[k++].coord;
                    /**/
                    transformPoint(&x0, &y0, Tu);
                    transformPoint(&x1, &y1, Tu);
                    transformPoint(&x2, &y2, Tu);
                    /**/
                    data[i++].seg = seg;
                    data[i++].coord = x0;
                    data[i++].coord = y0;
                    data[i++].coord = x1;
                    data[i++].coord = y1;
                    data[i++].coord = x2;
                    data[i++].coord = y2;
                    break;

                case VG_SCCWARC_TO:
                case VG_SCWARC_TO:
                case VG_LCCWARC_TO:
                case VG_LCWARC_TO:
                    rh = pSrcPath->data[k++].coord;
                    rv = pSrcPath->data[k++].coord;
                    rot = DEG_TO_RAD(pSrcPath->data[k++].coord);
                    x0 = pSrcPath->data[k++].coord;
                    y0 = pSrcPath->data[k++].coord;
                    /**/
                    /* Map the unit circle to the transformed ellipse (see specification, "Appendix A: Mathematics of
                     * Ellipses" for more information)
                     */
                    M[0] = (Tu[0] * (float)cos((double)rot) * rh) + (Tu[3] * (float)sin((double)rot) * rh);
                    M[1] = (Tu[1] * (float)cos((double)rot) * rh) + (Tu[4] * (float)sin((double)rot) * rh);
                    M[2] = 0.0;
                    M[3] = (Tu[3] * (float)cos((double)rot) * rv) - (Tu[0] * (float)sin((double)rot) * rv);
                    M[4] = (Tu[4] * (float)cos((double)rot) * rv) - (Tu[1] * (float)sin((double)rot) * rv);
                    M[5] = 0.0;
                    M[6] = Tu[6];
                    M[7] = Tu[7];
                    M[8] = 1.0;
                    /* Calculate the new rotation angle and the horizontal and vertical radii */
                    calcEllipseFromMatrix(M, &rh, &rv, &rot);
                    transformPoint(&x0, &y0, Tu);
                    /* Change winding if the transformation flips the Y axis */
                    if (DETERMINANT(Tu) < 0.0)
                        {
                        switch (seg & 0x000000fe)
                            {
                            case VG_SCCWARC_TO:
                                seg = (seg & 0x00000100) + (VG_SCWARC_TO << 8) + VG_SCWARC_TO;
                                break;

                            case VG_SCWARC_TO:
                                seg = (seg & 0x00000100) + (VG_SCCWARC_TO << 8) + VG_SCCWARC_TO;
                                break;

                            case VG_LCCWARC_TO:
                                seg = (seg & 0x00000100) + (VG_LCWARC_TO << 8) + VG_LCWARC_TO;
                                break;

                            case VG_LCWARC_TO:
                                seg = (seg & 0x00000100) + (VG_LCCWARC_TO << 8) + VG_LCCWARC_TO;
                                break;
                            }
                        }
                    /**/
                    data[i++].seg = seg;
                    data[i++].coord = rh;
                    data[i++].coord = rh;
                    data[i++].coord = RAD_TO_DEG(rot);
                    data[i++].coord = x0;
                    data[i++].coord = y0;
                    break;
                }
            }

        /* Update the counters */
        pDstPath->numSegs += pSrcPath->numSegs;
        pDstPath->numCoords += pSrcPath->numCoords;
        pDstPath->numUserCoords += numUserCoords;

        /* Reassign data */
        pDstPath->data = data;

        /* Mark the destination path as dirty to force a refresh of the geometry */
        pDstPath->dirty = TRUE;
        }
    }
