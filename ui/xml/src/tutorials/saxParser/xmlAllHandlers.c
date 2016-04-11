/* xmlAllHandlers.c - xml tutorial demonstrating the use of the Expat SAX parser*/

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,06nov03,tky 
01a,05Dec2002,tky   Written.
*/

/*
DESCRIPTION

This tutorial demonstrates the use of the Expat XML SAX parser and the XML
Output module.  Source documentation is based on the Expat source documentation
found in expat.h.  This tutorial requires the following components: 
XML SAX parser, XML output.  Or if building a project from source components,
this tutorial requires the following source files: xmlparse.c, xmlrole.c, 
xmltok.c, xmlop.c (located in INSTALL_DIR/target/src/webservices/xml).
*/

#include <errnoLib.h>
#include <stdio.h>
#include <string.h>
#include "webservices/xml/expat.h"
#include "webservices/xml/xmlop/xmlop.h"
#include "webservices/xml/xmlCanon/xmlCanon.h"

#define MAX_BUFFER_SIZE 4096

/****************************
* Used for userData example *
****************************/
typedef struct myData
    {
    BOOL myBool;        /* this variable turns handler printing on/off */
    int myTabCount;     /* this variable keeps track of the XML document hierarchy printing */
    BOOL myIndicator;   /* in this tutorial this turns on/off external entity references */
    BOOL usingNSparser; /* set to true if using the namespace parser  */
    FILE *pOutputFile;  /* file pointer for output file */
    XML_Output xmlOut;  /* pointer to xml output object */
    }MY_DATA;

/******************************
* Handler function prototypes *
******************************/
LOCAL void xmlElementDeclHandler(void *userData, const XML_Char *name, XML_Content *model);
LOCAL void xmlXmlDeclHandler(void *userData, const XML_Char *version, const XML_Char *encoding, int standalone);
LOCAL void xmlAttlistDeclHandler(void *userData, const XML_Char *elname, const XML_Char *attname, const XML_Char *att_type, const XML_Char *dflt, int isrequired);
LOCAL void xmlStartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
LOCAL void xmlEndElementHandler(void *userData, const XML_Char *name);
LOCAL void xmlEmptyElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
LOCAL void xmlCharacterDataHandler(void *userData, const XML_Char *s, int len);
LOCAL void xmlProcessingInstructionHandler(void *userData, const XML_Char *target, const XML_Char *data);
LOCAL void xmlCommentHandler(void *userData, const XML_Char *data);
LOCAL void xmlStartCdataSectionHandler(void *userData);
LOCAL void xmlEndCdataSectionHandler(void *userData);
/*LOCAL void xmlDefaultHandler(void *userData, const XML_Char *s, int len);*/
LOCAL void xmlStartDoctypeDeclHandler(void *userData, const XML_Char *doctypeName, const XML_Char *sysid, const XML_Char *pubid, int has_internal_subset);
LOCAL void xmlEndDoctypeDeclHandler(void *userData);
LOCAL void xmlEntityDeclHandler(void *userData, const XML_Char *entityName, int is_parameter_entity, const XML_Char *value, int value_length, const XML_Char *base, const XML_Char *systemId, const XML_Char *publicId, const XML_Char *notationName);
/*LOCAL void xmlUnparsedEntityDeclHandler(void *userData, const XML_Char *entityName, const XML_Char *base, const XML_Char *systemId, const XML_Char *publicId, const XML_Char *notationName);*/
LOCAL void xmlNotationDeclHandler(void *userData, const XML_Char *notationName, const XML_Char *base, const XML_Char *systemId, const XML_Char *publicId);
LOCAL void xmlStartNamespaceDeclHandler(void *userData, const XML_Char *prefix, const XML_Char *uri);
LOCAL void xmlEndNamespaceDeclHandler(void *userData, const XML_Char *prefix);
LOCAL int xmlNotStandaloneHandler(void *userData);
LOCAL int xmlExternalEntityRefHandler(XML_Parser parser, const XML_Char *context, const XML_Char *base, const XML_Char *systemId, const XML_Char *publicId);
LOCAL void xmlSkippedEntityHandler(void *userData, const XML_Char *entityName, int is_parameter_entity);
LOCAL int xmlUnknownEncodingHandler(void *encodingHandlerData, const XML_Char *name, XML_Encoding *info);
LOCAL void xmlPrintTabs(int numTabs);

/***************************************************************************
* xmlSaxParseFile - sends a file through the xml parser using the SAX interface,
* also handles external entities.
*
* Parses a file specified by pParseFileName, and writes the parser response to
* the screen (error message or accept message). If the parser variable is
* not null, then it assumes you wish to parse an external entity and follows
* accordingly.
*
* RETURNS: true if file accepted, false if an error was found.
*
* ERRNO: N/A
*/
BOOL xmlSaxParseFile
    (
    char *pBasePath,        /* base path to the file specified */
    char *pParseFileName,   /* filename to be parsed, with full relative path (relative to pBasePath) */
    XML_Parser parser       /* if you are passing the name of an external entity the parser of
                            the original parsing object must be passed along */
    )
    {
    FILE *pParseFile;               /* file pointer for file opened for parsing */
    XML_Parser xmlParser;           /* pointer to xml parser object */
    BOOL done = FALSE;              /* indicates if the entire document has been sent to parser during parsing */
    char buf[MAX_BUFFER_SIZE];      /* temp buffer to write file to to send to parser */
    MY_DATA appData;                /* user data to be sent throughout the parser */
    size_t len;                     /* temporary length storage for how much is read from file into parser */
    enum XML_Error xmlErrCode   = XML_ERROR_NONE; /* xml error code placeholder */


/*****************************************
*     Open the file to parse (read-only) *
*****************************************/
    strcpy(buf, pBasePath);
    strcat(buf, pParseFileName);
    pParseFile = fopen(buf, "r");
    if ( NULL == pParseFile)
        {
        fprintf(stderr, "%s:%d Error opening parse file %s- %s\n", __FILE__, __LINE__, buf, strerror(errnoGet()));
        return FALSE;
        }

    strcat( buf, ".output.xml" );
    appData.pOutputFile = fopen(buf, "w+");
    if ( NULL == appData.pOutputFile )
        {
        fprintf(stderr, "%s:%d Error opening output file %s- %s\n", __FILE__, __LINE__, buf, strerror(errnoGet()));
        fclose( pParseFile );
        return FALSE;
        }

    bzero(buf, MAX_BUFFER_SIZE);




/***********************************************************************************
*     If the file is an external entity, then the parser object for the original   *
*     document is passed in, this allows reuse of the parser object, and keeps the *
*     file parsing in the correct context                                          *
***********************************************************************************/
    if ( NULL != parser )
        {
        xmlParser = XML_ExternalEntityParserCreate(parser, NULL, NULL);
        /* no callback handlers needed, callbacks from the passed in parser object
        will be re-used */
        }
    else
        {
        /* initialize application data, this data is later passed throughout the
        handlers as userdata */
        appData.myBool = TRUE;
        appData.myTabCount = 0;
        appData.myIndicator = FALSE;
        appData.usingNSparser = FALSE;

/*****************************************
*         Create a new xml parser object *
*****************************************/

        /* there are 3 ways to create a new parser object:
            1. XML_ParserCreate(const XML_Char *encoding);
                - Constructs a new parser, without namespace processing;
                encoding is the encoding specified by the external
                protocol or NULL if there is none specified.

            2. XML_ParserCreateNS(const XML_Char *encoding, XML_Char namespaceSeparator);
                - Constructs a new parser and namespace processor.  Element type
                names and attribute names that belong to a namespace will be
                expanded; unprefixed attribute names are never expanded; unprefixed
                element type names are expanded only if there is a default
                namespace. The expanded name is the concatenation of the namespace
                URI, the namespace separator character, and the local part of the
                name.  If the namespace separator is '\0' then the namespace URI
                and the local part will be concatenated without any separator.
                When a namespace is not declared, the name and prefix will be
                passed through without expansion.

            3. XML_ParserCreate_MM(const XML_Char *encoding,
                    const XML_Memory_Handling_Suite *memsuite,
                    const XML_Char *namespaceSeparator);
                - Constructs a new parser using the memory management suit referred to
                by memsuite. If memsuite is NULL, then use the standard library memory
                suite. If namespaceSeparator is non-NULL it creates a parser with
                namespace processing as described above. The character pointed at
                will serve as the namespace separator.

                All further memory operations used for the created parser will come from
                the given suite.
        */

        xmlParser = XML_ParserCreateNS(NULL, NSSEP );
        appData.usingNSparser = TRUE;
        /* Create an XML Output object, by specifying the encoding type, the 
        namespace separator character, and the file that XML Output will be 
        written to.  Note: the XML Output module is not related to the printf's
        occurring throughout the handlers, the XML Output module API's are
        identical to the SAX API's, except with Write rather than Handler at the
        end of the function name, 
        eg. XML_ElementDeclWrite/XML_ElementDeclHandler*/
        appData.xmlOut = XML_OutputCreate( "UTF-8", NSSEP, appData.pOutputFile );
        XML_OutputFormatSet( appData.xmlOut, XML_TRUE );


/*****************************************************************************
*         Setup all the SAX event handlers                                   *
*             - see the individual handler functions below for documentation *
*             on what each of them do and how to use their arguments         *
*****************************************************************************/
        XML_SetElementDeclHandler(xmlParser, xmlElementDeclHandler);

        XML_SetAttlistDeclHandler(xmlParser, xmlAttlistDeclHandler);

        XML_SetXmlDeclHandler(xmlParser, xmlXmlDeclHandler);

        XML_SetEntityDeclHandler(xmlParser, xmlEntityDeclHandler);

        XML_SetElementHandler(xmlParser, xmlStartElementHandler, xmlEndElementHandler);

        XML_SetEmptyElementHandler(xmlParser, xmlEmptyElementHandler);

        XML_SetCharacterDataHandler(xmlParser, xmlCharacterDataHandler);

        XML_SetProcessingInstructionHandler(xmlParser, xmlProcessingInstructionHandler);

        XML_SetCommentHandler(xmlParser, xmlCommentHandler);

        XML_SetCdataSectionHandler(xmlParser, xmlStartCdataSectionHandler, xmlEndCdataSectionHandler);

        /* left out so that other handlers may be showcased */
        /*XML_SetDefaultHandler(xmlParser, xmlDefaultHandler);*/
        /*XML_SetDefaultHandlerExpand(xmlParser, xmlDefaultHandler);*/

        XML_SetDoctypeDeclHandler(xmlParser, xmlStartDoctypeDeclHandler, xmlEndDoctypeDeclHandler);

        /* OBSOLETE */
        /*XML_SetUnparsedEntityDeclHandler(xmlParser, xmlUnparsedEntityDeclHandler);*/

        XML_SetNotationDeclHandler(xmlParser, xmlNotationDeclHandler);

        XML_SetNamespaceDeclHandler(xmlParser, xmlStartNamespaceDeclHandler, xmlEndNamespaceDeclHandler);

        XML_SetNotStandaloneHandler(xmlParser, xmlNotStandaloneHandler);

        XML_SetExternalEntityRefHandler(xmlParser, xmlExternalEntityRefHandler);

        XML_SetSkippedEntityHandler(xmlParser, xmlSkippedEntityHandler);

        XML_SetUnknownEncodingHandler(xmlParser, xmlUnknownEncodingHandler, NULL);

/************************************
*         Other configuration items *
************************************/
        /*If the second argurment is non-zero, and namespace processing is in effect, 
        and a name has a prefix (i.e. an explicit namespace qualifier)
        then that name is returned as a triplet in a single string separated
        by the separator character specified when the parser was created: 
            URI + sep + local_name + sep + prefix.
        If the second argument is zero, then namespace information is returned in the 
        default manner (URI + sep + local_name) whether or not the name has 
        a prefix.
        
        Note: Calling XML_SetReturnNSTriplet after XML_Parse or XML_ParseBuffer has no effect.*/
        XML_SetReturnNSTriplet(xmlParser, 0);

        /* this will set the document encoding expected, the following encodings are supported
        by Expat, but untested by WindRiver:
                KW_ISO_8859_1,
                KW_US_ASCII,
                KW_UTF_16,
                KW_UTF_16BE,
                KW_UTF_16LE
        Only KW_UTF_8 has been fully tested.  The encoding is set by passing the previous
        described strings to the function as the second argument.*/
        XML_SetEncoding(xmlParser, NULL);

        /* this will cause the pointer passed as the second argument to be passed throughout
        the handlers as the void pointer userData argument, i.e. this allows you to pass
        you own data throughout the handlers, as most handlers have the userData argument.
        In this tutorial, the MY_DATA appData structure is used throughout the handlers to
        pass along state information, and to enable/disable tutorial features*/
        XML_SetUserData(xmlParser, (void *)&appData);

        /* this will cause the pointer to your parser object to be passed as the userData
        argument amongst handlers */
        /*XML_UseParserAsHandlerArg(xmlParser);*/

        /* If useDTD == XML_TRUE is passed to this function, then the parser
           will assume that there is an external subset, even if none is
           specified in the document. In such a case the parser will call the
           externalEntityRefHandler with a value of NULL for the systemId
           argument (the publicId and context arguments will be NULL as well).
           Note: If this function is called, then this must be done before
             the first call to XML_Parse or XML_ParseBuffer, since it will
             have no effect after that.  Returns
             XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING.
           Note: If the document does not have a DOCTYPE declaration at all,
             then startDoctypeDeclHandler and endDoctypeDeclHandler will not
             be called, despite an external subset being parsed.
           Note: If XML_DTD is not defined when Expat is compiled, returns
             XML_ERROR_FEATURE_REQUIRES_XML_DTD.
        */
        XML_UseForeignDTD(xmlParser, XML_FALSE);

        /* this will set the mode of parameter entity parsing to be used by the parser
        see expat.h or xmlparse.c for a more detailed description */
        if( XML_SetParamEntityParsing(xmlParser, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE) == 0)
            {
            printf("Parameter entity parsing request failed, this may be due to exclusion of DTD functionality, see xmlConfig.h.\n");
            }

        /* this will cause the string passed into the second argument to be passed throughout
        some of the handlers as the const XML_Char *base argument.  This is typically used
        for relative path accessing when referencing external entities */
        if( 0 == XML_SetBase(xmlParser, pBasePath) )
            {
            fprintf(stderr, "Error: XML Parser is out of memory, could not allocate memory for SetBase");
            xmlErrCode = XML_GetErrorCode(xmlParser);
            goto die;
            }

        }

/**************************************
*     Begin parsing your XML document *
**************************************/
    while(!done)
        {
        len = fread(buf, 1, sizeof(buf), pParseFile);
        done = len < sizeof(buf);

        if ( XML_Parse(xmlParser, buf, len, done) == 0 )
            {
/*************************************
*             Error handling example *
*************************************/
            xmlErrCode = XML_GetErrorCode(xmlParser);
            if (xmlErrCode != 0)
                {
                printf("File(%s)Error(%d):<%s> at line %d, col %d\n",
                        pParseFileName,
                        xmlErrCode,
                        XML_ErrorString(xmlErrCode),
                        XML_GetCurrentLineNumber(xmlParser),
                        XML_GetCurrentColumnNumber(xmlParser));
                }
            }

        }

/******************************************
*     Return your resources to the system *
******************************************/
die:
    XML_ParserFree(xmlParser);
    XML_OutputDestroy(appData.xmlOut);
    fclose(pParseFile);
    fclose(appData.pOutputFile);

/**********************************************************
*     Provide an indication on the success of the parsing *
**********************************************************/
    if ( xmlErrCode == XML_ERROR_NONE )
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }


    }

/*************************************************
*         Functions you can call inside handlers *
*************************************************/
/*      XML_ParserReset(XML_Parser parser, const XML_Char *encoding);
        XML_DefaultCurrent(XML_Parser parser);
        XML_GetSpecifiedAttributeCount(XML_Parser parser);
        XML_GetIdAttributeIndex(XML_Parser parser);*/


/***************************************************************************
* xmlElementDeclHandler - This is called on an !ELEMENT declaration
*
* This is called on an !ELEMENT declaration.  It is the caller's
* responsiblity to free model when finished with it.  Below is the structure
* for model:
*  struct XML_cp {
*  enum XML_Content_Type         type;
*  enum XML_Content_Quant        quant;
*  XML_Char *                    name;
*  unsigned int                  numchildren;
*  XML_Content *                 children;
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void xmlElementDeclHandler
    (
    void *userData,         /* user defined data */
    const XML_Char *name,   /* name of the !ELEMENT */
    XML_Content *model      /* structure containing ELEMENT related data */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the Element declartion to the XML output file */
    XML_ElementDeclWrite( ((MY_DATA*)userData)->xmlOut, name, model );

    if( printThis == TRUE )
        {
        printf("XML ELEMENT DECLARATION HANDLER called\n");
        printf("!ELEMENT name = %s\n", name);
        printf("\n");
        }

    /* It is the caller's responsibility to free model when finished */
    if(model != NULL)
        {
        free(model);
        }
    }

/***************************************************************************
* xmlXmlDeclHandler - This is called for both XML and TEXT declarations
*
* The XML declaration handler is called for *both* XML declarations
* and text declarations. The way to distinguish is that the version
* parameter will be NULL for text declarations. The encoding
* parameter may be NULL for XML declarations. The standalone
* parameter will be -1, 0, or 1 indicating respectively that there
* was no standalone parameter in the declaration, that it was given
* as no, or that it was given as yes.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlXmlDeclHandler
    (
    void *userData,             /* user defined data */
    const XML_Char *version,    /* XML version */
    const XML_Char *encoding,   /* XML document encoding type */
    int standalone              /* bool indicating if document is standalone */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the XML declartion to the XML output file */
    XML_XmlDeclWrite( ((MY_DATA*)userData)->xmlOut, version, encoding, standalone );

    if( printThis == TRUE )
        {
        if(version != NULL)
            {
            printf("XML XML DECLARATION HANDLER called\n");
            printf("Version = %s, Encoding = %s\n", version, encoding);
            }
        else
            {
            printf("XML XML(TEXT) DECLARATION HANDLER called\n");
            printf("Encoding = %s ", encoding);
            }

        switch(standalone)
            {
            case -1:
                printf("standalone = no standalone parameter\n");
                break;
            case 0:
                printf("standalone = no\n");
                break;
            case 1:
                printf("standalone = yes\n");
                break;
            default:
                break;
            }

        }
    }

/***************************************************************************
* xmlAttlistDeclHandler - This is called for each ATTRIBUTE in an ATTLIST
*
* The Attlist declaration handler is called for *each* attribute. So
* a single Attlist declaration with multiple attributes declared will
* generate multiple calls to this handler. The "default" parameter
* may be NULL in the case of the "#IMPLIED" or "#REQUIRED"
* keyword. The "isrequired" parameter will be true and the default
* value will be NULL in the case of "#REQUIRED". If "isrequired" is
* true and default is non-NULL, then this is a "#FIXED" default.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlAttlistDeclHandler
    (
    void *userData,             /* user defined data */
    const XML_Char *elname,     /* ELEMENT name that ATTLIST pertains to */
    const XML_Char *attname,    /* ATTRIBUTE name */
    const XML_Char *att_type,   /* ATTRIBUTE data type */
    const XML_Char *dflt,       /* DEFAULT value, if applicable */
    int isrequired              /* bool indicating #REQUIRED */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the Attlist declaration to the XML output file */
    XML_AttlistDeclWrite( ((MY_DATA*)userData)->xmlOut, elname, attname,
                          att_type, dflt, isrequired );


    if( printThis == TRUE )
        {
        printf("XML ATTLIST HANDLER called\n");
        printf("ELEMENT name = %s\n", elname);
        printf("ATTRIBUTE name = %s, ", attname);

        if(isrequired != 0)
            {
            if(dflt == NULL)
                printf("#REQUIRED\n");
            else
                printf("#FIXED\n");
            }
        else if(dflt == NULL)
            {
            printf("#IMPLIED\n");
            }
        else
            {
            printf("default = %s\n", dflt);
            }

        printf("ATTRIBUTE type = %s\n", att_type);
        }

    }



/***************************************************************************
* xmlStartElementHandler - This is called at the beginning of an XML element
*
* This is called at the beginning of an XML element.  Atts is an array of
* name/value pairs, i.e. atts[0] contains name, atts[1] contains value for
* atts[0], and so on... Names and values are NULL terminated.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlStartElementHandler
    (
    void *userData,         /* user defined data */
    const XML_Char *name,   /* Element name */
    const XML_Char **atts   /* Element Attribute list, provided in name value pairs */
                            /* i.e. atts[0] contains name, atts[1] contains value for */
                            /* atts[0], and so on...*/
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;
    int i=0;

    /* Write the start element tag to the XML output file */
    XML_StartElementWrite( ((MY_DATA*)userData)->xmlOut, name, atts );

    ((MY_DATA*)userData)->myTabCount++;

    if( printThis == TRUE )
        {
        xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
        printf("XML START ELEMENT HANDLER called\n");
        xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
        printf("Element name = %s\n", name);

        for(i=0;atts[i] != NULL; i=i+2)
            {
            xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
            printf("Attribute%d %s = %s\n", i/2, atts[i], atts[i+1]);
            }
        }

    }


/***************************************************************************
* xmlEndElementHandler - This is called at the end of an XML element
*
* This is called at the end of an XML element. Name contains the element
* name that is closing.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlEndElementHandler
    (
    void *userData,     /* user defined data */
    const XML_Char *name/* Element name */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the end element tag to the XML output file */
    XML_EndElementWrite( ((MY_DATA*)userData)->xmlOut, name );

    if( printThis == TRUE )
        {
        xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
        printf("XML END %s ELEMENT HANDLER called\n", name);
        }

    ((MY_DATA*)userData)->myTabCount--;
    }


/***************************************************************************
* xmlEmptyElementHandler - This is called when an empty element is
* encountered
*
* This is called when an empty element is encountered, such as: <element/>
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlEmptyElementHandler
    (
    void *userData,         /* user defined data */
    const XML_Char *name,   /* Element name */
    const XML_Char **atts   /* Element Attribute list, provided in name value pairs */
                            /* i.e. atts[0] contains name, atts[1] contains value for */
                            /* atts[0], and so on...*/
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;
    int i = 0;

    /* Write the start and end element tags to the XML output file */
    XML_StartElementWrite( ((MY_DATA*)userData)->xmlOut, name, atts );
    XML_EndElementWrite( ((MY_DATA*)userData)->xmlOut, name );

    if( printThis == TRUE )
        {
        xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
        printf("XML EMPTY ELEMENT HANDLER called\n");
        xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
        printf("Element name = %s\n", name);

        for(i=0;atts[i] != NULL; i=i+2)
            {
            xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
            printf("Attribute%d %s = %s\n", i, atts[i], atts[i+1]);
            }
        }

    }

/***************************************************************************
* xmlCharacterDataHandler - This is called when XML element character
* content is encountered
*
* This is called when XML element character content is encountered. The s
* XML_Char string is not NULL terminated.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlCharacterDataHandler
    (
    void *userData,     /* user defined data */
    const XML_Char *s,  /* non-null terminated character string */
    int len             /* length of s */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;
    int i=0;

    /* Write the encountered character data to the XML output file */
    XML_CharacterDataWrite( ((MY_DATA*)userData)->xmlOut, s, len );

    if( printThis == TRUE )
        {
        if(len > 0 && !XML_IsWhiteSpace(s, len))
            {
            xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
            printf("XML CHARACTER DATA HANDLER called\n");
            xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
            printf("Character length = %d\n", len);
            xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
            printf("Character data = ");
            for(i=0; i < len; i++)
                {
                printf("%c", s[i]);
                }

            printf("\n");
            }
/* the character data handler is also called on whitespace occurrences
   removing this code removes capturing that event
        else
            {
            if(len <= 0)
                {
                xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
                printf("No Character data\n");
                }
            else if(XML_IsWhiteSpace(s,len))
                {
                xmlPrintTabs(((MY_DATA*)userData)->myTabCount);
                printf("All characters are white spaces\n");
                }
            }
*/

        }
    }

/***************************************************************************
* xmlProcessingInstructionHandler - This is called when a <? ... ?> element,
* or processing instruction is encountered
*
* This is called when a <? ... ?> element, or processing instruction is
* encountered. target and data are NULL terminated.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlProcessingInstructionHandler
    (
    void *userData,         /* user defined data */
    const XML_Char *target, /* Processing Instruction target */
    const XML_Char *data    /* Processing Instruction data */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the encountered processing instruction to the XML output file */
    XML_ProcessingInstructionWrite( ((MY_DATA*)userData)->xmlOut, target, data );

    if( printThis == TRUE )
        {
        printf("XML PROCESSING INSTRUCTION HANDLER called\n");
        printf("PITarget = %s, PIData = %s\n", target, data);
        printf("\n");
        }

    }

/***************************************************************************
* xmlCommentHandler - This is called when a <!-- ... --> element, or XML
* comment is encountered
*
* This is called when a <!-- ... --> element, or XML comment is encountered.
* data is NULL terminated.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlCommentHandler
    (
    void *userData,     /* user defined data */
    const XML_Char *data/* XML comment contents */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the encountered XML comment to the XML output file */
    XML_CommentWrite( ((MY_DATA*)userData)->xmlOut, data );

    if( printThis == TRUE )
        {
        printf("XML COMMENT HANDLER called\n");
        printf("Comment: %s\n", data);
        printf("\n");
        }

    }


/***************************************************************************
* xmlStartCdataSectionHandler - This is called when the beginning of a
* <! [CDATA[....]]>, or CDATA section is encountered.
*
* This is called when the beginning of a <! [CDATA[....]]>, or CDATA section
* is encountered.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlStartCdataSectionHandler(void *userData)
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the start CDATA section declaration to the XML output file */
    XML_StartCdataSectionWrite( ((MY_DATA*)userData)->xmlOut );

    if( printThis == TRUE )
        {
        printf("XML START CDATA SECTION HANDLER called\n");
        }

    }

/***************************************************************************
* xmlEndCdataSectionHandler - This is called when the end of a
* <! [CDATA[....]]>, or CDATA section is encountered.
*
* This is called when the end of a <! [CDATA[....]]>, or CDATA section
* is encountered.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlEndCdataSectionHandler(void *userData)
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the end CDATA section declaration to the XML output file */
    XML_EndCdataSectionWrite( ((MY_DATA*)userData)->xmlOut );

    if( printThis == TRUE )
        {
        printf("XML END CDATA SECITON HANDLER called\n");
        printf("\n");
        }

    }


/***************************************************************************
* xmlDefaultHandler - This is called for any characters in the XML document
* for which there is no applicable handler.
*
* This is called for any characters in the XML document for which
* there is no applicable handler.  This includes both characters that
* are part of markup which is of a kind that is not reported
* (comments, markup declarations), or characters that are part of a
* construct which could be reported but for which no handler has been
* supplied. The characters are passed exactly as they were in the XML
* document except that they will be encoded in UTF-8 or UTF-16.
* Line boundaries are not normalized. Note that a byte order mark
* character is not passed to the default handler. There are no
* guarantees about how characters are divided between calls to the
* default handler: for example, a comment might be split between
* multiple calls.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
/* this function has been purposely compiled out due to the nature of its 
functionality explained above */
#if 0
LOCAL void xmlDefaultHandler
    (
    void *userData,     /* user defined data */
    const XML_Char *s,  /* non-null terminated string containing XML content */
                        /* default handler was called regarding */
    int len
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    if( printThis == TRUE )
        {
        printf("XML DEFAULT HANDLER called\n");
        printf("XML Data = %s\n", s);
        printf("\n");
        }
    return;
    }
#endif

