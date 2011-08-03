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

#include "logbook.h"
#include <boost/bind.hpp>
#include <iostream>
#include "reply.h"
#include "session.h"
#include "storagebackend.h"
#include "../md5.h"
#include "logbook_table.h"
#include "boost/lexical_cast.hpp"
#include "boost/foreach.hpp"

namespace HamLog {
namespace Responder {
	
LogBook::LogBook() : RequestResponder("LogBook module", "/logbook", false),
	m_getLogs("logbook") {
	CREATE_LOGBOOK_TABLE();
}

bool LogBook::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	if (request->getMethod() != "GET") {
		return false;
	}

	m_getLogs.where("user_id", boost::lexical_cast<std::string>(session->getId()));

	std::cout << "ID=" << boost::lexical_cast<std::string>(session->getId()) << "\n";
	std::list<std::list<std::string> > logbook;
	m_getLogs.into(&logbook);
	StorageBackend::getInstance()->select(m_getLogs);

	std::string data = "ID;USER_ID;CALL;DATE;QTH;LOC\n";
	BOOST_FOREACH(std::list<std::string> &entry, logbook) {
		BOOST_FOREACH(std::string &col, entry) {
			data += col + ";";
		}
		data += "\n";
	}

	reply->setContent(data);

	return true;
}

extern "C" {
	Module *module_init();
    Module *module_init() {
		return new LogBook();
    }
}

}
}
