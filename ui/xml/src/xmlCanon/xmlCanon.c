/* xmlCanon.c - XML parser canonicalization module*/

/* Copyright 2002-2003, 2005, 2009, 2013 Wind River Systems, Inc. */
/*
Copyright (c) 1998, 1999 Thai Open Source Software Center Ltd
See the file COPYING for copying permission.
*/

/*
modification history
--------------------
01w,09aug13,f_f  fixed some coverity issues (WIND00429907)
01v,29mar13,f_f  fixed the Coverity issue (WIND00410897)
01u,30mar09,f_f  upgrade to Expat 2.0.1
01t,23feb05,pas  added in-file comments from clearcase checkin comments
01s,11apr03,tky  code review changes
01r,07apr03,tky  remove dependencies on codepage.c and filemap.c
01q,31mar03,tky  some xml output API's changed
01p,29jan03,tky  - moved all Canonicalization functionality into the output
		 module, this option to canonicalize can be toggled on/off
		 based on a switch
		 - second canonical form is also based on a switch
		 - the xmlCanon module now is completely dependent on the
		 output module to do all of its output
01o,17dec02,tky  added DOCTYPE and NOTATION handlers to Canonicalization
		 module, for xml conformance testing 
01n,16dec02,tky  The xml conformance test suite does not use canonicalization
		 correctly. The output expected in output tests are not
		 actually canonical.  The xmlCanon module has been modified to
		 take both the test suite and true canonicalization into
		 account. The define W3C14N is used to include or exclude the
		 true canonical source sections.
01m,13dec02,tky  changing xmlwf to xmlCanon
01l,21nov02,tky  Expat 1.95.5 merge
01k,23aug02,tky  - provide more standard error messages
		 - control increment of pNsStack and pNsAxis to avoid overflows
01j,19aug02,tky  Created a better interface to the canonicalization module
01i,01aug02,tky  fixed potential memory leaks
01h,04jul02,dnc  processing instructions
01g,03jul02,dnc  removed extra spaces in PI data (xmltest-valid-sa-098)
01f,26jun02,zs   Fix processing instruction handling, fix " escaping to be &quot;
01e,18jun02,dnc  first cut of potentially working canonicalization code
01d,18jun02,tky  - Added a function to interface to the xmlwf main function
		 - changed the way output filenames are named, instead of
		 re-using the same filename, the full path is used and '/'
		 are replaced with '_' 
		 - added some error checking to xmlwf main
01c,14jun02,tky  removed call to setvbuf after the output file is opened,
		 for some reason this does not allow '<' to be printed at the
		 beginning of a file.
01b,31may02,tky  - include xmlwf.h which contains conditional compile defines
		 XML_VXWORKS and XML_UNIX
		 - Added conditional compile defines so that this can be used
		 in vxworks
01a,29apr02,zs   Initial version
*/

/*
DESCRIPTION

This module provides the means to canonicalize an existing valid
XML document. All output in this module is done by the XML output
module, therefore it must be included to use this module.

see Canonical XML Version 1.0 specs at http://www.w3.org/TR/xml-c14n
for canonicalization specifications.  Below is an excerpt defining
canonical form.

"The canonical form of an XML document is physical representation of 
the document produced by the method described in this specification. 
The changes are summarized in the following list:

\ml
\m - The document is encoded in UTF-8 
\m - Line breaks normalized to #xA on input, before parsing 
\m - Attribute values are normalized, as if by a validating processor 
\m - Character and parsed entity references are replaced 
\m - CDATA sections are replaced with their character content 
\m - The XML declaration and document type declaration (DTD) are removed 
\m - Empty elements are converted to start-end tag pairs 
\m - Whitespace outside of the document element and within start and end tags is normalized 
\m - All whitespace in character content is retained (excluding characters removed during line feed normalization) 
\m - Attribute value delimiters are set to quotation marks (double quotes) 
\m - Special characters in attribute values and character content are replaced by character references 
\m - Superfluous namespace declarations are removed from each element 
\m - Default attributes are added to each element 
\m - Lexicographic order is imposed on the namespace declarations and attributes of each element
\me
 

The term canonical XML refers to XML that is in canonical form. 
The XML canonicalization method is the algorithm defined by this specification 
that generates the canonical form of a given XML document or document subset.
The term XML canonicalization refers to the process of applying 
the XML canonicalization method to an XML document or document subset.

If an XML document must be converted to a node-set,
XPath REQUIRES that an XML processor be used to create the nodes 
of its data model to fully represent the document. 
The XML processor performs the following tasks in order:

\ml
\m - normalize line feeds 
\m - normalize attribute values 
\m - replace CDATA sections with their character content 
\m - resolve character and parsed entity references"
\me


Includes: stdio.h, stdlib.h, stddef.h, string.h, errnoLib.h, 
expat.h, codepage.h, xmlfile.h, xmltchar.h, xmlCanon.h, xmlop.h
*/


