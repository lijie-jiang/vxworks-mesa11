/* xpp.c - XML pull parser library */

/* Copyright 2002-2007, 2013 Wind River Systems, Inc. */

/*
modification history
--------------------
01r,09aug13,f_f  fixed some coverity issues (WIND00429907)
01q,05oct07,pas  create semaphore empty so initial lockStep gives it properly
01p,01aug07,pas  remove unneeded taskLock/Unlock in xppDestroy() for SMP
01o,05apr06,pas  fix a race condition in xppDestroy()
01n,06jul05,pas  - added support for nested external entities
                 - implemented missing xppGetAttributeQName()
                 - added xppGetErrorString(), xppGetFileName(), xppCreate()
                 - removed static isWhitespace struct flag - evaluate it if
                   and when when the user is actually interested
                 - fixed some potential race conditions and memory leaks
01m,23feb05,pas  added in-file comments from clearcase checkin comments
01l,26mar03,tky  converted all prints to debug prints, using XML_XPP_DEBUG
01k,25mar03,tky  code review changes
01j,21mar03,tky  updated source documentation, removed dead code
01i,04dec02,tky  - changed #define XPP_DEBUG to a variable for project facility
                 - added a initialization function for xppDebug variable
01h,23aug02,tky  - xppAlloc was not correctly checking for calloc fails
                 - fixed error messages
01g,25jul02,tky  fix for null saxparser access
01f,25jul02,tky  fixed memory leak
01e,23jul02,tky  fix for external entity handling and character data handling
01d,12jun02,tky  lockStep was lacking a return value, in the case it made it
                 all the way through the function
01c,10jun02,zs   Fix semephore problems
                 Included task delete hook to clean up file handles.
                 changed priority of spawned sax task.
01b,31may02,zs   Modified code for xppGetText.
                 Added taskDeleteHook for cleaning up file pointers.
                 Added more error handling.
01a,08may02,zs   Initial version
*/

/*
DESCRIPTION

This library provides routines for making stream-based
XML parsing easier by allowing the developer to iterate through
an XML document rather than respond to events as is the typical
interface provided by SAX based parsers.

It is very loosely based on the Java pull parser API described at
http://www.xmlpull.org/v1/doc/api/org/xmlpull/v1/XmlPullParser.html

Here is an example of how to use the XML pull parser:

\cs
BOOL
xmlXppParseFile1
    (
    char *pBasePath,
    char *pFileName
    )
    {
    XPP_EVENT_T eventType;
    XPP *pXpp = NULL;
    FILE *parseFile;
    char buf[MAX_BUFFER_SIZE];

    strcpy(buf, pBasePath);
    strcat(buf, pFileName);

 ********************************************
 *    Initializing the pXpp data structure  *
 ********************************************
    xppAlloc(&pXpp);
    xppInit(pXpp);

    printf("Initializing Pull Parser with file %s\n", buf);

    xppSetFileInput(pXpp, buf);
    eventType = xppGetEventType(pXpp);

 *************************************************************************
 *    Parse the XML document until the end of the document is reached    *
 *************************************************************************
    while (eventType != END_DOC)
        {

 *************************************************
 *    The start of the element is encountered    *
 *************************************************
        if (eventType == START_TAG)
            printf("found start tag: %s\n", xppGetQName(pXpp));

 ***********************************************
 *    The end of the element is encountered    *
 ***********************************************
        else if (eventType == END_TAG)
            printf("found end tag: %s\n", xppGetQName(pXpp));

 *************************************************************
 *    The text of the element or attribute is encountered    *
 *************************************************************
        else if (eventType == TEXT)
            {
            if (!xppIsWhitespace(pXpp))
                {
                XML_Char * text;
                int len = xppGetTextLength(pXpp);
                text = (XML_Char *)malloc(len * sizeof(XML_Char));
                if (text != NULL)
                    {
                    if (xppGetText(pXpp, text, len) == OK)
                        printf("found text: %s\n", text);
                    else
                        fprintf(stderr, "Error getting text: %s\n", xppGetErrorString(pXpp));
                    free (text);
                    }
                else
                    {
                    printf("found %d bytes of whitespace text\n", xppGetTextLength(pXpp));
                    }

                }
            }

        eventType = xppNext(pXpp);
        }

    *** End of Document Reached ***
    xppDestroy(pXpp);
    }
\ce

INCLUDE FILES: webservices/xml/xpp/xpp.h

*/


