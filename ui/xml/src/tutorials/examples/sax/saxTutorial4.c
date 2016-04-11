/* saxTutorial4.c - expat sax parser tutorial */

/* Copyright 1984-2009 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03sep09,f_f  make XML_Size use format '%d'
01a,12dec02,dt   Written.
*/


/*
DESCRIPTION
This tutorial demonstrates how to extract comments from an XML document.
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
void commentHandler4
    (
    void *data, 
    const char *comment
    )
    {
    printf("found a comment: %s\n", comment);
    }

BOOL
xmlSaxParseFile4
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


/**************************************
 *    Set the handler for comments    *
 **************************************/
    XML_SetCommentHandler(parser, commentHandler4);
    
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
void run_saxTutorial4()
    {
    char *xmlFile  = "saxTutorial.xml";

    xmlSaxParseFile4(xmlFile);
    }