/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errnoLib.h>

#include "webservices/xml/expat.h"
#include "webservices/xml/xmlop/xmlop.h"
#include "webservices/xml/xmlCanon/xmlfile.h"
#include "webservices/xml/xmlCanon/xmltchar.h"
#include "webservices/xml/xmlCanon/xmlCanon.h"

/* forward declarations */

LOCAL void startCanonNSDecl(void *userData, const XML_Char *prefix, const XML_Char *uri);
LOCAL void endCanonNSDecl(void *userData, const XML_Char *prefix);
LOCAL void startElementNS(void *userData, const XML_Char *name, const XML_Char **atts);
LOCAL void endElementNS(void *userData, const XML_Char *name);
LOCAL void characterData(void *userData, const XML_Char *s, int len);
LOCAL void processingInstruction(void *userData, const XML_Char *target, const XML_Char *data);
LOCAL void startDoctypeDeclHandler(void *userData, const XML_Char *doctypeName, const XML_Char *sysid, const XML_Char *pubid, int has_internal_subset);
LOCAL void endDoctypeDeclHandler(void *userData);
LOCAL void notationDeclHandler(void *userData, const XML_Char *notationName, const XML_Char *base, const XML_Char *systemId, const XML_Char *publicId);

/***************************************************************************
*
* xmlCanonFile - canonicalizes an XML document and places the output into the file specified by outputDocName
*
* This routine takes the XML document specified by <canonizeDocName>, 
* canonicalizes it, and places the canonicalized file output into the file
* specified by <outputDocName>.  Both document names must specify the full 
* path to the document.  Namespaces will be used; all the parameter 
* entities are parsed. If xmlCanonFile fails the outputDoc will not exist,
* or will have a zero length.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void xmlCanonFile
    (
    const char *canonizeDocName, /* name of document to canonicalize */
    const char *outputDocName /* name of document to place output in */
    )
    {
    const XML_Char *encoding = 0;
    unsigned processFlags = XML_EXTERNAL_ENTITIES;
    FILE *fp = 0;
    int result;
    XML_Parser parser;
    XML_Output output;

    if( outputDocName )
        {
        fp = tfopen(outputDocName, T("w+"));
        
        if( !fp )
            {
            fprintf(stderr, "%s: %s\n", outputDocName, strerror(errnoGet()));
            return;
            }

        output = XML_OutputCreate(encoding, NSSEP, fp);
        if (NULL == output)
            {
            fprintf(stderr, "%s:%d Output object create failed\n", __FILE__, __LINE__);
            fclose(fp);
            return;
            }
        XML_OutputCanonSet(output, XML_TRUE);
        XML_OutputSecondCanonFormSet(output, XML_TRUE);
#ifdef XML_UNICODE
        puttc(0xFEFF, fp);
#endif
        parser = XML_ParserCreateNS(encoding, NSSEP);
        if (NULL == parser)
            {
            fprintf(stderr, "%s:%d Parser create failed\n", __FILE__, __LINE__);
            fclose(fp);
            XML_OutputDestroy(output);
            return;
            }
        if (XML_SetParamEntityParsing(parser, XML_PARAM_ENTITY_PARSING_ALWAYS) == 0)
            {
			fprintf(stderr, "%s:%d Parameter entity parsing request failed\n", __FILE__, __LINE__);
		    }
        XML_SetUserData(parser, output);
        XML_SetElementHandler(parser, startElementNS, endElementNS);
        XML_SetNamespaceDeclHandler(parser, startCanonNSDecl, endCanonNSDecl);
        XML_SetCharacterDataHandler(parser, characterData);
        XML_SetProcessingInstructionHandler(parser, processingInstruction);
        XML_SetDoctypeDeclHandler(parser, startDoctypeDeclHandler, endDoctypeDeclHandler);
        XML_SetNotationDeclHandler(parser, notationDeclHandler);

        result = XML_ProcessFile(parser, canonizeDocName, processFlags);
        
        if( EOF == fclose(fp) )
            fprintf(stderr, "xmlwf: problems closing file %s\n", outputDocName);

        if( !result )
            if (0 != tremove(outputDocName))
                {
                fprintf(stderr, "%s: %s\n", outputDocName, strerror(errnoGet()));
                }
            
        XML_ParserFree(parser);
        XML_OutputDestroy(output);
        }
    else
        {
        fprintf(stderr, "%s:%d outputDoc argument is NULL\n", __FILE__, __LINE__);
        }

        
    }

