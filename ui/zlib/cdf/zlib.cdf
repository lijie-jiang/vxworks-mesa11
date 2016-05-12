/* zlib.cdf - user component description file */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,03sep13,h_l  written
*/

/*
DESCRIPTION
This file contains descriptions for the user components.
*/

/* Generic configuration parameters */

Folder FOLDER_ZLIB {
    NAME       zlib
    SYNOPSIS   zlib
    _CHILDREN  FOLDER_UTILITIES
    CHILDREN
}

Component INCLUDE_ZLIB {
    NAME  component ZLIB
    SYNOPSIS component ZLIB
    _CHILDREN FOLDER_ZLIB
	REQUIRES	INCLUDE_ZLIB_DEFLATE INCLUDE_ZLIB_INFLATE
	LINK_SYMS	gzseek \
                gzread \
				gzwrite
}

Component INCLUDE_ZLIB_DEFLATE {
	NAME		zlib deflate library
	SYNOPSIS	deflate
	MODULES		compress.o
	HDR_FILES	zlib.h
	_CHILDREN	FOLDER_ZLIB
	LINK_SYMS	compress
}

Component INCLUDE_ZLIB_INFLATE {
	NAME		zlib inflate library
	SYNOPSIS	inflate
	MODULES		uncompr.o inflate.o
	HDR_FILES	zlib.h
	_CHILDREN	FOLDER_ZLIB
	LINK_SYMS	inflateGetDictionary \
	            uncompress
}
