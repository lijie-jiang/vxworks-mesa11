/*
 * Copyright (c) 2014-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
09jan15,qsn  Fix buffer overflow problem in running splash_inflate (US50628)  
22dec14,yat  Fix build warnings for LP64 (US50456)
24jan14,mgc  Modified for VxWorks 7 release
*/

#ifndef __INC_inflate_inl
#define __INC_inflate_inl

extern unsigned int wrSplashInflatedSize;
extern unsigned char wrSplashDeflatedData[];
extern unsigned int wrSplashDeflatedSize;
#include <types/vxCpu.h>
#define Z_DEF_WBITS   15
#define Z_PRESET_DICT 0x20
#ifndef OF
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
#define Z_DEFLATED   8
#define Z_NULL  0
#define Z_BASE 65521L
#define Z_NMAX 5552
#define Z_ALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define Z_FREE(strm, addr) \
           if((strm)->zfree != NULL) \
               (*((strm)->zfree))((strm)->opaque, (z_voidp)(addr))
#define Z_TRY_FREE(s, p) {if (p) Z_FREE(s, p);}
#define Z_LENGTH_CODES 29
#define Z_LITERALS  256
#define Z_L_CODES (Z_LITERALS+1+Z_LENGTH_CODES)
#define Z_D_CODES   30
#define Z_BL_CODES  19
#define Z_HEAP_SIZE (2*Z_L_CODES+1)
#define Z_MAX_BITS 15
#define Z_INIT_STATE    42
#define Z_BUSY_STATE   113
#define Z_FINISH_STATE 666
#define z_Freq fc.freq
#define z_Code fc.code
#define z_Dad  dl.dad
#define z_Len  dl.len
#define Z_UPDBITS {s->bitb=b;s->bitk=k;}
#define Z_UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define Z_UPDOUT {s->write=q;}
#define Z_UPDATE {Z_UPDBITS Z_UPDIN Z_UPDOUT}
#define Z_LEAVE {Z_UPDATE return inflate_flush(s,z,r);}
#define Z_LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define Z_NEEDBYTE {if(n)r=Z_OK;else Z_LEAVE}
#define Z_NEXTBYTE (n--,*p++)
#define Z_NEEDBITS(j) {while(k<(j)){Z_NEEDBYTE;b|=((z_uLong)Z_NEXTBYTE)<<k;k+=8;}}
#define Z_DUMPBITS(j) {b>>=(j);k-=(j);}
#define Z_WAVAIL (z_uInt)(q<s->read?s->read-q-1:s->end-q)
#define Z_LOADOUT {q=s->write;m=(z_uInt)Z_WAVAIL;}
#define Z_WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(z_uInt)Z_WAVAIL;}}
#define Z_FLUSH {Z_UPDOUT r=inflate_flush(s,z,r); Z_LOADOUT}
#define Z_NEEDOUT {if(m==0){Z_WRAP if(m==0){Z_FLUSH Z_WRAP if(m==0) Z_LEAVE}}r=Z_OK;}
#define Z_OUTBYTE(a) {*q++=(z_Byte)(a);m--;}
#define Z_LOAD {Z_LOADIN Z_LOADOUT}
#define z_put_byte(s, c) {s->pending_buf[s->pending++] = (c);}
#define Z_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define Z_DO2(buf,i)  Z_DO1(buf,i); Z_DO1(buf,i+1);
#define Z_DO4(buf,i)  Z_DO2(buf,i); Z_DO2(buf,i+2);
#define Z_DO8(buf,i)  Z_DO4(buf,i); Z_DO4(buf,i+4);
#define Z_DO16(buf)   Z_DO8(buf,0); Z_DO8(buf,8);
#define z_base more.Base
#define next more.Next
#define exop word.what.Exop
#define bits word.what.Bits
#define Z_BMAX 15
#define Z_N_MAX 288
#define Z_C0 *p++ = 0;
#define Z_C2 Z_C0 Z_C0 Z_C0 Z_C0
#define Z_C4 Z_C2 Z_C2 Z_C2 Z_C2
#define Z_FIXEDH 530
#define Z_BUF_SIZE  100000
#define Z_MEM_ALIGN 4
#define Z_BLK_ALIGN sizeof(int)
#define Z_ROUND_UP(n)   ((n + Z_BLK_ALIGN - 1) & (~(Z_BLK_ALIGN - 1)))
#define Z_BLK_HDR_SIZE  (2 * sizeof(int))
#define Z_BLK_NEXT(b)   (*(((ULONG *)(b)) - 1))
#define Z_BLK_PREV(b)   (*(((ULONG *)(b)) - 2))
#if defined(__GNUC__)
#define Z_BLK_HDRS_LINK(this,next)      \
        {                               \
        Z_BLK_NEXT(this) = (ULONG)(next); \
        Z_BLK_PREV(next) = (ULONG)(this); \
        }
#else
#define Z_BLK_HDRS_LINK(this,next)      \
        {                               \
        Z_BLK_NEXT(this) = *(ULONG*)&(next); \
        Z_BLK_PREV(next) = *(ULONG*)&(this); \
        }
#endif
#define Z_BLK_IS_FREE(b)    (Z_BLK_NEXT(b) & 1)
#define Z_BLK_FREE_SET(b)   (Z_BLK_NEXT(b) |= 1)
#if defined(__GNUC__)
#define Z_BLK_IS_VALID(b)   (((char *)b == buf) \
        || (Z_BLK_PREV(Z_BLK_NEXT(b)) == (ULONG)(b)))
#else
#define Z_BLK_IS_VALID(b)   (((char *)b == buf) \
        || (Z_BLK_PREV(Z_BLK_NEXT(b)) == *(ULONG*)&(b)))