/***************************************************************************
* xmlCanonParamInit - sets up configuration parameters for the canonicalizer currently this routine is unused
*
* This routine is used to setup configuration parameters for the XML
* canonicalizer. Currently it is unused, but it is typically used by the 
* Tornado project facility for component setup.  The empty routine is 
* included is to ensure that the project facility inlcudes the xmlparse 
* object into the image.  If there are no calls to the functions within the 
* object, the object is optimized out automatically.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void xmlCanonParamInit()
    {
    }

/***************************************************************************
*
* startCanonNSDecl - SAX handler for namespace declarations
*
* This routine uses the XML output module routine 
* XML_StartNamespaceDeclWrite to output a canonicalized namespace 
* declaration.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void startCanonNSDecl
    (
    void *userData,         /* user defined data */
    const XML_Char *prefix, /* prefix representing namespace */
    const XML_Char *uri     /* URI for the namespace (unique identifier) */
    )
    {
    XML_Output output;

    output = (XML_Output) userData;
    XML_StartNamespaceDeclWrite(output, prefix, uri);
    }


/***************************************************************************
*
* endCanonNSDecl - writes out the end of a namespace declaration
*
* This routine uses the XML output module routine XML_EndNamespaceDeclWrite 
* to output a canonicalized end of namespace declaration.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void endCanonNSDecl
    (
    void *userData,         /* user defined data */
    const XML_Char *prefix  /* prefix representing namespace */
    )
    {
    XML_Output output;

    output = (XML_Output) userData;
    XML_EndNamespaceDeclWrite(output, prefix);
    }


/***************************************************************************
*
* startElementNS - SAX handler for a start tag, when namespaces are used
*
* This routine is called when a start tag is encountered. It uses XML output
* module routine XML_StartElementWrite to output a canonicalized XML start 
* tag.
*
* This follows 2.3 section "Element Nodes" of the w3c canonicalization spec:
* "The result is an open angle bracket (<), the element QName, the result of
* processing the namespace axis, the result of processing the attribute axis
* and a close angle bracket (>)." All these will be saved to the output 
* file, whose pointer is passed in through userData.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void startElementNS
    (
    void *userData,         /* user defined data */
    const XML_Char *name,   /* Element name */
    const XML_Char **atts   /* Element Attribute list, provided in name value pairs */
                            /* i.e. atts[0] contains name, atts[1] contains value for */
                            /* atts[0], and so on...*/
    )
    {
    XML_Output output;
    
    output = (XML_Output) userData;
    XML_StartElementWrite(output, name, atts);    
    }


