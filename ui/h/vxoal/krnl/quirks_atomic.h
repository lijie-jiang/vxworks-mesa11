/* quirks_atomic.h - atomic functionality header file */

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
24jan14,mgc  Modified for VxWorks 7 release
*/

static inline long atomic_add_unless (atomic_t *v, long a, long u)
{
    long c, old;
    c = atomic_read(v);
    while (c != u && (old = atomic_cmpxchg(v, c, c + a)) != c)
        c = old;
    return c;
}

