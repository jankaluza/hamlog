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

#include "reply.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace HamLog {

Reply::Reply(Status status, const std::string &content_type)
	: m_status(status), m_async(false) {
	m_headers.resize(2);
	m_headers[0].name = "Content-Type";
	m_headers[0].value = content_type;
	m_headers[1].name = "Content-Length";
	m_headers[1].value = "0";
}

bool Reply::hasHeader(const std::string &name) {
	BOOST_FOREACH(Header &header, m_headers) {
		if (header.name == name)
			return true;
	}
	return false;
}

std::string Reply::getHeader(const std::string &name) {
	BOOST_FOREACH(Header &header, m_headers) {
		if (header.name == name) {
			return header.value;
		}
	}
	return "";
}

void Reply::dump() {
	std::cout << toString();
}

std::string Reply::toString() {
	std::string buffers;

	switch (m_status) {
		case ok:
			buffers += ("HTTP/1.1 200 OK\r\n");
			break;
		case moved_permanently:
			buffers += ("HTTP/1.0 301 Moved Permanently\r\n");
			break;
		case moved_temporarily:
			buffers += ("HTTP/1.0 302 Moved Temporarily\r\n");
			break;
		case not_modified:
			buffers += ("HTTP/1.0 304 Not Modified\r\n");
			break;
		case bad_request:
			buffers += ("HTTP/1.0 400 Bad Request\r\n");
			break;
		case unauthorized:
			buffers += ("HTTP/1.0 401 Unauthorized\r\n");
			break;
		case forbidden:
			buffers += ("HTTP/1.0 403 Forbidden\r\n");
			break;
		case not_found:
			buffers += ("HTTP/1.0 404 Not Found\r\n");
			break;
		case internal_server_error:
			buffers += ("HTTP/1.0 500 Internal Server Error\r\n");
			break;
		default:
			buffers += ("HTTP/1.0 500 Internal Server Error\r\n");
	}

	m_headers[1].value = boost::lexical_cast<std::string>(m_content.size());

	BOOST_FOREACH(Header &header, m_headers) {
		buffers += (header.name);
		buffers += (": ");
		buffers += (header.value);
		buffers += ("\r\n");
	}

	buffers += ("\r\n");
	if (!m_content.empty()) {
		buffers += (m_content);
		buffers += ("\r\n");
	}

	return buffers;
}


}
