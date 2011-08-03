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

#include "modulemanager.h"
#include <boost/bind.hpp>
#include <iostream>
#include <string>
#include <dlfcn.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include "requestresponder.h"

namespace HamLog {

class SharedLibrary {
	public:
		~SharedLibrary() { dlclose(m_handle); }

		static SharedLibrary* openSharedLibrary(const std::string &name) {
			SharedLibrary *lib = new SharedLibrary(name);
			if (!lib->isLoaded()) {
				delete lib;
				lib = NULL;
			}
			return lib;
		}

		bool isLoaded() {
			return m_loaded;
		}

		void *findSymbol(const std::string &name) {
			return dlsym(m_handle, name.c_str());
		}

	private:
		SharedLibrary(const std::string &name) : m_handle(0), m_loaded(false) {
			m_handle = dlopen(name.c_str(), RTLD_NOW|RTLD_GLOBAL);
			m_loaded = m_handle != NULL;
		}

		void *m_handle;
		bool m_loaded;
};

ModuleManager *ModuleManager::m_instance = NULL;

ModuleManager::ModuleManager() {}

ModuleManager* ModuleManager::getInstance() {
    if (!m_instance) {
		m_instance = new ModuleManager();
	}

	return m_instance;
}

bool ModuleManager::handleRequest(Session *session, Request::ref request, Reply::ref reply) {
	std::string uri = request->getURI();
	if (uri.find("/", 1) != std::string::npos) {
		uri = uri.substr(0, uri.find("/", 1));
	}
	std::cout << "looking for handler for URI=" << uri << "\n";
	if (m_modules.find(request->getURI()) != m_modules.end()) {
		ModuleInfo *info = m_modules[request->getURI()];
		RequestResponder *responder = dynamic_cast<RequestResponder *>(info->module);
		return responder->handleRequest(session, request, reply);
	}
	return false;
}

void ModuleManager::loadModules(const std::string &path) {
	boost::filesystem::directory_iterator end_itr;
	for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr ) {
		if (boost::filesystem::extension(itr->path()) == ".so") {
			std::cout << "Trying to load module " << itr->path().string() << ": ";

			ModuleInfo* info = new ModuleInfo;
			info->library = SharedLibrary::openSharedLibrary(itr->path().string());
			if (info->library == NULL) {
				char *error = dlerror();
				if (error)
					std::cout << "Error while loading module:" << error << "\n";
				else 
					std::cout << "Error while loading module\n";
				delete info;
				continue;
			}

			module_init init = (module_init) info->library->findSymbol("module_init");
			if (init == NULL) {
				std::cout << "Couldn't find module_init symbol\n";
				delete info->library;
				delete info;
				continue;
			}

			info->module = (*init)();
			if (info->module == NULL) {
				std::cout << "Module was not created by module_init function\n";
				delete info->library;
				delete info;
				continue;
			}

			std::cout << "Module loaded\n";
			if (info->module->getType() == 0) {
				RequestResponder *responder = dynamic_cast<RequestResponder *>(info->module);
				m_modules[responder->getURI()] = info;
			}
			else {
				delete info->module;
				delete info->library;
				delete info;
			}
		}
	}
}

}
