/* idr.c - idr management functions */

/*
 * Copyright (c) 1999-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
14sep15,yat  Clean up code (US66034)
01apr15,rpc  Static analysis code review change CR-vx7-3237
30mar15,rpc  Static analysis 3rd pass for idr_add_to_free
30mar15,rpc  Static analysis 2nd pass for currentIdr
27mar15,rpc  Static analysis fixes (US50633)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides compatibility functions for the Linux idr operations.

NOMANUAL

*/

/* includes */

#include <errno.h> /* for ENOSPC */
#include <vxoal/krnl/system.h>
#include <vxoal/krnl/idr.h>
#include <vxoal/krnl/types.h>
#include <vxoal/krnl/mm.h> /* for kcalloc, kfree */
#include <limits.h>

/*******************************************************************************
*
* idr_add_to_free - add a slot from free list
*
* RETURNS: N/A
*
* SEE ALSO: 
*/

static void idr_add_to_free
    (
     struct idr *idp, 
     struct idr_element ** idrElem,
     int mode
     )
    {
    struct idr_element * freeIdr = idp->pFree;

    if ((idp->freeCnt > MAX_IDR_FREE) || (mode == REMOVE_IDR_FREE))
        {
        (void)kfree (*idrElem);
        *idrElem = NULL;
        return;
        }

    if (!freeIdr)
        {
        idp->pFree = *idrElem;
        }
    else
        {
         while (freeIdr->pNext)
             {
             freeIdr = freeIdr->pNext;
             }
         freeIdr->pNext = *idrElem;
        }
    (*idrElem)->pNext = 0;
    idp->freeCnt++;
    }

