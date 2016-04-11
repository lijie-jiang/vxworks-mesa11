/* Copyright 2002-2009 Wind River Systems, Inc. */
/* Copyright (c) 1998, 1999 Thai Open Source Software Center Ltd
   See the file COPYING for copying permission.
*/

/*
modification history
--------------------
01g,27mar09,f_f  upgrade to Expat 2.0.1
01f,23feb05,pas  added in-file comments from clearcase checkin comments
01e,13jan05,pas  upgraded to Expat 1.95.8
01d,21nov02,tky  Expat 1.95.5 merge
01c,16jul02,tky  upgrade to Expat 1.95.4 
01b,27jun02,tky  Updating to expat 1.95
01a,29apr02,zs   Initial checkin
*/

/* This file is included! */
#ifdef XML_TOK_NS_C

const ENCODING *
NS(XmlGetUtf8InternalEncoding)(void)
{
  return &ns(internal_utf8_encoding).enc;
}

const ENCODING *
NS(XmlGetUtf16InternalEncoding)(void)
{
#if BYTEORDER == 1234
  return &ns(internal_little2_encoding).enc;
#elif BYTEORDER == 4321
  return &ns(internal_big2_encoding).enc;
#else
  const short n = 1;
  return (*(const char *)&n
          ? &ns(internal_little2_encoding).enc
          : &ns(internal_big2_encoding).enc);
#endif
}

static const ENCODING * const NS(encodings)[] = {
  &ns(latin1_encoding).enc,
  &ns(ascii_encoding).enc,
  &ns(utf8_encoding).enc,
  &ns(big2_encoding).enc,
  &ns(big2_encoding).enc,
  &ns(little2_encoding).enc,
  &ns(utf8_encoding).enc /* NO_ENC */
};

static int PTRCALL
NS(initScanProlog)(const ENCODING *enc, const char *ptr, const char *end,
                   const char **nextTokPtr)
{
  return initScan(NS(encodings), (const INIT_ENCODING *)enc,
                  XML_PROLOG_STATE, ptr, end, nextTokPtr);
}

static int PTRCALL
NS(initScanContent)(const ENCODING *enc, const char *ptr, const char *end,
                    const char **nextTokPtr)
{
  return initScan(NS(encodings), (const INIT_ENCODING *)enc,
                  XML_CONTENT_STATE, ptr, end, nextTokPtr);
}

int
NS(XmlInitEncoding)(INIT_ENCODING *p, const ENCODING **encPtr,
                    const char *name)
{
  int i = getEncodingIndex(name);
  if (i == UNKNOWN_ENC)
    return 0;
  SET_INIT_ENC_INDEX(p, i);
  p->initEnc.scanners[XML_PROLOG_STATE] = NS(initScanProlog);
  p->initEnc.scanners[XML_CONTENT_STATE] = NS(initScanContent);
  p->initEnc.updatePosition = initUpdatePosition;
  p->encPtr = encPtr;
  *encPtr = &(p->initEnc);
  return 1;
}

static const ENCODING *
NS(findEncoding)(const ENCODING *enc, const char *ptr, const char *end)
{
#define ENCODING_MAX 128
  char buf[ENCODING_MAX];
  char *p = buf;
  int i;
  XmlUtf8Convert(enc, &ptr, end, &p, p + ENCODING_MAX - 1);
  if (ptr != end)
    return 0;
  *p = 0;
  if (streqci(buf, KW_UTF_16) && enc->minBytesPerChar == 2)
    return enc;
  i = getEncodingIndex(buf);
  if (i == UNKNOWN_ENC)
    return 0;
  return NS(encodings)[i];
}

int
NS(XmlParseXmlDecl)(int isGeneralTextEntity,
                    const ENCODING *enc,
                    const char *ptr,
                    const char *end,
                    const char **badPtr,
                    const char **versionPtr,
                    const char **versionEndPtr,
                    const char **encodingName,
                    const ENCODING **encoding,
                    int *standalone)
{
  return doParseXmlDecl(NS(findEncoding),
                        isGeneralTextEntity,
                        enc,
                        ptr,
                        end,
                        badPtr,
                        versionPtr,
                        versionEndPtr,
                        encodingName,
                        encoding,
                        standalone);
}

#endif /* XML_TOK_NS_C */