#endif
#define base more.Base
#define next more.Next
#define exop word.what.Exop
#define bits word.what.Bits
#define Z_GRABBITS(j) {while(k<(j)){b|=((z_uLong)Z_NEXTBYTE)<<k;k+=8;}}
#define Z_UNGRAB {n+=(c=k>>3);p-=c;k&=7;}
#ifdef DEBUG
#  include <stdio.h>
#  ifndef z_verbose
#    define z_verbose 0
#  endif
#  define z_Assert(cond,msg) {if(!(cond)) z_error(msg);}
#  define z_Trace(x) fprintf x
#  define z_Tracev(x) {if (z_verbose) fprintf x ;}
#  define z_Tracevv(x) {if (z_verbose>1) fprintf x ;}
#  define z_Tracec(c,x) {if (z_verbose && (c)) fprintf x ;}
#  define z_Tracecv(c,x) {if (z_verbose>1 && (c)) fprintf x ;}
#  define Z_DBG_PUT(a,b)    fprintf (stderr, a, (unsigned int)b);
#else
#  define z_Assert(cond,msg)
#  define z_Trace(x)
#  define z_Tracev(x)
#  define z_Tracevv(x)
#  define z_Tracec(c,x)
#  define z_Tracecv(c,x)
#  define Z_DBG_PUT(a,b)
#endif
typedef unsigned char   z_Byte;
typedef unsigned int    z_uInt;
typedef unsigned long   z_uLong;
typedef void *          z_voidp;
typedef unsigned char   z_uch;
typedef unsigned short  z_ush;
typedef unsigned long   z_ulg;
#ifdef __cplusplus
extern "C" {
#endif
typedef z_voidp (*alloc_func) OF((z_voidp opaque, z_uLong items, z_uLong size));
typedef void   (*free_func)  OF((z_voidp opaque, z_voidp address));
struct internal_state;
typedef struct z_stream_s {
    z_Byte    *next_in;
    z_uInt     avail_in;
    z_uLong    total_in;
    z_Byte    *next_out;
    z_uInt     avail_out;
    z_uLong    total_out;
    char      *msg;
    struct internal_state  *state;
    alloc_func zalloc;
    free_func  zfree;
    z_voidp    opaque;
    int        data_type;
    z_uLong    adler;
    z_uLong    reserved;
} z_stream;
typedef z_stream  *z_streamp;
#ifdef __cplusplus
}
#endif
typedef z_uLong (*check_func) OF((z_uLong check, const z_Byte *buf, z_uInt len));
typedef struct ct_data_s {
    union {
        z_ush  freq;
        z_ush  code;
    } fc;
    union {
        z_ush  dad;
        z_ush  len;
    } dl;
}  ct_data;
typedef struct static_tree_desc_s  static_tree_desc;
typedef struct tree_desc_s {
    ct_data *dyn_tree;
    int     max_code;
    static_tree_desc *stat_desc;
}  tree_desc;
typedef z_ush Pos;
typedef Pos  Posf;
typedef unsigned IPos;
typedef struct inflate_huft_s  inflate_huft;
struct inflate_huft_s {
  union {
    struct {
      z_Byte Exop;
      z_Byte Bits;
    } what;
    z_Byte *pad;
  } word;
  union {
    z_uInt Base;
    inflate_huft *Next;
  } more;
};
struct inflate_blocks_state;
typedef struct inflate_blocks_state  inflate_blocks_statef;
struct inflate_codes_state;
typedef struct inflate_codes_state  inflate_codes_statef;
typedef enum {
      TYPE,
      LENS,
      STORED,
      TABLE,
      BTREE,
      DTREE,
      CODES,
      DRY,
      DONE,
      BAD}
inflate_block_mode;
struct inflate_blocks_state {
  inflate_block_mode  mode;
  union {
    z_uInt left;
    struct {
      z_uInt table;
      z_uInt index;
      z_uInt *blens;
      z_uInt bb;
      inflate_huft *tb;
    } trees;
    struct {
      inflate_huft *tl;
      inflate_huft *td;
      inflate_codes_statef 
         *codes;
    } decode;
  } sub;
  z_uInt last;
  z_uInt bitk;
  z_uLong bitb;
  z_Byte *window;
  z_Byte *end;
  z_Byte *read;
  z_Byte *write;
  check_func checkfn;
  z_uLong check;
};
struct inflate_codes_state {
  enum {
      START,
      LEN,
      LENEXT,
      DIST,
      DISTEXT,
      COPY,
      LIT,
      WASH,
      END,
      BADCODE}
    mode;
  z_uInt len;
  union {
    struct {
      inflate_huft *tree;
      z_uInt need;
    } code;
    z_uInt lit;
    struct {
      z_uInt get;
      z_uInt dist;
    } copy;
  } sub;
  z_Byte lbits;
  z_Byte dbits;
  inflate_huft *ltree;
  inflate_huft *dtree;
};
struct internal_state {
  enum {
      METHOD,
      FLAG,
      DICT4,
      DICT3,
      DICT2,
      DICT1,
      DICT0,
      BLOCKS,
      CHECK4,
      CHECK3,
      CHECK2,
      CHECK1,
      INF_DONE,
      INF_BAD}
    mode;
  union {
    z_uInt method;
    struct {
      z_uLong was;
      z_uLong need;
    } check;
  } sub;
  int  nowrap;
  z_uInt wbits;
  inflate_blocks_statef 
    *blocks;
};
static z_uInt cplens[31] = {
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
static z_uInt cplext[31] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 192, 192};
static z_uInt cpdist[30] = {
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
static z_uInt cpdext[30] = {
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};
static z_uInt inflate_mask[17] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};
static z_uInt border[] = {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
static int fixed_built = 0;
static inflate_huft fixed_mem[Z_FIXEDH];
static z_uInt fixed_bl = 0;
static z_uInt fixed_bd = 0;
static inflate_huft *fixed_tl = 0;
static inflate_huft *fixed_td = 0;
static int  intBuf [(Z_BLK_HDR_SIZE + Z_BUF_SIZE)/sizeof(int)];
static char * buf       =   Z_BLK_HDR_SIZE + (char *)intBuf;
static char * nextBlock;
static int huft_build OF((
    z_uInt *,
    z_uInt,
    z_uInt,
    z_uInt *,
    z_uInt *,
    inflate_huft * *,
    z_uInt *,
    z_streamp ));
static z_voidp falloc OF((
    z_voidp,
    z_uInt,
    z_uInt));
static int inflate_trees_free OF((
    inflate_huft *  t,
    z_streamp       z
    ));
static int inflate_flush OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));
static int inflate_fast OF((
    z_uInt,
    z_uInt,
    inflate_huft *,
    inflate_huft *,
    inflate_blocks_statef *,
    z_streamp ));
