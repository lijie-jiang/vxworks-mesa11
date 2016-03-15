/* bitops.h - bit operation utility functions */

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
17sep15,qsn  Re-implement find_first_zero_bit (US66439)
15jul15,yat  Clean up vxoal (US60452)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*

DESCRIPTION

This file provides bit operation utility functions.

NOMANUAL

*/

#ifndef _VXOAL_BITOPTS_H_
#define _VXOAL_BITOPTS_H_

#include <string.h> /* for memset */
#include <vxoal/krnl/types.h> /* for DIV_ROUND_UP */
#include <vxoal/krnl/ilog2.h> /* for fls, ilog2 */

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE               8
#endif

#ifndef BITS_PER_LONG
#ifdef _WRS_CONFIG_LP64
#define BITS_PER_LONG               64
#else
#define BITS_PER_LONG               32
#endif
#endif

#ifndef BIT
#define BIT(bitNbr)                 (1UL << (bitNbr))
#endif

#ifndef BIT_MASK
#define BIT_MASK(bitNbr)            (1UL << ((bitNbr) % BITS_PER_LONG))
#endif

#ifndef BIT_WORD
#define BIT_WORD(bitNbr)            ((bitNbr) / BITS_PER_LONG)
#endif

#ifndef BITS_TO_LONGS
#define BITS_TO_LONGS(bitNbr)       DIV_ROUND_UP(bitNbr, BITS_PER_LONG)
#endif

#ifndef GENMASK
#define GENMASK(h, l)                                                          \
    (((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#endif

extern unsigned long drm_find_first_zero_bit
    (
    const unsigned long *addr,
    unsigned long size
    );

extern unsigned long drm_find_next_bit
    (
    const unsigned long *addr,
    unsigned long size,
    unsigned long start
    );

extern unsigned long drm_find_first_bit
    (
    const unsigned long *addr,
    unsigned long size
    );

#define find_first_zero_bit drm_find_first_zero_bit
#define find_next_bit drm_find_next_bit
#define find_first_bit drm_find_first_bit
#define for_each_set_bit(bit, addr, size)                                      \
    for ((bit) = find_first_bit((addr), (size)); (bit) < (size);               \
         (bit) = find_next_bit((addr), (size), (bit) + 1))

static inline unsigned int hweight8
    (
    uint64_t mask
    )
    {
    unsigned int cnt = 0;

    if (mask & (1ULL << 0)) cnt++;
    if (mask & (1ULL << 1)) cnt++;
    if (mask & (1ULL << 2)) cnt++;
    if (mask & (1ULL << 3)) cnt++;
    if (mask & (1ULL << 4)) cnt++;
    if (mask & (1ULL << 5)) cnt++;
    if (mask & (1ULL << 6)) cnt++;
    if (mask & (1ULL << 7)) cnt++;

    return cnt;
    }

static inline unsigned int hweight16
    (
    uint64_t mask
    )
    {
    return hweight8 (mask >> 8) + hweight8 (mask);
    }

static inline unsigned int hweight32
    (
    uint64_t mask
    )
    {
    return hweight16 (mask >> 16) + hweight16 (mask);
    }

static inline unsigned int hweight64
    (
    uint64_t mask
    )
    {
    return hweight32 (mask >> 32) + hweight32 (mask);
    }

#ifdef _WRS_CONFIG_LP64
#define hweight_long hweight64
#else
#define hweight_long hweight32
#endif

extern int drm_bitmap_weight
    (
    const unsigned long *addr,
    unsigned int bit_size
    );

extern void drm_bitmap_or
    (
    unsigned long *addr,
    const unsigned long *src1,
    const unsigned long *src2,
    unsigned int bit_size
    );

extern int drm_bitmap_empty
    (
    unsigned long *addr,
    unsigned int bit_size
    );

extern void drm_bitmap_set
    (
    unsigned long *addr,
    unsigned int start,
    int bit_size
    );

extern void drm_bitmap_zero
    (
    unsigned long *addr,
    unsigned int bit_size
    );

#ifndef DECLARE_BITMAP
#define DECLARE_BITMAP(symbol, bit_size)                                       \
    unsigned long symbol[BITS_TO_LONGS(bit_size)];
#endif

#ifndef BITMAP_FIRST_WORD_MASK
#define BITMAP_FIRST_WORD_MASK(start)                                          \
    (~0UL << ((start) & (BITS_PER_LONG - 1)))
#endif

#ifndef BITMAP_LAST_WORD_MASK
#define BITMAP_LAST_WORD_MASK(bit_size)                                        \
    (~0UL >> (-(bit_size) & (BITS_PER_LONG - 1)))
#endif

#define bitmap_weight drm_bitmap_weight
#define bitmap_or drm_bitmap_or
#define bitmap_empty drm_bitmap_empty
#define bitmap_set drm_bitmap_set
#define bitmap_zero drm_bitmap_zero

#endif /* _VXOAL_BITOPTS_H_ */
