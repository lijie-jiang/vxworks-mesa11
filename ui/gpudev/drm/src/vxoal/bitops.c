/* bitops.c - bit operation utility functions */

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
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides bit operation utility functions.

NOMANUAL

*/

/* includes */

#include <spinLockLib.h> /* for spinlockIsr_t */
#include <vxoal/krnl/bitops.h> /* BIT_MASK */
#include <vxoal/krnl/atomic.h> /* for extern */

static spinlockIsr_t bitOpsSpinLock;

/*******************************************************************************
*
* gfxDrmBitOpsInit - initialize spinlock for bit operation
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void gfxDrmBitOpsInit
    (
    void
    )
    {
    spinLockIsrInit (&bitOpsSpinLock, 0);
    }

/*******************************************************************************
*
* drm_clear_bit - clear a bit in memory
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void drm_clear_bit
    (
    int bitNbr, 
    volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);
 
    spinLockIsrTake(&bitOpsSpinLock);
    *pAddr &= ~mask;
    spinLockIsrGive(&bitOpsSpinLock);
    }

/*******************************************************************************
*
* drm_change_bit - change a bit in memory
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void drm_change_bit
    (
    int bitNbr, 
    volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);

    spinLockIsrTake(&bitOpsSpinLock);
    *pAddr ^= mask;
    spinLockIsrGive(&bitOpsSpinLock);
    }

/*******************************************************************************
*
* drm_set_bit - set a bit in memory
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void drm_set_bit
    (
    int bitNbr, 
    volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);

    spinLockIsrTake(&bitOpsSpinLock);
    *pAddr  |= mask;
    spinLockIsrGive(&bitOpsSpinLock);
    }

/*******************************************************************************
*
* drm_test_and_change_bit - test and change a bit in memory
*
* RETURNS: N/A
*
* SEE ALSO: 
*/

int drm_test_and_change_bit
    (
    int bitNbr, 
    volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);
    unsigned long old;

    spinLockIsrTake(&bitOpsSpinLock);
    old = *pAddr;
    *pAddr = old ^ mask;
    spinLockIsrGive(&bitOpsSpinLock);

    return (old & mask) != 0;
    }

/*******************************************************************************
*
* drm_test_and_clear_bit - test and clear a bit in memory
*
* RETURNS: N/A
*
* SEE ALSO: 
*/

int drm_test_and_clear_bit
    (
    int bitNbr, 
    volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);
    unsigned long old;

    spinLockIsrTake(&bitOpsSpinLock);
    old = *pAddr;
    *pAddr = old & ~mask;
    spinLockIsrGive(&bitOpsSpinLock);

    return (old & mask) != 0;
    }


/*******************************************************************************
*
* drm_test_and_set_bit - 
*
* RETURNS: N/A
*
* SEE ALSO: 
*/

int drm_test_and_set_bit
    (
    int bitNbr, 
    volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);
    unsigned long old;

    spinLockIsrTake(&bitOpsSpinLock);
    old = *pAddr;
    *pAddr = old | mask;
    spinLockIsrGive(&bitOpsSpinLock);

    return (old & mask) != 0;
    }

/*******************************************************************************
*
* drm_test_bit - test a bit in memory
*
* RETURNS: N/A
*
* SEE ALSO: 
*/

int drm_test_bit
    (
    int bitNbr,
    const volatile unsigned long *addr
    )
    {
    unsigned long mask = BIT_MASK(bitNbr);
    unsigned long *pAddr = ((unsigned long *)addr) + BIT_WORD(bitNbr);
    return (*pAddr & mask) ? 1 : 0;
    }

/*******************************************************************************
*
* drm_find_first_zero_bit - find first zero bit
* 
* RETURNS: unsigned long
*
* SEE ALSO: 
*/

unsigned long drm_find_first_zero_bit
    (
    const unsigned long *addr,
    unsigned long size
    )
    {
    unsigned long i;
    unsigned long *pAddr = (unsigned long *)addr;

    for (i = 0; i < size; i += BITS_PER_LONG, pAddr++)
        {
        if ((*pAddr) != ~0UL)
            {
            return min (ffsl (~(*pAddr)) + i, size);
            }
        }

    return size;
    }

/*******************************************************************************
*
* drm_find_first_bit - find first bit
* 
* RETURNS: unsigned long
*
* SEE ALSO: 
*/

unsigned long drm_find_first_bit
    (
    const unsigned long *addr,
    unsigned long size
    )
    {
    unsigned long i;
    unsigned long *pAddr = (unsigned long *)addr;

    for (i = 0; i < size; i += BITS_PER_LONG, pAddr++)
        {
        if ((*pAddr))
            {
            return min (ffsl (*pAddr) + i, size);
            }
        }

    return size;
    }

