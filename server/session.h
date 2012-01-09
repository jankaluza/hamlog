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
#include <string>
#include "requestparser.h"
#include "requesthandler.h"

namespace HamLog {

class Session : public boost::enable_shared_from_this<Session> {
	public:
		typedef boost::shared_ptr<Session> ref;

		class ModuleData {
			public:
				virtual ~ModuleData();
		};

		Session(boost::asio::io_service& ioService);
		~Session();

		boost::asio::ip::tcp::socket& getSocket();

		void start();

		void stop();

		boost::signal<void ()> onStopped;

		void setAuthenticated(bool authenticated, unsigned long id) {
			m_authenticated = authenticated;
			m_id = id;
		}

		unsigned long getId() {
			return m_id;
		}

		bool isAuthenticated() {
			return m_authenticated;
		}

		void setModuleData(std::string module, ModuleData *data) {
			m_modulesData[module] = data;
		}

		ModuleData *getModuleData(std::string module) {
			return m_modulesData[module];
		}

	private:
		void handleRead(const boost::system::error_code& e, std::size_t bytes);
		void handleWrite(const boost::system::error_code& e);

		boost::asio::ip::tcp::socket m_socket;
		bool m_authenticated;
		RequestParser m_requestParser;
		RequestHandler m_requestHandler;
		Request::ref m_req;
		boost::array<char, 8192> m_buffer;
		unsigned long m_id;
		std::map<std::string, ModuleData *> m_modulesData;
};

}
