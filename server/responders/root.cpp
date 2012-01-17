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

#include "root.h"
#include <boost/bind.hpp>
#include <iostream>
#include "reply.h"
#include "session.h"
#include "server.h"

namespace HamLog {
namespace Responder {
	
Root::Root() : RequestResponder("Root responder module", "/", Module::UNKNOWN, false) {
	
}

bool Root::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	reply->setContentType("text/html");
	reply->setContent("<html><head></head><body>This is HamLog server. Download HamLog and setup your username to connect it.</body></html>");
	return true;
}

extern "C" {
	Module *module_init(Server *);
    Module *module_init(Server *) {
		return new Root();
    }
}

}
}
