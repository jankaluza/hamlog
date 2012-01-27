
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
#include "../md5.h"
#include "module.h"
#include "users_table.h"
#include "boost/lexical_cast.hpp"
#include "server.h"
#include "log.h"

namespace HamLog {
namespace Responder {

DEFINE_LOGGER(logger, "Login Responder");
	
/* Inspiration taken from libgsasl */
static void digest_md5_parse(std::map<std::string, std::string> &ret, const char *digest) {
	const char *token_start, *val_start, *val_end, *cur;

	cur = digest;
	while(*cur != '\0') {
		/* Find the end of the token */
		int in_quotes = 0;
		char *name, *value = NULL;
		token_start = cur;
		while(*cur != '\0' && (in_quotes || (!in_quotes && *cur != ','))) {
			if (*cur == '"')
				in_quotes = !in_quotes;
			cur++;
		}

		/* Find start of value.  */
		val_start = strchr(token_start, '=');
		if (val_start == NULL || val_start > cur)
			val_start = cur;

		if (token_start != val_start) {
			name = strndup(token_start, val_start - token_start);

			if (val_start != cur) {
				val_start++;
				while (val_start != cur && (*val_start == ' ' || *val_start == '\t'
						|| *val_start == '\r' || *val_start == '\n'
						|| *val_start == '"'))
					val_start++;

				val_end = cur;
				while (val_end >= val_start && (*val_end == ' ' || *val_end == ',' || *val_end == '\t'
						|| *val_end == '\r' || *val_end == '\n'
						|| *val_end == '"'  || *val_end == '\0'))
					val_end--;

				if (val_end - val_start + 1 >= 0)
					value = strndup(val_start, val_end - val_start + 1);
			}
			ret[name] = value;
			free(name);
			free(value);
		}

		/* Find the start of the next token, if there is one */
		if (*cur != '\0') {
			cur++;
			while (*cur == ' ' || *cur == ',' || *cur == '\t'
					|| *cur == '\r' || *cur == '\n')
				cur++;
		}
	}

}
	
Login::Login() : RequestResponder("Login module", "/login", Module::UNKNOWN, false),
	m_getUser("users") {
	CREATE_USERS_TABLE();

	m_getUser.what("password");
	m_getUser.what("id");
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

bool Login::handleRequest(Session::ref session, Request::ref request, Reply::ref reply) {
	if (!request->hasHeader("Authorization")) {
		createAuthorizationRequest(reply);
	}
	else {
		std::string auth = request->getHeader("Authorization");
		std::map<std::string, std::string> fields;
		digest_md5_parse(fields, auth.substr(7).c_str());

#define HAS(NAME) (fields.find(NAME) != fields.end())

		if (!HAS("username") || !HAS("realm") || !HAS("nonce") || !HAS("uri") ||
			!HAS("qop") || !HAS("response") || !HAS("opaque")) {
			createAuthorizationRequest(reply);
			return true;
		}

#undef HAS

		m_getUser.where("name", fields["username"]);
		std::list<std::list<std::string> > user;
		m_getUser.into(&user);
		StorageBackend::getInstance()->select(m_getUser);

		if (user.empty()) {
			session->setAuthenticated(false, 0, "");
			createAuthorizationRequest(reply);
			return true;
		}

		std::string ha1 = user.front().front();
		std::string ha2 = MD5::getHashHEX("GET:/login");
		std::string a3 = ha1 + ":" + "dcd98b7102dd2f0e8b11d0f600bfb0c093" + ":" + ha2;
		std::string response = MD5::getHashHEX(a3);

		// TODO: check nonce and opaque
		if (response == fields["response"]) {
			unsigned long id = boost::lexical_cast<unsigned long>(user.front().back());
			session->setAuthenticated(true, id, fields["username"]);
			reply->setContent("Authorized");
			reply->setContentType("text/html");

			LOG_INFO(logger, session << ": User '" << fields["username"] << "' authorized");
		}
		else {
			session->setAuthenticated(false, 0, "");
			createAuthorizationRequest(reply);
			LOG_INFO(logger, session << ": User '" << fields["username"] << "' not authorized. Bad password or username");
		}
	}
	return true;
}

extern "C" {
	Module *module_init(Server *);
    Module *module_init(Server *) {
		return new Login();
    }
}

}
}
