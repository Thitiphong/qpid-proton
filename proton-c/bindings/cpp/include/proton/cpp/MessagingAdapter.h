#ifndef PROTON_CPP_MESSAGING_ADAPTER_H
#define PROTON_CPP_MESSAGING_ADAPTER_H

/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "proton/cpp/ProtonHandler.h"
#include "proton/cpp/MessagingHandler.h"

#include "proton/cpp/MessagingEvent.h"
#include "proton/event.h"
#include "proton/reactor.h"

namespace proton {
namespace reactor {

// Combine's Python's: EndpointStateHandler, IncomingMessageHandler, OutgoingMessageHandler


class MessagingAdapter : public MessagingHandler
{
  public:
    PN_CPP_EXTERN MessagingAdapter(MessagingHandler &delegate);
    PN_CPP_EXTERN virtual ~MessagingAdapter();
    PN_CPP_EXTERN virtual void onReactorInit(Event &e);
    PN_CPP_EXTERN virtual void onLinkFlow(Event &e);
    PN_CPP_EXTERN virtual void onDelivery(Event &e);
    PN_CPP_EXTERN virtual void onUnhandled(Event &e);
    PN_CPP_EXTERN virtual void onConnectionClosed(Event &e);
    PN_CPP_EXTERN virtual void onConnectionClosing(Event &e);
    PN_CPP_EXTERN virtual void onConnectionError(Event &e);
    PN_CPP_EXTERN virtual void onConnectionLocalOpen(Event &e);
    PN_CPP_EXTERN virtual void onConnectionRemoteOpen(Event &e);
    PN_CPP_EXTERN virtual void onConnectionRemoteClose(Event &e);
    PN_CPP_EXTERN virtual void onConnectionOpened(Event &e);
    PN_CPP_EXTERN virtual void onConnectionOpening(Event &e);
    PN_CPP_EXTERN virtual void onSessionClosed(Event &e);
    PN_CPP_EXTERN virtual void onSessionClosing(Event &e);
    PN_CPP_EXTERN virtual void onSessionError(Event &e);
    PN_CPP_EXTERN virtual void onSessionLocalOpen(Event &e);
    PN_CPP_EXTERN virtual void onSessionRemoteOpen(Event &e);
    PN_CPP_EXTERN virtual void onSessionRemoteClose(Event &e);
    PN_CPP_EXTERN virtual void onSessionOpened(Event &e);
    PN_CPP_EXTERN virtual void onSessionOpening(Event &e);
    PN_CPP_EXTERN virtual void onLinkClosed(Event &e);
    PN_CPP_EXTERN virtual void onLinkClosing(Event &e);
    PN_CPP_EXTERN virtual void onLinkError(Event &e);
    PN_CPP_EXTERN virtual void onLinkLocalOpen(Event &e);
    PN_CPP_EXTERN virtual void onLinkRemoteOpen(Event &e);
    PN_CPP_EXTERN virtual void onLinkRemoteClose(Event &e);
    PN_CPP_EXTERN virtual void onLinkOpened(Event &e);
    PN_CPP_EXTERN virtual void onLinkOpening(Event &e);
    PN_CPP_EXTERN virtual void onTransportTailClosed(Event &e);
  private:
    MessagingHandler &delegate;  // The handler for generated MessagingEvent's
};


}} // namespace proton::reactor

#endif  /*!PROTON_CPP_MESSAGING_ADAPTER_H*/