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

namespace HamLog {

class Reply : public boost::enable_shared_from_this<Reply> {
	public:
		typedef boost::shared_ptr<Reply> ref;

		typedef struct _Header {
			std::string name;
			std::string value;
		} Header;

		typedef enum {
			ok = 200,
			moved_permanently = 301,
			moved_temporarily = 302,
			not_modified = 304,
			bad_request = 400,
			unauthorized = 401,
			forbidden = 403,
			not_found = 404,
			internal_server_error = 500,
		} Status;


		Reply(Status status = Reply::ok, const std::string &content_type = "text/hamlog");

		void dump();

		std::string toString();

		void setStatus(const Status &status) {
			m_status = status;
		}

		void setContentType(const std::string &contentType) {
			m_headers[0].value = contentType;
		}

		void setContent(const std::string &content) {
			m_content = content;
		}

		void addHeader(const Header &header) {
			m_headers.push_back(header);
		}

		bool hasHeader(const std::string &name);

		std::string getHeader(const std::string &name);

	private:
		Status m_status;
		std::vector<Header> m_headers;
		std::string m_content;
};

}
