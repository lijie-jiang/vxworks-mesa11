/* saxTutorial2.c - expat sax parser tutorial */

/* Copyright 1984-2009 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03sep09,f_f  make XML_Size use format '%d'
01a,12dec02,dt   Written.
*/


/*
DESCRIPTION
This tutorial demonstrates how to extract the attribute name and value
of an element.  This tutorial requires the following components: 
XML SAX parser.  Or if building a project from source components, this 
tutorial requires the following source files: xmlparse.c, xmlrole.c, 
xmltok.c (located in INSTALL_DIR/target/src/webservices/xml).
*/

/* includes */
#include <stdio.h>
#include <string.h>
#include <errnoLib.h>
#include "webservices/xml/expat.h"

#define FILE_BUFFER_SIZE 8192

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
startElementHandler2
    (
    void *data, 
    const char *element, 
    const char **attr
    )
    {
    int i;

    printf("found start tag: %s\n", element);

/*****************************************************************************
 *    Get the attribute(s) for this element. The attribute's name is attr[i] *
 *    and the value is attr[i+1]                                             *
 *****************************************************************************/
    for (i = 0; attr[i]; i += 2)
        {
        printf("found attribute (name=value): %s='%s'\n", attr[i], attr[i + 1]);
        }

    }  /* End of start handler */


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
endElementHandler2
    (
    void *data, 
    const char *element
    )
    {
    printf("found end tag: %s\n", element);
    }  /* End of end handler */


BOOL
xmlSaxParseFile2
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


/******************************************
 *    Set the handler for the elements    *
 ******************************************/
    XML_SetElementHandler(parser, startElementHandler2, endElementHandler2);
    
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
void run_saxTutorial2()
    {
    char *xmlFile  = "saxTutorial.xml";

    xmlSaxParseFile2(xmlFile);
    }