static z_uLong adler32
    (
    z_uLong       adler,
    const z_Byte *buf,
    z_uInt        len
    )
    {
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL)
    return 1L;

    while (len > 0)
    {
        k = len < Z_NMAX ? len : Z_NMAX;
        len -= k;
        while (k >= 16)
        {
            Z_DO16(buf);
        buf += 16;
            k -= 16;
            }
        if (k != 0)
        do
        {
            s1 += *buf++;
        s2 += s1;
            } while (--k);

        s1 %= Z_BASE;
        s2 %= Z_BASE;
    }

    return (s2 << 16) | s1;
    }
static z_voidp zcalloc
    (
    z_voidp opaque,
    unsigned long items,
    unsigned long size
    )
    {
    z_voidp thisBlock = (z_voidp)nextBlock;
    unsigned long nBytes = Z_ROUND_UP (items * size);
    if ((char *)thisBlock + nBytes + Z_BLK_HDR_SIZE >= &buf[Z_BUF_SIZE])
    {
    Z_DBG_PUT ("zcalloc %d bytes: buffer overflow!\n", nBytes);
    return (0);
    }
    nextBlock = (char *)thisBlock + nBytes + Z_BLK_HDR_SIZE;
    Z_BLK_HDRS_LINK (thisBlock, nextBlock);
    return (thisBlock);
    }
static void  zcfree
    (
    z_voidp opaque,
    z_voidp ptr
    )
    {
    z_voidp thisBlock;
    if (!Z_BLK_IS_VALID(ptr))
    {
    Z_DBG_PUT ("free at invalid address 0x%x\n", ptr);
    return;
    }
    Z_BLK_FREE_SET (ptr);
    for (thisBlock = (z_voidp)Z_BLK_PREV(nextBlock);
     thisBlock != 0 && Z_BLK_IS_FREE(thisBlock);
     thisBlock = (z_voidp)Z_BLK_PREV(thisBlock))
    {
    nextBlock = thisBlock;
    Z_BLK_NEXT(nextBlock) = 0;
    }

    return;
    }
static z_uInt c[Z_BMAX+1];
static inflate_huft *u[Z_BMAX];
static z_uInt v[Z_N_MAX];
static z_uInt x[Z_BMAX+1];
static int huft_build
    (
    z_uInt *    b,
    z_uInt      n,
    z_uInt      s,
    z_uInt *    d,
    z_uInt *    e,  
    inflate_huft ** t,
    z_uInt *    m,
    z_streamp   zs
    )
    {
    z_uInt a;
    z_uInt f;
    int g;
    int h;
    register z_uInt i;
    register z_uInt j;
    register int k;
    int l;
    register z_uInt *p;
    inflate_huft *q;
    struct inflate_huft_s r;
    register int w;
    z_uInt *xp;
    int y;
    z_uInt z;
    p = c;
    Z_C4
    p = b;  i = n;
    do
    {
    c[*p++]++;
    } while (--i);
    if (c[0] == n)
    {
    *t = (inflate_huft *)Z_NULL;
    *m = 0;
    return Z_OK;
    }
    l = *m;
    for (j = 1; j <= Z_BMAX; j++)
    if (c[j])
        break;
    k = j;
    if ((z_uInt)l < j)
    l = j;
    for (i = Z_BMAX; i; i--)
    if (c[i])
        break;
    g = i;
    if ((z_uInt)l > i)
    l = i;
    *m = l;
    for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
        return Z_DATA_ERROR;
    if ((y -= c[i]) < 0)
    return Z_DATA_ERROR;
    c[i] += y;
    x[1] = j = 0;
    p = c + 1;  xp = x + 2;
    while (--i)
    {
    *xp++ = (j += *p++);
    }
    p = b;  i = 0;
    do
    {
    if ((j = *p++) != 0)
        v[x[j]++] = i;
    } while (++i < n);
    x[0] = i = 0;
    p = v;
    h = -1;
    w = -l;
    u[0] = (inflate_huft *)Z_NULL;
    q = (inflate_huft *)Z_NULL;
    z = 0;
    for (; k <= g; k++)
    {
    a = c[k];
    while (a--)
        {
        while (k > w + l)
        {
        h++;
            w += l;
            z = g - w;
            z = z > (z_uInt)l ? l : z;
            if ((f = 1 << (j = k - w)) > a + 1)
                {
                f -= a + 1;
                xp = c + k;
                if (j < z)
                while (++j < z)
                    {
                        if ((f <<= 1) <= *++xp)
                            break;
                    f -= *xp;
                }
                }
            z = 1 << j;
            q = (inflate_huft *)Z_ALLOC(zs,z + 1,sizeof(inflate_huft));
            if (q == Z_NULL)
                {
                if (h)
                    (void)inflate_trees_free(u[0], zs);
                return Z_MEM_ERROR;
                }
            *t = q + 1;
            *(t = &(q->next)) = Z_NULL;
            u[h] = ++q;
            if (h)
                {
                x[h] = i;
                r.bits = (z_Byte)l;
                r.exop = (z_Byte)j;
                r.next = q;
                j = i >> (w - l);
                u[h-1][j] = r;
                }
        }
        r.bits = (z_Byte)(k - w);
        if (p >= v + n)
            r.exop = 128 + 64;
        else if (*p < s)
            {
            r.exop = (z_Byte)(*p < 256 ? 0 : 32 + 64);
            r.base = *p++;
            }
        else
            {
            r.exop = (z_Byte)(e[*p - s] + 16 + 64);
            r.base = d[*p++ - s];
            }
        f = 1 << (k - w);
        for (j = i >> w; j < z; j += f)
            q[j] = r;
        for (j = 1 << (k - 1); i & j; j >>= 1)
            i ^= j;
        i ^= j;
        while ((i & ((1 << w) - 1)) != x[h])
            {
            h--;
            w -= l;
            }
        }
    }
    return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
    }
