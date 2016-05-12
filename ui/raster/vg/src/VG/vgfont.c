/* vgfont.c - Wind River VG Extension Functionality */

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
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
19jun09,m_c  Written
*/

/*
DESCRIPTION
These routines provide font functionality that support the OpenVG
implementation.
*/

/* includes */

#include <stdlib.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* forward declarations */

LOCAL int          addGlyph(font_t*, VGuint, VGHandle, char, const float*, const float*);
LOCAL void         avlDelete(glyph_t*);
LOCAL int          avlInsert(uint, VGHandle, char, const float*, const float*, glyph_t**);
LOCAL int          avlRemove(uint, glyph_t**);
LOCAL glyph_t *    avlRotateLL(glyph_t*);
LOCAL glyph_t *    avlRotateLR(glyph_t*);
LOCAL glyph_t *    avlRotateRL(glyph_t*);
LOCAL glyph_t *    avlRotateRR(glyph_t*);
LOCAL void         unbindGlyph(glyph_t*);

/*******************************************************************************
 *
 * balanceFactor - return the balance factor of the AVL tree at the specified glyph
 *
 */
LOCAL __inline__ int balanceFactor
    (
    glyph_t* pGlyph
    )
    {
    int                             h1, h2;

    if (pGlyph == NULL)
        return (0);
    h1 = (pGlyph->pLeftChild == NULL) ? (0) : (1 + pGlyph->pLeftChild->height);
    h2 = (pGlyph->pRightChild == NULL) ? (0) : (1 + pGlyph->pRightChild->height);
    return (h1 - h2);
    }

/*******************************************************************************
 *
 * treeHeight - return the height of the AVL tree at the specified glyph
 *
 */
LOCAL __inline__ int treeHeight
    (
    glyph_t* pGlyph
    )
    {
    return ((pGlyph == NULL) ? (-1) : (pGlyph->height));
    }

/*******************************************************************************
 *
 * treeHeight - return the largest of 2 tree heights
 *
 */
LOCAL __inline__ int treeMax
    (
    int h1,
    int h2
    )
    {
    return ((h1 > h2) ? (h1) : (h2));
    }

/*******************************************************************************
 *
 * addGlyph - add a glyph to the specified font
 *
 * RETURNS: Zero if successful
 *
 */
LOCAL int addGlyph
    (
    font_t* pFont,
    VGuint index,
    VGHandle handle,
    char type,
    const float* origin,
    const float* escapement
    )
    {
    glyph_t *   pGlyph;

    /* See if the font already has a glyph defined for the specified index */
    pGlyph = pFont->pRootGlyph;
    while (pGlyph != NULL)
        {
        if (index < pGlyph->index)
            pGlyph = pGlyph->pLeftChild;
        else if (index == pGlyph->index)
            break;
        else
            pGlyph = pGlyph->pRightChild;
        }

    /* Replace the existing glyph... */
    if (pGlyph != NULL)
        {
        unbindGlyph(pGlyph);

        /* Overwrite the glyph */
        pGlyph->handle = handle;
        pGlyph->type = type;
        pGlyph->index = index;
        pGlyph->origin[0] = origin[0];
        pGlyph->origin[1] = origin[1];
        pGlyph->origin[2] = 1.0;
        pGlyph->escapement[0] = escapement[0];
        pGlyph->escapement[1] = escapement[1];

        }
    /* ...or add it */
    else
        {
        /* Insert into the tree */
        if (avlInsert(index, handle, type, origin, escapement, &pFont->pRootGlyph))
            return (1);

        /* Update the font */
        pFont->numGlyphs++;
        }

    return (0);
    }

/*******************************************************************************
 *
 * avlDelete - delete the specified glyph and all its children
 *
 */
LOCAL void avlDelete
    (
    glyph_t* pGlyph
    )
    {
    if (pGlyph != NULL)
        {
        avlDelete(pGlyph->pLeftChild);
        avlDelete(pGlyph->pRightChild);
        unbindGlyph(pGlyph);
        free(pGlyph);
        }
    }

/*******************************************************************************
 *
 * avlInsert - insert a glyph into the specified glyph tree
 *
 * RETURNS: Zero if successful
 *
 */
