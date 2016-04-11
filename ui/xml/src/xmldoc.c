/* xmldoc.c - XML parser documentation file */

/* Copyright 2003-2009 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,01apr09,f_f  upgrade to Expat 2.0.1
01b,06sep07,pas  Minor updates for VxWorks 6.x
01a,20dec04,pas  Refgen comments for xmlparse.c
*/

/*
\TITLE xmlparse - XML parser based on Expat 1.95.8

DESCRIPTION: 

This library provides XML parsing capability through a SAX based API. 

This XML parser is based on the Expat XML parser (version 2.0.1), with
the following value added features: 

 - XML 1.0 compliant
 - DTD parsing bugs have been fixed
 - source and external documentation of the parser where applicable
 - example code and tutorials
 - an empty element handler has been added
 - source has been made more human readable where applicable
 - VxWorks compatible
 - Wind River Workbench integration
 - interoperability testing on a variety of Windriver supported platforms
 
The XML parser is a stream-oriented parser in which an application registers
callbacks for XML specific syntax events, for example, start tags, end tags,
!ELEMENT declarations etc. The parser provides a large amount of flexibility
allowing setup for such things as namespace handling, document encoding, 
parameter entity parsing, and external entity parsing. The parser also
provides error handling, as well as the ability to pass user data throughout
calls to your callbacks.
 

INCLUDE FILES: xmlConfig.h errno.h stddef.h string.h xmlLib.h xmltok.h xmlrole.h expat.h
*/

/***************************************************************************
* XML_ParserCreate - creates a parser that is not concerned with namespaces or an external memory suite
*
* This routine calls XML_ParserCreate_MM with a preset argument list that 
* discludes a namespace separator, and a custom memory suite structure. 
* XML_ParserCreate_MM constructs a new parser; encoding is the encoding 
* specified by the external protocol, or NULL if there is none specified.
* encodingName, if it is being set, should be the encoding string specified 
* in your document's XML declaration, i.e. \<?xml version="1.0" encoding=
* "UTF-8"?\>.
*
* RETURNS: A pointer to the parser object.
*
* ERRNO: N/A
*/
XML_Parser XML_ParserCreate
    (
    const XML_Char *encodingName /* '\0'-terminated string giving the name of the externally specified encoding */
    )
    { }

/***************************************************************************
* XML_ParserCreateNS - creates a parser that is not concerned with an external memory suite
*
* This routine calls XML_ParserCreate_MM with a preset argument list that 
* discludes a custom memory suite structure. XML_ParserCreate_MM constructs 
* a new parser and namespace processor.  Element type names and attribute 
* names that belong to a namespace are expanded; unprefixed attribute names 
* are never expanded; unprefixed element type names are expanded only if 
* there is a default namespace. The expanded name is the concatenation of 
* the namespace URI, the namespace separator character, and the local part 
* of the name.  If the namespace separator is '\0' then the namespace URI
* and the local part will be concatenated without any separator. When a 
* namespace is not declared, the name and prefix will be passed through 
* without expansion.
*
* RETURNS: A pointer to the parser object.
*
* ERRNO: N/A
*/
XML_Parser XML_ParserCreateNS
    (
    const XML_Char *encodingName, /* '\0'-terminated string giving the name of the externally specified encoding */
    XML_Char nsSep /* character to separate namespace from corresponding element */
    )
    { }

/***************************************************************************
* XML_ParserCreate_MM - general parser object setup and initialization
*
* This routine sets up and initializes the parser object.  This involves 
* allocating data buffers, setting up the memory handling routines, setting 
* up how namespaces are displayed, and member variable initialization. In 
* more detail, this routine constructs a new parser using the memory 
* management suite referred to by memsuite. If memsuite is NULL, then the 
* standard library memory suite is used. If namespaceSeparator is non-NULL 
* it creates a parser with namespace processing as described above.  The 
* character pointed at will serve as the namespace separator.
*
* All further memory operations used for the created parser will come from
* the given suite.
*
* RETURNS: A pointer to the parser object.
*
* ERRNO: N/A
*/
XML_Parser XML_ParserCreate_MM
    (
    const XML_Char *encodingName, /* '\0'-terminated string giving the name of the externally specified encoding */
    const XML_Memory_Handling_Suite *memsuite, /* pointer to user defined memory suite structure */
    const XML_Char *nameSep /* character to separate namespace from corresponding element */
    )
    { }

/***************************************************************************
* parserInit - initializes the parser object member variables
*
* This routine separates out most of the simpe member variable 
* initializations. 
*
* RETURNS: Returns 1 if everything succeeds 0 if there is a failure
*
* ERRNO: N/A
*/
static void FASTCALL parserInit
    (
    XML_Parser parser, /* parser object to initialize */
    const XML_Char *encodingName /* '\0'-terminated string giving the name of the externally specified encoding */
    )
    { }

/***************************************************************************
* moveToFreeBindingsList - moves list of bindings to freeBindingList
*
* Moves list of bindings to freeBindingList.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

static void FASTCALL moveToFreeBindingList
    (
    XML_Parser parser, 
    BINDING *bindings
    )
    { }

/***************************************************************************
* XML_ParserReset - re-initializes the whole parser object
*
* This routine frees all memory used by the present parser object, then 
* re-initializes the object in preparation for re-use.  This is particularly 
* valuable when memory allocation overhead is disproportionatly high, such 
* as when a large number of small documents need to be parsed. All handlers 
* are cleared from the parser.
*
* RETURNS: Returns 1 if everything succeeds, 0 if there is a failure.
*
* ERRNO: N/A
*/
XML_Bool XML_ParserReset
    (
    XML_Parser parser, /* parser object to reset */
    const XML_Char *encodingName /* '\0'-terminated string giving the name of the externally specified encoding */
    )
    { }

/***************************************************************************
* XML_SetEncoding - sets the encoding for the XML parser, indicating how the document to be parsed is encoded
*
* This routine sets the encoding for the XML parser, indicating how the 
* document to be parsed is encoded. It is equivalent to supplying an 
* encoding argument to XML_ParserCreate. It must not be called after 
* XML_Parse or XML_ParseBuffer.
*
* RETURNS: 1 for success, 0 for error.
*
* ERRNO: N/A
*/
int XML_SetEncoding
    (
    XML_Parser parser, /* parser object */
    const XML_Char *encodingName /* '\0'-terminated string giving the name of the externally specified encoding */
    )
    { }

