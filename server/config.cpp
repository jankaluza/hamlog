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

#include "config.h"
#include <fstream>

using namespace boost::program_options;

namespace HamLog {

bool Config::load(const std::string &configfile, boost::program_options::options_description &opts) {
	std::ifstream ifs(configfile.c_str());
	if (!ifs.is_open())
		return false;

	opts.add_options()
		("server.hostname", value<std::string>()->default_value("localhost"), "Hostname to bind server to")
		("server.port", value<int>()->default_value(7531), "Port the server is listening on")
		("database.type", value<std::string>()->default_value("none"), "Database type.")
		("database.database", value<std::string>()->default_value(""), "Database used to store data")
		("database.prefix", value<std::string>()->default_value(""), "Prefix of tables in database")
	;

	store(parse_config_file(ifs, opts), m_variables);
	notify(m_variables);

	m_file = configfile;

	onConfigReloaded();

	return true;
}

bool Config::load(const std::string &configfile) {
	options_description opts("Transport options");
	return load(configfile, opts);
}

bool Config::reload() {
	if (m_file.empty()) {
		return false;
	}

	return load(m_file);
}

}