/* includes */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <semLib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include "webservices/xml/expat.h"
#include "webservices/xml/xpp/xpp.h"
#include "xpp_internal.h"

/* forward declarations */

LOCAL STATUS xppStartInputTask(XPP *pXpp);
LOCAL BOOL xppEventIsStartTag(XPP *pXpp);
LOCAL BOOL xppEventIsEndTag(XPP *pXpp);
LOCAL BOOL xppIndexIsOutOfBounds(XPP *pXpp, int index);


/***************************************************************************
*
* xppParamInit - sets up configuration parameters used in the Tornado project facility
*
* This routine sets up configuration parameters used in the Tornado project
* facility. Currently it is unused, but it is typically used by the
* Tornado project facility for component setup.  The routine is included
* to ensure that the project facility inlcudes the xpp object into the
* image.  If there are no calls to the routines within the object, the
* object is optimized out automatically.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
void xppParamInit()
    {
    }


/***************************************************************************
*
* xppCreate - create and initialize a new XML Pull Parser
*
* This routine is a convenience wrapper around xppAlloc() and xppInit(), to
* provide greater consistency with the vxWorks model of taskCreate(),
* semBCreate(), etc.
*
* RETURNS: pointer to the XPP data structure, or NULL if it could not be
* created
*
* ERRNO: N/A
*/
XPP *xppCreate
    (
    void
    )
    {
    XPP *pXpp;

    if ((xppAlloc(&pXpp) == OK) && (xppInit(pXpp) == OK))
        {
        return pXpp;
        }
    else
        {
        xppDestroy(pXpp);
        return NULL;
        }
    }


/***************************************************************************
*
* xppAlloc - allocate memory for the XML Pull Parser
*
* This routine allocates and zeroes memory for the XPP data structure.
*
* RETURNS: OK if the memory was successfully allocated, ERROR otherwise.
*
* ERRNO: N/A
*/
STATUS xppAlloc
    (
    XPP **pXpp /* pointer to the XPP object to allocate for */
    )
    {
    /*printf("xppAlloc(%p)\n", pXpp);fflush(stdout);*/
    if (pXpp == NULL)
        return ERROR;

    /* calloc initializes allocated contents to 0 */
    if ((*pXpp = (XPP *)calloc(1, sizeof(XPP))) == NULL)
        {
        XML_XPP_DEBUG("%s:%d XPP object could not be allocated: %s\n",
                strerror(errnoGet()), 0, 0, 0);
        return ERROR;
        }

    return OK;
    }


/***************************************************************************
*
* xppInit - initialize the XML Pull Parser
*
* This routine initializes the XPP structure. It must be called before the
* parser is used.
*
* RETURNS: OK if the structure is successfully initialized, ERROR otherwise.
*
* ERRNO: N/A
*/
STATUS xppInit
    (
    XPP *pXpp /* pointer to XPP object to initialize */
    )
    {
    /*printf("xppInit(%p)\n", pXpp);fflush(stdout);*/
    if (pXpp == NULL)
        {
        return ERROR;
        }

    pXpp->pCurParserInfo = &pXpp->curParserInfo;

    pXpp->xppSemId = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if (pXpp->xppSemId == NULL)
        {
        XML_XPP_DEBUG("%s:%d Error creating binary semephore!\n", 0, 0, 0, 0);
        pXpp->errorCode = XML_ERR_SEM;
        strncpy(pXpp->errorString, "Error creating binary semephore",
                XPP_DEBUG_STRING_SIZE);
        return ERROR;
        }

    pXpp->turn = XML_TURN_XPP;
    return OK;
    }


/***************************************************************************
*
* xppSetFileInput - set the input file, and start parsing
*
* This routine sets the input file for the XPP parser, and
* immediately starts a separate task to parse to the first event.
*
* RETURNS: OK if the underlying SAX parser is started, ERROR otherwise.
*
* ERRNO: N/A
*/
STATUS xppSetFileInput
    (
    XPP *pXpp, /* pointer to XPP object being used for parsing */
    char *fileName /* name of file to parse */
    )
    {
    /*printf("xppSetFileInput(%p, %s)\n", pXpp, fileName);fflush(stdout);*/
    if (pXpp == NULL)
        {
        return ERROR;
        }

    pXpp->depth = 0;

    pXpp->_fileName = strdup(fileName);
    if ((pXpp->_fileName) == NULL)
        {
        XML_XPP_DEBUG("%s:%d malloc failed - %s\n", strerror(errnoGet()), 0,0,0);
        return ERROR;
        }

    return xppStartInputTask(pXpp);
    }