static int inflate_trees_bits
    (
    z_uInt *c,
    z_uInt *bb,
    inflate_huft ** tb,
    z_streamp z
    )
    {
    int r;
    r = huft_build(c, 19, 19, (z_uInt*)Z_NULL, (z_uInt*)Z_NULL, tb, bb, z);
    if (r == Z_DATA_ERROR)
    z->msg = (char*)"oversubscribed dynamic bit lengths tree";
    else if (r == Z_BUF_ERROR)
    {
    (void)inflate_trees_free(*tb, z);
    z->msg = (char*)"incomplete dynamic bit lengths tree";
    r = Z_DATA_ERROR;
    }
    return r;
    }
static int inflate_trees_dynamic
    (
    z_uInt          nl,
    z_uInt          nd,
    z_uInt *        c,
    z_uInt *        bl,
    z_uInt *        bd,
    inflate_huft ** tl,
    inflate_huft ** td,
    z_streamp       z
    )
    {
    int r;
    if ((r = huft_build(c, nl, 257, cplens, cplext, tl, bl, z)) != Z_OK)
    {
    if (r == Z_DATA_ERROR)
        z->msg = (char*)"oversubscribed literal/length tree";
    else if (r == Z_BUF_ERROR)
        {
        (void)inflate_trees_free(*tl, z);
        z->msg = (char*)"incomplete literal/length tree";
        r = Z_DATA_ERROR;
        }
    return r;
    }
    if ((r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, z)) != Z_OK)
    {
    if (r == Z_DATA_ERROR)
         z->msg = (char*)"oversubscribed literal/length tree";
    else if (r == Z_BUF_ERROR) {
        (void)inflate_trees_free(*td, z);
        z->msg = (char*)"incomplete literal/length tree";
        r = Z_DATA_ERROR;
        }
    (void)inflate_trees_free(*tl, z);
    return r;
    }
    return Z_OK;
    }
static z_voidp falloc
    (
    z_voidp q,
    z_uInt  n,
    z_uInt  s
    )
    {
    *(int *)q -= n;
    return (z_voidp)(fixed_mem + *(int *)q);
    }
static int inflate_trees_fixed
    (
    z_uInt *    bl,
    z_uInt *    bd,
    inflate_huft ** tl,
    inflate_huft ** td
    )
    {
    if (!fixed_built)
    {
    int k;
    unsigned *c = (unsigned *)zcalloc (0, 288, sizeof (unsigned));
    z_stream z;
    int f = Z_FIXEDH;
    z.zalloc = falloc;
    z.zfree = Z_NULL;
    z.opaque = (z_voidp)&f;

    if (c == NULL)
        return Z_ERRNO;

    for (k = 0; k < 144; k++)
        c[k] = 8;
    for (; k < 256; k++)
        c[k] = 9;
    for (; k < 280; k++)
        c[k] = 7;
        for (; k < 288; k++)
        c[k] = 8;
    fixed_bl = 7;
    (void) huft_build(c, 288, 257, cplens, cplext, &fixed_tl, &fixed_bl, &z);
    for (k = 0; k < 30; k++)
#if CPU_FAMILY==MC680X0
        ((volatile unsigned *)c)[k] = 5;
#else
        c[k] = 5;
#endif
    fixed_bd = 5;
    (void) huft_build(c, 30, 0, cpdist, cpdext, &fixed_td, &fixed_bd, &z);
    fixed_built = 1;
    zcfree (0, c);
    }
    *bl = fixed_bl;
    *bd = fixed_bd;
    *tl = fixed_tl;
    *td = fixed_td;
    return Z_OK;
    }
static int inflate_trees_free
    (
    inflate_huft *  t,
    z_streamp       z
    )
    {
    register inflate_huft *p, *q, *r;
    p = Z_NULL;
    q = t;
    while (q != Z_NULL)
    {
    r = (q - 1)->next;
    (q - 1)->next = p;
    p = q;
    q = r;
    }
    while (p != Z_NULL)
    {
    q = (--p)->next;
    Z_FREE(z,p);
    p = q;
    } 
    return Z_OK;
    }
static int inflate_flush
    (
    inflate_blocks_statef * s,
    z_streamp       z,
    int         r
    )
    {
    z_uInt n;
    z_Byte *p;
    z_Byte *q;
    p = z->next_out;
    q = s->read;
    n = (z_uInt)((q <= s->write ? s->write : s->end) - q);
    if (n > z->avail_out)
    n = z->avail_out;
    if (n && r == Z_BUF_ERROR)
    r = Z_OK;
    z->avail_out -= n;
    z->total_out += n;
    if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(s->check, q, n);
    memcpy(p, q, n);
    p += n;
    q += n;
    if (q == s->end)
    {
    q = s->window;
    if (s->write == s->end)
        s->write = s->window;
    n = (z_uInt)(s->write - q);
    if (n > z->avail_out)
        n = z->avail_out;
    if (n && r == Z_BUF_ERROR)
        r = Z_OK;
    z->avail_out -= n;
    z->total_out += n;
    if (s->checkfn != Z_NULL)
        z->adler = s->check = (*s->checkfn)(s->check, q, n);
    memcpy(p, q, n);
    p += n;
    q += n;
    }
    z->next_out = p;
    s->read = q;
    return r;
    }
