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

#include "dxcluster.h"
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

DEFINE_LOGGER(logger, "DXCluster");

using boost::asio::ip::tcp;

class DXClusterModuleData : public Session::ModuleData {
	public:
		DXClusterModuleData(Server *server) : socket(server->getIOService()) {}
		virtual ~DXClusterModuleData() {
			socket.close();
		}

		std::string pending_data;
		tcp::socket socket;
		boost::asio::streambuf response;
		boost::asio::streambuf request;
};

DXCluster::DXCluster(Server *server) : RequestResponder("DXCluster", "/dxcluster", Module::DXCLUSTER, true), m_server(server), m_resolver(server->getIOService()) {
	tcp::resolver::query query("dxspots.com", "telnet");
	m_resolver.async_resolve(query, boost::bind(&DXCluster::handleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void DXCluster::handleSessionFinished(Session::ref session) {
	DXClusterModuleData *data = dynamic_cast<DXClusterModuleData *>(session->getModuleData("/dxcluster"));
	if (!data) {
		return;
	}

	data->socket.close();
}

void DXCluster::handleResolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator) {
	if (!err) {
		m_endpoint = *endpoint_iterator;
		boost::system::error_code ec;
		std::string address = m_endpoint.address().to_string(ec);
		LOG_INFO(logger, "dxspots.com resolved to " << address);
	}
	else {
		LOG_ERROR(logger, "Error resolving dxspots.com: " << err.message());
	}
}

void DXCluster::handleDXClusterRead(Session::ref session, const boost::system::error_code& err) {
	DXClusterModuleData *data = dynamic_cast<DXClusterModuleData *>(session->getModuleData("/dxcluster"));
	if (err) {
		LOG_ERROR(logger, session->getUsername() << ": Error receiving data form dxspots.com");
		return;
	}

	// get the data and skip HTTP header
	std::ostringstream ss;
	ss << &data->response;
	std::string s = ss.str();

	data->pending_data += s;
	LOG_INFO(logger, s);

	boost::asio::async_read(data->socket, data->response, boost::asio::transfer_at_least(1), boost::bind(&DXCluster::handleDXClusterRead, this, session, boost::asio::placeholders::error));
}

void DXCluster::handleDXClusterLogin(Session::ref session, const boost::system::error_code& err) {
	DXClusterModuleData *data = dynamic_cast<DXClusterModuleData *>(session->getModuleData("/dxcluster"));
	if (err) {
		LOG_ERROR(logger, session->getUsername() << ": Can't login");
		return;
	}

	boost::asio::async_read(data->socket, data->response, boost::asio::transfer_at_least(1), boost::bind(&DXCluster::handleDXClusterRead, this, session, boost::asio::placeholders::error));
}

void DXCluster::handleDXClusterConnected(Session::ref session, const boost::system::error_code& err) {
	DXClusterModuleData *data = dynamic_cast<DXClusterModuleData *>(session->getModuleData("/dxcluster"));
	if (err) {
		LOG_ERROR(logger, session->getUsername() << ": Can't connect to dxspots.com server");
		return;
	}

	LOG_INFO(logger, session->getUsername() << ": Connected to dxspots.com server");
	
	std::ostream request_stream(&data->request);
	request_stream << session->getUsername() << "\n";
	boost::asio::async_write(data->socket, data->request, boost::bind(&DXCluster::handleDXClusterLogin, this, session, boost::asio::placeholders::error));
}

bool DXCluster::askDXCluster(Session::ref session, Reply::ref reply, const std::string &call) {
	DXClusterModuleData *data = dynamic_cast<DXClusterModuleData *>(session->getModuleData("/dxcluster"));
	if (data != NULL) {
		LOG_INFO(logger, session->getUsername() << ": Forwarwding DXCluster data");

		reply->setContent(data->pending_data);
		data->pending_data = "";
	}
	else {
		// We haven't created our module data for this user yet, so create it now
		if (data == NULL) {
			data = new DXClusterModuleData(m_server);
			session->setModuleData("/dxcluster", data);
		}

		LOG_INFO(logger, session->getUsername() << ": Connecting to dxspots.com");
		data->socket.async_connect(m_endpoint, boost::bind(&DXCluster::handleDXClusterConnected, this, session, boost::asio::placeholders::error));
	}

	return true;
}

void DXCluster::sendDXCluster(Session::ref session, Request::ref request, Reply::ref reply) {
	std::string call = request->getContent();
	askDXCluster(session, reply, call);
}
bool DXCluster::handleRequest(Session::ref session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();

	if (uri == "/dxcluster") {
		sendDXCluster(session, request, reply);
	}
	else {
		return false;
	}

	return true;
}

extern "C" {
	Module *module_init(Server *);
    Module *module_init(Server *server) {
		return new DXCluster(server);
    }
}

}
}

