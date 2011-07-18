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

namespace HamLog {
	
RequestHandler::RequestHandler() {
	
}

void RequestHandler::addResponder(RequestResponder::ref responder) {
	m_responders[responder->getURI()] = responder;
}

Reply::ref RequestHandler::handleRequest(Request::ref req) {
	if (m_responders.find(req->getURI()) == m_responders.end()) {
		Reply::ref reply(new Reply(Reply::not_found, "text/html"));
		reply->setContent("<html><head></head><body><b>test</b></body></html>");
		return reply;
	}
// 	Reply::ref reply(new Reply(Reply::ok, "text/html"));
	

	return Reply::ref();
}

}
