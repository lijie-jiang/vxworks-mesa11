/*
	wsrx.h

	WS-ReliableMessaging definitions:
	SOAP Header definitions for WS-RM 1.1 2007
	WS-RM Operations for CreateSequence, CloseSequence, TerminateSequence
	WS-RM SequenceAcknowledgement server operation (RM dest for AcksTo)

	Imported by import/wsrm.h

gSOAP XML Web services tools
Copyright (C) 2000-2010, Robert van Engelen, Genivia Inc., All Rights Reserved.
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
Copyright (C) 2000-2010, Robert van Engelen, Genivia Inc., All Rights Reserved.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

struct SOAP_ENV__Header
{
  struct wsrm__SequenceType       *wsrm__Sequence                0;
  int                              __sizeAckRequested            0;
  struct wsrm__AckRequestedType   *wsrm__AckRequested            0;
  int                              __sizeSequenceAcknowledgement 0;
  struct _wsrm__SequenceAcknowledgement *wsrm__SequenceAcknowledgement 0;
  struct wsrm__SequenceFaultType  *wsrm__SequenceFault           0;
};

//gsoap wsrm service name: wsrm

//gsoap wsrm service method-header-part:     CreateSequence wsa5__MessageID
//gsoap wsrm service method-header-part:     CreateSequence wsa5__RelatesTo
//gsoap wsrm service method-header-part:     CreateSequence wsa5__From
//gsoap wsrm service method-header-part:     CreateSequence wsa5__ReplyTo
//gsoap wsrm service method-header-part:     CreateSequence wsa5__FaultTo
//gsoap wsrm service method-header-part:     CreateSequence wsa5__To
//gsoap wsrm service method-header-part:     CreateSequence wsa5__Action
//gsoap wsrm service method-action:          CreateSequence http://docs.oasis-open.org/ws-rx/wsrm/200702/CreateSequence
//gsoap wsrm service method-output-action:   CreateSequence http://docs.oasis-open.org/ws-rx/wsrm/200702/CreateSequenceResponse
int __wsrm__CreateSequence(
  struct wsrm__CreateSequenceType         *wsrm__CreateSequence,
  struct wsrm__CreateSequenceResponseType *wsrm__CreateSequenceResponse);

//gsoap wsrm service method-header-part:     CloseSequence wsa5__MessageID
//gsoap wsrm service method-header-part:     CloseSequence wsa5__RelatesTo
//gsoap wsrm service method-header-part:     CloseSequence wsa5__From
//gsoap wsrm service method-header-part:     CloseSequence wsa5__ReplyTo
//gsoap wsrm service method-header-part:     CloseSequence wsa5__FaultTo
//gsoap wsrm service method-header-part:     CloseSequence wsa5__To
//gsoap wsrm service method-header-part:     CloseSequence wsa5__Action
//gsoap wsrm service method-action:          CloseSequence http://docs.oasis-open.org/ws-rx/wsrm/200702/CloseSequence
//gsoap wsrm service method-output-action:   CloseSequence http://docs.oasis-open.org/ws-rx/wsrm/200702/CloseSequenceResponse
int __wsrm__CloseSequence(
  struct wsrm__CloseSequenceType         *wsrm__CloseSequence,
  struct wsrm__CloseSequenceResponseType *wsrm__CloseSequenceResponse);

//gsoap wsrm service method-header-part:     TerminateSequence wsa5__MessageID
//gsoap wsrm service method-header-part:     TerminateSequence wsa5__RelatesTo
//gsoap wsrm service method-header-part:     TerminateSequence wsa5__From
//gsoap wsrm service method-header-part:     TerminateSequence wsa5__ReplyTo
//gsoap wsrm service method-header-part:     TerminateSequence wsa5__FaultTo
//gsoap wsrm service method-header-part:     TerminateSequence wsa5__To
//gsoap wsrm service method-header-part:     TerminateSequence wsa5__Action
//gsoap wsrm service method-action:          TerminateSequence http://docs.oasis-open.org/ws-rx/wsrm/200702/TerminateSequence
//gsoap wsrm service method-output-action:   TerminateSequence http://docs.oasis-open.org/ws-rx/wsrm/200702/TerminateSequenceResponse
int __wsrm__TerminateSequence(
  struct wsrm__TerminateSequenceType         *wsrm__TerminateSequence,
  struct wsrm__TerminateSequenceResponseType *wsrm__TerminateSequenceResponse);

//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__MessageID
//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__RelatesTo
//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__From
//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__ReplyTo
//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__FaultTo
//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__To
//gsoap wsrm service method-header-part:     CreateSequenceResponse wsa5__Action
//gsoap wsrm service method-action:          CreateSequenceResponse http://docs.oasis-open.org/ws-rx/wsrm/200702/CreateSequenceResponse
int __wsrm__CreateSequenceResponse(struct wsrm__CreateSequenceResponseType *wsrm__CreateSequenceResponse, void);

//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__MessageID
//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__RelatesTo
//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__From
//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__ReplyTo
//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__FaultTo
//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__To
//gsoap wsrm service method-header-part:     CloseSequenceResponse wsa5__Action
//gsoap wsrm service method-action:          CloseSequenceResponse http://docs.oasis-open.org/ws-rx/wsrm/200702/CloseSequenceResponse
int __wsrm__CloseSequenceResponse(struct wsrm__CloseSequenceResponseType *wsrm__CloseSequenceResponse, void);

//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__MessageID
//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__RelatesTo
//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__From
//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__ReplyTo
//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__FaultTo
//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__To
//gsoap wsrm service method-header-part:     TerminateSequenceResponse wsa5__Action
//gsoap wsrm service method-action:          TerminateSequenceResponse http://docs.oasis-open.org/ws-rx/wsrm/200702/TerminateSequenceResponse
int __wsrm__TerminateSequenceResponse(struct wsrm__TerminateSequenceResponseType *wsrm__TerminateSequenceResponse, void);

//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__MessageID
//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__RelatesTo
//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__From
//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__ReplyTo
//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__FaultTo
//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__To
//gsoap wsrm service method-header-part:     SequenceAcknowledgement wsa5__Action
//gsoap wsrm service method-action:          SequenceAcknowledgement http://docs.oasis-open.org/ws-rx/wsrm/200702/SequenceAcknowledgement
int __wsrm__SequenceAcknowledgement(void);

//gsoap wsrm service method-header-part:     AckRequested wsa5__MessageID
//gsoap wsrm service method-header-part:     AckRequested wsa5__RelatesTo
//gsoap wsrm service method-header-part:     AckRequested wsa5__From
//gsoap wsrm service method-header-part:     AckRequested wsa5__ReplyTo
//gsoap wsrm service method-header-part:     AckRequested wsa5__FaultTo
//gsoap wsrm service method-header-part:     AckRequested wsa5__To
//gsoap wsrm service method-header-part:     AckRequested wsa5__Action
//gsoap wsrm service method-action:          AckRequested http://docs.oasis-open.org/ws-rx/wsrm/200702/AckRequested
int __wsrm__AckRequested(void);
