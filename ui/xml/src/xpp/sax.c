/* sax.c - XML pull parser module's interface to the XML SAX parser*/

/* Copyright 2002-2003, 2005-2006, 2009, 2013 Wind River Systems, Inc. */

/*
modification history
--------------------
01s,29mar13,f_f  fix the Coverity issue (WIND00410897)
01r,01apr09,f_f  type cast when calling XML_GetUserData()
01q,05apr06,pas  fix a race condition in xppDestroy()
01p,02feb06,pas  bzero is in strings.h in user mode
01o,06jul05,pas  - added support for nested external entities
                 - removed static isWhitespace struct flag - evaluate it if
                   and when when the user is actually interested
                 - fixed some potential race conditions and memory leaks
01n,23feb05,pas  added in-file comments from clearcase checkin comments
01m,16apr03,tky  moved isWhitespace to XML_IsWhiteSpace within this component
01l,26mar03,tky  converted all prints to debug prints, using XML_XPP_DEBUG
                 macro in xpp.h
01k,25mar03,tky  code review changes
01j,21mar03,tky  updated source documentation, removed dead code
01i,04dec02,tky  - changed #define XPP_DEBUG to a variable for project facility
01h,23aug02,tky  moved static global variables fp and fpCount to the XPP
                 object to make the system more modular and allow concurrent
                 task execution
01g,25jul02,tky  string parsing now works identical to file parsing
01f,23jul02,tky  fix for external entity reference and character data handling
01e,20jun02,tky  fixed sax doctype decl handler
01d,10jun02,zs   Fixed semephore problems
01c,31may02,zs   Added emergency file pointer clean-up code.
01b,30may02,zs   Fixing deadlocks, xppGetText, error handling
01a,08may02,zs   Initial version
*/

/*
DESCRIPTION:

This file implements an instance of the XML SAX parser for the
purpose of use with the XML pull parser module.  The XML pull parser
is an additional XML parser interface that sits on top of the XML
SAX parser.
*/

#include <stdio.h>
#include <string.h>
#ifndef _WRS_KERNEL
#include <strings.h>
#endif
#include <errno.h>
#include <errnoLib.h>
#include <semLib.h>
#include "webservices/xml/expat.h"
#include "webservices/xml/xpp/xpp.h"
#include "xpp_internal.h"

static void saxCopyNameUtil(XPP *pXpp, const XML_Char *name);
static void saxCopyAttsUtil(XPP *pXpp, const XML_Char **atts);
static void saxParseNamespace(const XML_Char *input, struct tagName *tagName);
static void saxSetHandlersUtil(XPP *pXpp);
static void saxDisplayError(XPP *pXpp);

static void saxStartElementHandlerCommon(void *userData, const XML_Char *name, const XML_Char **atts, BOOL isEmptyElementTag);
static void saxStartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
static void saxEndElementHandler(void *userData, const XML_Char *name);
static void saxEmptyElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
static void saxCharacterDataHandler(void *userData, const XML_Char *s, int len);
static int  saxExternalEntityRefHandler(XML_Parser parser, const XML_Char *context, const XML_Char *base, const XML_Char *systemId, const XML_Char *publicId);

/* ------------------------------ handlers ----------------------------- */

static void saxStartElementHandler
    (
    void *userData,
    const XML_Char *name,
    const XML_Char **atts
    )
    {
    XPP *pXpp = (XPP *)userData;
    /*printf("saxStartElementHandler(%s)\n", name);fflush(stdout);*/
    saxStartElementHandlerCommon(userData, name, atts, FALSE);
    pXpp->depth++;
    }

static void saxEmptyElementHandler
    (
    void *userData,
    const XML_Char *name,
    const XML_Char **atts
    )
    {
    /*printf("saxEmptyElementHandler(%s)\n", name);fflush(stdout);*/
    saxStartElementHandlerCommon(userData, name, atts, TRUE);
    }

static void saxStartElementHandlerCommon
    (
    void *userData,
    const XML_Char *name,
    const XML_Char **atts,
    BOOL isEmptyElementTag
    )
    {
    int i;
    XPP *pXpp = (XPP *)userData;

    pXpp->eventType = START_TAG;
    pXpp->isEmptyElementTag = isEmptyElementTag;
    saxCopyNameUtil(pXpp, name);
    saxCopyAttsUtil(pXpp, atts);
    lockStep(pXpp, XML_TURN_SAX);

    /* free the dynamic data that we're done with */
    if (pXpp->elementName.nameSpace != NULL)
        {
        free(pXpp->elementName.nameSpace);
        pXpp->elementName.nameSpace = NULL;
        }

    for (i = 0; i < pXpp->nAttributes; ++i)
        {
        if (pXpp->attributes[i].name.nameSpace != NULL)
            {
            free(pXpp->attributes[i].name.nameSpace);
            pXpp->attributes[i].name.nameSpace = NULL;
            }
        }
    }


