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

#pragma once

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include "request.h"

namespace HamLog {

class RequestParser : public boost::enable_shared_from_this<RequestParser> {
	public:
		typedef boost::shared_ptr<RequestParser> ref;

		RequestParser();

		void reset();

		template <typename Iterator>
		bool parse(Request::ref req, Iterator begin, Iterator end) {
			while (begin != end) {
				if (handleChar(req, *begin++) == false) {
					return false;
				}
			}
			return true;
		}

	private:
		bool handleChar(Request::ref req, char c);
		bool isChar(int c);
		bool isControl(int c);
		bool isSpecial(int c);
		bool isDigit(int c);

		enum state {
			method_start,
			method,
			uri_start,
			uri,
			http_version_h,
			http_version_t_1,
			http_version_t_2,
			http_version_p,
			http_version_slash,
			http_version_major_start,
			http_version_major,
			http_version_minor_start,
			http_version_minor,
			expecting_newline_1,
			header_line_start,
			header_lws,
			header_name,
			space_before_header_value,
			header_value,
			expecting_newline_2,
			expecting_newline_3,
			content_start,
			content,
			expecting_newline_4
		} m_state;
};

}