/***************************************************************************
*
* xppSetStringInput - set the input string, and start parsing
*
* This routine sets the input string for the XPP parser, and
* immediately starts a separate task to parse to the first event.
*
* RETURNS: OK if the underlying SAX parser is started, ERROR otherwise.
*
* ERRNO: N/A
*/
STATUS xppSetStringInput
    (
    XPP *pXpp, /* pointer to XPP object to parse with */
    const XML_Char *xmlString /* pointer to string to parse */
    )
    {
    /*printf("xppSetStringInput(%p, %p)\n", pXpp, xmlString);fflush(stdout);*/
    if (pXpp == NULL)
        {
        return ERROR;
        }

    /* These fields should still be zeroed out from xppAlloc(), but we set
     * them here just in case the user decides to re-use the parser struct.
     */
    pXpp->depth = 0;
    pXpp->_fileName = NULL;
    pXpp->xmlString = xmlString;

    return xppStartInputTask(pXpp);
    }


/***************************************************************************
*
* xppDestroy - destroy an XML Pull Parser
*
* This routine kills the helper task, if necessary, closes any open files,
* and frees memory allocated by the XPP structure.  The XPP pointer should
* not be used after this, as it will point to free memory on the heap.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS xppDestroy
    (
    XPP *pXpp /* pointer to XPP object to recover resources from */
    )
    {
    int i;

    /*printf("xppDestroy(%p)\n", pXpp);fflush(stdout);*/
    if (pXpp == NULL)
        return OK;

    if (pXpp->saxTaskId != 0 && taskIdVerify(pXpp->saxTaskId) != ERROR)
        {
        taskDelete(pXpp->saxTaskId);
        }

    /* shut down external entity parsers, and the main parser */
    for (;;)
        {
        if (pXpp->_xmlParser != NULL)
            XML_ParserFree(pXpp->_xmlParser);
        if (pXpp->_filePtr != NULL)
            fclose(pXpp->_filePtr);
        if (pXpp->_fileName != NULL)
            free(pXpp->_fileName);
        if (pXpp->_basePath != NULL)
            free(pXpp->_basePath);

        if (pXpp->pCurParserInfo != &pXpp->curParserInfo)
            {
            struct _curParserInfo *cp = pXpp->pCurParserInfo;
            pXpp->pCurParserInfo = pXpp->pCurParserInfo->prev;
            free(cp);
            }
        else
            {
            break;
            }
        }

    /* free any allocated strings */
    if (pXpp->elementName.nameSpace != NULL)
        free(pXpp->elementName.nameSpace);
    if (pXpp->attributes != NULL)
        {
        for (i = 0; i < pXpp->maxAttributes; ++i)
            {
            if (pXpp->attributes[i].name.nameSpace != NULL)
                free(pXpp->attributes[i].name.nameSpace);
            }
        free(pXpp->attributes);
        }

    semDelete(pXpp->xppSemId);
    free(pXpp);

    return OK;
    }


/***************************************************************************
*
* xppSetExtEntBaseDir - set the base directory for parsing external entities
*
* This routine sets the base directory for parsing external entities.
* This is usually the same directory as the file given in xppSetFileInput().
*
* RETURNS: OK on success, ERROR on failure.
*
* ERRNO: N/A
*/
STATUS xppSetExtEntBaseDir
    (
    XPP *pXpp, /* pointer to XPP object to configure */
    char *baseDir /* base path */
    )
    {
    /*printf("xppSetExtEntBaseDir(%p, %s)\n", pXpp, baseDir);fflush(stdout);*/
    if (pXpp == NULL)
        {
        return ERROR;
        }

    if (pXpp->_basePath != NULL)
        {
        free((void *)pXpp->_basePath);
        pXpp->_basePath = NULL;
        }

    pXpp->_basePath = strdup(baseDir);
    if ((pXpp->_basePath) == NULL)
        {
        XML_XPP_DEBUG("%s:%d calloc failed - %s\n", strerror(errnoGet()),
                         0, 0, 0);
        pXpp->errorCode = XML_ERR_MEM;
        strncpy(pXpp->errorString, "calloc failed", XPP_DEBUG_STRING_SIZE);
        return ERROR;
        }

    if (pXpp->_xmlParser != NULL)
        {
        if (XML_SetBase(pXpp->_xmlParser, pXpp->_basePath) == 0)
            {
            XML_XPP_DEBUG("%s:%d Error: XML Parser is out of memory, could not allocate memory for SetBase",
                            0, 0, 0, 0);
            return ERROR;
            }
        }

    return OK;
    }