/***************************************************************************
* xmlStartDoctypeDeclHandler - This is called for the start of the DOCTYPE
* declaration, before any DTD or internal subset is parsed.
*
* This is called for the start of the DOCTYPE declaration, before any DTD or
* internal subset is parsed. doctypeName, sysid and pubid are all NULL
* terminated.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlStartDoctypeDeclHandler
    (
    void *userData,             /* user defined data */
    const XML_Char *doctypeName,/* DOCTYPE name */
    const XML_Char *sysid,      /* SYSTEM attribute contents */
    const XML_Char *pubid,      /* PUBLIC attribute contents */
    int has_internal_subset     /* bool indicating if internal or external subset */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the start DOCTYPE declaration to the XML output file */
    XML_StartDoctypeDeclWrite( ((MY_DATA*)userData)->xmlOut, doctypeName, sysid, pubid, has_internal_subset);

    if( printThis == TRUE )
        {
        printf("XML START DOCTYPE HANDLER called\n");
        printf("Doctype name = %s, sysId = %s, pubId = %s\n", doctypeName, sysid, pubid);
        }

    }

/***************************************************************************
* xmlEndDoctypeDeclHandler - This is called for the start of the DOCTYPE
* declaration when the closing > is encountered, but after processing any
* external subset.
*
* This is called for the start of the DOCTYPE declaration when the closing >
* is encountered, but after processing any external subset.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlEndDoctypeDeclHandler(void *userData)
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the end DOCTYPE declaration to the XML output file */
    XML_EndDoctypeDeclWrite( ((MY_DATA*)userData)->xmlOut );

    if( printThis == TRUE )
        {
        printf("XML END DOCTYPE HANDLER called\n");
        printf("\n");
        }

    }

/***************************************************************************
* xmlEntityDeclHandler - This is called for !ENTITY declarations
*
* This is called for entity declarations. The is_parameter_entity
* argument will be non-zero if the entity is a parameter entity, zero
* otherwise.
*
* For internal entities (<!ENTITY foo "bar">), value will
* be non-NULL and systemId, publicID, and notationName will be NULL.
* The value string is NOT nul-terminated; the length is provided in
* the value_length argument. Since it is legal to have zero-length
* values, do not use this argument to test for internal entities.
*
* For external entities, value will be NULL and systemId will be
* non-NULL. The publicId argument will be NULL unless a public
* identifier was provided. The notationName argument will have a
* non-NULL value only for unparsed entity declarations.
*
* Note that is_parameter_entity can't be changed to XML_Bool, since
* that would break binary compatibility.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlEntityDeclHandler
    (
    void *userData,             /* user defined data */
    const XML_Char *entityName, /* ENTITY name */
    int is_parameter_entity,    /* bool indicating parameter entity */
    const XML_Char *value,      /* ENTITY value, not null if internal entity, null */
                                /* if external entity, this string is non-null terminated */
    int value_length,           /* length of value */
    const XML_Char *base,       /* base path to entity set by XML_SetBase*/
    const XML_Char *systemId,   /* SYSTEM attribute contents */
    const XML_Char *publicId,   /* PUBLIC attribute contents */
    const XML_Char *notationName/* NOTATION attribute contents */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;
    int i=0;

    /* Write the ENTITY declaration to the XML output file */
    XML_EntityDeclWrite( ((MY_DATA*)userData)->xmlOut, entityName,
                         is_parameter_entity, value, value_length, base,
                         systemId, publicId, notationName );

    if( printThis == TRUE )
        {
        printf("XML ENTITY DECLARATION HANDLER called\n");

        /* If it is a parameter entity */
        if(is_parameter_entity != 0)
            {
            /* if it is an internal parameter entity */
            if(systemId == NULL && publicId == NULL && notationName == NULL)
                {
                printf("PARAMETER INTERNAL entity name = %s\n", entityName);
                printf("Entity value = ");
                
                for(i=0; i < value_length; i++)
                    {
                    printf("%c", value[i]);
                    }
                
                printf("\n");
                printf("Entity base = %s\n", base);
                }
            /* if it is an external parameter entity */
            else
                {
                printf("PARAMETER EXTERNAL entity name = %s\n", entityName);
                printf("Entity systemId = %s\n", systemId);

                if(publicId != NULL)
                    {
                    printf("Entity publicId = %s\n", publicId);
                    }

                if(notationName != NULL)
                    {
                    printf("Entity notationName = %s\n", notationName);
                    }
                }
            }
        else
            {
            /* if it is an internal entity */
            if(systemId == NULL && publicId == NULL && notationName == NULL)
                {
                printf("INTERNAL entity name = %s\n", entityName);
                printf("Entity value = ");
                
                for(i=0; i < value_length; i++)
                    {
                    printf("%c", value[i]);
                    }
                
                printf("\n");
                printf("Entity base = %s\n", base);
                }
            /* if it is an external entity */
            else
                {
                printf("EXTERNAL entity name = %s\n", entityName);
                printf("Entity systemId = %s\n", systemId);

                if(publicId != NULL)
                    {
                    printf("Entity publicId = %s\n", publicId);
                    }

                if(notationName != NULL)
                    {
                    printf("Entity notationName = %s\n", notationName);
                    }
                }
            }

        }

    }

/***************************************************************************
* xmlUnparsedEntityDeclHandler - OBSOLETE, This handler has been superceded
* by the EntityDeclHandler above.
*
* OBSOLETE -- OBSOLETE -- OBSOLETE
* This handler has been superceded by the EntityDeclHandler above.
* It is provided here for backward compatibility.
*
* This is called for a declaration of an unparsed (NDATA) entity.
* The base argument is whatever was set by XML_SetBase. The
* entityName, systemId and notationName arguments will never be
* NULL. The other arguments may be.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
#if 0
LOCAL void xmlUnparsedEntityDeclHandler
    (
    void *userData,             /* user defined data */
    const XML_Char *entityName, /* ENTITY name */
    const XML_Char *base,       /* base path to entity set by XML_SetBase*/
    const XML_Char *systemId,   /* SYSTEM attribute contents */
    const XML_Char *publicId,   /* PUBLIC attribute contents */
    const XML_Char *notationName/* NOTATION attribute contents */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    if( printThis == TRUE )
        {
        printf("XML UNPARSED ENTITY DECLARATION HANDLER called\n");
        printf("Entity Name = %s, base = %s\n", entityName, base);
        printf("sytemId = %s, publicId = %s, Notation Name = %s\n", systemId, publicId, notationName);
        printf("\n");
        }

    }
