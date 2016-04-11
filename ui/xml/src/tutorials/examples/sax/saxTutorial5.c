/* saxTutorial5.c - expat sax parser tutorial */

/* Copyright 1984-2009 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03sep09,f_f  make XML_Size use format '%d'
01a,12dec02,dt   Written.
*/


/*
DESCRIPTION
This tutorial combines the skills learned from tutorials 1-4 for parsing
an XML document. We will be using this skills to output an XML document.

In addition, a new handler is introduced in this tutorial. An empty element
handler (see emptyElementHandler5()) handles an element that does not contain
CDATA or another element within it. i.e. <element />

This tutorial requires the following components: XML SAX parser.  Or if 
building a project from source components, this tutorial requires the 
following source files: xmlparse.c, xmlrole.c, xmltok.c
(located in INSTALL_DIR/target/src/webservices/xml).
*/

/* includes */
#include <stdio.h>
#include <string.h>
#include <errnoLib.h>
#include "webservices/xml/expat.h"

#define FILE_BUFFER_SIZE 8192

int depth;

/***************************************************************************
* startElementHandler - This is called at the beginning of an XML element
*
* This is called at the beginning of an XML element.  Attr is an array of
* name/value pairs, i.e. attr[0] contains name, attr[1] contains value for 
* attr[0], and so on... Names and values are NULL terminated.
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
void
startElementHandler5
    (
    void *data, 
    const char *element, 
    const char **attr
    )
    {
    int i;

    /* indent */
    for (i = 0; i < depth; i++)
        {
        printf("  ");
        }

    /* print the element name */
    printf("<%s", element);
 
/*****************************************************************************
 *    Get the attribute(s) for this element. The attribute's name is attr[i] *
 *    and the value is attr[i+1]                                             *
 *****************************************************************************/
    for (i = 0; attr[i]; i += 2)
        {
        printf(" %s='%s'", attr[i], attr[i + 1]);
        }

    printf(">");
    depth++;
    }  /* End of start handler */


/***************************************************************************
* emptyElementHandler - This is called when an empty element is 
* encountered
*
* This is called when an empty element is encountered, such as: <element/>
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
void
emptyElementHandler5
    (
    void *data,
    const char *element,
    const char **attr
    )
    {
    int i;

    /* indent */
    for (i = 0; i < depth; i++)
        {
        printf("  ");
        }

    /* print the element name */
    printf("<%s", element);
 
/*****************************************************************************
 *    Get the attribute(s) for this element. The attribute's name is attr[i] *
 *    and the value is attr[i+1]                                             *
 *****************************************************************************/
    for (i = 0; attr[i]; i += 2)
        {
        printf(" %s='%s'", attr[i], attr[i + 1]);
        }

    printf("/>");

    }



/***************************************************************************
* endElementHandler - This is called at the end of an XML element
*
* This is called at the end of an XML element. Element contains the element 
* name that is closing.
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
void
endElementHandler5
    (
    void *data, 
    const char *element
    )
    {
    printf("</%s>", element);
    printf("\n");
    depth--;
    }  /* End of end handler */



/***************************************************************************
* cdataHandler - This is called when the beginning of a <! [CDATA[....]]>,
* or CDATA section is encountered.
*
* This is called when the beginning of a <! [CDATA[....]]>, or CDATA section
* is encountered. Whitespace and carriage return between element tags are
* considered CDATA. CDATA is not null terminated
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
void cdataHandler5
    (
    void *data, 
    const char *element, 
    int len
    )
    {
    int i;

    for (i=0; i < len; i++)
        {
        printf("%c", element[i]);
        }
    }



/***************************************************************************
* commentHandler - This is called when a <!-- ... --> element, or XML
* comment is encountered
*
* This is called when a <!-- ... --> element, or XML comment is encountered.
* data is NULL terminated.
*  
* RETURNS: N/A
*
* ERRNO: N/A
*/
void commentHandler5
    (
    void *data, 
    const char *comment
    )
    {
    printf("<!--%s-->\n", comment);
    }



BOOL
xmlSaxParseFile5
    (
    char *pFileName
    )
    {
    FILE *xmlFile;
    char fileBuf[FILE_BUFFER_SIZE];
    int len;
    int isDone=0;
    XML_Parser parser;

/********************************************
 *    Open the file to parse (read-only)    *
 ********************************************/
    xmlFile = fopen(pFileName, "r");
    if ( NULL == xmlFile)
        {
    	fprintf(stderr, "%s:%d Error opening parse file %s- %s\n", __FILE__, __LINE__, pFileName, strerror(errnoGet()));
    	return FALSE;
        }

/***************************
 *    Create the parser    *
 ***************************/
    parser = XML_ParserCreate(NULL);

    if (!parser)
        {
    	fprintf(stderr, "Failed to allocate memory for parser.\n");
    	return FALSE;
    	}		


/**************************
 *    Set the handlers    *
 **************************/
    XML_SetElementHandler(parser, startElementHandler5, endElementHandler5);
    XML_SetCharacterDataHandler(parser, cdataHandler5);
    XML_SetCommentHandler(parser, commentHandler5);
    XML_SetEmptyElementHandler(parser, emptyElementHandler5);
    
/********************************
 *    Parse the XML document    *
 ********************************/
    while(!isDone)
        {
        len = fread(fileBuf, 1, sizeof(fileBuf), xmlFile);
        isDone = len < sizeof(fileBuf);

        if (ferror(xmlFile))
            {
            fprintf(stderr, "Read error\n");
            return FALSE;
            }
        /*isDone = feof(xmlFile);*/

        if (! XML_Parse(parser, fileBuf, len, isDone))
            {
            fprintf(stderr, "Parse error at line %d:\n%s\n",
                    (int)XML_GetCurrentLineNumber(parser),
                    XML_ErrorString(XML_GetErrorCode(parser)));
            return FALSE;
            }
        }

        fclose(xmlFile);
	return TRUE;
    }


/****************************
 *    Start program here    * 
 ****************************/
void run_saxTutorial5()
    {
    char *xmlFile  = "saxTutorial.xml";

    xmlSaxParseFile5(xmlFile);
    }
