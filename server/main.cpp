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

#include <iostream>
#include "config.h"
#include "server.h"
#include "modulemanager.h"
#include "storagebackends/sqlite3backend.h"
#include "boost/lexical_cast.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#ifdef WITH_LOG4CXX
#include "log4cxx/logger.h"
#include "log4cxx/consoleappender.h"
#include "log4cxx/patternlayout.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/properties.h"
#include "log4cxx/helpers/transcoder.h"
#include "log4cxx/helpers/fileinputstream.h"
using namespace log4cxx;
#endif

using namespace HamLog;

int main(int argc, char **argv) {
	Config config;

	boost::program_options::options_description desc("Usage: hamlog_server [OPTIONS] <config_file.cfg>\nAllowed options");
	desc.add_options()
		("help,h", "help")
		("no-daemonize,n", "Do not run server as daemon")
		;
	try
	{
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
		if(vm.count("help"))
		{
			std::cout << desc << "\n";
			return 1;
		}
	}
	catch (std::runtime_error& e)
	{
		std::cout << desc << "\n";
		return 1;
	}
	catch (...)
	{
		std::cout << desc << "\n";
		return 1;
	}

	if (argc != 2) {
		std::cout << desc << "\n";
		return 1;
	}


	if (!config.load(argv[1])) {
		std::cerr << "Can't load configuration file.\n";
		return 2;
	}

#ifdef WITH_LOG4CXX
	if (CONFIG_STRING(&config, "logging.config").empty()) {
		LoggerPtr root = log4cxx::Logger::getRootLogger();
		root->addAppender(new ConsoleAppender(new PatternLayout("%d %-5p %c: %m%n")));
	}
	else {
		log4cxx::helpers::Properties p;
		log4cxx::helpers::FileInputStream *istream = new log4cxx::helpers::FileInputStream(CONFIG_STRING(&config, "logging.config"));

		p.load(istream);
		LogString pid, jid;
		log4cxx::helpers::Transcoder::decode(boost::lexical_cast<std::string>(getpid()), pid);
		log4cxx::helpers::Transcoder::decode(CONFIG_STRING(&config, "service.jid"), jid);
		p.setProperty("pid", pid);
		p.setProperty("jid", jid);

		std::string dir;
		BOOST_FOREACH(const log4cxx::LogString &prop, p.propertyNames()) {
			if (boost::ends_with(prop, ".File")) {
				log4cxx::helpers::Transcoder::encode(p.get(prop), dir);
				boost::replace_all(dir, "${jid}", jid);
				break;
			}
		}

		if (!dir.empty()) {
			// create directories
			try {
				boost::filesystem::create_directories(
					boost::filesystem::path(dir).parent_path().string()
				);
			}
			catch (...) {
				std::cerr << "Can't create logging directory directory " << boost::filesystem::path(dir).parent_path().string() << ".\n";
				return 1;
			}
		}
		log4cxx::PropertyConfigurator::configure(p);
	}
#endif /* WITH_LOG4CXX */

	StorageBackend *storage = NULL;

	if (CONFIG_STRING(&config, "database.type") == "sqlite3") {
		storage = new Storage::SQLite3(&config);
		storage->connect();
	}

	Server server(CONFIG_STRING(&config, "server.hostname"),
				  CONFIG_INT(&config, "server.port"));

	ModuleManager *moduleManager = ModuleManager::getInstance();
	moduleManager->loadModules(&server, CONFIG_STRING(&config, "modules.path"));

	server.start();

	delete moduleManager;
	delete storage;

	return 0;
}