#endif

/***************************************************************************
* xmlNotationDeclHandler - This is called for a declaration of !NOTATION
*
* This is called for a declaration of notation.  The base argument is
* whatever was set by XML_SetBase. The notationName will never be
* NULL.  The other arguments can be.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlNotationDeclHandler
    (
    void *userData,                 /* user defined data */
    const XML_Char *notationName,   /* NOTATION attribute contents */
    const XML_Char *base,           /* base path to entity set by XML_SetBase*/
    const XML_Char *systemId,       /* SYSTEM attribute contents */
    const XML_Char *publicId        /* PUBLIC attribute contents */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the NOTATION declaration to the XML output file */
    XML_NotationDeclWrite( ((MY_DATA*)userData)->xmlOut, notationName, base,
                           systemId, publicId );


    if( printThis == TRUE )
        {
        printf("XML NOTATION DECLARATION HANDLER called\n");
        printf("Notation Name = %s, base = %s, systemId = %s, publicId = %s\n", notationName, base, systemId, publicId);
        printf("\n");
        }

    }

/***************************************************************************
* xmlStartNamespaceDeclHandler - When namespace processing is enabled, this
* is called once for each namespace declaration.
*
* When namespace processing is enabled, this is called once for
* each namespace declaration. The call to the start and end element
* handlers occur between the calls to the start and end namespace
* declaration handlers. For an xmlns attribute, prefix will be
* NULL.  For an xmlns="" attribute, uri will be NULL.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlStartNamespaceDeclHandler
    (
    void *userData,         /* user defined data */
    const XML_Char *prefix, /* prefix representing namespace */
    const XML_Char *uri     /* URI for the namespace (unique identifier) */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the encountered namespace declaration to the XML output file */
    XML_StartNamespaceDeclWrite( ((MY_DATA*)userData)->xmlOut, prefix, uri );

    if( printThis == TRUE )
        {
        printf("XML START NAMESPACE DECLARATION HANDLER called\n");
        printf("Prefix = %s, URI = %s\n", prefix, uri);
        printf("\n");
        }

    }

/***************************************************************************
* xmlEndNamespaceDeclHandler - When namespace processing is enabled, this
* is called once for each namespace declaration.
*
* When namespace processing is enabled, this is called once for
* each namespace declaration. The call to the start and end element
* handlers occur between the calls to the start and end namespace
* declaration handlers. For an xmlns attribute, prefix will be
* NULL.  For an xmlns="" attribute, uri will be NULL.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlEndNamespaceDeclHandler
    (
    void *userData,         /* user defined data */
    const XML_Char *prefix  /* prefix representing namespace */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    /* Write the end namespace declaration to the XML output file */
    XML_EndNamespaceDeclWrite( ((MY_DATA*)userData)->xmlOut, prefix );

    if( printThis == TRUE )
        {
        printf("XML END NAMESPACE DECLARATION HANDLER called\n");
        printf("Prefix = %s\n", prefix);
        printf("\n");
        }

    }

/***************************************************************************
* xmlNotStandaloneHandler - This is called if the document is not standalone
*
* This is called if the document is not standalone, that is, it has an
* external subset or a reference to a parameter entity, but does not
* have standalone="yes". If this handler returns 0, then processing
* will not continue, and the parser will return a
* XML_ERROR_NOT_STANDALONE error.
* If parameter entity parsing is enabled, then in addition to the
* conditions above this handler will only be called if the referenced
* entity was actually read.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL int xmlNotStandaloneHandler(void *userData)
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    if( printThis == TRUE )
        {
        printf("XML NOTSTANDALONE HANDLER called\n");
        printf("\n");
        }

    return XML_STATUS_ERROR;
    }

/***************************************************************************
* xmlExternalEntityRefHandler - This is called for a reference to an
* external parsed general entity.
*
* This is called for a reference to an external parsed general
* entity.  The referenced entity is not automatically parsed.  The
* application can parse it immediately or later using
* XML_ExternalEntityParserCreate.
*
* The parser argument is the parser parsing the entity containing the
* reference; it can be passed as the parser argument to
* XML_ExternalEntityParserCreate.  The systemId argument is the
* system identifier as specified in the entity declaration; it will
* not be NULL.
*
* The base argument is the system identifier that should be used as
* the base for resolving systemId if systemId was relative; this is
* set by XML_SetBase; it may be NULL.
*
* The publicId argument is the public identifier as specified in the
* entity declaration, or NULL if none was specified; the whitespace
* in the public identifier will have been normalized as required by
* the XML spec.
*
* The context argument specifies the parsing context in the format
* expected by the context argument to XML_ExternalEntityParserCreate;
* context is valid only until the handler returns, so if the
* referenced entity is to be parsed later, it must be copied.
*
* The handler should return 0 if processing should not continue
* because of a fatal error in the handling of the external entity.
* In this case the calling parser will return an
* XML_ERROR_EXTERNAL_ENTITY_HANDLING error.
*
* Note that unlike other handlers the first argument is the parser,
* not userData.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL int xmlExternalEntityRefHandler
    (
    XML_Parser parser,          /* XML parser object currently parsing the docuemnt */
    const XML_Char *context,    /* pointer to XML document context */
    const XML_Char *base,       /* base path to entity set by XML_SetBase*/
    const XML_Char *systemId,   /* SYSTEM attribute contents */
    const XML_Char *publicId    /* PUBLIC attribute contents */
    )
    {
    BOOL printThis = ((MY_DATA*)XML_GetUserData(parser))->myBool;
    BOOL parseExtEntity = ((MY_DATA*)XML_GetUserData(parser))->myIndicator;
    BOOL parseContext = ((MY_DATA*)XML_GetUserData(parser))->usingNSparser;

    if (TRUE == parseContext)
        {
        XML_Char *extEntityName;
        XML_Char *i;

        /*  parse for entity name  */
        for ( i = (XML_Char *)context; *i != '\0'; i++ )
            {
            if ( '\f' == *i )
                {
                extEntityName = &(i[1]);
                }
            }
        /* Write the entity reference to the XML output file */
        XML_EntityRefWrite(  ((MY_DATA*)XML_GetUserData(parser))->xmlOut, extEntityName );
        } 
    else
        {
        /* Write the entity reference to the XML output file */
        XML_EntityRefWrite(  ((MY_DATA*)XML_GetUserData(parser))->xmlOut, context );
        }



    if( printThis == TRUE )
        {
        printf("XML EXTERNAL ENTITY HANDLER called\n");
        printf("Context = %s, base = %s, sytemId = %s, publicId = %s\n", context, base, systemId, publicId);
        if(parseExtEntity == TRUE)
            {
            /* Parse the file pointed to by external entity reference */
            xmlSaxParseFile( (char *)base, (char *)systemId, parser);
            }
        printf("\n");
        }
    return XML_STATUS_OK;
    }

