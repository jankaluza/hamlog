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

#include "login.h"
#include <boost/bind.hpp>
#include <iostream>
#include "reply.h"
#include "session.h"
#include "storagebackend.h"

namespace HamLog {
namespace Responder {
	
Login::Login() : RequestResponder("/login", false) {
	
}

void Login::createAuthorizationRequest(Reply::ref reply) {
	Reply::Header header;
	header.name = "WWW-Authenticate";

	std::string nonce = "dcd98b7102dd2f0e8b11d0f600bfb0c093"; // TODO: generate
	std::string opaque = "5ccc069c403ebaf9f0171e9517f40e41"; // TODO: generate
	header.value  = "Digest realm=\"realm@hamlog\",qop=\"auth,auth-int\",nonce=\"";
	header.value += nonce + "\",opaque=\"" + opaque + "\"";
	reply->addHeader(header);

	reply->setContent("Authentication");
	reply->setStatus(Reply::unauthorized);
	reply->setContentType("text/html");
}

bool Login::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	if (!request->hasHeader("Authorization")) {
		createAuthorizationRequest(reply);
	}
	else {
		StorageBackend::User user = StorageBackend::getInstance()->getUser("test");
		std::string auth = reply->getHeader("Authorization");
		session->setAuthenticated(true);
		reply->setContent("Authorized");
		reply->setContentType("text/html");
	}
	return true;
}

}
}
