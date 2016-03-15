/* xmlFile.h - performs file handling for the xmlCanon module's XML parser */

/* Copyright 1984-2009 Wind River Systems, Inc. */
/*
Copyright (c) 1998, 1999 Thai Open Source Software Center Ltd
See the file COPYING for copying permission.
*/

/*
modification history
--------------------
01b,30mar09,f_f  upgrade to Expat 2.0.1
01a,10Apr2003,tky   Source header added(see clearcase checkin messages)
*/

/*
DESCRIPTION

This file contains defines and function prototypes for the xmlfile 
part of the xmlCanon module.  xmlfile performs file handling for
the xmlCanon module's XML parser. 

*/

#ifndef __INCxmlFileh
#define __INCxmlFileh

#ifdef __cplusplus
extern "C" {
#endif

#define XML_EXTERNAL_ENTITIES 02
#define XML_CANON_PARSE_BUFFER_SIZE (1024*8)

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

extern int XML_ProcessFile(XML_Parser parser,
			   const XML_Char *filename,
			   unsigned flags);

#ifdef __cplusplus
}
#endif

#endif /* __INCxmlFileh */

