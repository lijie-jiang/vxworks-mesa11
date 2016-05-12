/* vgimpl.h - Wind River VG Implementation Specific Header */

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
25jun13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
*/

#ifndef __vgimpl_h
#define __vgimpl_h

/* includes */

#include <errno.h>
#include <VG/openvg.h>
#include <VG/vgwrs.h>
#include <gfxVgRaster.h>
#include <gfxVgUtils.h>

/* defines */

#define VG_STOP_OFFSET(n)  (((n) * 5) + 0)
#define VG_STOP_R(n)       (((n) * 5) + 1)
#define VG_STOP_G(n)       (((n) * 5) + 2)
#define VG_STOP_B(n)       (((n) * 5) + 3)
#define VG_STOP_A(n)       (((n) * 5) + 4)

#define VG_FLOAT   0
#define VG_INT     1
#define VG_BOOLEAN 1

#define VG_PAINT_MODE_MASK      0x00000003
#define VG_IMAGE_QUALITY_MASK   0x00000007
#define VG_IMAGE_CHANNEL_MASK   0x0000000f

/* Color ramp maximum coordinate and masks */
#define COLOR_RAMP_SIZE         (1 << COLOR_RAMP_SIZE2)
#define COLOR_RAMP_MAX_COORD    (COLOR_RAMP_SIZE - 1)
#define COLOR_RAMP_MASK_REPEAT  (COLOR_RAMP_SIZE - 1)
#define COLOR_RAMP_MASK_REFLECT ((COLOR_RAMP_SIZE * 2) - 1)

/* Error handling */
#define VG_FAIL(_err)       do                              \
                                {                           \
                                    pGc->vg.error = _err;   \
                                    errno = pGc->error;     \
                                    goto zz;                \
                                }                           \
                            while (0)
#define VG_FAIL_VOID(_err)  do                              \
                                {                           \
                                    pGc->vg.error = _err;   \
                                    errno = pGc->error;     \
                                    return;                 \
                                }                           \
                            while (0)

/* Limits */
#define MAX_IMAGE_BYTES             2147483646
#define MAX_IMAGE_HEIGHT            1073741822
#define MAX_IMAGE_PIXELS            2147483646
#define MAX_IMAGE_WIDTH             1073741822
#define MAX_INT                     2147483647
#define MAX_KERNEL_SIZE             7
#define MAX_SCISSOR_RECTS           ((32) * 4)
#define MAX_SEPARABLE_KERNEL_SIZE   15
#define MAX_STOPS                   ((32) * 5)

/* Matrix mode-to-index and index-to-mode conversion */
#define MATRIX_INDEX(_mode)         ((_mode) - 0x1400)
#define MATRIX_MODE(_i)             ((_i) + 0x1400)

/* Object types */
#define    OBJECT_TYPE_PATH         0
#define    OBJECT_TYPE_IMAGE        1
#define    OBJECT_TYPE_MASK_LAYER   2
#define    OBJECT_TYPE_FONT         3
#define    OBJECT_TYPE_PAINT        4

/* Paint pattern */
#define PATTERN_DENORM_LUT_SIZE     (1 << PATTERN_DENORM_LUT_SIZE2)
#define PATTERN_DENORM_LUT_SIZE2    (PATTERN_NORM_SIZE2 + 1)
#define PATTERN_NORM_SIZE           (1 << PATTERN_NORM_SIZE2)

/* Paths parameters */
#define    MAX_SUBDIV_LEVEL         10  /* maximum subdivision level */

/* Sizes */
#define ANY_SIZE        1   /* hint for variable size structures */
#define RECT_POOL_SIZE  102 /* rectangle pool size (roughly MAX_SCISSOR_RECTS/1.25) */

/* Type conversions */
#define    FROM_INT(_var, _type, _i)   do                               \
                                        {                               \
                                        if (_type == VG_INT)            \
                                            (_var).i = _i;              \
                                        else                            \
                                            (_var).f = (VGfloat)(_i);   \
                                        }                               \
                                    while (0)
#define FROM_FLOAT(_var, _type, _f) do                                  \
                                        {                               \
                                        if (_type == VG_INT)            \
                                            (_var).i = ifloor(_f);      \
                                        else                            \
                                            (_var).f = _f;              \
                                        }                               \
                                    while (0)
#define TO_FLOAT(_var, _type)   ((_type == VG_INT) ? ((VGfloat)(_var).i) : ((_var).f))
#define TO_INT(_var, _type)     ((_type == VG_INT) ? ((_var).i) : (ifloor((_var).f)))

/* - Vertex types */
#define VERTEX_TYPE_REGULAR     0
#define VERTEX_TYPE_START       1
#define VERTEX_TYPE_END         2
#define VERTEX_TYPE_CALCULATED  3

/* Paint (see specifications for details) */
#define linearGradient_x0 linearGradient[0]
#define linearGradient_y0 linearGradient[1]
#define linearGradient_x1 linearGradient[2]
#define linearGradient_y1 linearGradient[3]
#define linearGradient_dx linearGradient[4]
#define linearGradient_dy linearGradient[5]
#define linearGradient_q  linearGradient[6]

#define radialGradient_cx radialGradient[0]
#define radialGradient_cy radialGradient[1]
#define radialGradient_fx radialGradient[2]
#define radialGradient_fy radialGradient[3]
#define radialGradient_r  radialGradient[4]
#define radialGradient_a  radialGradient[5]
#define radialGradient_b  radialGradient[6]
#define radialGradient_c  radialGradient[7]
#define radialGradient_d  radialGradient[8]
#define radialGradient_e  radialGradient[9]

#define    isInvalidPtr(_ptr, _align)    (((_ptr) == NULL) || (IS_NOT_ALIGNED(_ptr, _align)))

/* enums */

enum
    {
    VG_R = 0,
    VG_G = 1,
    VG_B = 2,
    VG_A = 3
    };

/* typedefs */

typedef union
    {
    VGbyte    b;
    VGfloat   f;
    VGint     i;
    VGshort   s;
    } _VGType;

typedef struct
    {
    VGint   x0, y0;
    VGint   width, height;
    } _VGRectangle;

typedef struct font             font_t;
typedef struct glyph            glyph_t;
typedef struct image            image_t;
typedef struct pattern          pattern_t;
typedef struct paint            paint_t;
typedef struct path             path_t;
typedef union path_data         path_data_t;
typedef union pixel_ptr         pixel_ptr_t;
typedef struct vertex           vertex_t;
typedef struct vertex *         vertex_ptr;
typedef struct vertex_buffer    vertex_buffer_t;

/* Font */
struct font
    {
    struct font *   pNext;          /* linked list node */
    gc_t *          pGc;            /* graphics context */
    uint            refCount;       /* reference counter */
    char            deletePending;  /* TRUE if deletion is pending */
    int             glyphCapacity;  /* glyph capacity */
    int             numGlyphs;      /* number of glyphs */
    glyph_t *       pRootGlyph;     /* glyph AVL tree root node */
    };

/* Glyph */
struct glyph
    {
    VGHandle        handle;         /* handle */
    char            type;           /* type */
    uint            index;          /* user-defined index */
    float           origin[3];      /* origin (point)*/
    float           escapement[2];  /* escapement */
    struct glyph *  pLeftChild;     /* AVL tree node */
    struct glyph *  pRightChild;
    int             height;
    };

/* Image */
struct image
    {
    struct image *  pNext;          /* linked list node */
    gc_t *          pGc;            /* graphics context */
    uint            refCount;       /* reference counter */
    char            deletePending;  /* TRUE if deletion is pending */
    char            inUse;          /* TRUE if used as a rendering target */
    struct image *  pParent;        /* parent image */
    int             format, bpp;    /* pixel format, bit depth */
    int             x0, y0;         /* top-left corner */
    int             width, height;  /* width and height in pixels */
    surface_t *     pSurface;       /* EGL pbuffer surface */
    };

