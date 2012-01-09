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

#include "qrz.h"
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "reply.h"
#include "session.h"
#include "storagebackend.h"
#include "../md5.h"
#include "logbook_table.h"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"
#include <boost/algorithm/string.hpp>


namespace HamLog {
namespace Responder {
	
QRZ::QRZ() : RequestResponder("QRZ module", "/qrz", false) {

}

void QRZ::sendQRZ(Session *session, Request::ref request, Reply::ref reply) {
	reply->setStatus(Reply::bad_request);
	reply->setContent("unknown");
}

bool QRZ::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();

	if (uri == "/qrz") {
		sendQRZ(session, request, reply);
	}
	else {
		return false;
	}

	return true;
}

extern "C" {
	Module *module_init();
    Module *module_init() {
		return new QRZ();
    }
}

}
}
