/* ioctl.h - ioctl functionality header file*/

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

This file provides compatibility functions for the Linux ioctl operations.

NOMANUAL
*/

#ifndef _VXOAL_IOCTL_H_
#define _VXOAL_IOCTL_H_

#include <ioctl.h> /* Must define first */

#define VXWORKS_IOC_NRBITS               8
#define VXWORKS_IOC_TYPEBITS             8
#define VXWORKS_IOC_SIZEBITS            14
#define VXWORKS_IOC_DIRBITS              2

#define VXWORKS_IOC_NRMASK               ((1 << VXWORKS_IOC_NRBITS) - 1)
#define VXWORKS_IOC_TYPEMASK             ((1 << VXWORKS_IOC_TYPEBITS) - 1)
#define VXWORKS_IOC_SIZEMASK             ((1 << VXWORKS_IOC_SIZEBITS) - 1)
#define VXWORKS_IOC_DIRMASK              ((1 << VXWORKS_IOC_DIRBITS) - 1)

#define VXWORKS_IOC_NRSHIFT               0
#define VXWORKS_IOC_TYPESHIFT            (VXWORKS_IOC_NRSHIFT + VXWORKS_IOC_NRBITS)
#define VXWORKS_IOC_SIZESHIFT            (VXWORKS_IOC_TYPESHIFT+VXWORKS_IOC_TYPEBITS)
#define VXWORKS_IOC_DIRSHIFT             (VXWORKS_IOC_SIZESHIFT+VXWORKS_IOC_SIZEBITS)

#define VXWORKS_IOC(dir,type,nr,size) \
                                        (((dir)  << VXWORKS_IOC_DIRSHIFT) | \
                                        ((type) << VXWORKS_IOC_TYPESHIFT) | \
                                        ((nr)   << VXWORKS_IOC_NRSHIFT) | \
                                        ((size) << VXWORKS_IOC_SIZESHIFT))

#define VXWORKS_IOC_NONE                 0U
#define VXWORKS_IOC_WRITE                1U
#define VXWORKS_IOC_READ                 2U

/* used to create numbers */

#define VXWORKS_IO(type,nr)              VXWORKS_IOC(VXWORKS_IOC_NONE,(type),(nr),0)
#define VXWORKS_IOR(type,nr,size)        VXWORKS_IOC(VXWORKS_IOC_READ,(type),(nr),(sizeof(size)))
#define VXWORKS_IOW(type,nr,size)        VXWORKS_IOC(VXWORKS_IOC_WRITE,(type),(nr),(sizeof(size)))
#define VXWORKS_IOWR(type,nr,size)       VXWORKS_IOC(VXWORKS_IOC_READ | VXWORKS_IOC_WRITE,(type),(nr),(sizeof(size)))

/* used to decode ioctl numbers */

#define VXWORKS_IOC_DIR(nr)              (((nr) >> VXWORKS_IOC_DIRSHIFT) & VXWORKS_IOC_DIRMASK)
#define VXWORKS_IOC_TYPE(nr)             (((nr) >> VXWORKS_IOC_TYPESHIFT) & VXWORKS_IOC_TYPEMASK)
#define VXWORKS_IOC_NR(nr)               (((nr) >> VXWORKS_IOC_NRSHIFT) & VXWORKS_IOC_NRMASK)
#define VXWORKS_IOC_SIZE(nr)             (((nr) >> VXWORKS_IOC_SIZESHIFT) & VXWORKS_IOC_SIZEMASK)

/* ioctl commands are encoded in 32 bits; as follows:
*      bits 0 - 15:   the command
*      bit 16 - 29: size of parameter structure
*      bits 30 - 31: access mode
*/

#define _IOC_NRBITS             VXWORKS_IOC_NRBITS
#define _IOC_TYPEBITS           VXWORKS_IOC_TYPEBITS
#define _IOC_SIZEBITS           VXWORKS_IOC_SIZEBITS
#define _IOC_DIRBITS            VXWORKS_IOC_DIRBITS

#define _IOC_NRMASK             VXWORKS_IOC_NRMASK
#define _IOC_TYPEMASK           VXWORKS_IOC_TYPEMASK
#define _IOC_SIZEMASK           VXWORKS_IOC_SIZEMASK
#define _IOC_DIRMASK            VXWORKS_IOC_DIRMASK

#define _IOC_NRSHIFT            VXWORKS_IOC_NRSHIFT
#define _IOC_TYPESHIFT          VXWORKS_IOC_TYPESHIFT
#define _IOC_SIZESHIFT          VXWORKS_IOC_SIZESHIFT
#define _IOC_DIRSHIFT           VXWORKS_IOC_DIRSHIFT

#define _IOC_NONE               VXWORKS_IOC_NONE
#define _IOC_WRITE              VXWORKS_IOC_WRITE
#define _IOC_READ               VXWORKS_IOC_READ

#ifdef _IOC
#undef _IOC
#endif
#define _IOC(dir,type,nr,size)  VXWORKS_IOC(dir,type,nr,size)

/* used to create numbers */
#ifdef _IO
#undef _IO
#endif
#define _IO(type,nr)            VXWORKS_IO(type,nr)

#ifdef _IOR
#undef _IOR
#endif
#define _IOR(type,nr,size)      VXWORKS_IOR(type,nr,size)

#ifdef _IOW
#undef _IOW
#endif
#define _IOW(type,nr,size)      VXWORKS_IOW(type,nr,size)

#ifdef _IOWR
#undef _IOWR
#endif
#define _IOWR(type,nr,size)     VXWORKS_IOWR(type,nr,size)

/* used to decode ioctl numbers */
#ifdef _IOC_DIR
#undef _IOC_DIR
#endif
#define _IOC_DIR(nr)            VXWORKS_IOC_DIR(nr)

#ifdef _IOC_TYPE
#undef _IOC_TYPE
#endif
#define _IOC_TYPE(nr)           VXWORKS_IOC_TYPE(nr)

#ifdef _IOC_NR
#undef _IOC_NR
#endif
#define _IOC_NR(nr)             VXWORKS_IOC_NR(nr)

#ifdef _IOC_SIZE
#undef _IOC_SIZE
#endif
#define _IOC_SIZE(nr)           VXWORKS_IOC_SIZE(nr)

#ifdef IOC_IN
#undef IOC_IN
#endif
#define IOC_IN                  (_IOC_WRITE << _IOC_DIRSHIFT)

#ifdef IOC_OUT
#undef IOC_OUT
#endif
#define IOC_OUT                 (_IOC_READ << _IOC_DIRSHIFT)

#ifdef IOC_INOUT
#undef IOC_INOUT
#endif
#define IOC_INOUT               ((_IOC_WRITE|_IOC_READ) << _IOC_DIRSHIFT)

#ifdef IOCSIZE_MASK
#undef IOCSIZE_MASK
#endif
#define IOCSIZE_MASK            (_IOC_SIZEMASK << _IOC_SIZESHIFT)

#ifdef IOCSIZE_SHIFT
#undef IOCSIZE_SHIFT
#endif
#define IOCSIZE_SHIFT           (_IOC_SIZESHIFT)


#endif /* _VXOAL_IOCTL_H_ */
