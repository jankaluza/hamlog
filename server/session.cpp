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

#include "session.h"
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>
#include "request.h"
#include "log.h"
#include "requestparser.h"
#include "requesthandler.h"
#include "modulemanager.h"

namespace HamLog {

DEFINE_LOGGER(logger, "Session");
	
Session::Session(boost::asio::io_service& ioService)
	: m_socket(ioService),
	m_authenticated(false),
	m_requestParser(new RequestParser()),
	m_req(new Request()),
	parsed_total(0),
	bytes(0),
	m_username("") {
}

Session::~Session() {
	for(std::map<std::string, ModuleData *>::iterator it = m_modulesData.begin(); it != m_modulesData.end(); it++) {
		if (it->second) {
			delete it->second;
		}
	}

	delete m_requestParser;
	m_requestParser = NULL;

	if (m_username.empty()) {
		LOG_INFO(logger, this << ": Session destroyed");
	}
	else {
		LOG_INFO(logger, m_username << ": Session destroyed");
	}
}

boost::asio::ip::tcp::socket& Session::getSocket() {
	return m_socket;
}

void Session::start() {
	m_socket.async_read_some(boost::asio::buffer(m_buffer),
							 boost::bind(&Session::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	m_requestHandler = new RequestHandler(shared_from_this());
}

void Session::stop() {
	m_socket.close();

	ModuleManager::getInstance()->handleSessionFinished(shared_from_this());

	delete m_requestHandler;
	m_requestHandler = NULL;

	onStopped();
}

void Session::sendAsyncReply() {
	// send the reply
	boost::asio::async_write(m_socket, boost::asio::buffer(m_currentReply->toString()),
							boost::bind(&Session::handleWrite, shared_from_this(), boost::asio::placeholders::error));
	// remove it from memory
	m_currentReply.reset();
	// continue parsing data in buffer;
	boost::system::error_code e;
	handleRead(e, 0);
}

void Session::handleRead(const boost::system::error_code& e, std::size_t b) {
	if (e) {
		stop();
		return;
	}

	if (b != 0) {
		bytes = b;
	}

	if (m_username.empty()) {
		LOG_INFO(logger, this << ": " << bytes << " bytes received");
	}
	else {
		LOG_INFO(logger, m_username << ": " << bytes << " bytes received");
	}

	std::size_t parsed = 0;

	while (parsed_total != bytes) {

		parsed = m_requestParser->parse(m_req, m_buffer.begin() + parsed_total, m_buffer.begin() + bytes);
		if (parsed == 0) {
			LOG_ERROR(logger, "Parsing error!");
			// TODO: send Error
			stop();
			break;
		}
		parsed_total += parsed;

		if (m_req->isFinished()) {
			Reply::ref reply = m_requestHandler->handleRequest(m_req);
			m_req.reset(new Request());

			// Async replies are send by the module itself and we should not parse
			// more replies until this one is fully handled to keep proper order
			// of responses
			if (reply->isAsync()) {
				m_currentReply = reply;
				return;
			}
			else {
				boost::asio::async_write(m_socket, boost::asio::buffer(reply->toString()),
										boost::bind(&Session::handleWrite, shared_from_this(), boost::asio::placeholders::error));
			}
		}
	}

	parsed_total = 0;
	m_socket.async_read_some(boost::asio::buffer(m_buffer),
							 boost::bind(&Session::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Session::handleWrite(const boost::system::error_code& e) {
// 	if (!e) {
// 		boost::system::error_code ec;
// 		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
// 	}
// 
// 	if (e != boost::asio::error::operation_aborted) {
// 		stop();
// 	}
}

}
