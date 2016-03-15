/*

md5evp.h

gSOAP HTTP Content-MD5 digest plugin.

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

	Defines MD5 handler using EVP interface (e.g. using OpenSSL)

	int md5_handler(struct soap *soap, void **context, enum md5_action action, char *buf, size_t len)
	context can be set and passed to subsequent calls. Parameters:
	action =
	MD5_INIT:	init context
	MD5_UPDATE:	update context with data from buf with size len
	MD5_FINAL:	fill buf with 16 bytes MD5 hash value
	MD5_DELETE:	delete context
	buf		input data, output MD5 128 bit hash value
	len		length of input data

*/

#ifndef MD5EVP_H
#define MD5EVP_H

#include "stdsoap2.h"

#ifdef WITH_OPENSSL
#include <openssl/evp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum md5_action { MD5_INIT, MD5_UPDATE, MD5_FINAL, MD5_DELETE };

int md5_handler(struct soap *soap, void **context, enum md5_action action, char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif
