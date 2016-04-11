/* xmlConfig.h - Expat XML SAX parser configuration file */

/* Copyright 2002-2005 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,23feb05,pas  added in-file comments from clearcase checkin comments
01h,13jan05,pas  upgraded to Expat 1.95.8
01g,21mar03,tky  update copyright info
01f,04dec02,tky  added every option that can be configured, whether currently
		 defined or not
01e,24jul02,tky  changed config.h to xmlConfig.h
01d,11jul02,tky  define or undef XML_MIN_SIZE here now
01c,08jul02,tky  reverted XML_CONTEXT_BYTES back to 1024
01b,27jun02,tky  Updating to expat 1.95
01a,01may02,zs   Initial version
*/

/*
DESCRIPTION

Expat XML SAX parser configuration defines, originally auto generated by a
program called configure included with the Expat parser.
*/

#ifndef xmlConfig_INCLUDED
#define xmlConfig_INCLUDED 1

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* 1234 = LIL_ENDIAN, 4321 = BIGENDIAN */
#define BYTEORDER 1234

/* Define to make the XML Expat parser image size more compact */
#define XML_MIN_SIZE

/* Define to make XML Namespaces functionality available. */
#define XML_NS

/* Define to make parameter entity parsing functionality available. */
#define XML_DTD

/* Define to specify how much context to retain around the current parse
   point. */
#define XML_CONTEXT_BYTES 1024

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* whether byteorder is bigendian */
#undef WORDS_BIGENDIAN

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `long' if <sys/types.h> doesn't define.  */
#undef off_t 

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#undef size_t

#endif /* xmlConfig_INCLUDED */
