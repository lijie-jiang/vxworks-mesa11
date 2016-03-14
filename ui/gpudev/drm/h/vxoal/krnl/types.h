/* types.h - types header file*/

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
02oct15,qsn  Add U64_C (US67206)
15jul15,yat  Clean up vxoal (US60452)
25jun15,yat  Add DIV_ROUND_UP_ULL (US24946)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility types for the Linux operations.

NOMANUAL
*/

#ifndef _VXOAL_TYPES_H
#define _VXOAL_TYPES_H

#include <vxWorks.h>

/* Handle various attributes used within Linux */
#ifndef __init
#define __init
#endif

#ifndef __exit
#define __exit
#endif

#ifndef __initdata
#define __initdata
#endif

#ifndef __user
#define __user
#endif

#ifndef __iomem
#define __iomem
#endif

#ifndef __must_check
#define __must_check
#endif

#ifndef __force
#define __force
#endif

#ifndef __unused
#define __unused
#endif

#ifndef __read_mostly
#define __read_mostly
#endif

#ifndef inline
#define inline __inline__
#endif

#ifndef typeof
#define typeof __typeof__
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#ifndef EXPORT_SYMBOL
#define EXPORT_SYMBOL(sym)
#endif

#ifndef EXPORT_SYMBOL_GPL
#define EXPORT_SYMBOL_GPL(sym)
#endif

#ifndef subsys_initcall
#define subsys_initcall(sym)
#endif

#ifndef postcore_initcall
#define postcore_initcall(sym)
#endif

#ifndef __printf
#define __printf(p1, p2)
#endif

#ifndef likely
#define likely(x)       __builtin_expect((x),1)
#endif

#ifndef unlikely
#define unlikely(x)     __builtin_expect((x),0)
#endif

#ifndef prefetch
#define prefetch(x)     __builtin_prefetch(x)
#endif

#ifndef typecheck
#define typecheck(type,x)                                                      \
    ({                                                                         \
    type __dummy;                                                              \
    __typeof__(x) __dummy2;                                                    \
    (void)(&__dummy == &__dummy2);                                             \
    1;                                                                         \
    })
#endif

#ifndef container_of
#define container_of(ptr, type, member)                                        \
    ({                                                                         \
    (type *)((char *)(ptr) - offsetof(type,member));                           \
    })
#endif

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

#define uninitialized_var(x) x

#define le16_to_cpu(x) ((UINT16)(x))
#define cpu_to_le16(x) ((UINT16)(x))

enum
    {
    false   = 0,
    true    = 1
    };

typedef INT8            __s8;
typedef INT16           __s16;
typedef INT32           __s32;
typedef INT64           __s64;
typedef UINT8           __u8;
typedef UINT16          __u16;
typedef UINT32          __u32;
typedef UINT64          __u64;

typedef INT8            s8;
typedef INT16           s16;
typedef INT32           s32;
typedef INT64           s64;
typedef UINT8           u8;
typedef UINT16          u16;
typedef UINT32          u32;
typedef UINT64          u64;

#define U64_MAX         ((UINT64)~0U)

#define U64_C(x)        (x)

typedef BOOL            bool;

typedef unsigned        gfp_t;

typedef UINT16          __le16;

typedef long long       loff_t;

typedef unsigned long   pgoff_t;

/* Bus address can be wider than 32 bits */
typedef UINT64          dma_addr_t;
typedef UINT64          phys_addr_t;

typedef UINT64          async_cookie_t;

typedef phys_addr_t     resource_size_t;

typedef unsigned long   kernel_ulong_t;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifndef ECHRNG
#define ECHRNG           44
#endif

#ifndef ERESTARTSYS
#define ERESTARTSYS     512
#endif

#ifndef ENOTSUPP
#define ENOTSUPP        524  
#endif

#ifndef ffs
#define ffs __builtin_ffs
#endif

#ifndef __ffs
#define __ffs ffs
#endif

#ifndef ffsl
#define ffsl __builtin_ffsl
#endif

#ifndef __ffsl
#define __ffsl ffsl
#endif

#ifndef fls
#define fls fls32
#endif

#ifndef flsl
#ifdef _WRS_CONFIG_LP64
#define flsl fls64
#else
#define flsl fls32
#endif
#endif

#ifndef KB
#define KB(x) ((x) * 1024)
#endif

#ifndef MB
#define MB(x) (KB(KB(x)))
#endif

#ifndef do_div
#define do_div(n,base)                                                         \
    ({                                                                         \
    uint32_t __base = (base);                                                  \
    uint32_t __rem;                                                            \
    __rem = ((uint64_t)(n)) % __base;                                          \
    (n) = ((uint64_t)(n)) / __base;                                            \
    __rem;                                                                     \
    })
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif

#ifndef DIV_ROUND_UP_ULL
#define DIV_ROUND_UP_ULL(ll,d)                                                 \
    ({                                                                         \
    unsigned long long _tmp = (ll)+(d)-1; do_div(_tmp, d); _tmp;               \
    })
#endif

