/* log.h - logging functionality header file*/

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
15jul15,yat  Clean up vxoal (US60452)
05jun15,rpc  removed kvasprintf and kasprintf ref. (US59495)
09mar15,qsn  Added WARN_ON_SMP (US50613)
26jan15,qsn  Change printk to drv_printf and add drv_vprintf 
22jan15,qsn  Update printf to kprintf
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux logging operations.

NOMANUAL
*/

#ifndef _VXOAL_LOG_H
#define _VXOAL_LOG_H

#include <stdio.h> /* for printf, kprintf */
#include <intLib.h> /* for INT_CONTEXT */

/* Logging Types */

#define KERN_INFO       "Info "
#define KERN_ERR        "Error "
#define KERN_DEBUG      "Debug "
#define KERN_ALERT      "Alert "
#define KERN_WARNING    "Warning "

#define FW_BUG

enum
    {
    DUMP_PREFIX_NONE,
    DUMP_PREFIX_ADDRESS,
    DUMP_PREFIX_OFFSET
    };

/* Error */

#define MAX_ERRNO       (4095)
#define PTR_ERR(x)      ((long)x)
#define IS_ERR_VALUE(x) ((x) >= (unsigned long)-MAX_ERRNO)
#define IS_ERR(x)       IS_ERR_VALUE((unsigned long)x)

static __inline__ int PTR_ERR_OR_ZERO
    (
    void *ptr
    )
    {
    if (IS_ERR(ptr))
        {
        return (int)PTR_ERR(ptr);
        }
    else
        {
        return 0;
        }
    }

/* Use inline to resolve build warning */
static __inline__ void * ERR_PTR
    (
    long x
    )
    {
    return (void *) x;
    }

static __inline__ void * ERR_CAST
    (
    const void *x
    )
    {
    return (void *) x;
    }

static __inline__ long IS_ERR_OR_NULL
    (
    const void *ptr
    )
    {
    if ((ptr == NULL) || (IS_ERR_VALUE((unsigned long)ptr)))
        {
        return 1;
        }
    return 0;
    }

/* Print */

#ifndef printk
#define printk          pr_info
#endif

#ifndef printk_once
#define printk_once     pr_info_once
#endif

#ifndef pr_fmt
#define pr_fmt(fmt)     "%s:%d " fmt, __func__, __LINE__
#endif

