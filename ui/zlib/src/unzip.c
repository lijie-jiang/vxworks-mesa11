/* unzip.c - gzip compressed kernel support */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
14jan14,my_ written.
*/

#include <stdio.h>
#include <string.h>

#include <limits.h>
#include "zlib.h"

#ifdef GNUZIP_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define FTEXT (0x1 << 0)
#define FHCRC (0x1 << 1)
#define FEXTRA (0x1 << 2)
#define FNAME (0x1 << 3)
#define FCOMMENT (0x1 << 4)

#define DEFLATE 8

struct unzip_header {
    unsigned char id1;
    unsigned char id2;
    unsigned char cm;
    unsigned char flg;
    unsigned char mtime[4];
    unsigned char xfl;
    unsigned char os;

};

struct unzip_header_extra {
    unsigned char len1;
    unsigned char len2;
};

int vxInflate (unsigned char *, unsigned char *, unsigned int);

/*******************************************************************************
 *
 * unzip - inflate a compressed data
 *
 * This routine inflates a compressed data pointed by 'src' to 'dst'
 *
 * RETURNS: 0 on OK, or a negative error code.
 *
 * ERRNO:
 *    ENOMEM: if not enough memory
 *
 * \NOMANUAL
 */

int unzip
    (
    unsigned char *src,
    unsigned char *dst,
    unsigned int size
    )
    {
    unsigned int len;
    struct unzip_header * header = (struct unzip_header *)src;
    unsigned char *str = src + sizeof(struct unzip_header);

    if (header->cm != DEFLATE)
        {
        DEBUG("not unzip format\n");
        return -1;
        }

    size -= sizeof(struct unzip_header);

    DEBUG("skipping %u bytes\n", sizeof(struct unzip_header));

    if (header->flg & FEXTRA)
        {
        struct unzip_header_extra * extra = (struct unzip_header_extra *)(src + sizeof(struct unzip_header));
        len = (extra->len1 | (extra->len2 << 8)) + sizeof(struct unzip_header_extra);
        str += len;
        size -= (sizeof(struct unzip_header_extra) + len);
        DEBUG("skipping extra %u bytes\n", sizeof(struct unzip_header_extra) + len);
        }

    if (header->flg & FNAME)
        {
        len = strlen((char *)str) + 1;
        str += len;
        size -= len;
        DEBUG("skipping name %u bytes %x\n", len, *str);
        }

    if (header->flg & FCOMMENT)
        {
        len = strlen((char *)str) + 1;
        str += len;
        size -= len;
        DEBUG("skipping comments %u bytes\n", len + 1);
        }

    if (header->flg & FHCRC)
        {
        str += 2;
        size -= 2;
        DEBUG("skipping crc16%u bytes\n", 2);
        }

    return vxInflate(str, dst, size);
    }

/*******************************************************************************
 *
 * vxInflate - inflate compressed data
 *
 * This routine inflates  compressed data pointed by 'src' to 'dst'
 *
 * RETURNS: 0 on OK, or a negative error code.
 *
 * ERRNO:
 *    ENOMEM: if not enough memory
 *
 * \NOMANUAL
 */

int vxInflate
    (
    unsigned char *src,
    unsigned char *dst,
    unsigned int size
    )
    {
    int ret;
    z_stream strm;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    strm.next_in = src;
    strm.avail_in = size;
    strm.next_out = dst;
    strm.avail_out = UINT32_MAX;

    ret = inflateInit2(&strm,  -MAX_WBITS);
    if (ret != Z_OK)
        {
        DEBUG("inflateInit error - %d\n", ret);
        return ret;
        }

    ret = inflate(&strm, Z_FINISH);
    if (ret == Z_STREAM_END)
        {
        DEBUG("inflate stream end\n");
        ret = inflateEnd(&strm);
        }

    if (ret != Z_OK)
        {
        DEBUG("zlib error - %d\n", ret);
        return ret;
        }

    return 0;
    }