/***************************************************************************
*
* xppSetExtEntParseEnable - enable the external entity parsing feature
*
* This routine enables the XML Pull Parser to parse external entities.
* It must be called before the parser is started with xppSetFileInput() or
* xppSetStringInput().
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void xppSetExtEntParseEnable
    (
    XPP *pXpp /* pointer to XPP object to configure */
    )
    {
    pXpp->extEntParseEnabled = TRUE;
    }


/***************************************************************************
*
* xppGetEventType - get the current XPP event type
*
* This routine gets the current event type, which will be one of the following:
*
* \is
* \i START_TAG
* A starting element tag has been encountered, and all element
* data including its attributes are available (eg. \<tag attr="value"\>)
* Note: This event type will also occur for empty tags, but will not
* be followed by an END_TAG event (eg. \<tag attr="value"/\>).
*
* \i END_TAG
* An ending element tag has been encountered, and all element
* data is available (eg. \</tag\>).
*
* \i TEXT
* Text contained within a tag has been encountered.
* (eg. \<tag\>text_found\</tag\>).
*
* \i END_DOC
* The end of the document has been reached.
*
* \i DOC_ERROR
* A serious error has occurred.
* \ie
*
* RETURNS: XPP_EVENT_T - the type of event
*
* ERRNO: N/A
*/
XPP_EVENT_T xppGetEventType
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    /*printf("xppGetEventType(%p)\n", pXpp);fflush(stdout);*/
    return pXpp->eventType;
    }


/***************************************************************************
*
* xppNext - get the next XPP event type
*
* This routine signals the underlying SAX parser to parse to the next event.
*
* RETURNS: XPP_EVENT_T - the type of event (see xppGetEventType())
*
* ERRNO: N/A
*/
XPP_EVENT_T xppNext
    (
    XPP *pXpp  /* pointer to XPP object */
    )
    {
    /*printf("xppNext(%p)\n", pXpp);fflush(stdout);*/
    if (lockStep(pXpp, XML_TURN_XPP) != OK)
        {
        /* errorCode and errorString are already set in lockStep */
        return DOC_ERROR;
        }
    return pXpp->eventType;
    }


/***************************************************************************
*
* xppGetName - get the local name of the current start/end tag
*
* This routine gets the local (non-namespace-qualified) name of the current
* start/end tag.
*
* RETURNS: name, or NULL if the current event is not START_TAG or END_TAG.
*
* ERRNO: N/A
*/
XML_Char *xppGetName
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    /*printf("xppGetName(%p)\n", pXpp);fflush(stdout);*/
    if (!xppEventIsStartTag(pXpp) && !xppEventIsEndTag(pXpp))
        {
        return NULL;
        }

    return pXpp->elementName.localName;
    }


/***************************************************************************
*
* xppGetNamespace - get the namespace of the current start/end tag
*
* This routine gets the namespace URI of the current start/end tag.
*
* RETURNS: namespace, or NULL if the current event is not START_TAG or END_TAG.
*
* ERRNO: N/A
*/
XML_Char *xppGetNamespace
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    if (!xppEventIsStartTag(pXpp) && !xppEventIsEndTag(pXpp))
        {
        return NULL;
        }

    return pXpp->elementName.nameSpace;
    }

/***************************************************************************
*
* xppGetQName - get the namespace qualified name for the current start/end tag
*
* This routine gets the namespace qualified name for the current start/end
* tag.
*
* RETURNS: name, or NULL if the current event is not START_TAG or END_TAG.
*
* ERRNO: N/A
*/
XML_Char *xppGetQName
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    if (!xppEventIsStartTag(pXpp) && !xppEventIsEndTag(pXpp))
        {
        return NULL;
        }

    return pXpp->elementName.qName;
    }

/***************************************************************************
*
* xppLookupNamespace - get the namespace for the specified prefix
*
* This routine gets the namespace URI for the specified prefix.
*
* RETURNS: namespace, or NULL if there is
* no namespace associated with the specified prefix.
*
* ERRNO: N/A
*/
XML_Char *xppLookupNamespace
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    XML_Char *prefix /* prefix to lookup namespace of */
    )
    {
    int i;
    for (i = 0; i < pXpp->nNamespaces; i++)
        {
        if (strcmp(prefix, pXpp->nsPrefixes[i]) == 0)
            {
            return pXpp->nsUris[i];
            }
        }
    return NULL;
    }


