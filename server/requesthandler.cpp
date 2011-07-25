/**
 * Hamlog
 *
 * Copyright (C) 2011, Jan Kaluza <hanzz.k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "requesthandler.h"
#include <boost/bind.hpp>
#include <iostream>
#include "reply.h"
#include "responders/root.h"
#include "responders/login.h"
#include "responders/register.h"
#include "session.h"

namespace HamLog {
	
RequestHandler::RequestHandler(Session *session)
	: m_session(session) {
	addResponder(RequestResponder::ref(new Responder::Root()));
	addResponder(RequestResponder::ref(new Responder::Login()));
	addResponder(RequestResponder::ref(new Responder::Register()));
}

RequestHandler::~RequestHandler() {
	m_responders.clear();
}

void RequestHandler::addResponder(RequestResponder::ref responder) {
	m_responders[responder->getURI()] = responder;
}

Reply::ref RequestHandler::handleRequest(Request::ref req) {
	if (m_responders.find(req->getURI()) == m_responders.end()) {
		Reply::ref reply(new Reply(Reply::not_found, "text/html"));
		reply->setContent("<html><head></head><body>440 - This page does not exist</body></html>");
		return reply;
	}

	if (m_responders[req->getURI()]->needAuthentication() && !m_session->isAuthenticated()) {
		Reply::ref reply(new Reply(Reply::unauthorized, "text/html"));
		reply->setContent("401 - Unauthorized");
		return reply;
	}

	Reply::ref reply(new Reply(Reply::ok));
	if (m_responders[req->getURI()]->handleRequest(m_session, req, reply)) {
		return reply;
	}

	reply->setStatus(Reply::bad_request);
	reply->setContent("Bad request");

	return reply;
}

}