/***************************************************************************
*
* endElementNS - SAX handler for the end element tag, when namespaces are used
*
* This routine is called when an end tag is encountered. It uses the XML
* output module routine XML_EndElementWrite to output a canonical end 
* element tag to the file whose pointer is passed in through userData.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void endElementNS
    (
    void *userData,     /* user defined data */
    const XML_Char *name/* Element name */
    )
    {
    XML_Output output;

    output = (XML_Output) userData;
    XML_EndElementWrite(output, name);
    }


/***************************************************************************
*
* characterData - SAX handler for character data
*
* This routine is called when character data is found. It uses the XML
* output module routine XML_CharacterDataWrite. Character data sections are
* replaced with their character content; special characters in CDATA 
* content are replaced by character references, for example, in character 
* data content & is replaced with the entity &-amp-;. All output is written
* to the file whose pointer is passed in through userData.
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void characterData
    (
    void *userData,     /* user defined data */
    const XML_Char *s,  /* non-null terminated character string */
    int len             /* length of s */   
    )
    {
    XML_Output output;

    output = (XML_Output) userData;
    XML_CharacterDataWrite(output, s, len);
    }

/***************************************************************************
*
* processingInstruction - SAX handler called when a PI is encountered
*
* This routine processes the PI and writes the following to the file:
* (This follows section 2.3 "Processing Instruction (PI) Nodes" of the
* w3c canonicalization spec)
* The opening PI symbol (\<?), the PI target name of the node,a leading space
* and the string value if it is not empty, and the closing PI symbol (?>).
* If the string value is empty, then the leading space is not added.
* Also, a trailing #xA is rendered after the closing PI symbol for 
* PI children of the root node with a lesser document order than
* the document element, and a leading #xA is rendered before the 
* opening PI symbol of PI children of the root node with a greater document 
* order than the document element. A canonical PI output is written
* to the file whose pointer is passed in through userData.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void processingInstruction
    (
    void *userData,         /* user defined data */
    const XML_Char *target, /* Processing Instruction target */
    const XML_Char *data    /* Processing Instruction data */
    )
    {
    XML_Output output;

    output = (XML_Output) userData;
    XML_ProcessingInstructionWrite(output, target, data);
    }

/***************************************************************************
* startDoctypeDeclHandler - this is called for the start of the DOCTYPE 
* declaration, before any DTD or internal subset is parsed
*
* This is called for the start of the DOCTYPE declaration, before any DTD or
* internal subset is parsed. doctypeName, sysid and pubid are all NULL
* terminated.
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void startDoctypeDeclHandler
	(
	void *userData,				/* user defined data */
    const XML_Char *doctypeName,/* DOCTYPE name */
    const XML_Char *sysid,		/* SYSTEM attribute contents */
    const XML_Char *pubid,		/* PUBLIC attribute contents */
    int has_internal_subset		/* bool indicating if internal or external subset */
	)
	{                          
    XML_Output output;

    output = (XML_Output) userData;
    XML_StartDoctypeDeclWrite(output, doctypeName, sysid, pubid, has_internal_subset);
    }


/***************************************************************************
* endDoctypeDeclHandler - this is called for the start of the DOCTYPE 
* declaration when the closing > is encountered, but after processing any 
* external subset
*
* This is called for the start of the DOCTYPE declaration when the closing >
* is encountered, but after processing any external subset.
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void endDoctypeDeclHandler(void *userData)
	{
    XML_Output output;

    output = (XML_Output) userData;
    XML_EndDoctypeDeclWrite(output);
	}


/***************************************************************************
* notationDeclHandler - this is called for a declaration of !NOTATION
*
* This is called for a declaration of notation.  The base argument is
* whatever was set by XML_SetBase. The notationName will never be
* NULL.  The other arguments can be. 
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
LOCAL void notationDeclHandler
	(
	void *userData,					/* user defined data */
	const XML_Char *notationName,	/* NOTATION attribute contents */
	const XML_Char *base,			/* base path to entity set by XML_SetBase*/
	const XML_Char *systemId,		/* SYSTEM attribute contents */
	const XML_Char *publicId		/* PUBLIC attribute contents */
	)
	{
    XML_Output output;
    
    output = (XML_Output) userData;
    XML_NotationDeclWrite(output, notationName, base, systemId, publicId);
	} 
