/*

xmime4.h

Use #import "xmlmime.h" in a gSOAP header file to enable xmlmime 2004/11
bindings.

The xmime4:contentType attribute can be used to associate a MIME type with
binary content, as in:

#import "xmime4.h"
struct ns__myBinaryData
{ unsigned char *__ptr;
  int __size;
  @char *xmime4__contentType;
};

Use soapcpp2 option -Ipath:path:... to specify the path(s) for #import

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
Copyright (C) 2000-2005 Robert A. van Engelen, Genivia inc. All Rights Reserved.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

//gsoap xmime4 schema import: http://www.w3.org/2004/11/xmlmime
