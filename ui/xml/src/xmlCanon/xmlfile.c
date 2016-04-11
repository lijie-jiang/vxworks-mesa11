/* xmlfile.c - performs file handling for the xmlCanon module's XML parser */

/* Copyright 2002-2005, 2009, 2013 Wind River Systems, Inc. */
/*
Copyright (c) 1998, 1999 Thai Open Source Software Center Ltd
See the file COPYING for copying permission.
*/

/*
modification history
--------------------
01l,09aug13,f_f  fixed some coverity issues (WIND00429907)
01k,29mar13,f_f  fixed the Coverity issue (WIND00410897)
01j,27mar09,f_f  upgrade to Expat 2.0.1
01i,23feb05,pas  added in-file comments from clearcase checkin comments
01h,08dec04,pas  moved xmlCanon.h to the appropriate place in the include list,
		 to fix a user-mode build error
01g,22apr03,tky  remove unused process file function
01f,11apr03,tky  code review changes
01e,07apr03,tky  remove dependencies on codepage.c and filemap.c
01d,20dec02,tky  changed exit(1) to return 0 in XML_ProcessFile, this might
		 have been causing memory leaks.
01c,13dec02,tky  change xmlwf to xmlCanon
01b,23aug02,tky  Provide a more standard error message
01a,29apr02,zs   Initial version
*/

/*
DESCRIPTION

This file performs file handling for the xmlCanon module's XML parser. 

*/

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <errnoLib.h>
#include <ioLib.h>

#include "webservices/xml/expat.h"
#include "webservices/xml/xmlCanon/xmlfile.h"
#include "webservices/xml/xmlCanon/xmltchar.h"
#include "webservices/xml/xmlCanon/xmlCanon.h"

typedef struct {
  XML_Parser parser;
  int *retPtr;
} PROCESS_ARGS;

static void
reportError(XML_Parser parser, const XML_Char *filename)
{
  enum XML_Error code = XML_GetErrorCode(parser);
  const XML_Char *message = XML_ErrorString(code);
  if (message)
    XML_CANON_DEBUG(T("%s:%d %s:%" XML_FMT_INT_MOD "u:%" XML_FMT_INT_MOD "u: %s\n"),
             filename,
             XML_GetErrorLineNumber(parser),
             XML_GetErrorColumnNumber(parser),
             message);
  else
    XML_CANON_DEBUG(T("%s:%d %s: (unknown message %d)\n"), filename, code, 0,0);
}

static const XML_Char *
resolveSystemId(const XML_Char *base, const XML_Char *systemId,
                XML_Char **toFree)
{
  XML_Char *s;
  *toFree = 0;
  if (!base
      || *systemId == T('/')
     )
    return systemId;
  *toFree = (XML_Char *)malloc((tcslen(base) + tcslen(systemId) + 2)
                               * sizeof(XML_Char));
  if (!*toFree)
    return systemId;
  tcscpy(*toFree, base);
  s = *toFree;
  if (tcsrchr(s, T('/')))
    s = tcsrchr(s, T('/')) + 1;
  tcscpy(s, systemId);
  return *toFree;
}


static int
processStream(const XML_Char *filename, XML_Parser parser)
    {
    FILE *fd = fopen(filename, "rb");

    if( fd == NULL )
        {
        XML_CANON_DEBUG("%s:%d Error opening file - %s\n", strerror(errnoGet()), 0,0,0);
        return 0;
        }
    for( ;; )
        {
        int nread;
        char *buf = (char *)XML_GetBuffer(parser, XML_CANON_PARSE_BUFFER_SIZE);
        if( !buf )
            {
            fclose(fd);
            XML_CANON_DEBUG(T("%s%d: %s: out of memory\n"), filename, 0,0,0);
            return 0;
            }
        nread = fread(buf, sizeof(char), XML_CANON_PARSE_BUFFER_SIZE, fd);
        if( nread < 0 )
            {
            tperror(filename);
            fclose(fd);
            return 0;
            }
        if( !XML_ParseBuffer(parser, nread, nread == 0) )
            {
            reportError(parser, filename);
            fclose(fd);
            return 0;
            }
        if( nread == 0 )
            {
            fclose(fd);
            break;;
            }
        }
    return 1;
    }

static int
externalEntityRefStream(XML_Parser parser,
                        const XML_Char *context,
                        const XML_Char *base,
                        const XML_Char *systemId,
                        const XML_Char *publicId)
{
  XML_Char *s;
  const XML_Char *filename;
  int ret;
  XML_Parser entParser = XML_ExternalEntityParserCreate(parser, context, 0);
  if (NULL == entParser)
      {
      XML_CANON_DEBUG(T("%s:%d Error creating entParser"), 0,0,0,0);
      return 0;
      }
  filename = resolveSystemId(base, systemId, &s);
  if (XML_SetBase(entParser, filename) == XML_STATUS_ERROR)
      {
      XML_CANON_DEBUG(T("%s:%d %s: out of memory"), filename, 0,0,0);
      }
  ret = processStream(filename, entParser);
  free(s);
  XML_ParserFree(entParser);
  return ret;
}

int
XML_ProcessFile(XML_Parser parser,
                const XML_Char *filename,
                unsigned flags)
{
  int result;

  if (!XML_SetBase(parser, filename)) {
    XML_CANON_DEBUG(T("%s:%d %s: out of memory"), filename, 0,0,0);
    return 0;
  }

  if (flags & XML_EXTERNAL_ENTITIES)
      XML_SetExternalEntityRefHandler(parser, externalEntityRefStream);

  result = processStream(filename, parser);
  
  return result;
}
