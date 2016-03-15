/*

httpmd5.h

gSOAP HTTP Content-MD5 digest plugin.

Usage (both client and server, see httpmd5test.h/.c for example):
soap_register_plugin(&soap, http_md5);
This enables HTTP MD5 checksum generation and checking for SOAP/XML messages
WITHOUT attachments.

Compile with -DWITH_OPENSSL
Link with OpenSSL (for md5evp.c), httpmd5.c, and md5evp.c

gSOAP XML Web services tools
Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc., All Rights Reserved.
This part of the software is released under one of the following licenses:
the gSOAP public license, or Genivia's license for commercial use.
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
Copyright (C) 2000-2005, Robert van Engelen, Genivia, Inc., All Rights Reserved.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#ifndef HTTPMD5_H
#define HTTPMD5_H

#include "stdsoap2.h"
#include "md5evp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_MD5_ID "HTTP-MD5-1.2" /* plugin identification */

extern const char http_md5_id[];

int http_md5(struct soap *soap, struct soap_plugin *p, void *arg);

struct http_md5_data
{ int (*fposthdr)(struct soap*, const char*, const char*);
  int (*fparsehdr)(struct soap*, const char*, const char*);
  int (*fprepareinitsend)(struct soap*);
  int (*fprepareinitrecv)(struct soap*);
  int (*fpreparesend)(struct soap*, const char*, size_t);
  int (*fpreparerecv)(struct soap*, const char*, size_t);
  int (*fpreparefinalrecv)(struct soap*);
  void *context;
  char digest[16];
};

#ifdef __cplusplus
}
#endif

#endif
