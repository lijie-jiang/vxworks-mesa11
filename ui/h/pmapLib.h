/* pmapLib.h - physical address mapping library header */

/* 
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use 
 * of this software may be licensed only pursuant to the terms 
 * of an applicable Wind River license agreement. 
 */ 

/*
modification history
--------------------
05apr14,pcs  Updated to add support for physically aligned allocation and free 
             routines.(US37959)
24jan14,pcs  removed prototype for pmapLibInit.
17jan14,pcs  added prototype for pmapLibInit.
25nov13,pcs  changes corresponding to MMU-less kernel support.
11sep13,pcs  created.
*/

#ifndef __INCpmapLibh
#define __INCpmapLibh

/* includes */

#include <vxWorks.h>
#include <vsbConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PMAP_FAILED      ((void *) -1)   /* pmapxxxMap() error return value */

#ifndef	_ASMLANGUAGE

/* typedefs */

/* function prototypes */

extern void * pmapGlobalMap (PHYS_ADDR physAddr, size_t len, UINT attr);
extern STATUS pmapGlobalUnmap (void * virtAddr, size_t len);
extern void * pmapPrivateMap (PHYS_ADDR physAddr, size_t len, UINT attr);
extern STATUS pmapPrivateUnmap (void * virtAddr, size_t len);

extern void * pmapGlobalPhysAlignedAlloc (size_t len, size_t alignment,
                                          UINT attr);
extern STATUS pmapGlobalPhysAlignedFree (void * virtAddr, size_t len);
extern void * pmapPrivatePhysAlignedAlloc (size_t len, size_t alignment,
                                          UINT attr);
extern STATUS pmapPrivatePhysAlignedFree (void * virtAddr, size_t len);

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpmapLibh */
