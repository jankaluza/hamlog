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

#include "request.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/foreach.hpp>

namespace HamLog {
	
Request::Request()
	: m_majorVersion(0),
	m_minorVersion(0),
	m_finished(false) {
	m_method.reserve(4);
}

bool Request::hasHeader(const std::string &name) {
	BOOST_FOREACH(Header &header, m_headers) {
		if (header.name == name)
			return true;
	}
	return false;
}

std::string Request::getHeader(const std::string &name) {
	BOOST_FOREACH(Header &header, m_headers) {
		if (header.name == name) {
			return header.value;
		}
	}
	return "";
}

void Request::dump() {
	std::cout << "Method: " << m_method << "\n";
	std::cout << "HTTP major version: " << m_majorVersion << "\n";
	std::cout << "HTTP minor version: " << m_minorVersion << "\n";
	std::cout << "URI: " << m_uri << "\n";
	std::cout << "Finished: " << m_finished << "\n";
}


}