/***************************************************************************
*
* xppGetNamespaceCount - get the number of namespaces declared
*
* This routine gets the number of namespaces declared in the current document.
*
* RETURNS: number of namespaces declared
*
* ERRNO: N/A
*/
int xppGetNamespaceCount
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    return pXpp->nNamespaces;
    }


/***************************************************************************
*
* xppGetPrefix - get the prefix of the current start/end tag
*
* This routine gets the prefix of the current start/end tag.
*
* RETURNS: prefix, or NULL if the current event is not START_TAG or END_TAG.
*
* ERRNO: N/A
*/
XML_Char *xppGetPrefix
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    int i;
    XML_Char *nameSpace = pXpp->elementName.nameSpace;
    for (i = 0; i < pXpp->nNamespaces; i++)
        {
        if (strcmp(nameSpace, pXpp->nsUris[i]) == 0)
            {
            return pXpp->nsPrefixes[i];
            }
        }
    return NULL;
    }

/***************************************************************************
*
* xppGetAttributeCount - get the number of attributes found in the current start tag
*
* This routine gets the number of attributes available from the current
* start tag.
*
* RETURNS: number of attributes, or -1 if the current event is not START_TAG.
*
* ERRNO: N/A
*/
int xppGetAttributeCount
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    if (!xppEventIsStartTag(pXpp))
        {
        return -1;
        }

    return pXpp->nAttributes;
    }


/***************************************************************************
*
* xppGetAttributeQName - get the namespace qualified name of the attribute at the specified index
*
* This routine gets the namespace qualified name of the attribute at the
* specified index.
* <index> must be in the range: [0 ... xppGetAttributeCount() - 1]
*
* RETURNS: name, or NULL if the current event is not START_TAG
* or the index is out of range
*
* ERRNO: N/A
*/
XML_Char *xppGetAttributeQName
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    int index /* index of attribute name to retrieve */
    )
    {
    if (!xppEventIsStartTag(pXpp) || xppIndexIsOutOfBounds(pXpp, index))
        {
        return NULL;
        }

    return pXpp->attributes[index].name.qName;
    }


/***************************************************************************
*
* xppGetAttributeName - get the local name of the attribute at the specified index
*
* This routine gets the local (non-namespace-qualified) name of the attribute
* at the specified index.
* <index> must be in the range: [0 ... xppGetAttributeCount() - 1]
*
* RETURNS: name, or NULL if the current event is not START_TAG
* or the index is out of range.
*
* ERRNO: N/A
*/
XML_Char *xppGetAttributeName
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    int index /* index of attribute name to retrieve */
    )
    {
    if (!xppEventIsStartTag(pXpp) || xppIndexIsOutOfBounds(pXpp, index))
        {
        return NULL;
        }

    return pXpp->attributes[index].name.localName;
    }


/***************************************************************************
*
* xppGetAttributeNamespace - get the namespace of the attribute at the specified index
*
* This routine gets the namespace URI of the attribute at the specified
* index.
* <index> must be in the range: [0 ... xppGetAttributeCount() - 1]
*
* RETURNS: namespace, or NULL if the current event is not START_TAG
* or the index is out of range.
*
* ERRNO: N/A
*/
XML_Char *xppGetAttributeNamespace
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    int index /* index of attribute to retrieve */
    )
    {
    if (!xppEventIsStartTag(pXpp) || xppIndexIsOutOfBounds(pXpp, index))
        {
        return NULL;
        }

    return pXpp->attributes[index].name.nameSpace;
    }

/***************************************************************************
*
* xppGetAttributePrefix - get the prefix of the attribute at the specified index
*
* This routine gets the prefix of the attribute at the specified index.
* <index> must be in the range: [0 ... xppGetAttributeCount() -1]
*
* RETURNS: prefix, or NULL if the current event is not START_TAG
* or the index is out of range.
*
* ERRNO: N/A
*/
XML_Char *xppGetAttributePrefix
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    int index /* index of attribute to retrieve */
    )
    {
    int i;
    XML_Char *nameSpace = pXpp->attributes[index].name.nameSpace;

    if (!xppEventIsStartTag(pXpp) || xppIndexIsOutOfBounds(pXpp, index))
        {
        return NULL;
        }

    for (i = 0; i < pXpp->nNamespaces; i++)
        {
        if (strcmp(nameSpace, pXpp->nsUris[i]) == 0)
            {
            return pXpp->nsPrefixes[i];
            }
        }
    return NULL;
    }

/***************************************************************************
*
* xppGetAttributeValue - get the value of the attribute at the specified index
*
* This routine gets the value of the attribute at the specified index.
* <index> must be in the range: [0 ... xppGetAttributeCount() - 1]
*
* RETURNS: value string, or NULL if the current event is not START_TAG
* or the index is out of range.
*
* ERRNO: N/A
*/
XML_Char *xppGetAttributeValue
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    int index /* index of attribute value to retrieve */
    )
    {
    if (!xppEventIsStartTag(pXpp) || xppIndexIsOutOfBounds(pXpp, index))
        {
        return NULL;
        }

    return pXpp->attributes[index].value;
    }


/***************************************************************************
*
* xppGetAttributeValueByName - get the value of the attribute for the specified namespace and name
*
* This routine gets the value of the attribute for the specified namespace
* and name.
*
* RETURNS: value string, or NULL if the attribute is not
* found or an error occurs.
*
* ERRNO: N/A
*/
XML_Char *xppGetAttributeValueByName
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    XML_Char *nameSpace, /* namespace of the attribute to retrieve */
    XML_Char *name /* name of attribute to retrieve */
    )
    {
    int i;
    char *pNamespace, *pName;

    if (!xppEventIsStartTag(pXpp))
        {
        return NULL;
        }

    for (i = 0; i < pXpp->nAttributes; i++)
        {
        pNamespace = xppGetAttributeNamespace(pXpp, i);
        if (pNamespace != NULL && strcmp(nameSpace, pNamespace) == 0)
            {
            pName = xppGetAttributeName(pXpp, i);
            if (pName != NULL && strcmp(name, pName) == 0)
                {
                return pXpp->attributes[i].value;
                }
            }
        }
    /* not found */
    pXpp->errorCode = XML_ERR_NOT_FOUND;
    pXpp->errorString[0] = '\0';
    return NULL;
    }


/***************************************************************************
*
* xppGetTextLength - gets the length of the text available
*
* This routine gets the length of the text available.
*
* This is typically called before calling xppGetText() to allocate a buffer
* large enough to store the text.  The value returned includes an extra
* byte for the terminating '/0'.
*
* RETURNS: The number of bytes of text available, or -1 if the current event
* is not TEXT.
*
* ERRNO: N/A
*/
int xppGetTextLength
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    if (pXpp->eventType != TEXT)
        {
        XML_XPP_DEBUG("%s:%d xppGetTextLength: The current eventType must be TEXT.\n",
                         0, 0, 0, 0);
        pXpp->errorCode = XML_ERR_XPP;
        strncpy(pXpp->errorString,
                "INVALID STATE: current eventType must be TEXT",
                XPP_DEBUG_STRING_SIZE);
        return -1;
        }
    return pXpp->characterDataLen + 1;
    }


/***************************************************************************
*
* xppGetText - get the currently available text
*
* This routine copies the currently available text into the buffer. Memory
* must already be allocated to the buffer.  The number of bytes that will
* be written to the buffer is xppGetTextLength() + 1.
*
* RETURNS: OK on success, ERROR otherwise.
*
* ERRNO: N/A
*/
STATUS xppGetText
    (
    XPP *pXpp, /* pointer to XPP object to retrieve from */
    XML_Char *pBuffer, /* buffer to return text into */
    int len /* length of pBuffer */
    )
    {
    /*printf("xppGetText(%p, %p, %d)\n", pXpp, pBuffer, len);fflush(stdout);*/
    if (pXpp->eventType != TEXT)
        {
        XML_XPP_DEBUG("%s:%d xppGetText: The current eventType must be TEXT.\n",
                        0, 0, 0, 0);
        pXpp->errorCode = XML_ERR_XPP;
        strncpy(pXpp->errorString,
                "INVALID STATE: current eventType must be TEXT",
                XPP_DEBUG_STRING_SIZE);
        return ERROR;
        }
    if (pBuffer == NULL)
        {
        pXpp->errorCode = XML_ERR_MEM;
        strncpy(pXpp->errorString, "memory must be allocated to buffer",
                XPP_DEBUG_STRING_SIZE);
        return ERROR;
        }
    if (pXpp->characterDataLen > len - 1)
        {
        pXpp->errorCode = XML_ERR_MEM;
        strncpy(pXpp->errorString, "insufficient text buffer size",
                XPP_DEBUG_STRING_SIZE);
        return ERROR;
        }

    strncpy(pBuffer, pXpp->characterData, pXpp->characterDataLen);
    pBuffer[(pXpp->characterDataLen)] = '\0';

    return OK;
    }


