/*
	httpget.h

	gSOAP HTTP GET plugin.

	See httpget.c for usage instructions.

gSOAP XML Web services tools
Copyright (C) 2000-2008, Robert van Engelen, Genivia Inc., All Rights Reserved.
This part of the software is released under ONE of the following licenses:
the gSOAP public license, OR Genivia's license for commercial use.
--------------------------------------------------------------------------------
gSOAP public license.

The contents of this file are subject to the gSOAP Public License Version 1.3
(the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at
http://www.cs.fsu.edu/~engelen/soaplicense.html
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Initial Developer of the Original Code is Robert A. van Engelen.
Copyright (C) 2000-2008 Robert A. van Engelen, Genivia inc. All Rights Reserved.
--------------------------------------------------------------------------------
*/

#ifndef HTTPGET_H
#define HTTPGET_H

#include "stdsoap2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_GET_ID "HTTP-GET-1.1" /* plugin identification */

extern const char http_get_id[];

/* This is the local plugin data shared among all copies of the soap struct: */
struct http_get_data
{ int (*fparse)(struct soap*); /* to save and call the internal HTTP header parser */
  int (*fget)(struct soap*); /* user-defined server-side HTTP GET handler */
  size_t stat_get;  /* HTTP GET usage statistics */
  size_t stat_post; /* HTTP POST usage statistics */
  size_t stat_fail; /* HTTP failure statistics */
  size_t min[60]; /* Hits by the minute */
  size_t hour[24]; /* Hits by the hour */
  size_t day[366]; /* Hits by day */
};

int http_get(struct soap*, struct soap_plugin*, void*);
int soap_get_connect(struct soap*, const char*, const char*);

char *query(struct soap*);
char *query_key(struct soap*, char**);
char *query_val(struct soap*, char**);

int soap_encode_string(const char*, char*, size_t);
const char* soap_decode_string(char*, size_t, const char*);

#ifdef __cplusplus
}
#endif

#endif
