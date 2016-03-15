/*

	WS-example.h

	Example gSOAP header file to demonstrate WS-Header.h

	Compile this file with soapcpp2.

--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2004-2008, Robert van Engelen, Genivia Inc. All Rights Reserved.
This software is released under one of the following two licenses:
Genivia's license for commercial use.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#import "WS-Header.h"

//gsoap ns service name:        calc
//gsoap ns service style:       rpc
//gsoap ns service encoding:    encoded
//gsoap ns service namespace:   http://websrv.cs.fsu.edu/~engelen/calc.wsdl
//gsoap ns service location:    http://websrv.cs.fsu.edu/~engelen/calcserver.cgi

//gsoap ns schema namespace:	urn:calc
int ns__add(double a, double b, double *result);
int ns__sub(double a, double b, double *result);
int ns__mul(double a, double b, double *result);
int ns__div(double a, double b, double *result);
int ns__pow(double a, double b, double *result);