/***************************************************************************
* xmlSkippedEntityHandler - This is called for special entity reference
* situations
*
* This is called in two situations:
*   1) An entity reference is encountered for which no declaration
*      has been read *and* this is not an error.
*   2) An internal entity reference is read, but not expanded, because
*      XML_SetDefaultHandler has been called.
*   Note: skipped parameter entities in declarations and skipped general
*         entities in attribute values cannot be reported, because
*         the event would be out of sync with the reporting of the
*         declarations or attribute values
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlSkippedEntityHandler
    (
    void *userData,             /* user defined data */
    const XML_Char *entityName, /* ENTITY name */
    int is_parameter_entity     /* bool indicating parameter entity */
    )
    {
    BOOL printThis = ((MY_DATA*)userData)->myBool;

    if( printThis == TRUE )
        {
        printf("XML SKIPPED ENTITY HANDLER called\n");
        printf("Entity name = %s\n", entityName);
        printf("\n");
        }

    }

/***************************************************************************
* xmlUnknownDecodingHandler - This is called for an encoding that is unknown
* to the parser
*
* This is called for an encoding that is unknown to the parser.
*
* The encodingHandlerData argument is that which was passed as the
* second argument to XML_SetUnknownEncodingHandler.
*
* The name argument gives the name of the encoding as specified in
* the encoding declaration.
*
* If the callback can provide information about the encoding, it must
* fill in the XML_Encoding structure, and return 1.  Otherwise it
* must return 0.
*
* If info does not describe a suitable encoding, then the parser will
* return an XML_UNKNOWN_ENCODING error.
*
* Filling in the XML_Encoding structure:
*
*
* This structure is filled in by the XML_UnknownEncodingHandler to
* provide information to the parser about encodings that are unknown
* to the parser.
*
* The map[b] member gives information about byte sequences whose
* first byte is b.
*
* If map[b] is c where c is >= 0, then b by itself encodes the
* Unicode scalar value c.
*
* If map[b] is -1, then the byte sequence is malformed.
*
* If map[b] is -n, where n >= 2, then b is the first byte of an
* n-byte sequence that encodes a single Unicode scalar value.
*
* The data member will be passed as the first argument to the convert
* function.
*
* The convert function is used to convert multibyte sequences; s will
* point to a n-byte sequence where map[(unsigned char)*s] == -n.  The
* convert function must return the Unicode scalar value represented
* by this byte sequence or -1 if the byte sequence is malformed.
*
* The convert function may be NULL if the encoding is a single-byte
* encoding, that is if map[b] >= -1 for all bytes b.
*
* When the parser is finished with the encoding, then if release is
* not NULL, it will call release passing it the data member; once
* release has been called, the convert function will not be called
* again.
*
* Expat places certain restrictions on the encodings that are supported
* using this mechanism.
*
* 1. Every ASCII character that can appear in a well-formed XML document,
*   other than the characters
*
*   $@\^`{}~
*
*   must be represented by a single byte, and that byte must be the
*   same byte that represents that character in ASCII.
*
* 2. No character may require more than 4 bytes to encode.
*
* 3. All characters encoded must have Unicode scalar values <=
*   0xFFFF, (i.e., characters that would be encoded by surrogates in
*   UTF-16 are  not allowed).  Note that this restriction doesn't
*   apply to the built-in support for UTF-8 and UTF-16.
*
* 4. No Unicode character may be encoded by more than one distinct
*   sequence of bytes.
*
*   typedef struct {
*     int map[256];
*     void *data;
*     int (*convert)(void *data, const char *s);
*     void (*release)(void *data);
*   } XML_Encoding;
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL int xmlUnknownEncodingHandler
    (
    void *encodingHandlerData,  /* user data */
    const XML_Char *name,       /* encoding name */
    XML_Encoding *info          /* structure containing encoding info */
    )
    {
    printf("XML UNKNOWN ENCODING HANDLER called\n");
    printf("name = %s\n", name);
    printf("\n");

    return XML_STATUS_ERROR;
    }


/***************************************************************************
* xmlPrintTabs - Prints to stdout the number of tabs specified by numTabs
*
* Prints to stdout the number of tabs specified by numTabs
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void xmlPrintTabs(int numTabs)
    {
    int i=0;

    for(i=1; i<numTabs; i++)
        {
        printf("\t");
        }

    }

void run_xmlAllHandlersTutorial()
    {
    char *basePath = "";
    char *xmlFile  = "xmlAllHandlers.xml";

    xmlSaxParseFile(basePath, xmlFile, NULL);
    }