struct pattern
    {
    VGTilingMode    tilingMode; /* tiling mode */
    VGuint          fillColor;  /* fill color */
    image_t *       pImage;     /* underlying image */
#ifdef PATTERN_DENORM_LUT_IS_DYNAMIC    /* denormalization look-up table */
    int *           denormLut;
#else
    int             denormLut[PATTERN_DENORM_LUT_SIZE * 2];
#endif /* PATTERN_DENORM_LUT_IS_DYNAMIC */
    };

/* Paint */
struct paint
    {
    struct paint *          pNext;                  /* linked list node */
    gc_t *                  pGc;                    /* graphics context */
    uint                    refCount;               /* reference counter */
    char                    deletePending;          /* TRUE if deletion is pending */
    char                    dirty;                  /* TRUE if paint has been modified */
    VGPaintType             type;                   /* type */
    float                   color[4];               /* color */
    VGColorRampSpreadMode   spreadMode;             /* color ramp spread mode */
    int                     numStops;               /* number of color ramp stops * 5 */
    float                   stops[MAX_STOPS];       /* color ramp stops */
    int                     premultiplyColorRamp;   /* VG_TRUE or VG_FALSE (practically, it's ignored) */
    float                   linearGradient[4 + 3];  /* x0, y0, x1, y1 */
    float                   radialGradient[5 + 5];  /* cx, cy, fx, fy, r */
    VGImageFormat           format;                 /* internal color format */
    VGuint                  color2;                 /* converted color */
    VGuint *                colorRamp;              /* color ramp */
    pattern_t               pattern;                /* paint pattern */
    };

/* Path */
struct path
    {
    struct path *           pNext;              /* linked list node */
    gc_t *                  pGc;                /* graphics context */
    uint                    refCount;           /* reference counter */
    char                    deletePending;      /* TRUE if deletion is pending */
    char                    dirty;              /* TRUE if path has been modified */
    int                     format;             /* VG_PATH_FORMAT_STANDARD */
    char                    datatype, datasize; /* type and size of input data */
    float                   scale, bias;        /* scale and bias */
    uint                    caps;               /* capabilities */
    int                     numSegs;            /* number of segments */
    int                     numCoords;          /* number of coordinates */
    int                     numUserCoords;      /* number of user-supplied coordinates */
    path_data_t *           data;               /* user-supplied data */
    int*                    index;              /* segment-to-vertex index */
    int                     numVertices;        /* number of vertices */
    vertex_buffer_t *       pVertexBuffer;      /* vertices */
    int                     winding;            /* winding rule at time of triangulation */
    int                     maxTriangles;       /* maximum number of triangles */
    int                     numFillTriangles;   /* number of fill triangles */
    int                     numStrokeTriangles; /* number of stroke triangles */
    vertex_ptr *            triangles;          /* triangles */
    float                   min[2];             /* bounding box */
    float                   max[2];
    };

union path_data
    {
    int     seg;    /* segment */
    float   coord;  /* coordinate */
    };

union pixel_ptr
    {
    void *      v;      /* abstract */
    uchar *     u8;     /* 8 bpp */
    ushort *    u16;    /* 16 bpp */
    uint *      u32;    /* 32 bpp */
    };

/* Vertex */
struct vertex
    {
    char    type;       /* type */
    float   x, y, w;    /* homogeneous coordinates */
    float   u, v;       /* mapping coordinates */
    float   length;     /* running length */
    };

/* Vertex buffer */
struct vertex_buffer
    {
    struct vertex_buffer *  pNext;                  /* linked list node */
    int                     k;                      /* number of vertices in the bucket */
    vertex_t                bucket[BUCKET_SIZE];    /* vertex bucket */
    };


/* forward declarations */

/* Helper routines */
int isInvalidDataPtr(const void* pData, const int format);
int isInvalidFont(const font_t* pFont, gc_t* pGc);
int isInvalidImage(const image_t* pImage, gc_t* pGc);
int isInvalidPaint(const paint_t* pPaint, gc_t* pGc);
int isInvalidPath(const path_t* pPath, gc_t* pGc);
int getObjectType(const void* pObject, gc_t* pGc);

#endif    /* __vgimpl_h */
