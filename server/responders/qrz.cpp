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
#include "qrz_users_table.h"

#include "tinyxml.h"

#define CREATE_QRZ_FIELD(X) 	TiXmlHandle X##Handle(&d); \
	TiXmlElement *X##Element = X##Handle.FirstChild("QRZDatabase").FirstChild("Callsign").FirstChild(#X).ToElement(); \
	std::string X = X##Element ? X##Element->GetText() : "";


namespace HamLog {
namespace Responder {

using boost::asio::ip::tcp;

class QRZModuleData : public Session::ModuleData {
	public:
		QRZModuleData(Server *server) : socket(server->getIOService()) {}
		virtual ~QRZModuleData() {}

		std::string key;
		tcp::socket socket;
		std::string call;
		boost::asio::streambuf request;
		boost::asio::streambuf response;
		Reply::ref reply;
};

QRZ::QRZ(Server *server) : RequestResponder("QRZ module", "/qrz", true), m_server(server), m_resolver(server->getIOService()), m_addUser("qrz_users"), m_getUser("qrz_users") {
	CREATE_QRZ_USERS_TABLE();

	m_addUser.what(&m_addData);

	m_getUser.what("name");
	m_getUser.what("password");

	tcp::resolver::query query("qrz.com", "http");
	m_resolver.async_resolve(query, boost::bind(&QRZ::handleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void QRZ::handleResolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator) {
	if (!err) {
		m_endpoint = *endpoint_iterator;
		boost::system::error_code ec;
		std::string address = m_endpoint.address().to_string(ec);
		std::cout << "QRZ: qrz.com resolved to " << address << "\n";
	}
	else {
		std::cout << "Error resolving QRZ.com: " << err.message() << "\n";
	}
}

void QRZ::handleQRZReadKey(Session *session, const boost::system::error_code& err) {
	QRZModuleData *data = dynamic_cast<QRZModuleData *>(session->getModuleData("/qrz"));
	if (err) {
		data->call = "";
		data->reply->setStatus(Reply::bad_request);
		data->reply->setContent("Error receiving data from QRZ.com server");
		session->sendAsyncReply();
		data->reply.reset();
		return;
	}

	// get the data and skip HTTP header
	std::string xml(boost::asio::buffer_cast<const char*>(data->response.data()));
	xml = xml.substr(xml.find("\r\n\r\n") + 4);

	data->response.consume(data->response.size());
	data->socket.close();

	std::cout << "RECEIVED FROM QRZ: " << xml << "\n";

	// parse XML
	TiXmlDocument d;
	d.Parse(xml.c_str());

	// handle error
	TiXmlHandle errorHandle(&d);
	TiXmlElement *error = errorHandle.FirstChild("QRZDatabase").FirstChild("Session").Child("Error", 0).ToElement();
	if (error) {
		data->call = "";
		data->reply->setStatus(Reply::unauthorized);
		data->reply->setContent(error->GetText());
		session->sendAsyncReply();
		data->reply.reset();
		return;
	}

	// get the key
	TiXmlHandle keyHandle(&d);
	TiXmlElement *key = keyHandle.FirstChild("QRZDatabase").FirstChild("Session").Child("Key", 0).ToElement();
	if (key) {
		data->key = key->GetText();
	}

	// handle Callsign
	TiXmlHandle callsignHandle(&d);
	TiXmlElement *callsign = callsignHandle.FirstChild("QRZDatabase").FirstChild("Callsign").ToElement();
	if (callsign) {
		CREATE_QRZ_FIELD(fname);
		CREATE_QRZ_FIELD(name);

		std::string res = "fname;name\n";
		res += fname + ";";
		res += name;

		data->call = "";
		data->reply->setContent(res);
		session->sendAsyncReply();
		data->reply.reset();
	}
	else {
		// we haven't received callsign yet, so ask for the data->call
		std::ostream request_stream(&data->request);
		request_stream << "GET http://www.qrz.com/xml?s=" << data->key << ";callsign=" << data->call << " HTTP/1.0\r\n";
		request_stream << "Host: www.qrz.com\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
		data->socket.async_connect(m_endpoint, boost::bind(&QRZ::handleQRZConnected, this, session, boost::asio::placeholders::error));
	}

}

void QRZ::handleQRZWriteRequest(Session *session, const boost::system::error_code& err) {
	QRZModuleData *data = dynamic_cast<QRZModuleData *>(session->getModuleData("/qrz"));
	if (err) {
		data->call = "";
		data->reply->setStatus(Reply::bad_request);
		data->reply->setContent("Can't send data to QRZ.com server");
		session->sendAsyncReply();
		data->reply.reset();
		return;
	}

	boost::asio::async_read_until(data->socket, data->response, "</QRZDatabase>", boost::bind(&QRZ::handleQRZReadKey, this, session, boost::asio::placeholders::error));
}

void QRZ::handleQRZConnected(Session *session, const boost::system::error_code& err) {
	QRZModuleData *data = dynamic_cast<QRZModuleData *>(session->getModuleData("/qrz"));
	if (err) {
		data->call = "";
		data->reply->setStatus(Reply::bad_request);
		data->reply->setContent("Can't connect QRZ.com server");
		session->sendAsyncReply();
		data->reply.reset();
		return;
	}

	boost::asio::async_write(data->socket, data->request, boost::bind(&QRZ::handleQRZWriteRequest, this, session, boost::asio::placeholders::error));
}

bool QRZ::askQRZ(Session *session, Reply::ref reply, const std::string &call) {
	QRZModuleData *data = dynamic_cast<QRZModuleData *>(session->getModuleData("/qrz"));
	if (data != NULL && !data->key.empty()) {
		// TODO: ask qrz with data->key
	}
	else {
		// We haven't created our module data for this user yet, so create it now
		if (data == NULL) {
			data = new QRZModuleData(m_server);
			session->setModuleData("/qrz", data);
		}

		// Get the QRZ username/password from database
		std::list<std::list<std::string> > user;
		m_getUser.into(&user);
		m_getUser.where("user_id", boost::lexical_cast<std::string>(session->getId()));
		StorageBackend::getInstance()->select(m_getUser);

		if (user.empty()) {
			reply->setStatus(Reply::unauthorized);
			reply->setContent("Register your QRZ account before querying the database");
			return false;
		}

		// Since now it's clear we will ask the server and won't be able to answer just now,
		// so switch to async reply mode
		reply->setAsync();

		std::string username = user.front().front();
		std::string password = user.front().back();

		// generate the request to get the key
		std::ostream request_stream(&data->request);
		request_stream << "GET http://www.qrz.com/xml?username=" << username << ";password=" << password << ";agent=hamlog HTTP/1.0\r\n";
		request_stream << "Host: www.qrz.com\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// store the data for this request and send the request
		data->call = call;
		data->reply = reply;
		data->socket.async_connect(m_endpoint, boost::bind(&QRZ::handleQRZConnected, this, session, boost::asio::placeholders::error));
	}

	return true;
}

void QRZ::sendQRZ(Session *session, Request::ref request, Reply::ref reply) {
	std::string call = request->getContent();
	askQRZ(session, reply, call);
}

void QRZ::addUser(Session *session, Request::ref request, Reply::ref reply) {
	std::string username = request->getHeader("username");
	std::string password = request->getHeader("password");


	m_addData["name"] = username;
	m_addData["password"] = password;
	m_addData["user_id"] = boost::lexical_cast<std::string>(session->getId());

	if (StorageBackend::getInstance()->insert(m_addUser)) {
		reply->setContent("Registered");
	}
	else {
		if (StorageBackend::getInstance()->update(m_addUser)) {
			reply->setContent("Updated");
		}
		else {
			reply->setStatus(Reply::bad_request);
			reply->setContent("Bad data or SQL error");
		}
	}
}

void QRZ::sendUsername(Session *session, Request::ref request, Reply::ref reply) {
	m_getUser.where("user_id", boost::lexical_cast<std::string>(session->getId()));
	std::list<std::list<std::string> > user;
	m_getUser.into(&user);
	StorageBackend::getInstance()->select(m_getUser);

	if (user.empty()) {
		reply->setContent("");
		return;
	}

	reply->setContent(user.front().front());
}

bool QRZ::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();

	if (uri == "/qrz") {
		sendQRZ(session, request, reply);
	}
	else if (uri == "/qrz/register") {
		addUser(session, request, reply);
	}
	else if (uri == "/qrz/username") {
		sendUsername(session, request, reply);
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

