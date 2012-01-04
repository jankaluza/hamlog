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
	m_getLogs("logbook"),
	m_addLog("logbook"),
	m_removeLog("logbook") {
	CREATE_LOGBOOK_TABLE();
	m_addLog.what(&m_record);
}

void LogBook::sendLogs(Session *session, Reply::ref reply) {
	m_getLogs.where("user_id", boost::lexical_cast<std::string>(session->getId()));

	std::cout << "ID=" << boost::lexical_cast<std::string>(session->getId()) << "\n";
	std::list<std::list<std::string> > logbook;
	m_getLogs.into(&logbook);
	StorageBackend::getInstance()->select(m_getLogs);
	std::cout << "LOGBOOK SIZE=" << logbook.size() << "\n";

	std::string data = "id;user_id;callsign;date;qth;loc;latitude;longitude;country;continent;itu;cq\n";
	BOOST_FOREACH(std::list<std::string> &entry, logbook) {
		BOOST_FOREACH(std::string &col, entry) {
			data += col + ";";
		}
		data.erase(data.end() - 1);
		data += "\n";
	}

	reply->setContent(data);
}

std::vector<std::vector<std::string> > LogBook::parse(const std::string &str) {
	std::vector<std::vector<std::string> > tokens;
     
	unsigned int pos = 0;
	bool quotes = false;
	std::string field = "";

	tokens.resize(tokens.size() + 1);

    while(pos < str.length() && str[pos] != 0) {
		char c = str[pos];
		if (!quotes && c == '"' ) {
			quotes = true;
		}
		else if (quotes && c == '"' ) {
			if (pos + 1 <str.length() && str[pos+1]== '"' ) {
				field.push_back(c);
				pos++;
			}
			else {
				quotes = false;
			}
		}
		else if (!quotes && c == ';') {
			tokens.back().push_back(field);
			field.clear();
		}
		else if (!quotes && ( c == '\n' || c == '\r' )) {
			tokens.back().push_back(field);
			std::cout << "'" << field << "'\n";
			field.clear();
			tokens.resize(tokens.size() + 1);
		}
		else {
			field.push_back(c);
		}
		pos++;
	}

	return tokens;
}

void LogBook::addLog(Session *session, Request::ref request, Reply::ref reply) {
	std::vector<std::vector<std::string> > data = parse(request->getContent());
	std::cout << "DATA=" << request->getContent() << "\n";
	std::vector<std::string> header = data.front();
	data.erase(data.begin());

	m_record.clear();
	BOOST_FOREACH(std::vector<std::string> &row, data) {
		if (row.size() == 0)
			continue;

		if (header.size() != row.size()) {
			reply->setContent("Bad column count.");
			reply->setStatus(Reply::bad_request);
			return;
		}

		for (int i = 0; i != row.size(); i++) {
			m_record[header[i]] = row[i];
		}

		// "-1" is new record
		if (m_record["id"] == "-1") {
			m_record.erase("id");
			m_record["user_id"] = boost::lexical_cast<std::string>(session->getId());
			if (!StorageBackend::getInstance()->insert(m_addLog)) {
				reply->setContent("Bad input data or database error.");
				reply->setStatus(Reply::bad_request);
				return;
			}
			reply->setContent(boost::lexical_cast<std::string>(StorageBackend::getInstance()->lastInsertedID()) + ";" + boost::lexical_cast<std::string>(session->getId()));
		}
		else {
			m_addLog.where("user_id", boost::lexical_cast<std::string>(session->getId()));
			m_addLog.where("id", m_record["id"]);
			m_record.erase("user_id");
			std::string id = m_record["id"];
			m_record.erase("id");
			if (!StorageBackend::getInstance()->update(m_addLog)) {
				reply->setContent("Bad input data or database error.");
				reply->setStatus(Reply::bad_request);
				return;
			}
			reply->setContent(id + ";" + boost::lexical_cast<std::string>(session->getId()));
		}
		
	}

	
}

void LogBook::removeLog(Session *session, Request::ref request, Reply::ref reply) {
	std::vector<std::vector<std::string> > data = parse(request->getContent());
	std::vector<std::string> ids = data.front();
	data.erase(data.begin());

	BOOST_FOREACH(std::string &id, ids) {
		m_removeLog.where("id", id);
		m_removeLog.where("user_id", boost::lexical_cast<std::string>(session->getId()));
		StorageBackend::getInstance()->remove(m_removeLog);
	}
	reply->setContent("Removed.");
}

bool LogBook::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();
	std::cout << "LogBook::handleRequest " << uri << "\n";

	if (uri == "/logbook") {
		sendLogs(session, reply);
	}
	else if (uri == "/logbook/add") {
		addLog(session, request, reply);
	}
	else if (uri == "/logbook/remove") {
		removeLog(session, request, reply);
	}
	else {
		return false;
	}

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
