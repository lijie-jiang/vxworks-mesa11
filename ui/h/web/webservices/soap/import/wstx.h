/*
	wstx.h

	WS-Trust definitions:
	SOAP Header definitions for WS-Trust
	WS-Trust operations

	Imported by import/wst.h

gSOAP XML Web services tools
Copyright (C) 2000-2012, Robert van Engelen, Genivia Inc., All Rights Reserved.
This part of the software is released under ONE of the following licenses:
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
Copyright (C) 2000-2012, Robert van Engelen, Genivia Inc., All Rights Reserved.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

struct SOAP_ENV__Header
{ int								__sizeIssuedTokens 0;
  struct wst__RequestSecurityTokenResponseCollectionType	*wst__IssuedTokens 0;
};

//gsoap wst service name: wst

//gsoap wst service method-header-part:     RequestSecurityToken wsa5__MessageID
//gsoap wst service method-header-part:     RequestSecurityToken wsa5__RelatesTo
//gsoap wst service method-header-part:     RequestSecurityToken wsa5__From
//gsoap wst service method-header-part:     RequestSecurityToken wsa5__ReplyTo
//gsoap wst service method-header-part:     RequestSecurityToken wsa5__FaultTo
//gsoap wst service method-header-part:     RequestSecurityToken wsa5__To
//gsoap wst service method-header-part:     RequestSecurityToken wsa5__Action
//gsoap wst service method-action:          RequestSecurityToken http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue
//gsoap wst service method-output-action:   RequestSecurityToken http://schemas.xmlsoap.org/ws/2005/02/trust/RSTR/Issue
int __wst__RequestSecurityToken(
  struct wst__RequestSecurityTokenType		*wst__RequestSecurityToken,
  struct wst__RequestSecurityTokenResponseType	*wst__RequestSecurityTokenResponse
);
