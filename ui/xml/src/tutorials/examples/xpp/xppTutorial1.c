/* xppTutorial1.c - xml pull parser tutorial */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,11dec02,dt   Written.
*/

/*
DESCRIPTION
This tutorial demonstrates how to use the basic start tag, end tag and
text event types in the XPP parser during the parsing of an XML document.
This tutorial requires the following components: XML SAX parser, XML pull
parser.  Or if building a project from source components, this tutorial 
requires the following source files: xmlparse.c, xmlrole.c, xmltok.c, 
xpp.c, sax.c (located in INSTALL_DIR/target/src/webservices/xml).
*/

/* includes */

#include <stdio.h>
#include <string.h>
#include "webservices/xml/xpp/xpp.h"

void
xmlXppParseFile1
    (
    char *pFileName
    )
    {
    XPP_EVENT_T eventType;
    XPP *pXpp = NULL;

/******************************************** 
 *    Initializing the pXpp data structure  *
 ********************************************/
    xppAlloc(&pXpp);
    xppInit(pXpp);

    printf("Initializing Pull Parser with file %s\n", pFileName);

    xppSetFileInput(pXpp, pFileName);
    eventType = xppGetEventType(pXpp);

/*************************************************************************
 *    Parse the XML document until the end of the document is reached    *
 *************************************************************************/
    while (eventType != END_DOC)
        {

/*************************************************
 *    The start of the element is encountered    *
 *************************************************/
        if (eventType == START_TAG)
            {
            printf("found start tag: %s\n", xppGetQName(pXpp));
            }

/***********************************************
 *    The end of the element is encountered    *
 ***********************************************/
        else if (eventType == END_TAG) 
            {
            printf("found end tag: %s\n", xppGetQName(pXpp));
            }

/*************************************************************
 *    The text of the element or attribute is encountered    *
 *************************************************************/
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
                        {
                        printf("found text: %s\n", text);
                        }
                    else
                        {
                        fprintf(stderr, "Error getting text: %s\n", xppGetErrorString(pXpp));
                        }
                    free (text);
                    }
                }
            }

        eventType = xppNext(pXpp);
        }

    /*** End of Document Reached ***/
    xppDestroy(pXpp);
    }



void run_xppTutorial1()
    {
    char *xmlFile  = "xppTutorial.xml";

    xmlXppParseFile1(xmlFile);
    }