/***************************************************************************
*
* xppIsWhitespace - determine whether the current TEXT node contains only whitespace
*
* This routine indicates whether the current TEXT node contains only
* whitespace.
*
* RETURNS: TRUE if the current TEXT node contains only whitespace or the
* current event type is not TEXT.  Otherwise FALSE is returned.
*
* ERRNO: N/A
*/
BOOL xppIsWhitespace
    (
    XPP *pXpp /* pointer to XPP object to analyze */
    )
    {
    return
      (pXpp->eventType != TEXT) ||
      XML_IsWhiteSpace(pXpp->characterData, pXpp->characterDataLen);
    }


/***************************************************************************
*
* xppIsEmptyElementTag - determine whether the current start tag is empty
*
* This routine indicates whether the current start tag is empty or not.
* An empty tag looks like: \<tag/\>
*
* RETURNS: TRUE if the current start tag is empty, FALSE otherwise.
*
* ERRNO: N/A
*/
BOOL xppIsEmptyElementTag
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    return pXpp->isEmptyElementTag;
    }


/***************************************************************************
*
* xppGetColumnNumber - get the current column position within the document being parsed
*
* This routine gets the current column position of the underlying SAX parser.
*
* RETURNS: current column position
*
* ERRNO: N/A
*/
int xppGetColumnNumber
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    return XML_GetCurrentColumnNumber(pXpp->_xmlParser);
    }


/***************************************************************************
*
* xppGetLineNumber - get the current line position within the document being parsed
*
* This routine gets the current line number of the underlying SAX parser.
*
* RETURNS: current line number
*
* ERRNO: N/A
*/
int xppGetLineNumber
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    return XML_GetCurrentLineNumber(pXpp->_xmlParser);
    }


/***************************************************************************
*
* xppGetDepth - get the current nesting depth
*
* This routine gets the current nesting depth.  The nesting depth of the
* first element is 0 and is increased by one for each level.
*
* RETURNS: current nesting depth
*
* ERRNO: N/A
*/
int xppGetDepth
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    return pXpp->depth;
    }


/***************************************************************************
*
* lockStep - blocks waiting for the next event to become available from the
* underlying SAX parser
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
STATUS lockStep
    (
    XPP *pXpp,
    int myTurn
    )
    {
    STATUS s;

    /*printf("lockStep(%p, %d)\n", pXpp, myTurn);fflush(stdout);*/
    if (pXpp->turn != myTurn)
        {
        XML_XPP_DEBUG("%s:%d lockStep (%s): not my turn\n",
                      myTurn == XML_TURN_XPP ? "xpp" : "sax", 0, 0, 0);
        /* we're probably hopelessly screwed, but go on and attempt to recover */
        }

    s = semGive(pXpp->xppSemId);
    if (s != OK)
        {
        snprintf(pXpp->errorString, XPP_DEBUG_STRING_SIZE,
                 "semGive error: %s\n", strerror(errnoGet()));
        XML_XPP_DEBUG("%s:%d lockStep: %s\n", pXpp->errorString, 0, 0, 0);
        pXpp->errorCode = XML_ERR_SEM;
        return ERROR;
        }

    while (pXpp->turn == myTurn)
        {
        taskDelay(0);           /* don't care if interrupted */
        }

    s = semTake(pXpp->xppSemId, WAIT_FOREVER);
    if (s != OK)
        {
        snprintf(pXpp->errorString, XPP_DEBUG_STRING_SIZE,
                 "semTake error: %s\n", strerror(errnoGet()));
        XML_XPP_DEBUG("%s:%d lockStep: %s\n", pXpp->errorString, 0, 0, 0);
        pXpp->errorCode = XML_ERR_TIMEOUT;
        return ERROR;
        }

    pXpp->turn = myTurn;

    return s;
    }