#ifndef DIV_ROUND_CLOSEST
#define DIV_ROUND_CLOSEST(x, divisor) DIV_ROUND_UP(x, divisor)
#endif

#ifndef DIV_ROUND_CLOSEST_ULL
#define DIV_ROUND_CLOSEST_ULL(ll, d)                                           \
    ({                                                                         \
    unsigned long long _tmp = (ll)+(d)/2; do_div(_tmp, d); _tmp;               \
    })
#endif

#ifndef round_up
#define round_up(x, y) ROUND_UP(x, y)
#endif

#ifndef roundup
#define roundup(x, y) ROUND_UP(x, y)
#endif

#ifndef round_down
#define round_down(x, y) ROUND_DOWN(x, y)
#endif

#ifndef rounddown
#define rounddown(x, y) ROUND_DOWN(x, y)
#endif

#ifndef upper_32_bits
#define upper_32_bits(n) ((UINT32)(((n) >> 16) >> 16))
#endif

#ifndef lower_32_bits
#define lower_32_bits(n) ((UINT32)(n))
#endif

#ifndef min_t
#define min_t(type, x, y)                                                      \
    ({                                                                         \
    type __m1 = (x);                                                           \
    type __m2 = (y);                                                           \
    __m1 < __m2 ? __m1: __m2;                                                  \
    })
#endif

#ifndef max_t
#define max_t(type, x, y)                                                      \
    ({                                                                         \
    type __m1 = (x);                                                           \
    type __m2 = (y);                                                           \
    __m1 > __m2 ? __m1: __m2;                                                  \
    })
#endif

#ifndef min3
#define min3(x, y, z) min((typeof(x))min(x, y), z)
#endif

#ifndef clamp_t
#define clamp_t(val_type, value, min_val, max_val)                             \
    min_t(val_type, max_t(val_type, value, min_val), max_val)
#endif

#ifndef clamp
#define clamp(value, min_val, max_val) clamp_t(int, value, min_val, max_val)
#endif

#ifndef abs64
#define abs64(x)                                                               \
    ({                                                                         \
    long long __x = (x);                                                       \
    (__x < 0) ? -__x : __x;                                                    \
    })
#endif

#ifndef mult_frac
#define mult_frac(x, n, d)                                                     \
    ({                                                                         \
    typeof(x) q = (x) / (d);                                                   \
    typeof(x) r  = (x) % (d);                                                  \
    (q * (n)) + ((r * (n)) / (d));                                             \
    })
#endif

static inline UINT64 div64_u64
    (
    UINT64 dividend,
    UINT64 divisor
    )
    {
    return dividend / divisor;
    }

static inline INT64 div64_s64
    (
    INT64 dividend,
    INT64 divisor
    )
    {
    return dividend / divisor;
    }

static inline unsigned long long div_u64
    (
    unsigned long long dividend,
    unsigned int divisor
    )
    {
    return dividend / divisor;
    }
#define div_s64 div_u64

static inline bool is_power_of_2
    (
    unsigned long n
    )
    {
    if (n == 0)
        return 0;
    return ((n & (n - 1)) == 0);
    }

static inline unsigned int roundup_pow_of_two
    (
    unsigned int n
    )                         
    {
    int i;

    if (n == 0)
        return n;

    for (i = 0; i < 32; i ++)
        {
        if (n == (1 << i))
            return n;
        if ((n > (1 << i)) && (n <= (1 << (i + 1))))
            return (1 << (i + 1));
        }
    return (1 << 31);
    }

#ifndef swap
#define swap(a, b)                                                             \
    ({                                                                         \
    __typeof__(a) __tmp = (a);                                                 \
    (a) = (b);                                                                 \
    (b) = __tmp;                                                               \
    })
#endif

#ifndef cmpxchg
#define cmpxchg(ptr, o, n)                                                     \
    ({                                                                         \
    __typeof__(*(ptr)) __o = (o);                                              \
    __typeof__(*(ptr)) __n = (n);                                              \
    __typeof__(*(ptr)) __tmp = *(ptr);                                         \
    if (__tmp == __o)                                                          \
        *(ptr) = __n;                                                          \
    __tmp;                                                                     \
    })
#endif

#ifndef __stringify2
#define __stringify2(s) #s
#endif

#ifndef __stringify
#define __stringify(s) __stringify2(s)
#endif

#ifndef kstrdup
#define kstrdup(s, flag) strdup(s)
#endif

#ifndef kstrtou32
#define kstrtou32(s, base, res) ({*(res) = strtoul((s), (base), 0);0;})
#endif

/* Users of kasprintf must call kfree to prevent memory leaks */
#ifndef kasprintf
#define kasprintf(flags, fmt, ...)                                             \
    ({                                                                         \
    int len = snprintf(NULL, 0, fmt, ##__VA_ARGS__);                           \
    void *buf = kmalloc(len + 1, flags);                                       \
    if (buf) { snprintf(buf, len + 1, fmt, ##__VA_ARGS__); }                   \
    buf;                                                                       \
    })
#endif

#endif /* _VXOAL_TYPES_H */