/*******************************************************************************
*
* drm_find_next_bit - find next bit
* 
* RETURNS: unsigned long
*
* SEE ALSO: 
*/

unsigned long drm_find_next_bit
    (
    const unsigned long *addr,
    unsigned long size,
    unsigned long start
    )
    {
    WARN_DEV;

    return size;
    }

/*******************************************************************************
*
* drm_bitmap_weight - get bitmap weight
* 
* RETURNS: int
*
* SEE ALSO: 
*/

int drm_bitmap_weight
    (
    const unsigned long *addr,
    unsigned int bit_size
    )
    {
    unsigned int i;
    unsigned int n = bit_size / BITS_PER_LONG;
    int w = 0;

    for (i = 0; i < n; i++)
        {
        w += hweight_long(addr[i]);
        }

    if (bit_size % BITS_PER_LONG)
        {
        w += hweight_long(addr[i] & BITMAP_LAST_WORD_MASK(bit_size));
        }

    return w;
    }

/*******************************************************************************
*
* drm_bitmap_or - or bitmap
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void drm_bitmap_or
    (
    unsigned long *addr,
    const unsigned long *src1,
    const unsigned long *src2,
    unsigned int bit_size
    )
    {
    unsigned int i;
    unsigned int n = bit_size / BITS_PER_LONG;

    for (i = 0; i < n; i++)
        {
        addr[i] = src1[i] | src2[i];
        }
    }

/*******************************************************************************
*
* drm_bitmap_empty - check bitmap empty
* 
* RETURNS: int
*
* SEE ALSO: 
*/

int drm_bitmap_empty
    (
    unsigned long *addr,
    unsigned int bit_size
    )
    {
    unsigned int i;
    unsigned int n = bit_size / BITS_PER_LONG;

    for (i = 0; i < n; i++)
        {
        if (addr[i])
            {
            return 0;
            }
        }

    if (bit_size % BITS_PER_LONG)
        {
        if (addr[i] & BITMAP_LAST_WORD_MASK(bit_size))
           {
           return 0;
           }
        }

    return 1;
    }

/*******************************************************************************
*
* drm_bitmap_set - set bitmap
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void drm_bitmap_set
    (
    unsigned long *addr,
    unsigned int start,
    int bit_size
    )
    {
    memset ((void*)(addr + BIT_WORD(start)), 0xff,
            BITS_TO_LONGS(bit_size) * sizeof (unsigned long));
    }

/*******************************************************************************
*
* drm_bitmap_zero - zero bitmap
* 
* RETURNS: N/A
*
* SEE ALSO: 
*/

void drm_bitmap_zero
    (
    unsigned long *addr,
    unsigned int bit_size
    )
    {
    memset ((void*)addr, 0, BITS_TO_LONGS(bit_size) * sizeof (unsigned long));
    }

#ifdef BITOPS_TEST

void bitops_test
    (
    void
    )
    {
    unsigned long bitmap[10];

    memset (bitmap, 0xFF, sizeof(bitmap));
    bitmap[9] = 0xFFFF;
    printf("find_first_zero_bit:%d\n",
           drm_find_first_zero_bit (bitmap, sizeof(bitmap) * BITS_PER_BYTE));
    memset (bitmap, 0, sizeof(bitmap));
    bitmap[9] = 0x10000;
    printf("find_first_bit:%d\n",
           drm_find_first_bit (bitmap, sizeof(bitmap) * BITS_PER_BYTE));

    printf("hweight8(0x33):%d\n", hweight8(0x33));
    printf("hweight16(0x3300):%d\n", hweight16(0x3300));
    printf("hweight32(0x33000000):%d\n", hweight32(0x33000000));
    printf("hweight64(0x3300000000000000):%d\n", hweight64(0x3300000000000000));

    printf("ffs(0x10000000):%d\n", ffs(0x10000000));
    printf("fls(0x10000000):%d\n", fls(0x10000000));
    printf("ffs(0x00010000):%d\n", ffs(0x00010000));
    printf("fls(0x00010000):%d\n", fls(0x00010000));
    printf("ffs(0x10010000):%d\n", ffs(0x10010000));
    printf("fls(0x10010000):%d\n", fls(0x10010000));
    printf("ffsl(0x10000000):%d\n", ffsl(0x10000000));
    printf("flsl(0x10000000):%d\n", flsl(0x10000000));
    printf("ffsl(0x00010000):%d\n", ffsl(0x00010000));
    printf("flsl(0x00010000):%d\n", flsl(0x00010000));
    printf("ffsl(0x10010000):%d\n", ffsl(0x10010000));
    printf("flsl(0x10010000):%d\n", flsl(0x10010000));
    printf("ilog2(256):%d\n", ilog2(256));
    printf("ilog2(1024):%d\n", ilog2(1024));
    }

#endif
