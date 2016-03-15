/* xmlop.h - XML output header file */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03nov03,tky 
01a,11dec2002,ssk   created.
*/

/*
DESCRIPTION

Function prototypes for the XML output module
*/



#ifndef __INCxmloph
#define __INCxmloph

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include <stdio.h>
#include "webservices/xml/expat.h"
#include "webservices/xml/xmlCanon/xmltchar.h"

/* errno status codes */

#define M_xmlop 851 << 16

#define S_xmlop_NSSTACK_OVERFLOW (M_xmlop | 1) /* Namespace stack overflow, the number of namespaces has exceeded the maximum namespace stack size */
#define S_xmlop_NSAXIS_OVERFLOW  (M_xmlop | 2) /* Namespace axis overflow, the number of namespace axes has exceeded the maximum size of the namespace axis stack */
#define S_xmlop_NAMEBUF_OVERFLOW (M_xmlop | 3) /* Namespace, or URI has exceeded XMLOP_NS_LENGTH, XMLOP_URI_LENGTH respectively*/
#define S_xmlop_CDATA_ACTIVE     (M_xmlop | 4) /* a CDATA section is already active, and the user is attempting to start another CDATA section */
#define S_xmlop_NO_CDATA_ACTIVE  (M_xmlop | 5) /* a CDATA section is not already active and the user is attempting to end a CDATA section */
#define S_xmlop_ILLEGAL_STRING   (M_xmlop | 6) /* an illegal string combination has been embedded into XML related text */
#define S_xmlop_MALLOC_FAILED    (M_xmlop | 7)
#define S_xmlop_BUF_CONCAT       (M_xmlop | 8) /* an internal buffer copy has exceeded XMLCANON_NOTATIONNAME_SIZE, XMLOP_URI_LENGTH respectively and has been concatenated*/

/* defines */

/* Canonical XML Version 1.0 specs at http://www.w3.org/TR/xml-c14n */
/* if defined source following the xml canonical spec (above) will
be used, otherwise, non-canonical xml source will be followed.  This
was included because the xml conformance test suite, used to test the
xml parser, was not using canonical form correctly and the code was
modified accordingly.  This must be defined to have true canonical
output, according to the specification */
#define W3C14N
#undef W3C14N

#define XMLOP_NS_LENGTH    10  /* maximum length of namespace local name */
#define XMLOP_URI_LENGTH   128 /* maximum length of namespace URI */
#define XMLOP_AXIS_SIZE    10  /* maximum number of namespaces for an element*/
#define XMLOP_STACK_SIZE 25    /* maximum size of stack containing namespaces axes */
#define XML_NAMESPACE_PREFIX    "xml"
#define XML_NAMESPACE_URI       "http://www.w3.org/XML/1998/namespace"

#define XMLCANON_DOCTYPEBUF_SIZE 1024 /* maximum DOCTYPE declaration buffer size */
#define XMLCANON_NOTATIONLIST_SIZE 100 /* maximum notation name list storage size */
#define XMLCANON_NOTATIONNAME_SIZE 50 /* maximum notation name length storage size */

/* typedefs */

/* namespace Entry */
typedef struct nsEntry
    {
    XML_Char localName[XMLOP_NS_LENGTH+1]; /* namespace's localName */
    XML_Char uri[XMLOP_URI_LENGTH+1];       /*  namespace's URI */
    } NSENTRY_T;   


/* namespace Axis */
typedef struct nsAxis
    {
    int size;                       /* number of valid entries in the array */
    NSENTRY_T namespaces[XMLOP_AXIS_SIZE];/* array of namespace entries */
    } NSAXIS_T;

/* stack of namespace Axes */
typedef struct nsStack
    {
    int size;
    NSAXIS_T nsAxis[XMLOP_STACK_SIZE];
    } NSSTACK_T;

struct XML_OutputStruct
    {
    FILE * fp;
    NSSTACK_T * pNsStack;
    NSAXIS_T * pNsAxis;
    XML_Char nsSep; 
    XML_Bool cdataActive;
    XML_Bool doFormat;
    XML_Bool doCanonicalize;
    XML_Bool doSecondCanonForm;
    int indent;
    };

typedef struct XML_OutputStruct * XML_Output;

/* function prototypes */

XML_Output XML_OutputCreate(const XML_Char *encoding, XML_Char nsSep, FILE *fp);
STATUS XML_OutputReset(XML_Output output, const XML_Char *encoding,XML_Char nsSep, FILE *fp);
STATUS XML_OutputDestroy(XML_Output output);
STATUS XML_OutputFormatSet(XML_Output output, XML_Bool format);
STATUS XML_OutputCanonSet(XML_Output output, XML_Bool canonicalize);
STATUS XML_OutputSecondCanonFormSet(XML_Output output, XML_Bool secondForm);
STATUS XML_XmlDeclWrite(XML_Output output, const XML_Char *version, const XML_Char *encoding, int standalone);
STATUS XML_StartNamespaceDeclWrite (XML_Output output, const XML_Char *prefix, const XML_Char *uri);
STATUS XML_EndNamespaceDeclWrite (XML_Output output, const XML_Char *prefix);
STATUS XML_StartElementWrite (XML_Output output, const XML_Char *name, const XML_Char **atts);
STATUS XML_EndElementWrite (XML_Output output, const XML_Char *name);
STATUS XML_CharacterDataWrite (XML_Output output, const XML_Char *s, int len);
STATUS XML_ProcessingInstructionWrite (XML_Output output, const XML_Char *target, const XML_Char *data);
STATUS XML_CommentWrite (XML_Output output, const XML_Char *data);
STATUS XML_StartCdataSectionWrite (XML_Output output);
STATUS XML_EndCdataSectionWrite (XML_Output output);
STATUS XML_EntityRefWrite (XML_Output output, const XML_Char *name);
XML_Char* XML_EntityNameFromContextGet(XML_Parser parser, const XML_Char *context);
STATUS XML_StartDoctypeDeclWrite(XML_Output output, const XML_Char *doctypeName, const XML_Char *sysid, const XML_Char *pubid, int has_internal_subset);
STATUS XML_EndDoctypeDeclWrite(XML_Output output);
STATUS XML_EntityDeclWrite(XML_Output output, const XML_Char * entityName, int is_parameter_entity, const XML_Char * value, int value_length, const XML_Char * base, const XML_Char * systemId, const XML_Char * publicId, const XML_Char * notationName);
STATUS XML_NotationDeclWrite(XML_Output output, const XML_Char * notationName, const XML_Char * base, const XML_Char * systemId, const XML_Char * publicId);
STATUS XML_AttlistDeclWrite(XML_Output output, const XML_Char *elname, const XML_Char *attname, const XML_Char *att_type, const XML_Char *dflt, int isrequired);
STATUS XML_ElementDeclWrite(XML_Output output, const XML_Char *name, XML_Content *model);
void xmlOutputInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __INCxmloph */
