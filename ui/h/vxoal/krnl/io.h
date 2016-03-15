/* io.h - io functionality header file*/

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
17sep15,qsn  Clean up ioread32 and iowrite32 (US66439)
15jul15,yat  Clean up vxoal (US60452)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux io operations.

NOMANUAL
*/

#ifndef _VXOAL_IO_H_
#define _VXOAL_IO_H_

#include <sysLib.h> /* for sysInLong, sysOutLong */
#include <stdint.h> /* for intptr_t */

/* Enable the following to include debugging output for the io operations */
#if 0
#define IO_DEBUG  printf
#endif

#define IO_END_ADDR (0x40000)

static inline void writeb
    (
    UINT8 b,
    volatile void *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("writeb 0x%x --> 0x%x\n",b, addr);
#endif
    *(volatile UINT8 *) addr = b;
    }

static inline void writew
    (
    UINT16 b,
    volatile void *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("writew 0x%x --> 0x%x\n",b, addr);
#endif
    *(volatile UINT16 *) addr = b;
    }

static inline void writel
    (
    UINT32 b,
    volatile void *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("writel 0x%x --> 0x%x\n",b, addr);
#endif
    *(volatile UINT32 *) addr = b;
    }

static inline void writeq
    (
    UINT64 b,
    volatile void *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("writeq 0x%x --> 0x%x\n",b, addr);
#endif
    *(volatile UINT64 *) addr = b;
    }
#define writel_relaxed(b, addr) writel(b, addr)

static inline UINT8 __raw_readb
    (
    const volatile void  *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("__raw_readb 0x%x <-- 0x%x\n",addr, *(const volatile UINT8  *) addr);
#endif
    return *(const volatile UINT8 *) addr;
    }

static inline UINT16 __raw_readw
    (
    const volatile void  *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("__raw_readw 0x%x <-- 0x%x\n",addr, *(const volatile UINT16  *) addr);
#endif
    return *(const volatile UINT16 *) addr;
    }

static inline UINT32 __raw_readl
    (
    const volatile void  *addr
    )
    {
#ifdef IO_DEBUG
    IO_DEBUG("__raw_readl 0x%x <-- 0x%x\n",addr, *(const volatile UINT32  *) addr);
#endif
    return *(const volatile UINT32 *) addr;
    }

static inline UINT64 __raw_readq
    (
    const volatile void  *addr
    )
    {
    UINT64 data;
    data = ((UINT64) __raw_readl(addr)) | (((UINT64) __raw_readl(addr+4UL)) << 32);
#ifdef IO_DEBUG
    IO_DEBUG("__raw_readl 0x%x <-- 0x%ll\n",addr, data);
#endif
    return data;
    }

#define readb(addr) __raw_readb(addr)
/* NOTE: These reads assume an endianess match between the two memory regions */
#define readw(addr) __raw_readw(addr)
#define readl(addr) __raw_readl(addr)
#define readq(addr) __raw_readq(addr)

static inline UINT8 inb
    (
    unsigned long addr
    )
    {
    return readb((volatile void *) addr);
    }

static inline UINT16 inw
    (
    unsigned long addr
    )
    {
    return readw((volatile void *) addr);
    }

static inline UINT32 inl
    (
    unsigned long addr
    )
    {
     return readl((volatile void *) addr);
    }

static inline void outb
    (
    UINT8 b,
    unsigned long addr
    )
    {
    writeb(b, (volatile void *) addr);
    }

static inline void outw
    (
    UINT16 b,
    unsigned long addr
    )
    {
    writew(b, (volatile void *) addr);
    }

static inline void outl
    (
    UINT32 b,
    unsigned long addr
    )
    {
    writel(b, (volatile void *) addr);
    }

static inline unsigned int ioread32
    (
    unsigned int *addr
    )
    {
    if ((unsigned long)addr > IO_END_ADDR)
        {
        return readl (addr);
        }
    else
        {
        return (unsigned int)sysInLong ((_WRS_IOLONG)((intptr_t)addr));
        }
    }

static inline void iowrite32
    (
    unsigned int val,
    unsigned int *addr
    )
    {
    if ((unsigned long)addr > IO_END_ADDR)
        {
        writel (val, addr);
        }
    else
        {
        sysOutLong ((_WRS_IOLONG)((intptr_t)addr), val);
        }
    }

#define gpiod_set_value_cansleep(p1, p2) ({WARN_DEV;})
#define gpiod_put(p1) ({WARN_DEV;})
#define gpiod_get(p1, p2, p3) ({WARN_DEV; NULL;})
#define GPIOD_OUT_HIGH 7
#endif /* _VXOAL_IO_H_ */