/*******************************************************************************
*
* idr_remove_all_elements - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
static void idr_remove_all_elements
    (
    struct idr *idp,
    int mode
    )
    {
    struct idr_element  * currentIdr;
    struct idr_element  * nextIdr;
     if (idp == NULL)
         return;

     currentIdr = idp->pInUse;
     idp->pInUse = 0;

    do  
        {
        if (!currentIdr)
            return;
        nextIdr = currentIdr->pNext;
        idr_add_to_free(idp, &currentIdr, mode);
        currentIdr = nextIdr;
        } while(nextIdr);
    }

/*******************************************************************************
*
* idr_pre_get - 
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/
static void idr_pre_get
    (
    struct idr *idp,
    gfp_t gfp_mask
    )
    {
    struct idr_element * newIdr;
    struct idr_element * freeIdr;

    if (!idp) return;

    freeIdr = idp->pFree;
    if (freeIdr) return;

    newIdr = (struct idr_element *)kcalloc (1, sizeof(struct idr_element), gfp_mask);
    if (!newIdr) return;

    idr_add_to_free (idp, &newIdr, KEEP_IDR_FREE);
    }

/*******************************************************************************
*
* idr_get_slot - get a slot from free list
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
static struct idr_element * idr_get_slot
    (
    struct idr *idp
    )
    {
    struct idr_element * freeIdr = idp->pFree;
    struct idr_element * prevIdr = 0;

    if (!freeIdr)
        {
        idr_pre_get(idp, 0);
        if (idp->pFree == 0)
            return (0);
        else
            freeIdr = idp->pFree;
        }

    if (freeIdr->pNext)
        {
        while (freeIdr->pNext)
            {
            prevIdr = freeIdr;
            freeIdr = freeIdr->pNext;
            }
        prevIdr->pNext = 0;
        }
    else
        {
        idp->pFree = 0;
        }

    idp->freeCnt--;
    return(freeIdr);
    }

/*******************************************************************************
*
* idr_init - initialize an idr
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void idr_init
    (
    struct idr *idp
    )
    {
    if (idp)
         {
         bzero ((void *)idp, sizeof (struct idr));
         }
    }

/*******************************************************************************
*
* idr_replace - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void *idr_replace
    (
    struct idr *idp, 
    void *ptr, 
    int id
    )
    {
    struct idr_element  * inUseIdr;

    if (!idp)
        return (0);

    inUseIdr = idp->pInUse;
    if (!inUseIdr)
        return (0);

    do 
        {
        if (inUseIdr->id == id)
            {
             void * oldPtr = inUseIdr->pData;
             inUseIdr->pData = ptr;
             return (oldPtr);
             }
        inUseIdr = inUseIdr->pNext;
        } while (inUseIdr);

    return (0);
    }

/*******************************************************************************
*
* idr_find - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void *idr_find
    (
    struct idr *idp, 
    int id
    )
    {
    struct idr_element  * inUseIdr;

    if (!idp)
        return (0);

    inUseIdr = idp->pInUse;
    if (!inUseIdr)
        return (0);

    do 
        {
        if (inUseIdr->id == id)
             return (inUseIdr->pData);
        inUseIdr = inUseIdr->pNext;
        } while (inUseIdr);

    return (0);

    }

/*******************************************************************************
*
* idr_remove - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void idr_remove
    (
    struct idr *idp, 
    int id
    )
    {
    struct idr_element  * inUseIdr;
    struct idr_element  * prevIdr;

     if (!idp)
         return;

     inUseIdr = idp->pInUse;
     if (!inUseIdr)
         return;

     if (inUseIdr->id == id)
        {
        idp->pInUse = inUseIdr->pNext;
        idr_add_to_free(idp, &inUseIdr, KEEP_IDR_FREE);
        return;
        }
    do
        {
        prevIdr = inUseIdr;
        inUseIdr = inUseIdr->pNext;
        if (inUseIdr->id == id)
            {
            prevIdr->pNext = inUseIdr->pNext;
            idr_add_to_free(idp, &inUseIdr, KEEP_IDR_FREE);
            return;
            }
        } while (inUseIdr->pNext);

    return;
    }

/*******************************************************************************
*
* idr_remove_all - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void idr_remove_all
    (
    struct idr *idp
    )
    {
    idr_remove_all_elements(idp, KEEP_IDR_FREE);
    }

/*******************************************************************************
*
* idr_destroy - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void idr_destroy
    (
    struct idr *idp
    )
    {
    idr_remove_all_elements(idp, REMOVE_IDR_FREE);
    idr_init(idp);
    }

/*******************************************************************************
*
* idr_for_each - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
int idr_for_each
    (
    struct idr *idp,
    int (*fn)(int id, void *p, void *data), 
    void *data
    )
    {
    struct idr_element  * inUseIdr;

    if (!idp)
        return (0);

    inUseIdr = idp->pInUse;
    if (!inUseIdr)
        return (0);

    do
        {
        fn(inUseIdr->id, inUseIdr->pData, data);
        inUseIdr = inUseIdr->pNext;
        } while(inUseIdr);

    return (1);
    }

/*******************************************************************************
*
* ida_init - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void ida_init (struct ida *pIda)
    {
    bzero ((void *)pIda, sizeof (struct ida));
    idr_init (&pIda->idr);
    }

/*******************************************************************************
*
* ida_destroy - 
*
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/
void ida_destroy (struct ida *pIda)
    {
    idr_destroy (&pIda->idr);
    }

/*******************************************************************************
*
* ida_remove - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void ida_remove (struct ida *pIda, int id)
    {
    idr_remove (&pIda->idr, id);
    }

/*******************************************************************************
*
* idr_alloc - 
*
* RETURNS: id or -ENOSPC
*
* SEE ALSO: 
*/
int idr_alloc
    (
    struct idr *idr, 
    void *ptr, 
    int start, 
    int end, 
    gfp_t gfp_mask
    )
    {
    int id;
    unsigned int maxId;
    struct idr_element  * inUseIdr;
    struct idr_element  * pIdr;

    maxId = (end == 0) ? UINT_MAX : end;

    for (id = start; id < maxId; id ++)
        {
        if (NULL == idr_find (idr, id))
            break;
        }

    if (id < maxId)
        {
        pIdr = idr_get_slot(idr);
        if (!pIdr)
            return -ENOSPC;

        inUseIdr = idr->pInUse;
        idr->lastUsed = id;

        if (inUseIdr)
            {
            while (inUseIdr->pNext)
                {
                inUseIdr = inUseIdr->pNext;
                }
            inUseIdr->pNext = pIdr;
            }
        else
            idr->pInUse = pIdr;

        pIdr->pNext = 0;
        pIdr->id    = id;
        pIdr->pData = ptr;

        return id;
        }
    else
        {
        return -ENOSPC;
        }
    }

int ida_simple_get
    (
    struct ida *ida, 
    unsigned int start, 
    unsigned int end,
    gfp_t gfp_mask
    )
    {
    return (idr_alloc (&ida->idr, NULL, start, end, gfp_mask));
    }
