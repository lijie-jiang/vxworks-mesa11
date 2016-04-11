/* canonTutorial.c - canonical tutorial */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,13dec02,dt   Written.
*/


/*
DESCRIPTION
This tutorial demonstrates how to canonicalize an XML document. This tutorial 
requires the following components: XML canonicalizer.  
Or if building a project from source components, this tutorial requires the 
following source files: xmlCanon.c, xmlfile.c
*/

/* includes */
#include "webservices/xml/xmlCanon/xmlCanon.h"


void run_canonTutorial()
    {
    const char *inputFile  = "canonTutorial.xml";
    const char *outputFile = "out.xml"; 

/****************************************************************************
 * Call xmlCanonFile that takes the XML document specified by inputFile,    *
 * canonicalizes it and places the canonicalized file output into the file  *
 * specified by outputFile.  Both document names must specify the full      *
 * path to the document. If xmlCanonFile fails the outputFile will not      *
 * exist, or have a zero length.                                            *
 ****************************************************************************/
    xmlCanonFile(inputFile, outputFile);

    }
