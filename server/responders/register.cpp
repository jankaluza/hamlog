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

#include "register.h"
#include <boost/bind.hpp>
#include <iostream>
#include "reply.h"
#include "session.h"
#include "storagebackend.h"
#include "../md5.h"
#include "users_table.h"

namespace HamLog {
namespace Responder {
	
Register::Register() : RequestResponder("Register module", "/register", Module::UNKNOWN, false),
	m_addUser("users") {
	CREATE_USERS_TABLE();
	m_addUser.what(&m);
}

bool Register::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	if (request->getMethod() != "GET") {
		return false;
	}

	std::string username = request->getHeader("username");
	std::string password = MD5::getHashHEX(username + ":realm@hamlog:" + request->getHeader("password"));


	m["name"] = username;
	m["password"] = password;

	if (StorageBackend::getInstance()->insert(m_addUser)) {
		reply->setContent("Registered");
	}
	else {
		reply->setStatus(Reply::bad_request);
		reply->setContent("This username is already taken.");
	}

	return true;
}

extern "C" {
	Module *module_init();
    Module *module_init() {
		return new Register();
    }
}

}
}
