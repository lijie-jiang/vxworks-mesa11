/*************************************************************************
 *
 * $Id: triodef.h,v 1.3 2002/01/19 15:40:17 breese Exp $
 *
 * Copyright (C) 2001 Bjorn Reese <breese@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
 ************************************************************************/

#ifndef TRIO_TRIODEF_H
#define TRIO_TRIODEF_H

/*************************************************************************
 * Platform and compiler support detection
 */
#if defined(__GNUC__)
# define TRIO_COMPILER_GCC
#elif defined(__SUNPRO_C)
# define TRIO_COMPILER_SUNPRO
#elif defined(__SUNPRO_CC)
# define TRIO_COMPILER_SUNPRO
# define __SUNPRO_C __SUNPRO_CC
#elif defined(__xlC__) || defined(__IBMC__) || defined(__IBMCPP__)
# define TRIO_COMPILER_XLC
#elif defined(_AIX) && !defined(__GNUC__)
# define TRIO_COMPILER_XLC /* Workaround for old xlc */
#elif defined(__DECC) || defined(__DECCXX)
# define TRIO_COMPILER_DECC
#elif defined(_MSC_VER)
# define TRIO_COMPILER_MSVC
#endif

#if defined(unix) || defined(__unix) || defined(__unix__)
# define TRIO_PLATFORM_UNIX
#elif defined(TRIO_COMPILER_XLC) || defined(_AIX)
# define TRIO_PLATFORM_UNIX
#elif defined(TRIO_COMPILER_DECC) || defined(__osf__)
# define TRIO_PLATFORM_UNIX
#elif defined(__NetBSD__)
# define TRIO_PLATFORM_UNIX
#elif defined(__QNX__)
# define TRIO_PLATFORM_UNIX
# define TRIO_PLATFORM_QNX
#elif defined(__CYGWIN__)
# define TRIO_PLATFORM_UNIX
#elif defined(AMIGA) && defined(TRIO_COMPILER_GCC)
# define TRIO_PLATFORM_UNIX
#elif defined(TRIO_COMPILER_MSVC) || defined(WIN32) || defined(_WIN32)
# define TRIO_PLATFORM_WIN32
#elif defined(VMS) || defined(__VMS)
# define TRIO_PLATFORM_VMS
#elif defined(mpeix) || defined(__mpexl)
# define TRIO_PLATFORM_MPEIX
#endif

#if defined(__STDC__)
# define TRIO_COMPILER_SUPPORTS_C90
# if defined(__STDC_VERSION__)
#  if (__STDC_VERSION__ >= 199409L)
#   define TRIO_COMPILER_SUPPORTS_C94
#  endif
#  if (__STDC_VERSION__ >= 199901L)
#   define TRIO_COMPILER_SUPPORTS_C99
#  endif
# elif defined(TRIO_COMPILER_SUNPRO)
#  if (__SUNPRO_C >= 0x420)
#   define TRIO_COMPILER_SUPPORTS_C94
#  endif
# endif
#endif

#if defined(_XOPEN_SOURCE)
# if defined(_XOPEN_SOURCE_EXTENDED)
#  define TRIO_COMPILER_SUPPORTS_UNIX95
# endif
# if (_XOPEN_VERSION >= 500)
#  define TRIO_COMPILER_SUPPORTS_UNIX98
# endif
#endif

/*************************************************************************
 * Generic defines
 */

#if !defined(TRIO_PUBLIC)
# define TRIO_PUBLIC
#endif
#if !defined(TRIO_PRIVATE)
# define TRIO_PRIVATE static
#endif

#if defined(TRIO_COMPILER_SUPPORTS_C90) || defined(__cplusplus)
# define TRIO_CONST const
# define TRIO_VOLATILE volatile
# define TRIO_POINTER void *
# define TRIO_PROTO(x) x
#else
# define TRIO_CONST
# define TRIO_VOLATILE
# define TRIO_POINTER char *
# define TRIO_PROTO(x) ()
#endif

#if defined(TRIO_COMPILER_SUPPORTS_C99) || defined(__cplusplus)
# define TRIO_INLINE inline
#elif defined(TRIO_COMPILER_GCC)
# define TRIO_INLINE __inline__
#else
# define TRIO_INLINE
#endif

#endif /* TRIO_TRIODEF_H */