static int inflate_fast
    (
    z_uInt                  bl,
    z_uInt                  bd,
    inflate_huft          * tl,
    inflate_huft          * td,
    inflate_blocks_statef * s,
    z_streamp               z
    )
    {
    inflate_huft *t;
    z_uInt e;
    z_uLong b;
    z_uInt k;
    z_Byte *p;
    z_uInt n;
    z_Byte *q;
    z_uInt m;
    z_uInt ml;
    z_uInt md;
    z_uInt c;
    z_uInt d;
    z_Byte *r;
  Z_LOAD
  ml = inflate_mask[bl];
  md = inflate_mask[bd];
  do {
    Z_GRABBITS(20)
    if ((e = (t = tl + ((z_uInt)b & ml))->exop) == 0)
    {
      Z_DUMPBITS(t->bits)
      z_Tracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                "inflate:         * literal '%c'\n" :
                "inflate:         * literal 0x%02x\n", t->base));
      *q++ = (z_Byte)t->base;
      m--;
      continue;
    }
    do {
      Z_DUMPBITS(t->bits)
      if (e & 16)
      {
        e &= 15;
        c = t->base + ((z_uInt)b & inflate_mask[e]);
        Z_DUMPBITS(e)
        z_Tracevv((stderr, "inflate:         * length %u\n", c));
        Z_GRABBITS(15);
        e = (t = td + ((z_uInt)b & md))->exop;
        do {
          Z_DUMPBITS(t->bits)
          if (e & 16)
          {
            e &= 15;
            Z_GRABBITS(e)
            d = t->base + ((z_uInt)b & inflate_mask[e]);
            Z_DUMPBITS(e)
            z_Tracevv((stderr, "inflate:         * distance %u\n", d));
            m -= c;
            if ((z_uInt)(q - s->window) >= d)
            {
              r = q - d;
              *q++ = *r++;  c--;
              *q++ = *r++;  c--;
            }
            else
            {
              e = d - (z_uInt)(q - s->window);
              r = s->end - e;
              if (c > e)
              {
                c -= e;
                do {
                  *q++ = *r++;
                } while (--e);
                r = s->window;
              }
            }
            do {
              *q++ = *r++;
            } while (--c);
            break;
          }
          else if ((e & 64) == 0)
            e = (t = t->next + ((z_uInt)b & inflate_mask[e]))->exop;
          else
          {
            z->msg = (char*)"invalid distance code";
            Z_UNGRAB
            Z_UPDATE
            return Z_DATA_ERROR;
          }
        } while (1);
        break;
      }
      if ((e & 64) == 0)
      {
        if ((e = (t = t->next + ((z_uInt)b & inflate_mask[e]))->exop) == 0)
        {
          Z_DUMPBITS(t->bits)
          z_Tracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                    "inflate:         * literal '%c'\n" :
                    "inflate:         * literal 0x%02x\n", t->base));
          *q++ = (z_Byte)t->base;
          m--;
          break;
        }
      }
      else if (e & 32)
      {
        z_Tracevv((stderr, "inflate:         * end of block\n"));
        Z_UNGRAB
        Z_UPDATE
        return Z_STREAM_END;
      }
      else
      {
        z->msg = (char*)"invalid literal/length code";
        Z_UNGRAB
        Z_UPDATE
        return Z_DATA_ERROR;
      }
    } while (1);
  } while (m >= 258 && n >= 10);
  Z_UNGRAB
  Z_UPDATE
  return Z_OK;
}
static inflate_codes_statef *inflate_codes_new
    (
    z_uInt bl,
    z_uInt bd,
    inflate_huft *tl,
    inflate_huft *td,
    z_streamp z
    )
    {
    inflate_codes_statef *c;
    if ((c = (inflate_codes_statef *)
       Z_ALLOC(z,1,sizeof(struct inflate_codes_state))) != Z_NULL)
        {
        c->mode = START;
        c->lbits = (z_Byte)bl;
        c->dbits = (z_Byte)bd;
        c->ltree = tl;
        c->dtree = td;
        z_Tracev((stderr, "inflate:       codes new\n"));
        }
    return c;
    }
static int inflate_codes
    ( 
    inflate_blocks_statef *s,
    z_streamp z,
    int r
    )
{
  z_uInt j;
  inflate_huft *t;
  z_uInt e;
  z_uLong b;
  z_uInt k;
  z_Byte *p;
  z_uInt n;
  z_Byte *q;
  z_uInt m;
  z_Byte *f;
  inflate_codes_statef *c = s->sub.decode.codes;
  Z_LOAD
  while (1) switch (c->mode)
  {
    case START:
#ifndef SLOW
      if (m >= 258 && n >= 10)
      {
        Z_UPDATE
        r = inflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        Z_LOAD
        if (r != Z_OK)
        {
          c->mode = r == Z_STREAM_END ? WASH : BADCODE;
          break;
        }
      }
#endif
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:
      j = c->sub.code.need;
      Z_NEEDBITS(j)
      t = c->sub.code.tree + ((z_uInt)b & inflate_mask[j]);
      Z_DUMPBITS(t->bits)
      e = (z_uInt)(t->exop);
      if (e == 0)
      {
        c->sub.lit = t->base;
        z_Tracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                 "inflate:         literal '%c'\n" :
                 "inflate:         literal 0x%02x\n", t->base));
        c->mode = LIT;
        break;
      }
      if (e & 16)
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)
      {
        c->sub.code.need = e;
        c->sub.code.tree = t->next;
        break;
      }
      if (e & 32)
      {
        z_Tracevv((stderr, "inflate:         end of block\n"));
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;
      z->msg = (char*)"invalid literal/length code";
      r = Z_DATA_ERROR;
      Z_LEAVE
    case LENEXT:
      j = c->sub.copy.get;
      Z_NEEDBITS(j)
      c->len += (z_uInt)b & inflate_mask[j];
      Z_DUMPBITS(j)
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      z_Tracevv((stderr, "inflate:         length %u\n", c->len));
      c->mode = DIST;
      /* fall through */
    case DIST:
      j = c->sub.code.need;
      Z_NEEDBITS(j)
      t = c->sub.code.tree + ((z_uInt)b & inflate_mask[j]);
      Z_DUMPBITS(t->bits)
      e = (z_uInt)(t->exop);
      if (e & 16)
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)
      {
        c->sub.code.need = e;
        c->sub.code.tree = t->next;
        break;
      }
      c->mode = BADCODE;
      z->msg = (char*)"invalid distance code";
      r = Z_DATA_ERROR;
      Z_LEAVE
    case DISTEXT:
      j = c->sub.copy.get;
      Z_NEEDBITS(j)
      c->sub.copy.dist += (z_uInt)b & inflate_mask[j];
      Z_DUMPBITS(j)
      z_Tracevv((stderr, "inflate:         distance %u\n", c->sub.copy.dist));
      c->mode = COPY;
      /* fall through */
    case COPY:
