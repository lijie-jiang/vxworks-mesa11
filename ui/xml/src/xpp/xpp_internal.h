/* xpp_internal.h - XML pull parser library internal functions and data structures */

/* Copyright 2002-2005 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06jul05,pas  - split from xpp.h
                 - added support for nested external entities
                 - removed almost all fixed-length buffers and arrays
                 - removed static isWhitespace struct flag
*/

#ifndef __INCxppintlh
#define __INCxppintlh

/* includes */

#include <taskLib.h>
#include <semLib.h>
#include "webservices/xml/expat.h"

#ifdef XPP_DEBUG
#define XML_XPP_DEBUG(msg, x1, x2, x3, x4) \
        fprintf(stderr, msg, __FILE__, __LINE__, x1, x2, x3, x4)
#else
#define XML_XPP_DEBUG(msg, x1, x2, x3, x4)
#endif

#define XPP_DEBUG_STRING_SIZE 160 /* size of XPP.errorString, a string used to store error messages */

/* lockStep routine related turn indicators */
#define XML_TURN_XPP            0
#define XML_TURN_SAX            1

/* parser structure */

struct XPP
    {
    /* The following struct contains the fields that are specific to a
     * particular input file.  We can parse arbitrarily nested external
     * entities by pushing a new parser to the head of the list.
     */
    struct _curParserInfo {
        struct _curParserInfo * prev;
        char *          _cp_fileName;
        FILE *          _cp_filePtr;
        char *          _cp_basePath;
        XML_Parser      _cp_xmlParser;
    }                   curParserInfo,
                      * pCurParserInfo;
    #define _fileName   pCurParserInfo->_cp_fileName
    #define _filePtr    pCurParserInfo->_cp_filePtr
    #define _basePath   pCurParserInfo->_cp_basePath
    #define _xmlParser  pCurParserInfo->_cp_xmlParser

    const XML_Char *    xmlString;
    int                 errorCode;
    char                errorString[XPP_DEBUG_STRING_SIZE + 1];

    int                 turn;
    int                 saxTaskId;
    SEM_ID              xppSemId;

    BOOL                extEntParseEnabled;
    XPP_EVENT_T         eventType;

    int                 depth;

    struct tagName {
        XML_Char *      qName;
        XML_Char *      localName;
        XML_Char *      nameSpace;
    }                   elementName;

    struct tagAttr {
        struct tagName  name;
        XML_Char *      value;
    }                 * attributes;
    int                 nAttributes;
    int                 maxAttributes;

    XML_Char **         nsPrefixes;
    XML_Char **         nsUris;
    int                 nNamespaces;

    BOOL                isEmptyElementTag;

    int                 characterDataLen;
    const XML_Char *    characterData;
    };

STATUS lockStep(XPP *pXpp, int myTurn);

/* from sax.c */
void saxParseFile_start(XPP *pXpp);
void saxParseString_start(XPP *pXpp);

#endif /* __INCxppintlh */
