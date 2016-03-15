/*
	httppost.h

	gSOAP HTTP POST plugin for non-SOAP payloads.

	See httppost.c for instructions.

	Revisions:
	register multiple POST content handlers, each for a content type

gSOAP XML Web services tools
Copyright (C) 2004-2009, Robert van Engelen, Genivia, Inc. All Rights Reserved.

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
Copyright (C) 2000-2004 Robert A. van Engelen, Genivia inc. All Rights Reserved.
--------------------------------------------------------------------------------
*/

#ifndef HTTPPOST_H
#define HTTPPOST_H

#include "stdsoap2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_POST_ID "HTTP-POST-1.1" /* plugin identification */

extern const char http_post_id[];

typedef int (*http_handler_t)(struct soap*);

struct http_post_handlers
{ const char *type;
  http_handler_t handler;
};

/* This is the local plugin data shared among all copies of the soap struct: */
struct http_post_data
{ int (*fparsehdr)(struct soap*, const char*, const char*); /* to save and call the internal HTTP header parser */
  int (*fput)(struct soap*); /* to save */
  int (*fdel)(struct soap*); /* to save */
  struct http_post_handlers *handlers; /* the server-side POST content type handlers */
};

/* the http post plugin, note: argument should be a table of type-handler pairs */
int http_post(struct soap*, struct soap_plugin*, void*);

/* a function to send HTTP POST, should be followd by a soap_send to transmit and soap_get_http_body to retrieve the HTTP body returned into an internal buffer */
int soap_post_connect(struct soap*, const char *endpoint, const char *action, const char *type);

/* a function to send HTTP PUT, should be followed by a soap_send to transmit data */
int soap_put_connect(struct soap*, const char *endpoint, const char *action, const char *type);

/* a function to send HTTP DELETE */
int soap_delete_connect(struct soap*, const char *endpoint, const char *action, const char *type);

/* a function to retrieve the HTTP body into an internal buffer */
int soap_http_body(struct soap*, char **buf, size_t *len);

#ifdef __cplusplus
}
#endif

#endif