LOCAL int avlInsert
    (
    uint index,
    VGHandle handle,
    char type,
    const float* origin,
    const float* escapement,
    glyph_t** ppRootGlyph
    )
    {
    glyph_t * pGlyph = *ppRootGlyph;

    /* Is this an empty tree? */
    if (pGlyph == NULL)
        {
        /* Yes, create a new glyph... */
        pGlyph = malloc(sizeof(glyph_t));
        if (pGlyph == NULL)
            return (1);
        /* ...and initialize it */
        pGlyph->handle = handle;
        pGlyph->type = type;
        pGlyph->index = index;
        pGlyph->origin[0] = origin[0];
        pGlyph->origin[1] = origin[1];
        pGlyph->origin[2] = 1.0;
        pGlyph->escapement[0] = escapement[0];
        pGlyph->escapement[1] = escapement[1];
        pGlyph->pLeftChild = pGlyph->pRightChild = NULL;
        pGlyph->height = 0;
        }
    else
        {
        /* No, figure out where to add the new glyph */
        if (index < pGlyph->index)
            {
            /* Insert into the left subtree... */
            if (0 != avlInsert(index, handle, type, origin, escapement, &pGlyph->pLeftChild))
                return (1);
            /* ...and make sure the tree is still balanced */
            if (balanceFactor(pGlyph) == 2)
                {
                if (index < pGlyph->pLeftChild->index)
                    pGlyph = avlRotateLL(pGlyph);
                else
                    pGlyph = avlRotateLR(pGlyph);
                }
            }
        else if  (index == pGlyph->index)
            {
            /* The glyph is already in the tree! Do nothing as, per specifications, this should not happen */
            }
        else
            {
            /* Insert into the right subtree... */
            if (0 != avlInsert(index, handle, type, origin, escapement, &pGlyph->pRightChild))
                return (1);
            /* ...and make sure the tree is still balanced */
            if (balanceFactor(pGlyph) == -2)
                {
                if (index > pGlyph->pRightChild->index)
                    pGlyph = avlRotateRR(pGlyph);
                else
                    pGlyph = avlRotateRL(pGlyph);
                }
            }

        /* Update tree height */
        pGlyph->height = treeMax(treeHeight(pGlyph->pLeftChild), treeHeight(pGlyph->pRightChild)) + 1;
        }

    /* Update tree root as it might have changed */
    *ppRootGlyph = pGlyph;

    return (0);
    }

/*******************************************************************************
 *
 * avlRemove - remove the glyph with the specified index
 *
 * RETURNS: Zero if successful
 *
 */
LOCAL int avlRemove
    (
    uint index,
    glyph_t ** ppRootGlyph
    )
    {
    glyph_t *   pDeadGlyph;
    glyph_t *   pGlyph = *ppRootGlyph;

    if (pGlyph == NULL)
        return (1);
    else if (index < pGlyph->index)
        {
        if (avlRemove(index, &pGlyph->pLeftChild))
            return (1);
        if (balanceFactor(pGlyph) == -2)
            {
            if (balanceFactor(pGlyph->pRightChild) <= 0)
                pGlyph = avlRotateRR(pGlyph);
            else
                pGlyph = avlRotateRL(pGlyph);
            }
        }
    else if (index == pGlyph->index)
        {
        /* Unbind glyph (this ensure the underlying object get deleted if necessary) */
        unbindGlyph(pGlyph);

        /* Determine how to proceed */
        if ((pGlyph->pLeftChild != NULL) && (pGlyph->pRightChild != NULL))
            {
            /* Find predecessor */
            pDeadGlyph = pGlyph->pRightChild;
            while (pDeadGlyph->pLeftChild != NULL)
                pDeadGlyph = pDeadGlyph->pLeftChild;

            /* Replace glyph by predecessor */
            pGlyph->handle = pDeadGlyph->handle;
            pGlyph->type = pDeadGlyph->type;
            pGlyph->index = pDeadGlyph->index;
            pGlyph->origin[0] = pDeadGlyph->origin[0];
            pGlyph->origin[1] = pDeadGlyph->origin[1];
            pGlyph->origin[2] = pDeadGlyph->origin[2];
            pGlyph->escapement[0] = pDeadGlyph->escapement[0];
            pGlyph->escapement[1] = pDeadGlyph->escapement[1];

            /* Prevent removal of bound object */
            pDeadGlyph->handle = (VGHandle)NULL;

            /* Complete removal */
            (void) avlRemove(pDeadGlyph->index, &pGlyph->pRightChild);
            }
        else
            {
            if (pGlyph->pLeftChild != NULL)
                {
                pDeadGlyph = pGlyph->pLeftChild;

                /* Replace by left child */
                *pGlyph = *pGlyph->pLeftChild;
                }
            else if (pGlyph->pRightChild != NULL)
                {
                pDeadGlyph = pGlyph->pRightChild;

                /* Replace by right child */
                *pGlyph = *pGlyph->pRightChild;
                }
            else
                {
                pDeadGlyph = pGlyph;

                pGlyph = NULL;
                }

            /* Discard the now unused glyph */
            free(pDeadGlyph);
            }
        }
    else
        {
        if (avlRemove(index, &pGlyph->pRightChild))
            return (1);
        if (balanceFactor(pGlyph) == 2)
            {
            if (balanceFactor(pGlyph->pLeftChild) >= 0)
                pGlyph = avlRotateLL(pGlyph);
            else
                pGlyph = avlRotateLR(pGlyph);
            }
        }

    /* Update tree height */
    if (pGlyph != NULL)
        pGlyph->height = treeMax(treeHeight(pGlyph->pLeftChild), treeHeight(pGlyph->pRightChild)) + 1;

    /* Update tree root as it might have changed */
    *ppRootGlyph = pGlyph;

    return (0);
    }