static void saxEndElementHandler
    (
    void *userData,
    const XML_Char *name
    )
    {
    XPP *pXpp = (XPP *)userData;
    /*printf("saxEndElementHandler(%s)\n", name);fflush(stdout);*/

    saxCopyNameUtil(pXpp, name);
    pXpp->eventType = END_TAG;

    pXpp->depth--;
    lockStep(pXpp, XML_TURN_SAX);

    /* free the dynamic data that we're done with */
    if (pXpp->elementName.nameSpace != NULL)
        {
        free(pXpp->elementName.nameSpace);
        pXpp->elementName.nameSpace = NULL;
        }
    }

static void saxCharacterDataHandler
    (
    void *userData,
    const XML_Char *s,
    int len
    )
    {
    XPP *pXpp = (XPP *)userData;
    /*printf("saxCharacterDataHandler(%p)\n", s);fflush(stdout);*/

    pXpp->characterData = s;
    pXpp->characterDataLen = len;
    pXpp->eventType = TEXT;
    lockStep(pXpp, XML_TURN_SAX);
    }

static int saxExternalEntityRefHandler
    (
    XML_Parser parser,
    const XML_Char *context,
    const XML_Char *base,
    const XML_Char *systemId,
    const XML_Char *publicId
    )
    {
    XPP *pXpp = (XPP *)XML_GetUserData(parser);
    struct _curParserInfo *cp;
    XML_Char buf[2048];
    int done;

    /*printf("saxExternalEntityRefHandler(%s %s)\n", base, systemId);fflush(stdout);*/

    /* add new parser information to the list, make it current */
    if ((cp = calloc(1, sizeof(*cp))) == NULL)
        {
        XML_XPP_DEBUG("%s:%d malloc failed - %s\n", strerror(errnoGet()), 0,0,0);
        pXpp->eventType = DOC_ERROR;
        goto out;
        }
    cp->prev = pXpp->pCurParserInfo;
    pXpp->pCurParserInfo = cp;

    pXpp->_fileName = (char *) malloc(strlen(base) + strlen(systemId) + 1);
    if ((pXpp->_fileName) == NULL)
        {
        XML_XPP_DEBUG("%s:%d malloc failed - %s\n", strerror(errnoGet()), 0,0,0);
        pXpp->eventType = DOC_ERROR;
        goto out;
        }
    strcpy(pXpp->_fileName, base);
    strcat(pXpp->_fileName, systemId);

    /* obtain the path to the present file */
    pXpp->_basePath = strdup(pXpp->_fileName);
    if (pXpp->_basePath == NULL)
        {
        XML_XPP_DEBUG("%s:%d malloc failed - %s\n", strerror(errnoGet()), 0,0,0);
        pXpp->eventType = DOC_ERROR;
        goto out;
        }
    else
        {
        /* remove the file name, leaving just the path (with trailing '/') */
        char *slash = strrchr(pXpp->_basePath, '/');
        if (slash == NULL)       /* no path, just filename */
            *pXpp->_basePath = '\0';
        else
            *++slash = '\0';
        }

    /*XML_XPP_DEBUG("%s:%d my first turn.\n", 0,0,0,0);*/
    /*XML_XPP_DEBUG("%s:%d pXpp:%p  pXpp->_fileName:%s\n", pXpp, pXpp->_fileName, 0,0);*/

    pXpp->_xmlParser = XML_ExternalEntityParserCreate(parser, context, NULL);

    /* this doesn't seem to get copied into the new parser */
    if (XML_SetBase(pXpp->_xmlParser, pXpp->_basePath) == XML_STATUS_ERROR)
        {
        XML_XPP_DEBUG("%s:%d Error: XML Parser is out of memory, could not allocate memory for SetBase", 0,0,0,0);
        }

    pXpp->_filePtr = fopen(pXpp->_fileName,"r");
    if (pXpp->_filePtr == NULL)
        {
        XML_XPP_DEBUG("%s:%d Error opening external entity input file \"%s\": %s\n",
                      pXpp->_fileName, strerror(errnoGet()), 0,0);
        pXpp->eventType = DOC_ERROR;
        goto out;
        }

    do
        {
        size_t len = fread(buf, 1, sizeof(buf), pXpp->_filePtr);
        done = len < sizeof(buf);
        if (XML_Parse(pXpp->_xmlParser, buf, len, done) == 0)
            saxDisplayError(pXpp);
        } while (!done);

    /*XML_XPP_DEBUG("%s:%d %s: %s\n", pXpp->_fileName,
                  (pXpp->eventType == DOC_ERROR) ? "REJECTED" : "ACCEPTED",
                  0,0);*/
    if (pXpp->eventType == DOC_ERROR)
        {
        /*printf(pXpp->errorString);*/
        lockStep(pXpp, XML_TURN_SAX);
        }

    out:
    if (pXpp->_filePtr != NULL)
        fclose(pXpp->_filePtr);
    if (pXpp->_fileName != NULL)
        free(pXpp->_fileName);
    if (pXpp->_basePath != NULL)
        free(pXpp->_basePath);
    if (pXpp->_xmlParser != NULL)
        XML_ParserFree(pXpp->_xmlParser);
    if (cp != NULL)
        {
        pXpp->pCurParserInfo = cp->prev;
        free(cp);
        }

    return (pXpp->eventType != DOC_ERROR);
    }

