/* xmlCanon.h - XML parser canonicalization module header file*/

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,30July2002,tky   Source header added(see clearcase checkin messages)
*/

/*
DESCRIPTION

Defines and function prototypes for the XML parser Canonicalization module
*/

#ifndef __INCxmlCanonh
#define __INCxmlCanonh

#ifdef __cplusplus
extern "C" {
#endif

/*includes */

#include "webservices/xml/expat.h"
#include "webservices/xml/xmlop/xmlop.h"

/* defines */

#undef CANON_DEBUG

#ifdef CANON_DEBUG
#define XML_CANON_DEBUG(msg, x1, x2, x3, x4) \
        fprintf(stderr, msg, __FILE__, __LINE__, x1, x2, x3, x4)
#else
#define XML_CANON_DEBUG(msg, x1, x2, x3, x4) 
#endif

#define NSSEP T('\001')

#define XMLCANON_DOCTYPEBUF_SIZE 1024
#define XMLCANON_NOTATIONLIST_SIZE 100
#define XMLCANON_NOTATIONNAME_SIZE 50

/* function declarartions */
void xmlCanonParamInit();

extern void xmlCanonFile(const char *canonizeDocName, const char *outputDocName);

#ifdef __cplusplus
}
#endif

#endif /* __INCxmlCanonh */

