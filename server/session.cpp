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

namespace HamLog {
	
Session::Session(boost::asio::io_service& ioService)
	: m_socket(ioService),
	m_authenticated(false),
	m_requestHandler(this),
	m_req(new Request()) {
}

Session::~Session() {
	for(std::map<std::string, ModuleData *>::iterator it = m_modulesData.begin(); it != m_modulesData.end(); it++) {
		if (it->second) {
			delete it->second;
		}
	}
}

boost::asio::ip::tcp::socket& Session::getSocket() {
	return m_socket;
}

void Session::start() {
	m_socket.async_read_some(boost::asio::buffer(m_buffer),
							 boost::bind(&Session::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Session::stop() {
	m_socket.close();
	onStopped();
}

void Session::handleRead(const boost::system::error_code& e, std::size_t bytes) {
	if (e) {
		stop();
		return;
	}

	std::cout << "data received. size = " << bytes << "\n";

	std::size_t parsed = 0;
	std::size_t parsed_total = 0;

	while (parsed_total != bytes) {

		parsed = m_requestParser.parse(m_req, m_buffer.begin() + parsed_total, m_buffer.begin() + bytes);
		if (parsed == 0) {
			std::cout << "PARSING ERROR\n";
			// TODO: send Error
			stop();
			break;
		}
		parsed_total += parsed;

		if (m_req->isFinished()) {
	// 		m_req->dump();
			Reply::ref reply = m_requestHandler.handleRequest(m_req);
			m_req.reset(new Request());

	// 		reply->dump();

			boost::asio::async_write(m_socket, boost::asio::buffer(reply->toString()),
									boost::bind(&Session::handleWrite, shared_from_this(), boost::asio::placeholders::error));
		}
	}

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