/* ------------------------- utility functions ------------------------ */

static void saxCopyNameUtil
    (
    XPP *pXpp,
    const XML_Char *name
    )
    {
    saxParseNamespace(name, &pXpp->elementName);
    }

static void saxCopyAttsUtil
    (
    XPP *pXpp,
    const XML_Char **atts
    )
    {
    int i = 0;

    /* first count the attributes and allocate the array */
    for (i = 0; atts[i]; ++i)
        {
        }
    pXpp->nAttributes = i / 2;
    if (pXpp->nAttributes > pXpp->maxAttributes)
        {
        if (pXpp->attributes != NULL)
            free(pXpp->attributes);
        pXpp->attributes = calloc(pXpp->nAttributes, sizeof(struct tagAttr));
        if (pXpp->attributes == NULL)
            {
            XML_XPP_DEBUG("%s:%d malloc failed - %s\n", strerror(errnoGet()), 0,0,0);
            return;
            }
        }

    /* copy the attributes into the XPP structure */
    for (i = 0; *atts; ++i)
        {
        saxParseNamespace(*atts++, &pXpp->attributes[i].name);
        pXpp->attributes[i].value = (XML_Char *)*atts++;
        }
    }

static void saxParseNamespace
    (
    const XML_Char *input,
    struct tagName *tagName
    )
    {
    XML_Char *colon;

    tagName->qName = (XML_Char *)input;
    tagName->nameSpace = strdup(input);
    if (tagName->nameSpace == NULL)
        {
        XML_XPP_DEBUG("%s:%d malloc failed - %s\n", strerror(errnoGet()), 0,0,0);
        return;
        }

    /* find ':' in string.  This delimits namespace and local name */
    for (colon = strchr(tagName->nameSpace, ':');
         colon != NULL;
         colon = strchr(colon+1, ':'))
        {
        if (strncmp(colon + 1, "//", 2) != 0)
            {
            *colon = '\0';
            tagName->localName = colon + 1;
            return;
            }
        }

    /* no namespace found, so local is the same as qname */
    free(tagName->nameSpace);
    tagName->nameSpace = NULL;
    tagName->localName = tagName->qName;
    }


