/* xmltchar.h - contains redefines for typical character handling functions */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,30July2002,tky   Source header added(see clearcase checkin messages)
*/

/*
DESCRIPTION

This header file contains redefines for typical character handling 
functions to allow users to change between 8 and 16 bit character
representations.
*/

#ifndef __INCxmltcharh
#define __INCxmltcharh

#ifdef __cplusplus
extern "C" {
#endif

#ifdef XML_UNICODE
#ifndef XML_UNICODE_WCHAR_T
#error xmlCanon requires a 16-bit Unicode-compatible wchar_t 
#endif
#define T(x) L ## x
#define ftprintf fwprintf
#define tfopen _wfopen
#define fputts fputws
#define puttc putwc
#define tcscmp wcscmp
#define tcscpy wcscpy
#define tcsncpy wcsncpy
#define tcscat wcscat
#define tcschr wcschr
#define tcsrchr wcsrchr
#define tcslen wcslen
#define tperror _wperror
#define topen _wopen
#define tmain wmain
#define tremove _wremove
#define tcsstr wcsstr
#else /* not XML_UNICODE */
#define T(x) x
#define ftprintf fprintf
#define tfopen fopen
#define fputts fputs
#define puttc putc
#define tcscmp strcmp
#define tcscpy strcpy
#define tcsncpy strncpy
#define tcscat strcat
#define tcschr strchr
#define tcsrchr strrchr
#define tcslen strlen
#define tperror perror
#define topen open
#define tmain main
#define tremove remove
#define tcsstr strstr
#endif /* not XML_UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* __INCxmltcharh */
