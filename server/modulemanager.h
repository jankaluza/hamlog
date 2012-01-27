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
#include "session.h"
#include <list>

namespace HamLog {

class SharedLibrary;
class Server;
class Module;

typedef Module* (*module_init)(Server *);

class ModuleManager {
	public:
		static ModuleManager *getInstance();

		void loadModules(Server *server, const std::string &path);

		bool handleRequest(Session::ref session, Request::ref request, Reply::ref reply);

		void handleSessionFinished(Session::ref session);

		void sendModulesList(Session::ref session, Request::ref request, Reply::ref reply);
	
	private:
        struct ModuleInfo {
			SharedLibrary *library;
			Module *module;
		};

		ModuleManager();
		static ModuleManager *m_instance;
		std::map<std::string, ModuleInfo *> m_modules;
	
};

}