/***************************************************************************
* XML_ExternalEntityParserCreate - creates an XML_Parser object that can parse an external general entity
*
* This routine creates an XML_Parser object that can parse an external 
* general entity.  It creates a new parser that retains all of the 
* characteristics of the original document parser that is passed into this 
* routine(i.e. handlers user date etc...).
* context is a '\0'-terminated string specifying the parse context;
* encoding is a '\0'-terminated string giving the name of the externally 
* specified encoding, or NULL if there is no externally specified encoding.
* The context string consists of a sequence of tokens separated by formfeeds
* (\f); a token consisting of a name specifies that the general entity of 
* the name is open; a token of the form prefix=uri specifies the namespace 
* for a particular prefix; a token of the form =uri specifies the default
* namespace.  This can be called at any point after the first call to an 
* ExternalEntityRefHandler as long as the parser has not yet been freed.
* The new parser is completely independent and may safely be used in a 
* separate thread.  The handlers and userData are initialized from the 
* parser argument.  
*
* RETURNS: Returns 0 if out of memory. Otherwise returns a new XML_Parser
* object.
*
* ERRNO: N/A
*/
XML_Parser XML_ExternalEntityParserCreate
    (
    XML_Parser oldParser, /* parser object */
    const XML_Char *context, /* '\0'-terminated string specifying the parse context */
    const XML_Char *encodingName /* '\0'-terminated string giving the name of the externally specified encoding */
    )
    { }

/***************************************************************************
* destroyBindings - frees memory related to bindings
*
* Frees memory related to bindings.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static void FASTCALL destroyBindings
    (
    BINDING *bindings, 
    XML_Parser parser
    )
    { }

/***************************************************************************
* XML_ParserFree - frees all memory used by the parser object passed in
*
* This routine frees all memory used by the parser object passed in.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_ParserFree
    (
    XML_Parser parser /* parser object to free */
    )
    { }

/***************************************************************************
* XML_UseParserAsHandlerArg - sets the m_handlerArg member variable of the parser passed in to be the parser
*
* This routine sets the m_handlerArg member variable of the parser passed in
* to be the parser. If this routine is called, then the parser will be 
* passed as the first argument to callbacks instead of userData.  The 
* userData will still be accessible using XML_GetUserData.
*
* RETURNS: N/A
*
* ERRNO:
*  S_xmlLib_CANT_CHANGE_FEATURE_ONCE_PARSING
*  S_xmlLib_FEATURE_REQUIRES_XML_DTD
*/
void XML_UseParserAsHandlerArg
    (
    XML_Parser parser /* parser object to configure*/
    )
    { }

/***************************************************************************
* XML_UseForeignDTD - forces the parser to assume that there is an external subset
*
* This routine forces the parser to assume that there is an external subset
* If useDTD == XML_TRUE is passed to this routine, then the parser will 
* assume that there is an external subset, even if none is specified in the
* document. In such a case the parser will call the externalEntityRefHandler
* with a value of NULL for the systemId argument (the publicId and context 
* arguments will be NULL as well). 
*
* Note: If this routine is called, it must be done before the first
* call to XML_Parse or XML_ParseBuffer. It will have no effect after 
* that.  Returns XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING. 
*
* Note: If the document does not have a DOCTYPE declaration at all, then 
* startDoctypeDeclHandler and endDoctypeDeclHandler will not be called, 
* despite an external subset being parsed.  
*
* Note: If XML_DTD is not defined when Expat is compiled, returns 
* XML_ERROR_FEATURE_REQUIRES_XML_DTD.                                
*
* RETURNS: Returns an XML_Error enumeration, XML_ERROR_NONE if OK, 
* otherwise an enumerated error.
*
* ERRNO:
*  S_xmlLib_CANT_CHANGE_FEATURE_ONCE_PARSING
*  S_xmlLib_FEATURE_REQUIRES_XML_DTD
*/


enum XML_Error XML_UseForeignDTD
    (
    XML_Parser parser, /* parser object to configure */
    XML_Bool useDTD /* force foreign DTD to be used */
    )
    { }

/***************************************************************************
* XML_SetReturnNSTriplet - sets how namespace triplets are displayed, e.g. URI + seperator + local_name
*
* This routine sets how namespace triplets are displayed, 
* e.g. URI + seperator + local_name. If do_nst is non-zero, and namespace 
* processing is in effect, and a name has a prefix (i.e. an explicit 
* namespace qualifier) then that name is returned as a triplet in a single 
* string separated by the separator character specified when the parser was 
* created: URI + sep + local_name + sep + prefix. If do_nst is zero, then 
* namespace information is returned in the default manner (URI + sep + 
* local_name) whether or not the name has a prefix.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetReturnNSTriplet
    (
    XML_Parser parser, /* parser object to configure */
    int do_nst /* cause display of names as URI + sep + local_name */
    )
    { }

/***************************************************************************
* XML_SetUserData - assigns an arbitrary piece of data/structure to be passed throughout routines as userData
*
* This routine assigns an arbitrary piece of data/structure to be passed 
* throughout routines as userData.  This is done by setting the m_userData 
* member variable of the parser passed in to be the passed in p variable.  
* This allows whatever p is to be passed throughout handler routines as the 
* userData variable.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetUserData
    (
    XML_Parser parser, /* parser object to configure */
    void *p /* arbitrary data to set as userData */
    )
    { }

/***************************************************************************
* XML_SetBase - sets the base directory of the parser object, for relative paths used in entity handling
*
* This routine copies the base path p into the parser member variable 
* m_curBase.  In general, it sets the base to be used for resolving relative
* URIs in system identifiers in declarations.  Resolving relative 
* identifiers is left to the application: this value will be passed through
* as the base argument to the XML_ExternalEntityRefHandler, 
* XML_NotationDeclHandler, and XML_UnparsedEntityDeclHandler. The base
* argument will be copied.  
*
* RETURNS: Returns zero if out of memory, non-zero otherwise.
*
* ERRNO: N/A
*/
int XML_SetBase
    (
    XML_Parser parser, /* parser object to configure */
    const XML_Char *p /* base path string */
    )
    { }

/***************************************************************************
* XML_GetBase - retrieves the base path stored in the parser object
*
* This routine retrieves the m_curBase member variable of the parser.  This
* variable is used for external entity relative paths during entity parsing.
*
* RETURNS: A pointer to the m_curBase member variable of the parser object.
*
* ERRNO: N/A
*/
const XML_Char *XML_GetBase
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_GetSpecifiedAttributeCount - returns the number of the attribute/value pairs specified in the last element, not including default attributes
* 
* This routine returns the number of the attribute/value pairs passed in 
* last call to the XML_StartElementHandler that were specified in the 
* start-tag rather than defaulted. Each attribute/value pair counts as 2; 
* this corresponds to an index into the atts array passed to the 
* XML_StartElementHandler.
*
* RETURNS: parser->nSpecifiedAtts variable value.
*
* ERRNO: N/A
*/
int XML_GetSpecifiedAttributeCount
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_GetIdAttributeIndex - retrieves the ID attribute index passed in the last call to XML_StartElementHandler
*
* This routine returns the index of the ID attribute passed in the last call 
* to XML_StartElementHandler, or -1 if there is no ID attribute.  Each 
* attribute/value pair counts as 2; this corresponds to an index into the 
* atts array passed to the XML_StartElementHandler.
*
* RETURNS: parser->m_idAttIndex variable value.
*
* ERRNO: N/A
*/
int XML_GetIdAttributeIndex
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_SetElementHandler - assigns the start and end element callbacks to the passed in parser object
*
* This routine sets the start and end element callback function pointers in
* the passed in parser object.  These callbacks are later called for every 
* start and end element event. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetElementHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartElementHandler start, /* function pointer to start element handler */
    XML_EndElementHandler end /* function pointer to end element handler */
    )
    { }

/***************************************************************************
* XML_SetStartElementHandler - assigns the start element callback to the passed in parser object
*
* This routine sets the start element callback function pointer in the 
* passed in parser object.  This callback is later called for every start
* element event. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetStartElementHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartElementHandler start /* function pointer to start element handler */
    )
    { }

/***************************************************************************
* XML_SetEndElementHandler - assigns the end element callback to the passed in parser object
*
* This routine sets the end element callback function pointer in the passed
* in parser object.  This callback is later called for every end element 
* event. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetEndElementHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_EndElementHandler end /* function pointer to end element handler */
    )
    { }

/***************************************************************************
* XML_SetEmptyElementHandler - assigns the empty element callback to the passed in parser object
*
* This routine sets the empty element callback function pointer in the 
* passed in parser object.  This callback is called whenever a empty element
* is encountered, i.e. \<element /\> which is also equivalent to 
* \<element\>\</element\>.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetEmptyElementHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_EmptyElementHandler empty /* function pointer to empty element handler */
    )
    { }

/***************************************************************************
* XML_SetCharacterDataHandler - assigns the character data callback to the passed in parser object
*
* This routine sets the character data callback function pointer in the 
* passed in parser object.  This callback is called whenever element 
* character content is encountered.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetCharacterDataHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_CharacterDataHandler handler /* function pointer to character data handler */
    )
    { }

/***************************************************************************
* XML_SetProcessingInstructionHandler - assigns the processing instruction callback to the passed in parser object
*
* This routine sets the processing instruction callback function pointer in
* the passed in parser object.  This callback is called whenever a 
* processing instruction is encountered.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetProcessingInstructionHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_ProcessingInstructionHandler handler /* function pointer to processing instruction handler */
    )
    { }

/***************************************************************************
* XML_SetCommentHandler - assigns the comment handler callback to the passed in parser object
*
* This routine sets the comment handler callback function pointer in the 
* passed in parser object.  This callback is called whenever a comment is 
* encountered in the XML document.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetCommentHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_CommentHandler handler /* function pointer to comment handler */
    )
    { }

/***************************************************************************
* XML_SetCdataSectionHandler - assigns the CDATA section start and end callbacks to the passed in parser object
*
* This routine sets the CDATA section start and end callback function 
* pointers in the passed in parser object.  This callback is called whenever
* a CDATA section is encountered. It is important to note that CDATA is 
* different from the character data. Character data refers to text within 
* elements.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetCdataSectionHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartCdataSectionHandler start, /* function pointer to start CDATA section handler */
    XML_EndCdataSectionHandler end /* function pointer to end CDATA section handler */
    )
    { }

/***************************************************************************
* XML_SetStartCdataSectionHandler - assigns the start CDATA section callback to the passed in parser object
*
* This routine sets the start CDATA section callback function pointer in the
* passed in parser object.  This callback is called whenever the start of a
* CDATA section is encountered.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetStartCdataSectionHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartCdataSectionHandler start  /* function pointer to start CDATA section handler */
    )
    { }

/***************************************************************************
* XML_SetEndCdataSectionHandler - assigns the end CDATA section callback to the passed in parser object
*
* This routine sets the end CDATA section callback function pointer in the 
* passed in parser object.  This callback is called whenever the end of a 
* CDATA section is encountered.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetEndCdataSectionHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_EndCdataSectionHandler end /* function pointer to end CDATA section handler */
    )
    { }

/***************************************************************************
* XML_SetDefaultHandler - assigns the default callback to the passed in parser object
*
* This routine sets the default handler and also inhibits expansion of 
* internal entities. These entity references will be passed to the default 
* handler, or to the skipped entity handler, if one is set. 
*
* The callback is called for any characters in the XML document for which 
* there is no applicable handler.  This includes characters that are part 
* of markup which is of a kind that is not reported (comments, markup 
* declarations), and characters that are part of a construct which could be 
* reported but for which no handler has been supplied. The characters are 
* passed exactly as they were in the XML document except that they are 
* encoded in UTF-8.  Line boundaries are not normalized. Note that a byte 
* order mark character is not passed to the default handler. There are no
* guarantees about how characters are divided between calls to the default 
* handler. For example, a comment might be split between multiple calls.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetDefaultHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_DefaultHandler handler /* function pointer to default handler */
    )
    { }

/***************************************************************************
* XML_SetDefaultHandlerExpand - assigns the default expand internal entities callback to the passed in parser object
*
* This routine sets the default handler but does not inhibit expansion of 
* internal entities.  The entity reference is not passed to the default
* handler.  This callback is called whenever an XML event occurs and there 
* is no specific callback already assigned for that event. For example, if 
* no callbacks are set, the default handler is the only callback 
* called, if it is set.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetDefaultHandlerExpand
    (
    XML_Parser parser, /* parser object to configure */
    XML_DefaultHandler handler  /* function pointer to default handler */
    )
    { }

/***************************************************************************
* XML_SetDoctypeDeclHandler - assigns the start and end DOCTYPE declaration callbacks to the passed in parser object
*
* This routine sets the start and end DOCTYPE declaraction callback function 
* pointers in the passed in parser object.  The start is called for the 
* start of the DOCTYPE declaration, before any DTD or internal subset is 
* parsed. The end is called for the start of the DOCTYPE declaration when 
* the closing > is encountered, but after processing any external subset.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetDoctypeDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartDoctypeDeclHandler start, /* function pointer to start DOCTYPE declaration handler */
    XML_EndDoctypeDeclHandler end /* function pointer to end DOCTYPE declaration handler */
    )
    { }