/***************************************************************************
*
* xppStartInputTask - starts a separate task to parse to the first event
*
* This routine starts a new task to begin the SAX parser parsing to the
* first event.
*
* RETURNS: OK, otherwise ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*/
LOCAL STATUS xppStartInputTask
    (
    XPP *pXpp /* pointer to XPP object to retrieve from */
    )
    {
    char *taskName;
    FUNCPTR taskEntry;
    STATUS status;
    int priority = 0;
    int taskId;

    /*printf("xppStartInputTask(%p)\n", pXpp);fflush(stdout);*/

    /* _fileName is set in xppSetFileInput, null in xppSetStringInput */
    if (pXpp->_fileName != NULL)
        {
        taskName = "tXppSaxFile";
        taskEntry = (FUNCPTR)saxParseFile_start;
        }
    else
        {
        taskName = "tXppSaxString";
        taskEntry = (FUNCPTR)saxParseString_start;
        }

    taskId = taskIdSelf();
    status = taskPriorityGet(taskId, &priority);
    if (status != OK)
        {
        XML_XPP_DEBUG("%s:%d Unable to get the priority of task %d needed to spawn %s.\n",
            taskId, taskName, 0, 0);
        pXpp->errorCode = XML_ERR_TASK;
        strncpy(pXpp->errorString, "Unable to get the priority of"
                " current task.\n", XPP_DEBUG_STRING_SIZE);
        return status;
        }

    pXpp->saxTaskId = taskSpawn(taskName, priority, 0, 20*1024,
                                taskEntry, (int)pXpp,
                                0,0,0,0,0,0,0,0,0);
    if (pXpp->saxTaskId == ERROR)
        {
        /* XML_XPP_DEBUG("%s:%d Error creating tXppSaxFile Task\n", 0,0,0,0); */
        perror("Error creating SAX input Task");
        pXpp->errorCode = XML_ERR_TASK;
        strncpy(pXpp->errorString, "Unable to create SAX input Task",
                XPP_DEBUG_STRING_SIZE);
        return ERROR;
        }

    /*XML_XPP_DEBUG("%s:%d xppSetFileInput: lockStep.\n", 0,0,0,0);*/
    if (lockStep(pXpp, XML_TURN_XPP) != OK)
        {
        /* errorCode and errorString are already set in lockStep */
        return ERROR;
        }

    return OK;

    }

/***************************************************************************
*
* xppEventIsEndTag - indicates if most recent XPP event is an end tag
*
* This routine indicates if most recent XPP event is an end tag.
*
* RETURNS: TRUE if pXpp->evenType is END_TAG, otherwise FALSE
*
* ERRNO: N/A
*
* NOMANUAL
*/
LOCAL BOOL xppEventIsEndTag
    (
    XPP *pXpp  /* pointer to XPP object */
    )
    {
    return (pXpp->eventType == END_TAG);
    }

/***************************************************************************
*
* xppEventIsStartTag - indicates if most recent XPP event is a start tag
*
* This routine indicates if most recent XPP event is a start tag.
*
* RETURNS: TRUE if pXpp->evenType is START_TAG, otherwise FALSE
*
* ERRNO: N/A
*
* NOMANUAL
*/
LOCAL BOOL xppEventIsStartTag
    (
    XPP *pXpp /* pointer to XPP object */
    )
    {
    return (pXpp->eventType == START_TAG);
    }

/***************************************************************************
*
* xppIndexIsOutOfBounds - indicates if <index> is within the bounds of the
* Attributes array
*
* This routine indicates if <index> is within the bounds of the Attributes
* array.
*
* RETURNS: TRUE if within bounds, otherwise FALSE
*
* ERRNO: N/A
*
* NOMANUAL
*/
LOCAL BOOL xppIndexIsOutOfBounds
    (
    XPP *pXpp, /* pointer to XPP object */
    int index /* index to evaluate */
    )
    {
    return ((index < 0) || (index >= pXpp->nAttributes));
    }


/***************************************************************************
*
* xppGetErrorString - get the current XPP error string
*
* This routine gets the current error string.  This is only valid after an
* error has been reported with a DOC_ERROR event.
*
* RETURNS: pointer to error string
*
* ERRNO: N/A
*/
char *xppGetErrorString
    (
    XPP *pXpp
    )
    {
    return pXpp->errorString;
    }


/***************************************************************************
*
* xppGetFileName - get the current XPP file name
*
* This routine gets the current parse file name.  This could be different from
* the name given to xppSetFileInput(), if the parser has opened an external
* entity.
*
* RETURNS: file name string
*
* ERRNO: N/A
*/
char *xppGetFileName
    (
    XPP *pXpp
    )
    {
    return pXpp->_fileName;
    }
