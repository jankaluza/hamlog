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
#include "server.h"



namespace HamLog {
namespace Responder {

using boost::asio::ip::tcp;

QRZ::QRZ(Server *server) : RequestResponder("QRZ module", "/qrz", false), m_server(server), m_resolver(server->getIOService()) {
	tcp::resolver::query query("qrz.com", "http");
	m_resolver.async_resolve(query, boost::bind(&QRZ::handleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void QRZ::handleResolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator) {
	if (!err) {
		m_endpoint = *endpoint_iterator;
		boost::system::error_code ec;
		std::string address = m_endpoint.address().to_string(ec);
		std::cout << "QRZ: qrz.com resolved to " << address << "\n";

// 		socket_.async_connect(endpoint, boost::bind(&client::handle_connect, this, boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else {
		std::cout << "Error resolving QRZ.com: " << err.message() << "\n";
	}
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
	Module *module_init(Server *);
    Module *module_init(Server *server) {
		return new QRZ(server);
    }
}

}
}