/***************************************************************************
* XML_SetStartDoctypeDeclHandler - assigns the start DOCTYPE declaration callback to the passed in parser object
*
* This routine sets the start DOCTYPE declaration callback function pointer 
* in the passed in parser object.  The start is called for the start of the 
* DOCTYPE declaration, before any DTD or internal subset is parsed.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetStartDoctypeDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartDoctypeDeclHandler start /* function pointer to start DOCTYPE declaration handler */
    )
    { }

/***************************************************************************
* XML_SetEndDoctypeDeclHandler - assigns the end DOCTYPE declaration callback to the passed in parser object
*
* This routine sets the end DOCTYPE declaration callback function pointer in
* the passed in parser object.  The end is called for the start of the 
* DOCTYPE declaration when the closing > is encountered, but after 
* processing any external subset.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetEndDoctypeDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_EndDoctypeDeclHandler end /* function pointer to end DOCTYPE declaration handler */
    )
    { }

/***************************************************************************
* XML_SetUnparsedEntityDeclHandler - (OBSOLETE) assigns the unparsed entity declaration callback to the passed in parser object
*
* OBSOLETE -- OBSOLETE -- OBSOLETE 
* This routine sets the unparsed entity decaration callback function pointer 
* in the passed in parser object.  This handler has been superceded by the 
* EntityDeclHandler. It is provided here for backward compatibility. The 
* callback is called for a declaration of an unparsed (NDATA) entity. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetUnparsedEntityDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_UnparsedEntityDeclHandler handler  /* function pointer to unparsed entity declaration handler */
    )
    { }

/***************************************************************************
* XML_SetNotationDeclHandler - assigns the NOTATION declaration callback to the passed in parser object
*
* This routine sets the NOTATION declaration callback function pointer in 
* the passed in parser object.  The callback is called for a declaration of 
* NOTATION.  
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetNotationDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_NotationDeclHandler handler  /* function pointer to NOTATION declaration handler */
    )
    { }

/***************************************************************************
* XML_SetNamespaceDeclHandler - assigns the start and end namespace declaration callbacks to the passed in parser object
*
* This routine sets the start and end namespace declaration callback 
* function pointers in the passed in parser object.  When namespace 
* processing is enabled, these callbacks are called once for each namespace
* declaration. The call to the start and end element handlers occur between
* the calls to the start and end namespace declaration handlers.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetNamespaceDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartNamespaceDeclHandler start, /* function pointer to start namespace declaration handler */
    XML_EndNamespaceDeclHandler end /* function pointer to end namespace declaration handler */
    )
    { }

/***************************************************************************
* XML_SetStartNamespaceDeclHandler - assigns the start namespace declaration callback to the passed in parser object
*
* This routine sets the start namespace declaration callback function 
* pointer in the passed in parser object.  When namespace processing is 
* enabled, this callback is called once for each namespace declaration. The
* call to the start and end element handlers occur between the calls to the
* start and end namespace declaration handlers.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetStartNamespaceDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_StartNamespaceDeclHandler start /* function pointer to start namespace declaration handler */
    )
    { }

/***************************************************************************
* XML_SetEndNamespaceDeclHandler - assigns the end namespace declaration callback to the passed in parser object
*
* This routine sets the end namespace declaration callback function pointer 
* in the passed in parser object.  When namespace processing is enabled, this
* callback is called once for each namespace declaration. The call to the 
* start and end element handlers occur between the calls to the start and 
* end namespace declaration handlers.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetEndNamespaceDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_EndNamespaceDeclHandler end /* function pointer to end namespace declaration handler */
    )
    { }

/***************************************************************************
* XML_SetNotStandaloneHandler - assigns the not stand alone callback to the passed in parser object
*
* This routine sets the not standalone callback function pointer in the 
* passed in parser object.  This callback is called if the document is not 
* standalone (it has an external subset or a reference to a parameter entity,
* but does not have standalone="yes").
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetNotStandaloneHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_NotStandaloneHandler handler  /* function pointer to notstandalone handler */
    )
    { }

/***************************************************************************
* XML_SetExternalEntityRefHandler - assigns the external entity reference callback to the passed in parser object
*
* This routine sets the external entity reference callback function pointer 
* in the passed in parser object.  This callback is called whenever an 
* external entity is referenced in the parsed XML document.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetExternalEntityRefHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_ExternalEntityRefHandler handler /* function pointer to external entity reference handler */
    )
    { }

/***************************************************************************
* XML_SetExternalEntityRefHandlerArg - sets arg to be passed as the first argument to your external entity ref handler instead of the parser object
*
* This routine sets arg to be passed as the first argument to your external 
* entity ref handler instead of the parser object.  If a non-NULL value for
* arg is specified here, then it will be passed as the first argument to the
* external entity ref handler instead of the parser object.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetExternalEntityRefHandlerArg
    (
    XML_Parser parser,  /* parser object to configure */
    void *arg /* arbitrary data to be set as external entity reference user data */
    )
    { }

/***************************************************************************
* XML_SetSkippedEntityHandler - assigns the skipped entity callback to the passed in parser object
*
* This routine sets the skipped entity callback function pointer in the 
* passed in parser object. This callback is called in two situations:
*   1) An entity reference is encountered for which no declaration
*      has been read *and* this is not an error.
*   2) An internal entity reference is read, but not expanded, because
*      XML_SetDefaultHandler has been called.
*   Note: Skipped parameter entities in declarations and skipped general
*         entities in attribute values cannot be reported because the event 
*         would be out of sync with the reporting of the declarations or 
*         attribute values .
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetSkippedEntityHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_SkippedEntityHandler handler /* function pointer to skipped entity handler */
    )
    { }

/***************************************************************************
* XML_SetUnknownEncodingHandler - assigns the unknown encoding callback to the passed in parser object
*
* This routine sets the unknown encoding callback function pointer in the 
* passed in parser object.  This callback is called for an encoding that is 
* unknown to the parser.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetUnknownEncodingHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_UnknownEncodingHandler handler,  /* function pointer to unknown encoding handler */
    void *data
    )
    { }

/***************************************************************************
* XML_SetElementDeclHandler - assigns the ELEMENT declaration callback to the passed in parser object
*
* This routine sets the ELEMENT declaration callback function pointer in the
* passed in parser object.  This callback is called for an element declaration. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetElementDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_ElementDeclHandler eldecl  /* function pointer to ELEMENT declaration handler */
    )
    { }

/***************************************************************************
* XML_SetAttlistDeclHandler - assigns the ATTLIST declaration callback to the passed in parser object
*
* This routine sets the ATTLIST declaration callback function pointer in the
* passed in parser object.  The ATTLIST declaration handler is called for 
* *each* attribute. A single Attlist declaration with multiple attributes 
* declared will generate multiple calls to this handler. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetAttlistDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_AttlistDeclHandler attdecl  /* function pointer to ATTLIST declaration handler */
    )
    { }

/***************************************************************************
* XML_SetEntityDeclHandler - assigns the ENTITY declaration callback to the passed in parser object
*
* This routine sets the ENTITY declaration callback function pointer in the
* passed in parser object.  This is called for entity declarations. 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetEntityDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_EntityDeclHandler handler /* function pointer to ENTITY declaration handler */
    )
    { }

/***************************************************************************
* XML_SetXmlDeclHandler - assigns the XML declaration callback to the passed in parser object
*
* This routine sets the XML declaration callback function pointer in the 
* passed in parser object.  The XML declaration handler is called for *both*
* XML declarations and text declarations.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_SetXmlDeclHandler
    (
    XML_Parser parser, /* parser object to configure */
    XML_XmlDeclHandler handler  /* function pointer to xml declaration handler */
    )
    { }

/***************************************************************************
* XML_SetParamEntityParsing - assigns the parameter entity parsing callback to the passed in parser object
*
* This routine sets the parameter entity parsing callback function pointer 
* in the passed in parser object.  It controls parsing of parameter entities 
* (including the external DTD subset). If parsing of parameter entities is 
* enabled, then references to external parameter entities (including the 
* external DTD subset) are passed to the handler set with 
* XML_SetExternalEntityRefHandler.  The context passed is 0.
*
* Unlike external general entities, external parameter entities can only be 
* parsed synchronously.  If the external parameter entity is to be parsed, 
* it must be parsed during the call to the external entity ref handler. The
* complete sequence of XML_ExternalEntityParserCreate, 
* XML_Parse/XML_ParseBuffer and XML_ParserFree calls must be made during 
* this call.  After XML_ExternalEntityParserCreate has been called to create
* the parser for the external parameter entity (context must be 0 for this 
* call), it is illegal to make any calls on the old parser until 
* XML_ParserFree has been called on the newly created parser.  If the 
* library has been compiled without support for parameter entity parsing 
* (without XML_DTD being defined), then XML_SetParamEntityParsing will 
* return 0 if parsing of parameter entities is requested; otherwise it will 
* return non-zero.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
int XML_SetParamEntityParsing
    (
    XML_Parser parser,  /* parser object to configure */
    enum XML_ParamEntityParsing peParsing /* desired parameter entity parsing mode */
    )
    { }

/***************************************************************************
* XML_Parse - parses input s, once isFinal is non-zero
*
* This routine takes in the document either in pieces or all at once.  Once
* the document has been completely passed the last call to XML_Parse must 
* have isFinal true; len may be zero for this call (or any other).  The 
* document is parsed as it is passed in, meaning, if it is passed in pieces
* then each piece is parsed as it is passed, and if it is passed in whole
* then the document is parsed as a whole.  The way it is passed in has no 
* influence on the final parse result. When XML_Parse is called the document
* or document piece being passed into that routine is copied into a buffer 
* within the parser object.  The buffer that contains the document or 
* document piece is then retrieved using XML_GetBuffer to retrieve a 
* pointer to the buffer, and parsed by calling XML_ParseBuffer.
*
* RETURNS: Returns 0 if a fatal error is detected, non-zero otherwise
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*/
enum XML_Status XML_Parse
    (
    XML_Parser parser,  /* parser object to parse with */
    const char *s, /* buffer containing xml document to parse */
    int len, /* length of s */
    int isFinal /* indicate if this is the final piece of the xml document being passed in */
    )
    { }

/***************************************************************************
* XML_ParseBuffer - parses the buffer stored within the parser object
*
* This routine performs the XML parsing on the buffer in the parser object
* containing the document or document piece being parsed.  This is called 
* by XML_Parse, and should not be called outside of that routine.
*
* RETURNS: Returns 0 if a fatal error is detected, non-zero otherwise
*
* ERRNO: N/A
*/
enum XML_Status XML_ParseBuffer
    (
    XML_Parser parser,  /* parser object to parse */
    int len, /* length of the buffer to parse */
    int isFinal /* indicate if this is the final piece of buffer to parse */
    )
    { }

/***************************************************************************
* XML_GetBuffer - retrieves the parser object parse buffer
*
* This routine retrieves a void pointer to the parser object parse buffer.
*
* RETURNS: N/A
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*/
void *XML_GetBuffer
    (
    XML_Parser parser, /* parser object to retrieve from */
    int len /* length of buffer to retrieve */
    )
    { }

/***************************************************************************
* XML_GetErrorCode - returns the latest parser error code
*
* This routine returns the latest parser error code.
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string.
*
* ERRNO: N/A
*/
enum XML_Error XML_GetErrorCode
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_GetCurrentByteIndex - returns the current byte index that the parser is presently at within the document being parse
*
* This routine returns information about the current parse location. It
* may be called when XML_Parse or XML_ParseBuffer return 0; in this case the
* location is the location of the character at which the error was detected.
* This may also be called from any other callback called to report some 
* parse event; in this the location is the location of the first of the 
* sequence of characters that generated the event.
*
* RETURNS: Returns the current byte index that the parser is 
* presently at within the document being parsed.
*
* ERRNO: N/A
*/
XML_Index XML_GetCurrentByteIndex
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_GetCurrentByteCount - return the number of bytes in the current event. Returns 0 if the event is in an internal entity
*
* This routine returns the number of bytes in the current event. Returns 0 
* if the event is in an internal entity.
*
* RETURNS: Return the number of bytes in the current event. Returns 0 if the
* event is in an internal entity.
*
* ERRNO: N/A
*/
int XML_GetCurrentByteCount
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_GetInputContext - returns the parser object buffer, as well as sets the size and offset argument variables
*
* This routine returns the parser object buffer, as well as sets the size 
* and offset argument variables. If XML_CONTEXT_BYTES is defined, returns 
* the input buffer, sets the integer pointed to by offset to the offset 
* within this buffer of the current parse position, and sets the integer 
* pointed to by size to the size of this buffer (the number of input bytes). 
* Otherwise returns a NULL pointer. Also returns a NULL pointer if a parse 
* isn't active.
*
* NOTE: The character pointer returned should not be used outside the 
* handler that makes the call.
*
* RETURNS: Returns the parser object buffer (m_buffer member variable).
*
* ERRNO: N/A
*/
const char *XML_GetInputContext
    (
    XML_Parser parser,  /* parser object to retrieve from */
    int *offset, 
    int *size
    )
    { }

/***************************************************************************
* XML_GetCurrentLineNumber - returns the current line that the parser is presently at within the document being parsed
*
* This routine returns information about the current parse location. It
* may be called when XML_Parse or XML_ParseBuffer return 0; in this case the
* location is the location of the character at which the error was detected.
* This may also be called from any other callback called to report some 
* parse event; in this the location is the location of the first of the 
* sequence of characters that generated the event.
*
* RETURNS: Returns the current line that the parser is at within the 
* document being parsed.
*
* ERRNO:
*  S_xmlLib_UNKNOWN_ENCODING
*/
XML_Size XML_GetCurrentLineNumber
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_GetCurrentColumnNumber - returns the current column that the parser is presently at within the document being parsed
*
* This routine returns information about the current parse location. It
* may be called when XML_Parse or XML_ParseBuffer return 0; in this case the
* location is the location of the character at which the error was detected.
* This may also be called from any other callback called to report some 
* parse event; in this the location is the location of the first of the 
* sequence of characters that generated the event.
*
* RETURNS: Returns the current column that the parser is at within the 
* document being parsed
*
* ERRNO:
*  S_xmlLib_UNKNOWN_ENCODING
*/
XML_Size XML_GetCurrentColumnNumber
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_DefaultCurrent - passes processing on to the default handler, but only for start element, end element, processing instruction and character data handlers
*
* This routine passes processing on to the default handler, but only for 
* start element, end element, processing instruction and character data 
* handlers. This can be called within a handler for a start element, end 
* element, processing instruction or character data.  It causes the 
* corresponding markup to be passed to the default handler.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void XML_DefaultCurrent
    (
    XML_Parser parser /* parser object to retrieve from */
    )
    { }

/***************************************************************************
* XML_ErrorString - returns the corresponding error string for the passed in error code
*
* This routine returns the corresponding error string for the passed in 
* error code.
*
* RETURNS: Returns the corresponding error string for the passed in error
* code.
*
* ERRNO: N/A
*/
const XML_LChar *XML_ErrorString
    (
    enum XML_Error code /* Expat SAX XML parser specific error code to decode */
    )
    { }

/***************************************************************************
* XML_ExpatVersion - returns a string containing the Expat verion number
*
* This routine returns a string containing the Expat version number.
*
* RETURNS: Returns a string containing the Expat version number.
*
* ERRNO: N/A
*/
const XML_LChar * XML_ExpatVersion(void)
    { }

/***************************************************************************
* XML_ExpatVersionInfo - returns a structure containing the Expat version number
*
* This routine returns a structure containing the Expat version number.
*
* RETURNS: Returns a structure containing the Expat version number.
*
* ERRNO: N/A
*/
XML_Expat_Version XML_ExpatVersionInfo(void)
    { }

/***************************************************************************
* contentProcessor - passes processing on to doContent
*
* Passes document processing on to doContent
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*/
static enum XML_Error contentProcessor
    (
    XML_Parser parser,
    const char *start,
    const char *end,
    const char **endPtr
    )
    { }

/***************************************************************************
* externalEntityInitProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static enum XML_Error FASTCALL externalEntityInitProcessor
    (
    XML_Parser parser,
    const char *start,
    const char *end,
    const char **endPtr
    )
    { }

/***************************************************************************
* externalEntityInitProcessor2 - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_TOKEN
*/
static
enum XML_Error externalEntityInitProcessor2
    (
    XML_Parser parser,
    const char *start,
    const char *end,
    const char **endPtr
    )
    { }

/***************************************************************************
* externalEntityInitProcessor3 - 
*
* 
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_TOKEN
*/
static
enum XML_Error FASTCALL externalEntityInitProcessor3
    (
    XML_Parser parser,
    const char *start,
    const char *end,
    const char **endPtr
    )
    { }

/***************************************************************************
* externalEntityContentProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*/
static
enum XML_Error externalEntityContentProcessor
    (
    XML_Parser parser,
    const char *start,
    const char *end,
    const char **endPtr
    )
    { }

/***************************************************************************
* doContent - handles entities declared within xml documents
*
* Parses the entities within XML documents and performs appropriate 
* actions for xml specific tokens. The recursive entity error handling does
* not seem to work? 
*
* Macro translations: 
* XmlContentTok(enc, s, end, &next) = 
* (((enc)->scanners[XML_CONTENT_STATE])(enc, ptr, end, nextTokPtr))
*   where:  enc is <typedef struct encoding ENCODING> found in xmltok.h
*           scanners is an array of callbacks(ie. initScanPrlog, initScanContent,
*           both call initScan or PREFIX(prologTok), PREFIX(contentTok), 
*           PREFIX(cdataSectionTok)), depending on what scanners is 
*           initialized to.
*
* RETURNS: XML_Error enum
*
* ERRNO:
*  S_xmlLib_ASYNC_ENTITY
*  S_xmlLib_BAD_CHAR_REF
*  S_xmlLib_BINARY_ENTITY_REF
*  S_xmlLib_ENTITY_DECLARED_IN_PE
*  S_xmlLib_EXTERNAL_ENTITY_HANDLING
*  S_xmlLib_ILLEGAL_PI_TARGET
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_MISPLACED_XML_PI
*  S_xmlLib_NO_ELEMENTS
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_RECURSIVE_ENTITY_REF
*  S_xmlLib_TAG_MISMATCH
*  S_xmlLib_UNCLOSED_TOKEN
*  S_xmlLib_UNDEFINED_ENTITY
*/
static 
enum XML_Error FASTCALL doContent(XML_Parser parser,
          int startTagLevel,
          const ENCODING *enc,
          const char *s,
          const char *end,
          const char **nextPtr)
    { }

/***************************************************************************
* storeAtts - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_DUPLICATE_ATTRIBUTE
*  S_xmlLib_NO_MEMORY
*/
static 
enum XML_Error FASTCALL storeAtts(XML_Parser parser, const ENCODING *enc,
                                const char *attStr, TAG_NAME *tagNamePtr,
                                BINDING **bindingsPtr)
    { }

/***************************************************************************
* addBinding - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
int FASTCALL addBinding(XML_Parser parser, PREFIX *prefix, const ATTRIBUTE_ID *attId,
               const XML_Char *uri, BINDING **bindingsPtr)
    { }

/***************************************************************************
* cdataSectionProcessor - 
*
* The idea here is to avoid using stack for each CDATA section when the 
* whole file is parsed with one call.
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static
enum XML_Error FASTCALL cdataSectionProcessor(XML_Parser parser,
                                     const char *start,
                                     const char *end,
                                     const char **endPtr)
    { }

/***************************************************************************
* doCdataSection - 
*
* startPtr gets set to non-null is the section is closed, and to null if the
* section is not yet closed.
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_CDATA_SECTION
*  S_xmlLib_UNEXPECTED_STATE
*/
static
enum XML_Error FASTCALL doCdataSection(XML_Parser parser,
                              const ENCODING *enc,
                              const char **startPtr,
                              const char *end,
                              const char **nextPtr)
    { }