#ifndef pr_info
#if defined(GFX_USE_DEBUG)
#define pr_info(fmt, ...)                                                      \
    ({                                                                         \
    if (INT_CONTEXT())                                                         \
        {                                                                      \
        kprintf(pr_fmt(fmt), ##__VA_ARGS__);                                   \
        }                                                                      \
    else                                                                       \
        {                                                                      \
        printf(pr_fmt(fmt), ##__VA_ARGS__);                                    \
        }                                                                      \
    })
#else
#define pr_info(fmt, ...)
#endif
#endif

#ifndef pr_info_once
#if defined(GFX_USE_DEBUG)
#define pr_info_once(fmt, ...)                                                 \
    ({                                                                         \
    static int __once = 0;                                                     \
    if (!__once)                                                               \
        {                                                                      \
        __once = 1;                                                            \
        if (INT_CONTEXT())                                                     \
            {                                                                  \
            kprintf(pr_fmt(fmt), ##__VA_ARGS__);                               \
            }                                                                  \
        else                                                                   \
            {                                                                  \
            printf(pr_fmt(fmt), ##__VA_ARGS__);                                \
            }                                                                  \
        }                                                                      \
    })
#else
#define pr_info_once(fmt, ...)
#endif
#endif

#ifndef pr_err
#define pr_err(fmt, ...)                                                       \
    ({                                                                         \
    if (INT_CONTEXT())                                                         \
        {                                                                      \
        kprintf(pr_fmt(fmt), ##__VA_ARGS__);                                   \
        }                                                                      \
    else                                                                       \
        {                                                                      \
        printf(pr_fmt(fmt), ##__VA_ARGS__);                                    \
        }                                                                      \
    })
#endif

#ifndef pr_warn
#define pr_warn pr_err
#endif

#ifndef pr_debug
#define pr_debug pr_info
#endif

#ifndef pr_notice
#define pr_notice pr_info
#endif

#ifndef dev_info
#define dev_info(dev, format...) pr_info(format)
#endif

#ifndef dev_err
#define dev_err(dev, format...) pr_err(format)
#endif

#ifndef dev_warn
#define dev_warn(dev, format...) pr_err(format)
#endif

#ifndef WARN_DEV
#define WARN_DEV pr_warn("DEV\n")
#endif

#ifndef WARN
#define WARN(condition, format...)                                             \
    ({                                                                         \
    int __ret_warn_on = !!(condition);                                         \
    if (unlikely(__ret_warn_on))                                               \
        {                                                                      \
        pr_warn(format);                                                       \
        }                                                                      \
    unlikely(__ret_warn_on);                                                   \
    })
#endif

#ifndef WARN_ONCE
#define WARN_ONCE(condition, format...)                                        \
    ({                                                                         \
    static int __warned = 0;                                                   \
    int __ret_warn_once = !!(condition);                                       \
    if (unlikely(__ret_warn_once))                                             \
        {                                                                      \
        if (WARN(!__warned, format))                                           \
            {                                                                  \
            __warned = 1;                                                      \
            }                                                                  \
        }                                                                      \
    unlikely(__ret_warn_once);                                                 \
    })
#endif

#ifndef WARN_ON
#define WARN_ON(condition)                                                     \
    ({                                                                         \
    int __ret_warn_on = !!(condition);                                         \
    if (unlikely(__ret_warn_on))                                               \
        {                                                                      \
        pr_warn("WARN\n");                                                     \
        }                                                                      \
    unlikely(__ret_warn_on);                                                   \
    })
#endif

#ifndef WARN_ON_SMP
#define WARN_ON_SMP WARN_ON
#endif

#ifndef WARN_ON_ONCE
#define WARN_ON_ONCE(condition)                                                \
    ({                                                                         \
    static int  __warned = 0;                                                  \
    int __ret_warn_once = !!(condition);                                       \
    if (unlikely(__ret_warn_once))                                             \
        {                                                                      \
        if (WARN_ON(!__warned))                                                \
            {                                                                  \
            __warned = 1;                                                      \
            }                                                                  \
        }                                                                      \
    unlikely(__ret_warn_once);                                                 \
    })
#endif

#ifndef BUG
#define BUG()                                                                  \
    ({                                                                         \
    if (INT_CONTEXT())                                                         \
        {                                                                      \
        kprintf(pr_fmt("BUG\n"));                                              \
        }                                                                      \
    else                                                                       \
        {                                                                      \
        printf(pr_fmt("BUG\n"));                                               \
        }                                                                      \
    do { } while (1);                                                          \
    })
#endif

#ifndef BUG_ON
#define BUG_ON(condition)                                                      \
    ({                                                                         \
    int __ret_bug_on = !!(condition);                                          \
    if (unlikely(__ret_bug_on))                                                \
        {                                                                      \
        BUG();                                                                 \
        }                                                                      \
    })
#endif

#ifndef BUILD_BUG
#define BUILD_BUG() BUG()
#endif

#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(n) BUG_ON(n)
#endif

#ifndef BUILD_BUG_ON_MSG
#define BUILD_BUG_ON_MSG(n, m) BUG_ON(n)
#endif

#ifndef BUILD_BUG_ON_NOT_POWER_OF_2
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)                                         \
    BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))
#endif

#ifndef print_hex_dump
#define print_hex_dump(k, e, ...) pr_err(e)
#endif

static __inline__ int scnprintf
    (
    char *buf, 
    size_t size, 
    const char *fmt, 
    ...
    )
    { 
    va_list args;           
    int len, i;
 
    va_start(args, fmt);

    len = vsnprintf (buf, size, fmt, args);
    if (len <= 0)
        {
        va_end(args);
        return 0;
        }

    va_end(args);

    for (i = 0; i < size; i ++)
        {
        if (buf[i] == '\0')
            {
            return (i);
            }
        }
    return 0;
    }

#endif /* _VXOAL_LOG_H */
