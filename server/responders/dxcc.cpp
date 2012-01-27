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

#include "dxcc.h"
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
#include "server.h"
#include "log.h"


namespace HamLog {
namespace Responder {

DEFINE_LOGGER(logger, "DXCC");
	
DXCC::DXCC() : RequestResponder("DXCC module", "/dxcc", Module::CALLINFO, false) {
	std::string line;
	std::ifstream f("cty.csv");
	if (!f.is_open()) {
		LOG_ERROR(logger, "Unable to open cty.csv file!"); 
		return;
	}

	while(f.good()) {
		getline(f, line);
		std::vector<std::string> fields;
		boost::split(fields, line, boost::is_any_of(","));

		if (fields.size() != 10)
			continue;

		Entity e;
		e.name = fields[1];
		e.continent = fields[3];
		e.cq = boost::lexical_cast<int>(fields[4]);
		e.itu = boost::lexical_cast<int>(fields[5]);
		e.lat = boost::lexical_cast<float>(fields[6]);
		e.lon = boost::lexical_cast<float>(fields[7]);
		e.tz =  boost::lexical_cast<float>(fields[8]);

		std::string data = fields[9].substr(0, fields[9].size() - 2);

		std::vector<std::string> prefixes;
		boost::split(prefixes, data, boost::is_any_of(" "));
		BOOST_FOREACH(std::string prefix, prefixes) {
			if (prefix.find("=") == 0) {
				prefix.erase(prefix.begin());
			}
			m_prefixes[prefix] = e;
		}
	}
    f.close();
}

void DXCC::sendDXCC(Session::ref session, Request::ref request, Reply::ref reply) {
	std::string call = request->getContent();
	if (call.find("/") != std::string::npos) {
		// TODO: handle special IDs
	}

	LOG_INFO(logger, "Answering DXCC request");

	// TODO: rewrite me once I will fully understand dxcc :-P
	while (call.size() != 0) {
		if (m_prefixes.find(call) != m_prefixes.end()) {
			Entity e = m_prefixes[call];

			std::string data = "qth;continent;cq;itu;latitude;longitude;tz\n";
			data += e.name + ";";
			data += e.continent + ";";
			data += boost::lexical_cast<std::string>(e.cq) + ";";
			data += boost::lexical_cast<std::string>(e.itu) + ";";
			data += boost::lexical_cast<std::string>(e.lat) + ";";
			data += boost::lexical_cast<std::string>(e.lon) + ";";
			data += boost::lexical_cast<std::string>(e.tz);
			reply->setContent(data);
			return;
		}
		call.erase(call.end() - 1);
	}
	reply->setStatus(Reply::bad_request);
	reply->setContent("unknown");
}

bool DXCC::handleRequest(Session::ref session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();

	if (uri == "/dxcc") {
		sendDXCC(session, request, reply);
	}
	else {
		return false;
	}

	return true;
}

extern "C" {
	Module *module_init(Server *);
    Module *module_init(Server *) {
		return new DXCC();
    }
}

}
}