/***************************************************************************
* ignoreSectionProcessor - 
*
* The idea here is to avoid using stack for each IGNORE section when the 
* whole file is parsed with one call.
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static
enum XML_Error FASTCALL ignoreSectionProcessor(XML_Parser parser,
                                      const char *start,
                                      const char *end,
                                      const char **endPtr)
    { }

/***************************************************************************
* doIgnoreSection - 
*
* startPtr gets set to non-null is the section is closed, and to null if the
* section is not yet closed.
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_SYNTAX
*  S_xmlLib_UNEXPECTED_STATE
*/
static
enum XML_Error FASTCALL doIgnoreSection(XML_Parser parser,
                               const ENCODING *enc,
                               const char **startPtr,
                               const char *end,
                               const char **nextPtr)
    { }

/***************************************************************************
* initializeEncoding - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static 
enum XML_Error FASTCALL initializeEncoding(XML_Parser parser)
    { }

/***************************************************************************
* processXmlDecl - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_INCORRECT_ENCODING
*  S_xmlLib_NO_MEMORY
*  S_xmlLib_SYNTAX
*/
static 
enum XML_Error FASTCALL processXmlDecl(XML_Parser parser, int isGeneralTextEntity,
               const char *s, const char *next)
    { }

/***************************************************************************
* handleUnknownEncoding - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*  S_xmlLib_UNKNOWN_ENCODING
*/
static 
enum XML_Error FASTCALL handleUnknownEncoding(XML_Parser parser, const XML_Char *encodingName)
    { }

/***************************************************************************
* prologInitProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static 
enum XML_Error FASTCALL prologInitProcessor(XML_Parser parser,
                    const char *s,
                    const char *end,
                    const char **nextPtr)
    { }

/***************************************************************************
* externalParEntInitProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static 
enum XML_Error FASTCALL externalParEntInitProcessor(XML_Parser parser,
                            const char *s,
                            const char *end,
                            const char **nextPtr)
    { }

/***************************************************************************
* entityValueInitProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_TOKEN
*/
static
enum XML_Error FASTCALL entityValueInitProcessor(XML_Parser parser,
                         const char *s,
                         const char *end,
                         const char **nextPtr)
    { }

/***************************************************************************
* externalParEntProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_TOKEN
*/
static 
enum XML_Error FASTCALL externalParEntProcessor(XML_Parser parser,
                        const char *s,
                        const char *end,
                        const char **nextPtr)
    { }

/***************************************************************************
* entityValueProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_TOKEN
*/
static 
enum XML_Error FASTCALL entityValueProcessor(XML_Parser parser,
                     const char *s,
                     const char *end,
                     const char **nextPtr)
    { }

/***************************************************************************
* prologProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static
enum XML_Error FASTCALL prologProcessor(XML_Parser parser,
                const char *s,
                const char *end,
                const char **nextPtr)
    { }

/***************************************************************************
* doProlog - handles stand alone part of documents, ie xml docs not 
* including external references, external sections handled indirectly
* by doContent
*
* Parses the stand alone part of XML documents and performs appropriate 
* actions for xml specific tokens.  For example, calls other decl handlers
* depending on the token type.  Entities are handled indirectly by doContent
* through calls to entity handlers.  XmlTokenRole will translate the token
* it reads differently depending on the state of m_prologState(prologState)
* (See xmlrole.c, "static PROLOG_HANDLER" for a list of all the handlers).
*
* Macro translations: 
* XmlTokenRole(&prologState, XML_TOK_NONE, end, end, enc) = 
* (((Parser *)parser)->m_prologState->handler)((((Parser *)parser)->m_prologState), XML_TOK_NONE, end, end, enc)
*
* XmlPrologTok(enc, s, end, &next) = 
* (((enc)->scanners[XML_PROLOG_STATE])(enc, ptr, end, nextTokPtr))
*   where:  enc is <typedef struct encoding ENCODING> found in xmltok.h
*           scanners is an array of callbacks(ie. initScanProlog, initScanContent,
*           both call initScan)
*           Within initScan there are more calls to XmlTok again.
*
* RETURNS: XML_Error enum
*
* ERRNO:
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_ENTITY_DECLARED_IN_PE
*  S_xmlLib_EXTERNAL_ENTITY_HANDLING
*  S_xmlLib_MISPLACED_XML_PI
*  S_xmlLib_NO_ELEMENTS
*  S_xmlLib_NOT_STANDALONE
*  S_xmlLib_PARAM_ENTITY_REF
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_RECURSIVE_ENTITY_REF
*  S_xmlLib_SYNTAX
*  S_xmlLib_UNDEFINED_ENTITY
*  S_xmlLib_UNCLOSED_TOKEN
*/
static 
enum XML_Error FASTCALL doProlog
    (
    XML_Parser parser,
    const ENCODING *enc,
    const char *s,
    const char *end,
    int tok,
    const char *next,
    const char **nextPtr
    )
    { }

/***************************************************************************
* epilogProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_ILLEGAL_PI_TARGET
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_JUNK_AFTER_DOC_ELEMENT
*  S_xmlLib_NO_MEMORY
*  S_xmlLib_PARTIAL_CHAR
*  S_xmlLib_UNCLOSED_TOKEN
*/
static
enum XML_Error FASTCALL epilogProcessor(XML_Parser parser,
                               const char *s,
                               const char *end,
                               const char **nextPtr)
    { }

/***************************************************************************
* processInternalParamEntity - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static
enum XML_Error FASTCALL processInternalParamEntity(XML_Parser parser, ENTITY *entity)
    { }

/***************************************************************************
* errorProcessor - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO: N/A
*/
static
enum XML_Error FASTCALL errorProcessor(XML_Parser parser,
                              const char *s,
                              const char *end,
                              const char **nextPtr)
    { }

/***************************************************************************
* storeAttributeValue - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*/
static 
enum XML_Error FASTCALL storeAttributeValue(XML_Parser parser, const ENCODING *enc, XML_Bool isCdata,
                    const char *ptr, const char *end,
                    STRING_POOL *pool)
    { }

/***************************************************************************
* appendAttributeValue - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_ATTRIBUTE_EXTERNAL_ENTITY_REF
*  S_xmlLib_BAD_CHAR_REF
*  S_xmlLib_BINARY_ENTITY_REF
*  S_xmlLib_ENTITY_DECLARED_IN_PE
*  S_xmlLib_INVALID_TOKEN
*  S_xmlLib_NO_MEMORY
*  S_xmlLib_RECURSIVE_ENTITY_REF
*  S_xmlLib_UNDEFINED_ENTITY
*  S_xmlLib_UNEXPECTED_STATE
*/
static
enum XML_Error FASTCALL appendAttributeValue(XML_Parser parser, const ENCODING *enc, XML_Bool isCdata,
                     const char *ptr, const char *end,
                     STRING_POOL *pool)
    { }