void saxParseFile_start
    (
    XPP *pXpp
    )
    {
    XML_Char buf[2048];
    int done;

    /*printf("saxParseFile_start(%s)\n", pXpp->_fileName);fflush(stdout);*/
    if (semTake(pXpp->xppSemId, WAIT_FOREVER) != OK)
        {
        XML_XPP_DEBUG("%s:%d saxParseFile_start: semTake error: %s\n", strerror(errnoGet()), 0, 0, 0);
        }
    pXpp->turn = XML_TURN_SAX;

    if ((pXpp->_filePtr = fopen(pXpp->_fileName,"r")) == NULL)
        {
        XML_XPP_DEBUG("%s:%d Error opening input file \"%s\": %s\n",
                      pXpp->_fileName,strerror(errnoGet()), 0,0);
        pXpp->eventType = DOC_ERROR;
        semGive(pXpp->xppSemId);
        return;
        }

    pXpp->_xmlParser = XML_ParserCreateNS(NULL, (XML_Char)':');
    XML_SetUserData(pXpp->_xmlParser, (void *)pXpp);

    if (XML_SetBase(pXpp->_xmlParser, pXpp->_basePath) == 0)
        {
        XML_XPP_DEBUG("%s:%d Error: XML Parser is out of memory, could not allocate memory for SetBase", 0,0,0,0);
        fclose(pXpp->_filePtr);
        pXpp->_filePtr = NULL;
        pXpp->eventType = DOC_ERROR;
        semGive(pXpp->xppSemId);
        return;
        }

    saxSetHandlersUtil(pXpp);

    pXpp->nNamespaces = 0;

    do
        {
        size_t len = fread(buf, 1, sizeof(buf), pXpp->_filePtr);
        done = len < sizeof(buf);
        if (XML_Parse(pXpp->_xmlParser, buf, len, done) == 0)
            saxDisplayError(pXpp);
        } while (!done);

    XML_ParserFree(pXpp->_xmlParser);
    pXpp->_xmlParser = NULL;

    if (pXpp->eventType != DOC_ERROR)
        {
        pXpp->eventType = END_DOC;
        }

    fclose(pXpp->_filePtr);
    pXpp->_filePtr = NULL;

    /* tell xppDestroy() not to delete us; we're going away on our own */
    pXpp->saxTaskId = 0;

    semGive(pXpp->xppSemId);
    return;
    }

void saxParseString_start
    (
    XPP *pXpp
    )
    {
    /*printf("saxParseSring_start()\n");fflush(stdout);*/
    if (semTake(pXpp->xppSemId, WAIT_FOREVER) != OK)
        {
        XML_XPP_DEBUG("%s:%d saxParseString_start: semTake error: %s\n", strerror(errnoGet()), 0, 0, 0);
        }
    pXpp->turn = XML_TURN_SAX;

    pXpp->_xmlParser = XML_ParserCreateNS(NULL, (XML_Char)':');
    XML_SetUserData(pXpp->_xmlParser, (void *)pXpp);

    if (XML_SetBase(pXpp->_xmlParser, pXpp->_basePath) == 0)
        {
        XML_XPP_DEBUG("%s:%d Error: XML Parser is out of memory, could not allocate memory for SetBase", 0,0,0,0);
        pXpp->eventType = DOC_ERROR;
        semGive(pXpp->xppSemId);
        return;
        }

    saxSetHandlersUtil(pXpp);

    pXpp->nNamespaces = 0;

    if (XML_Parse(pXpp->_xmlParser, pXpp->xmlString, strlen(pXpp->xmlString), 1) == 0)
        saxDisplayError(pXpp);

    XML_ParserFree(pXpp->_xmlParser);
    pXpp->_xmlParser = NULL;

    if (pXpp->eventType != DOC_ERROR)
        {
        pXpp->eventType = END_DOC;
        }

    /* tell xppDestroy() not to delete us; we're going away on our own */
    pXpp->saxTaskId = 0;

    semGive(pXpp->xppSemId);
    return;
    }

static void saxDisplayError
    (
    XPP *pXpp
    )
    {
    int errCode;

    errCode = XML_GetErrorCode(pXpp->_xmlParser);

    if (errCode != 0)
        {
        snprintf(pXpp->errorString, XPP_DEBUG_STRING_SIZE,
                 "File(%s) Error(%d):%s at line %d, col %d\n",
                 pXpp->_fileName,
                 errCode,
                 XML_ErrorString(errCode),
                 xppGetLineNumber(pXpp),
                 xppGetColumnNumber(pXpp));

        /*!!! this is a SIDE EFFECT that the caller depends on... */
        pXpp->eventType = DOC_ERROR;
        }
    }

static void saxSetHandlersUtil
    (
    XPP *pXpp
    )
    {
    XML_Parser p = pXpp->_xmlParser;
    XML_SetElementHandler(p, saxStartElementHandler, saxEndElementHandler);
    XML_SetEmptyElementHandler(p, saxEmptyElementHandler);
    XML_SetCharacterDataHandler(p, saxCharacterDataHandler);

    if (pXpp->extEntParseEnabled)
        {
        XML_SetExternalEntityRefHandler(p, saxExternalEntityRefHandler);
        if (XML_SetParamEntityParsing(p, XML_PARAM_ENTITY_PARSING_ALWAYS) == 0)
            {
            XML_XPP_DEBUG("%s:%d Parameter entity parsing request failed\n", 0,0,0,0);
            }
        }
    }
