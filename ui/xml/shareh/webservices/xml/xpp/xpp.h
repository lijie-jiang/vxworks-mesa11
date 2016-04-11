/* xpp.h - XML pull parser library */

/* Copyright 2002-2005 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,06jul05,pas  - split internal data and functions into a separate file
                 - added xppGetErrorString(), xppGetFileName(), xppCreate()
01b,13may02,zs   Updated documentation.
01a,08apr02,zs   Written.
*/

/*
  DESCRIPTION
  
  see xpp.c
  
  INCLUDE FILES: taskLib.h semLib.h expat.h
*/

#ifndef __INCxpph
#define __INCxpph

/* includes */

#include "webservices/xml/expat.h"

#ifdef __cplusplus
extern "C" {
#endif

/* XML namespace example:
<x xmlns:edi='http://ecommerce.org/schema'>
  <!-- the "edi" prefix is bound to http://ecommerce.org/schema
       for the "x" element and contents -->
</x>

where   edi = xml namespace prefix
        http://ecommerce.org/schema = xml namespace URI

Example 2:        
<?xml version="1.0"?>
<!-- all elements here are explicitly in the HTML namespace -->
<html:html xmlns:html='http://www.w3.org/TR/REC-html40'>
  <html:head><html:title>Frobnostication</html:title></html:head>
  <html:body><html:p>Moved to 
    <html:a href='http://frob.com'>here.</html:a></html:p></html:body>
</html:html> 
        
where html:head = fully qualified name, also applicable to attributes attached to namespaces
*/

/* XPP object error type indicators */
#define XML_ERR_SEM             100 /* semaphore related error */
#define XML_ERR_MEM             101 /* memory related error */
#define XML_ERR_TASK            102 /* task related error */
#define XML_ERR_XPP             103
#define XML_ERR_ARRAY           104
#define XML_ERR_NOT_FOUND       105
#define XML_ERR_TIMEOUT         106

/* typedefs */

typedef enum {START_TAG, TEXT, END_TAG, END_DOC, DOC_ERROR=-1} XPP_EVENT_T;

struct XPP;
typedef struct XPP XPP, *PXPP;

/* XPP Initialization Routines */
void            xppParamInit();
XPP *           xppCreate(void);
STATUS          xppAlloc(XPP **pXpp);
STATUS          xppInit(XPP * const pXpp);
STATUS          xppDestroy(XPP *pXpp);
STATUS          xppSetFileInput(XPP *pXpp, char *fileName);
STATUS          xppSetExtEntBaseDir(XPP *pXpp, char *baseDir);
void            xppSetExtEntParseEnable(XPP *pXpp);
STATUS          xppSetStringInput(XPP *pXpp, const XML_Char *xmlString);

/* XPP Iteration Routines */
XPP_EVENT_T     xppGetEventType(XPP *pXpp);
XPP_EVENT_T     xppNext(XPP *pXpp);

/* Element Routines */
BOOL            xppIsEmptyElementTag(XPP *pXpp);
BOOL            xppIsWhitespace(XPP *pXpp);
XML_Char *      xppGetQName(XPP *pXpp);
XML_Char *      xppGetName(XPP *pXpp);
XML_Char *      xppGetNamespace(XPP *pXpp);
XML_Char *      xppGetPrefix(XPP *pXpp);

/* Attribute Routines */
int             xppGetAttributeCount(XPP *pXpp);
XML_Char *      xppGetAttributeQName(XPP *pXpp, int index);
XML_Char *      xppGetAttributeName(XPP *pXpp, int index);
XML_Char *      xppGetAttributeValue(XPP *pXpp, int index);
XML_Char *      xppGetAttributeValueByName(XPP *pXpp, XML_Char *nameSpace, XML_Char *name);
XML_Char *      xppGetAttributeNamespace(XPP *pXpp, int index);
XML_Char *      xppGetAttributePrefix(XPP *pXpp, int index);

/* Parser Position Routines */
int             xppGetColumnNumber(XPP *pXpp);
int             xppGetDepth(XPP *pXpp);
int             xppGetLineNumber(XPP *pXpp);

/* Namespace Routines */
XML_Char *      xppLookupNamespace(XPP *pXpp, XML_Char *prefix);
int             xppGetNamespaceCount(XPP *pXpp);

/* Character Data Routines */
int             xppGetTextLength(XPP *pXpp);
STATUS          xppGetText(XPP *pXpp, XML_Char *buffer, int len);

/* Error Reporting Routines */
char *          xppGetErrorString(XPP *pXpp);
char *          xppGetFileName(XPP *pXpp);

#ifdef __cplusplus
}
#endif

#endif /* __INCxpph */