/***************************************************************************
* storeEntityValue - 
*
* 
*
* RETURNS: Returns XML_Error enumeration, that has a corresponding error 
* string
*
* ERRNO:
*  S_xmlLib_NO_MEMORY
*/
static
enum XML_Error FASTCALL storeEntityValue(XML_Parser parser,
                                const ENCODING *enc,
                                const char *entityTextPtr,
                                const char *entityTextEnd)
    { }

/***************************************************************************
* normalizeLines - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL normalizeLines(XML_Char *s)
    { }

/***************************************************************************
* reportProcessingInstruction - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
int FASTCALL reportProcessingInstruction(XML_Parser parser, const ENCODING *enc,
                            const char *start, const char *end)
    { }

/***************************************************************************
* reportComment - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
int FASTCALL reportComment(XML_Parser parser, const ENCODING *enc,
              const char *start, const char *end)
    { }

/***************************************************************************
* reportDefault - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL reportDefault(XML_Parser parser, const ENCODING *enc,
              const char *s, const char *end)
    { }

/***************************************************************************
* defineAttribute - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
int FASTCALL defineAttribute(ELEMENT_TYPE *type, ATTRIBUTE_ID *attId, XML_Bool isCdata,
                XML_Bool isId, const XML_Char *value, XML_Parser parser)
    { }

/***************************************************************************
* setElementTypePrefix - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
int FASTCALL setElementTypePrefix(XML_Parser parser, ELEMENT_TYPE *elementType)
    { }

/***************************************************************************
* getAttributeId - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
ATTRIBUTE_ID * FASTCALL getAttributeId(XML_Parser parser, const ENCODING *enc,
               const char *start, const char *end)
    { }

/***************************************************************************
* getContext - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
const XML_Char * FASTCALL getContext(XML_Parser parser)
    { }

/***************************************************************************
* setContext - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static XML_Bool FASTCALL setContext(XML_Parser parser, const XML_Char *context)
    { }

/***************************************************************************
* normalizePublicId - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL normalizePublicId(XML_Char *publicId)
    { }

/***************************************************************************
* dtdInit - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static void FASTCALL dtdInit(DTD *p, XML_Parser parser)
    { }

/***************************************************************************
* dtdSwap - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL dtdSwap(DTD *p1, DTD *p2)
    { }

/***************************************************************************
* dtdDestroy - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL dtdDestroy(DTD *p, XML_Parser parser)
    { }

/***************************************************************************
* dtdCopy - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
int FASTCALL dtdCopy(DTD *newDtd, const DTD *oldDtd, XML_Parser parser)
    { }

/***************************************************************************
* copyEntityTable - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
int FASTCALL copyEntityTable(HASH_TABLE *newTable,
                           STRING_POOL *newPool,
                           const HASH_TABLE *oldTable,
                           XML_Parser parser)
    { }

/***************************************************************************
* keyeq - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
int FASTCALL keyeq(KEY s1, KEY s2)
    { }

/***************************************************************************
* hash - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
unsigned long FASTCALL hash(KEY s)
    { }

/***************************************************************************
* lookup - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
NAMED * FASTCALL lookup(HASH_TABLE *table, KEY name, size_t createSize)
    { }

/***************************************************************************
* hashTableDestroy - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static void FASTCALL hashTableDestroy(HASH_TABLE *table)
    { }

/***************************************************************************
* hashTableInit - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL hashTableInit(HASH_TABLE *p, XML_Memory_Handling_Suite *ms)
    { }

/***************************************************************************
* hashTableIterInit - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void hashTableIterInit(HASH_TABLE_ITER *iter, const HASH_TABLE *table)
    { }

/***************************************************************************
* hashTableIterNext - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
NAMED * FASTCALL hashTableIterNext(HASH_TABLE_ITER *iter)
    { }

/***************************************************************************
* poolInit - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL poolInit(STRING_POOL *pool, XML_Memory_Handling_Suite *ms)
    { }

/***************************************************************************
* poolClear - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL poolClear(STRING_POOL *pool)
    { }

/***************************************************************************
* poolDestroy - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static
void FASTCALL poolDestroy(STRING_POOL *pool)
    { }

/***************************************************************************
* poolAppend - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
XML_Char * FASTCALL poolAppend(STRING_POOL *pool, const ENCODING *enc,
                     const char *ptr, const char *end)
    { }

/***************************************************************************
* poolCopyString - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
const XML_Char * FASTCALL poolCopyString(STRING_POOL *pool, const XML_Char *s)
    { }

/***************************************************************************
* poolCopyStringN - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
const XML_Char * FASTCALL poolCopyStringN(STRING_POOL *pool,
                                       const XML_Char *s, int n)
    { }

/***************************************************************************
* poolAppendString - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
const XML_Char * FASTCALL poolAppendString(STRING_POOL *pool, const XML_Char *s)
    { }

/***************************************************************************
* poolStoreString - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
XML_Char * FASTCALL poolStoreString(STRING_POOL *pool, const ENCODING *enc,
                          const char *ptr, const char *end)
    { }

/***************************************************************************
* poolGrow - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
XML_Bool FASTCALL poolGrow(STRING_POOL *pool)
    { }

/***************************************************************************
* nextScaffoldPart - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
int FASTCALL nextScaffoldPart(XML_Parser parser)
    { }

/***************************************************************************
* build_node - 
*
* 
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
static 
void FASTCALL build_node (XML_Parser parser,
            int src_node,
            XML_Content *dest,
            XML_Content **contpos,
            XML_Char **strpos)     
    { }

/***************************************************************************
* build_model - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
XML_Content * FASTCALL build_model (XML_Parser parser)
    { }

/***************************************************************************
* getElementType - 
*
* 
*
* RETURNS: 
*
* ERRNO: N/A
*/
static
ELEMENT_TYPE * FASTCALL getElementType(XML_Parser parser,
               const ENCODING *enc,
               const char *ptr,
               const char *end)
    { }

/****************************************************************************/
/* Wind River added functions */
/****************************************************************************/

/***************************************************************************
* XML_IsWhiteSpace - detects if the src buffer is whitespace
*
* This routine detects if the src buffer is whitespace.
*
* RETURNS: XML_TRUE if src is whitespace, XML_FALSE otherwise.
*
* ERRNO: N/A
*/
XML_Bool XML_IsWhiteSpace
    (
    const char *src,
    int len
    )
    { }

