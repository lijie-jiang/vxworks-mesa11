/*

xml.h

Literal XML strings.

A literal XML string contains XML content. This declaration provides a simple
interface to the literal XML string handler. The XML string contains UTF8
content when the SOAP_C_UTFSTRING flag is set.

Note: the built-in _XML type provides the same functionality.

Example:
#import "xml.h"
struct ns__MyData
{ int num;
  XML str; // has XML content
};
struct ns__Mixed
{ int num;
  XML __any; // __any is not a tag: this allows parsing of mixed content
};


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

typedef char *XML;