/*******************************************************************************
 *
 * avlRotateLL
 *
 * RETURNS: The new root node
 *
 */
LOCAL glyph_t * avlRotateLL
    (
    glyph_t* pRootGlyph
    )
    {
    glyph_t * pNewRootGlyph;

    pNewRootGlyph = pRootGlyph->pLeftChild;
    pRootGlyph->pLeftChild = pNewRootGlyph->pRightChild;
    pNewRootGlyph->pRightChild = pRootGlyph;

    pRootGlyph->height = treeMax(treeHeight(pRootGlyph->pLeftChild), treeHeight(pRootGlyph->pRightChild)) + 1;
    pNewRootGlyph->height = treeMax(treeHeight(pNewRootGlyph->pLeftChild), pRootGlyph->height)+ 1;

    return (pNewRootGlyph);
    }

/*******************************************************************************
 *
 * avlRotateLR
 *
 * RETURNS: The new root node
 *
 */
LOCAL glyph_t * avlRotateLR
    (
    glyph_t* pRootGlyph
    )
    {
    pRootGlyph->pLeftChild = avlRotateRR(pRootGlyph->pLeftChild);

    return (avlRotateLL(pRootGlyph));
    }

/*******************************************************************************
 *
 * avlRotateRL
 *
 * RETURNS: The new root node
 *
 */
LOCAL glyph_t * avlRotateRL
    (
    glyph_t* pRootGlyph
    )
    {
    pRootGlyph->pRightChild = avlRotateLL(pRootGlyph->pRightChild);

    return (avlRotateRR(pRootGlyph));
    }

/*******************************************************************************
 *
 * avlRotateRR
 *
 * RETURNS: The new root node
 *
 */
LOCAL glyph_t * avlRotateRR
    (
    glyph_t* pRootGlyph
    )
    {
    glyph_t *   pNewRootGlyph;

    pNewRootGlyph = pRootGlyph->pRightChild;
    pRootGlyph->pRightChild = pNewRootGlyph->pLeftChild;
    pNewRootGlyph->pLeftChild = pRootGlyph;

    pRootGlyph->height = treeMax(treeHeight(pRootGlyph->pLeftChild), treeHeight(pRootGlyph->pRightChild)) + 1;
    pNewRootGlyph->height = treeMax(treeHeight(pNewRootGlyph->pRightChild), pRootGlyph->height) + 1;

    return (pNewRootGlyph);
    }

/*******************************************************************************
 *
 * unbindGlyph - unbind the specified glyph
 *
 */
LOCAL void unbindGlyph
    (
    glyph_t* pGlyph
    )
    {
    image_t *   pImage; /* image */
    path_t *    pPath;  /* path */

    /* Unbind the glyph and, if needed, destroy the underlying object */
    if (pGlyph->handle != (VGHandle)NULL)
        {
        if (pGlyph->type == OBJECT_TYPE_PATH)
            {
            pPath = (path_t *)pGlyph->handle;
            pPath->refCount--;
            if ((pPath->refCount == 0) && (pPath->deletePending))
                {
                pPath->deletePending = FALSE;
                vgDestroyPath((VGPath)pPath);
                }
            }
        else if (pGlyph->type == OBJECT_TYPE_IMAGE)
            {
            pImage = (image_t *)pGlyph->handle;
            pImage->refCount--;
            if ((pImage->refCount == 0) && (pImage->deletePending))
                {
                pImage->deletePending = FALSE;
                vgDestroyImage((VGImage)pImage);
                }
            }

        /* This will make sure the operation is not done twice */
        pGlyph->handle = (VGHandle)NULL;
        }
    }

/*******************************************************************************
 *
 * vgClearGlyph
 *
 */
VG_API_CALL void VG_API_ENTRY vgClearGlyph
    (
    VGFont font,
    VGuint index
    ) VG_API_EXIT
    {
    font_t * pFont = (font_t *) font;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidFont(pFont, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Remove the specified glyph */
        if (avlRemove(index, &pFont->pRootGlyph))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Update counter */
        pFont->numGlyphs--;
        }
    }

/*******************************************************************************
 *
 * vgCreateFont
 *
 */
VG_API_CALL VGFont VG_API_ENTRY vgCreateFont
    (
    VGint glyphCapacityHint
    ) VG_API_EXIT
    {
    font_t *    pFont = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (glyphCapacityHint < 0)
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Allocate memory for the structure */
        pFont = malloc(sizeof(font_t));
        if (pFont == NULL)
            VG_FAIL(VG_OUT_OF_MEMORY_ERROR);

        /* Initialize to default values */
        LL_ADD_HEAD(pGc->vg.pFonts, pFont);
        pFont->pGc = pGc;
        pFont->refCount = 0;
        pFont->deletePending = FALSE;
        pFont->glyphCapacity = 0;
        pFont->numGlyphs = 0;
        pFont->pRootGlyph = NULL;

        /*
         * Because the glyphs are stored in an AVL tree, the "glyphCapacityHint"
         * argument is ignored. This, to some extent, violates the
         * specifications:
         *
         * " The glyphCapacityHint argument provides a hint as to the capacity
         *   of a VGFont, i.e., the total number of glyphs that this VGFont
         *   object will be required to accept. "
         */
        }

zz: return ((VGFont)pFont);
    }

/*******************************************************************************
 *
 * vgDestroyFont
 *
 */
VG_API_CALL void VG_API_ENTRY vgDestroyFont
    (
    VGFont font
    ) VG_API_EXIT
    {
    font_t * pFont = (font_t *)font;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidFont(pFont, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Delete the font */
        if (pFont->refCount > 0)
            pFont->deletePending = TRUE;
        else
            {
            /* Unlink */
            LL_REMOVE(pFont->pGc->vg.pFonts, pFont);

            /* Discard AVL tree */
            avlDelete(pFont->pRootGlyph);

            /* Free allocated memory */
            free(pFont);
            }
        }
    }

/*******************************************************************************
 *
 * vgSetGlyphToImage
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetGlyphToImage
    (
    VGFont font,
    VGuint index,
    VGImage image,
    const VGfloat origin[2],
    const VGfloat escapement[2]
    ) VG_API_EXIT
    {
    font_t * pFont = (font_t *)font;
    image_t* pImage = (image_t *)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidFont(pFont, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((pImage != NULL) && (isInvalidImage(pImage, pGc)))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidPtr(origin, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(escapement, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Add the glyph to the font */
        if (addGlyph(pFont, index, (VGHandle)pImage, OBJECT_TYPE_IMAGE, origin, escapement))
            VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);

        /* Bind the image to the font */
        if (pImage != NULL)
            pImage->refCount++;
        }

    }

/*******************************************************************************
 *
 * vgSetGlyphToPath
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetGlyphToPath
    (
    VGFont font,
    VGuint index,
    VGPath path,
    VGboolean isHinted,
    const VGfloat origin[2],
    const VGfloat escapement[2]
    ) VG_API_EXIT
    {
    font_t * pFont = (font_t *)font;
    path_t* pPath = (path_t *)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidFont(pFont, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((pPath != NULL) && (isInvalidPath(pPath, pGc)))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidPtr(origin, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(escapement, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Add the glyph to the font */
        if (addGlyph(pFont, index, (VGHandle)pPath, OBJECT_TYPE_PATH, origin, escapement))
            VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);

        /* Bind the path to the font */
        if (pPath != NULL)
            pPath->refCount++;
        }
    }