#ifndef __TURBOC__
      f = (z_uInt)(q - s->window) < c->sub.copy.dist ?
          s->end - (c->sub.copy.dist - (q - s->window)) :
          q - c->sub.copy.dist;
#else
      f = q - c->sub.copy.dist;
      if ((z_uInt)(q - s->window) < c->sub.copy.dist)
        f = s->end - (c->sub.copy.dist - (z_uInt)(q - s->window));
#endif
      while (c->len)
      {
        Z_NEEDOUT
        Z_OUTBYTE(*f++)
        if (f == s->end)
          f = s->window;
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:
      Z_NEEDOUT
      Z_OUTBYTE(c->sub.lit)
      c->mode = START;
      break;
    case WASH:
      Z_FLUSH
      if (s->read != s->write)
        Z_LEAVE
      c->mode = END;
      /* fall through */
    case END:
      r = Z_STREAM_END;
      Z_LEAVE
    case BADCODE:
      r = Z_DATA_ERROR;
      Z_LEAVE
    default:
      r = Z_STREAM_ERROR;
      Z_LEAVE
  }
}
static void inflate_codes_free
    (
    inflate_codes_statef *c,
    z_streamp z
    )
    {
    Z_FREE(z, c);
    }
static void inflate_blocks_reset
    (
    inflate_blocks_statef *s,
    z_streamp z,
    z_uLong *c
    )
{
  if (s->checkfn != Z_NULL)
    *c = s->check;
  if (s->mode == BTREE || s->mode == DTREE)
    Z_FREE(z, s->sub.trees.blens);
  if (s->mode == CODES)
  {
    inflate_codes_free(s->sub.decode.codes, z);
    (void)inflate_trees_free(s->sub.decode.td, z);
    (void)inflate_trees_free(s->sub.decode.tl, z);
  }
  s->mode = TYPE;
  s->bitk = 0;
  s->bitb = 0;
  s->read = s->write = s->window;
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(0L, Z_NULL, 0);
  z_Trace((stderr, "inflate:   blocks reset\n"));
}
static inflate_blocks_statef *inflate_blocks_new
    (
    z_streamp z,
    check_func c,
    z_uInt w
    )
{
  inflate_blocks_statef *s;
  if ((s = (inflate_blocks_statef *)Z_ALLOC
       (z,1,sizeof(struct inflate_blocks_state))) == Z_NULL)
    return s;
  if ((s->window = (z_Byte *)Z_ALLOC(z, 1, w)) == Z_NULL)
  {
    Z_FREE(z, s);
    return Z_NULL;
  }
  s->end = s->window + w;
  s->checkfn = c;
  s->mode = TYPE;
  z_Trace((stderr, "inflate:   blocks allocated\n"));
  inflate_blocks_reset(s, z, &s->check);
  return s;
}
static int inflate_blocks
    (
    inflate_blocks_statef *s,
    z_streamp z,
    int r
    )
{
  z_uInt t;
  z_uLong b;
  z_uInt k;
  z_Byte *p;
  z_uInt n;
  z_Byte *q;
  z_uInt m;
  Z_LOAD
  while (1) switch (s->mode)
  {
    case TYPE:
      Z_NEEDBITS(3)
      t = (z_uInt)b & 7;
      s->last = t & 1;
      switch (t >> 1)
      {
        case 0:
          z_Trace((stderr, "inflate:     stored block%s\n",
                 s->last ? " (last)" : ""));
          Z_DUMPBITS(3)
          t = k & 7;
          Z_DUMPBITS(t)
          s->mode = LENS;
          break;
        case 1:
          z_Trace((stderr, "inflate:     fixed codes block%s\n",
                 s->last ? " (last)" : ""));
          {
            z_uInt bl = 0;
            z_uInt bd = 0;
            inflate_huft *tl = NULL;
            inflate_huft *td = NULL;
            inflate_trees_fixed(&bl, &bd, &tl, &td);
            s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
            if (s->sub.decode.codes == Z_NULL)
            {
              r = Z_MEM_ERROR;
              Z_LEAVE
            }
            s->sub.decode.tl = Z_NULL;
            s->sub.decode.td = Z_NULL;
          }
          Z_DUMPBITS(3)
          s->mode = CODES;
          break;
        case 2:
          z_Trace((stderr, "inflate:     dynamic codes block%s\n",
                 s->last ? " (last)" : ""));
          Z_DUMPBITS(3)
          s->mode = TABLE;
          break;
        case 3:
          Z_DUMPBITS(3)
          s->mode = BAD;
          z->msg = (char*)"invalid block type";
          r = Z_DATA_ERROR;
          Z_LEAVE
      }
      break;
    case LENS:
      Z_NEEDBITS(32)
      if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
      {
        s->mode = BAD;
        z->msg = (char*)"invalid stored block lengths";
        r = Z_DATA_ERROR;
        Z_LEAVE
      }
      s->sub.left = (z_uInt)b & 0xffff;
      b = k = 0;
      z_Tracev((stderr, "inflate:       stored length %u\n", s->sub.left));
      s->mode = s->sub.left ? STORED : (s->last ? DRY : TYPE);
      break;
    case STORED:
      if (n == 0)
        Z_LEAVE
      Z_NEEDOUT
      t = s->sub.left;
      if (t > n) t = n;
      if (t > m) t = m;
      memcpy(q, p, t);
      p += t;  n -= t;
      q += t;  m -= t;
      if ((s->sub.left -= t) != 0)
        break;
      z_Tracev((stderr, "inflate:       stored end, %lu total out\n",
              z->total_out + (q >= s->read ? q - s->read :
              (s->end - s->read) + (q - s->window))));
      s->mode = s->last ? DRY : TYPE;
      break;
    case TABLE:
      Z_NEEDBITS(14)
      s->sub.trees.table = t = (z_uInt)b & 0x3fff;
      if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
      {
        s->mode = BAD;
        z->msg = (char*)"too many length or distance symbols";
        r = Z_DATA_ERROR;
        Z_LEAVE
      }
      t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
      if ((s->sub.trees.blens = (z_uInt*)Z_ALLOC(z, t, sizeof(z_uInt))) == Z_NULL)
      {
        r = Z_MEM_ERROR;
        Z_LEAVE
      }
      Z_DUMPBITS(14)
      s->sub.trees.index = 0;
      z_Tracev((stderr, "inflate:       table sizes ok\n"));
      s->mode = BTREE;
      /* fall through */
    case BTREE:
      while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
      {
        Z_NEEDBITS(3)
        s->sub.trees.blens[border[s->sub.trees.index++]] = (z_uInt)b & 7;
        Z_DUMPBITS(3)
      }
      while (s->sub.trees.index < 19)
        s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
      s->sub.trees.bb = 7;
      t = inflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
                             &s->sub.trees.tb, z);
      if (t != Z_OK)
      {
        Z_LEAVE
      }
      s->sub.trees.index = 0;
      z_Tracev((stderr, "inflate:       bits tree ok\n"));
      s->mode = DTREE;
      /* fall through */
    case DTREE:
      while (t = s->sub.trees.table,
             s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
      {
        inflate_huft *h;
        z_uInt i, j, c;

        t = s->sub.trees.bb;
        Z_NEEDBITS(t)
        h = s->sub.trees.tb + ((z_uInt)b & inflate_mask[t]);
        t = h->word.what.Bits;
        c = h->more.Base;
        if (c < 16)
        {
          Z_DUMPBITS(t)
          s->sub.trees.blens[s->sub.trees.index++] = c;
        }
        else
        {
          i = c == 18 ? 7 : c - 14;
          j = c == 18 ? 11 : 3;
          Z_NEEDBITS(t + i)
          Z_DUMPBITS(t)
          j += (z_uInt)b & inflate_mask[i];
          Z_DUMPBITS(i)
          i = s->sub.trees.index;
          t = s->sub.trees.table;
          if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
              (c == 16 && i < 1))
          {
            s->mode = BAD;
            z->msg = (char*)"invalid bit length repeat";
            r = Z_DATA_ERROR;
            Z_LEAVE
          }
          c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
          do {
            s->sub.trees.blens[i++] = c;
          } while (--j);
          s->sub.trees.index = i;
        }
      }
      (void)inflate_trees_free(s->sub.trees.tb, z);
      s->sub.trees.tb = Z_NULL;
      {
        z_uInt bl, bd;
        inflate_huft *tl, *td;
        inflate_codes_statef *c;
        bl = 9;
        bd = 6;
        t = s->sub.trees.table;
        t = inflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
                                  s->sub.trees.blens, &bl, &bd, &tl, &td, z);
        if (t != Z_OK)
        {
          if (t == (z_uInt)Z_DATA_ERROR)
            s->mode = BAD;
          r = t;
          Z_LEAVE
        }
        z_Tracev((stderr, "inflate:       trees ok, %d * %d bytes used\n",
              inflate_hufts, sizeof(inflate_huft)));
        if ((c = inflate_codes_new(bl, bd, tl, td, z)) == Z_NULL)
        {
          (void)inflate_trees_free(td, z);
          (void)inflate_trees_free(tl, z);
          r = Z_MEM_ERROR;
          Z_LEAVE
        }
        Z_FREE(z, s->sub.trees.blens);
        s->sub.decode.codes = c;
        s->sub.decode.tl = tl;
        s->sub.decode.td = td;
      }
      s->mode = CODES;
      /* fall through */
    case CODES:
      Z_UPDATE
      if ((r = inflate_codes(s, z, r)) != Z_STREAM_END)
        return inflate_flush(s, z, r);
      r = Z_OK;
      inflate_codes_free(s->sub.decode.codes, z);
      (void)inflate_trees_free(s->sub.decode.td, z);
      (void)inflate_trees_free(s->sub.decode.tl, z);
      Z_LOAD
      z_Tracev((stderr, "inflate:       codes end, %lu total out\n",
              z->total_out + (q >= s->read ? q - s->read :
              (s->end - s->read) + (q - s->window))));
      if (!s->last)
      {
        s->mode = TYPE;
        break;
      }
      if (k > 7)
      {
        z_Assert(k < 16, "inflate_codes grabbed too many bytes")
        k -= 8;
        n++;
        p--;
      }
      s->mode = DRY;
      /* fall through */
    case DRY:
      Z_FLUSH
      if (s->read != s->write)
        Z_LEAVE
      s->mode = DONE;
      /* fall through */
    case DONE:
      r = Z_STREAM_END;
      Z_LEAVE
    case BAD:
      r = Z_DATA_ERROR;
      Z_LEAVE
    default:
      r = Z_STREAM_ERROR;
      Z_LEAVE
  }
}
static int inflate_blocks_free
    (
    inflate_blocks_statef *s,
    z_streamp z,
    z_uLong *c
    )
{
  inflate_blocks_reset(s, z, c);
  Z_FREE(z, s->window);
  Z_FREE(z, s);
  return Z_OK;
}
static int inflateReset
    (
    z_streamp z
    )
{
  z_uLong c;
  if (z == Z_NULL || z->state == Z_NULL)
    return Z_STREAM_ERROR;
  z->total_in = z->total_out = 0;
  z->msg = Z_NULL;
  z->state->mode = z->state->nowrap ? BLOCKS : METHOD;
  inflate_blocks_reset(z->state->blocks, z, &c);
  z_Trace((stderr, "inflate: reset\n"));
  return Z_OK;
}
static int inflateEnd
    (
    z_streamp z
    )
{
  z_uLong c;
  if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->state->blocks != Z_NULL)
    (void)inflate_blocks_free(z->state->blocks, z, &c);
  Z_FREE(z, z->state);
  z->state = Z_NULL;
  z_Trace((stderr, "inflate: end\n"));
  return Z_OK;
}
static int inflateInit
    (
    z_streamp z
    )
{
  int w = Z_DEF_WBITS;
  if (z == Z_NULL)
    return Z_STREAM_ERROR;
  z->msg = Z_NULL;
  if (z->zalloc == Z_NULL)
  {
    z->zalloc = zcalloc;
    z->opaque = (z_voidp)0;
  }
  if (z->zfree == Z_NULL) z->zfree = zcfree;
  if ((z->state = (struct internal_state  *)
       Z_ALLOC(z,1,sizeof(struct internal_state))) == Z_NULL)
    return Z_MEM_ERROR;
  z->state->blocks = Z_NULL;
  z->state->nowrap = 0;
  z->state->wbits = (z_uInt)w;
  if ((z->state->blocks =
      inflate_blocks_new(z, z->state->nowrap ? Z_NULL : adler32, (z_uInt)1 << w))
      == Z_NULL)
  {
    (void) inflateEnd(z);
    return Z_MEM_ERROR;
  }
  z_Trace((stderr, "inflate: allocated\n"));
  (void)inflateReset(z);
  return Z_OK;
}
#undef  Z_NEEDBYTE
#undef  Z_NEXTBYTE
#define Z_NEEDBYTE {if(z->avail_in==0)return r;r=Z_OK;}
#define Z_NEXTBYTE (z->avail_in--,z->total_in++,*z->next_in++)
static int zinflate
    (
    z_streamp z,
    int f
    )
{
  int r;
  z_uInt b;
  if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL || f < 0)
    return Z_STREAM_ERROR;
  r = Z_BUF_ERROR;
  while (1) switch (z->state->mode)
  {
    case METHOD:
      Z_NEEDBYTE
      if (((z->state->sub.method = Z_NEXTBYTE) & 0xf) != Z_DEFLATED)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"unknown compression method";
        break;
      }
      if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"invalid window size";
        break;
      }
      z->state->mode = FLAG;
    case FLAG:
      Z_NEEDBYTE
      b = Z_NEXTBYTE;
      if (((z->state->sub.method << 8) + b) % 31)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"incorrect header check";
        break;
      }
      z_Trace((stderr, "inflate: zlib header ok\n"));
      if (!(b & Z_PRESET_DICT))
      {
        z->state->mode = BLOCKS;
    break;
      }
      z->state->mode = DICT4;
      /* fall through */
    case DICT4:
      Z_NEEDBYTE
      z->state->sub.check.need = (z_uLong)Z_NEXTBYTE << 24;
      z->state->mode = DICT3;
      /* fall through */
    case DICT3:
      Z_NEEDBYTE
      z->state->sub.check.need += (z_uLong)Z_NEXTBYTE << 16;
      z->state->mode = DICT2;
      /* fall through */
    case DICT2:
      Z_NEEDBYTE
      z->state->sub.check.need += (z_uLong)Z_NEXTBYTE << 8;
      z->state->mode = DICT1;
      /* fall through */
    case DICT1:
      Z_NEEDBYTE
      z->state->sub.check.need += (z_uLong)Z_NEXTBYTE;
      z->adler = z->state->sub.check.need;
      z->state->mode = DICT0;
      return Z_NEED_DICT;
    case DICT0:
      z->state->mode = INF_BAD;
      z->msg = (char*)"need dictionary";
      return Z_STREAM_ERROR;
    case BLOCKS:
      r = inflate_blocks(z->state->blocks, z, r);
      if (r == Z_DATA_ERROR)
      {
        z->state->mode = INF_BAD;
        break;
      }
      if (r != Z_STREAM_END)
        return r;
      r = Z_OK;
      inflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
      if (z->state->nowrap)
      {
        z->state->mode = INF_DONE;
        break;
      }
      z->state->mode = CHECK4;
      /* fall through */
    case CHECK4:
      Z_NEEDBYTE
      z->state->sub.check.need = (z_uLong)Z_NEXTBYTE << 24;
      z->state->mode = CHECK3;
      /* fall through */
    case CHECK3:
      Z_NEEDBYTE
      z->state->sub.check.need += (z_uLong)Z_NEXTBYTE << 16;
      z->state->mode = CHECK2;
      /* fall through */
    case CHECK2:
      Z_NEEDBYTE
      z->state->sub.check.need += (z_uLong)Z_NEXTBYTE << 8;
      z->state->mode = CHECK1;
      /* fall through */
    case CHECK1:
      Z_NEEDBYTE
      z->state->sub.check.need += (z_uLong)Z_NEXTBYTE;

      if (z->state->sub.check.was != z->state->sub.check.need)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"incorrect data check";
        break;
      }
      z_Trace((stderr, "inflate: zlib check ok\n"));
      z->state->mode = INF_DONE;
      /* fall through */
    case INF_DONE:
      return Z_STREAM_END;
    case INF_BAD:
      return Z_DATA_ERROR;
    default:
      return Z_STREAM_ERROR;
  }
}
static int splash_inflate
    (
    z_Byte *    src,
    z_Byte *    dest,
    int         nBytes
    )
{
    z_stream d_stream;
    int err;
    if (nBytes == 3)
    {
        memset(dest, 0, wrSplashInflatedSize);
        dest[0] = 'B';
        dest[1] = 'M';
        dest[10]= 90;
        dest[54] = src[2];
        dest[55] = src[1];
        dest[56] = src[0];
        return (0);
    }
    fixed_built = 0;
    fixed_bl = 0;
    fixed_bd = 0;
    fixed_tl = 0;
    fixed_td = 0;
    bzero ((void*)fixed_mem, sizeof (fixed_mem));
    nextBlock = Z_BLK_HDR_SIZE + (char *)intBuf;
    Z_BLK_PREV(nextBlock) = 0;  /* set first block's prev pointer */
    if (*src != Z_DEFLATED)
    {
    Z_DBG_PUT ("inflate error: *src = %d. not Z_DEFLATED data\n", *src);
    return (-1);
    }
    src++;
    nBytes -= 3;
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (z_voidp)0;
    d_stream.next_in  = src;
    d_stream.avail_in = nBytes;
    d_stream.next_out = dest;
    d_stream.avail_out = nBytes * 100;
    if (inflateInit(&d_stream) != Z_OK)
    return (-1);
    err = zinflate(&d_stream, 0);
    if (err == Z_STREAM_END)
        err = inflateEnd(&d_stream);
    if (err != Z_OK)
        return (-1);
    return (0);
}

#endif  /* __INC_inflate_inl */
