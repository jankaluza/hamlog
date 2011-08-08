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

#include <string>
#include <map>
#include "storagebackend.h"
#include <sqlite3.h>

namespace HamLog {

class Session;
class Config;

namespace Storage {

class SQLite3 : public StorageBackend {
	public:
		SQLite3(Config *config);
		virtual ~SQLite3();

		bool connect();

		bool createDatabase();

		bool addUser(const std::string &username, const std::string &password);
		StorageBackend::User getUser(const std::string &username);

		bool createTable(const std::string &name, const std::list<Column> &columns);
		bool select(Select &query);
		bool insert(Insert &query);
		bool update(Insert &query);

		void beginTransaction();
		void commitTransaction();

	private:
		bool exec(const std::string &query);

		sqlite3 *m_db;
		Config *m_config;
		std::string m_prefix;

		// statements
		sqlite3_stmt *m_addUser;
		sqlite3_stmt *m_getUser;
// 		sqlite3_stmt *m_getUserSetting;
// 		sqlite3_stmt *m_setUserSetting;
// 		sqlite3_stmt *m_updateUserSetting;
// 		sqlite3_stmt *m_removeUser;
// 		sqlite3_stmt *m_removeUserBuddies;
// 		sqlite3_stmt *m_removeUserSettings;
// 		sqlite3_stmt *m_removeUserBuddiesSettings;
// 		sqlite3_stmt *m_addBuddy;
// 		sqlite3_stmt *m_updateBuddy;
// 		sqlite3_stmt *m_updateBuddySetting;
// 		sqlite3_stmt *m_getBuddies;
// 		sqlite3_stmt *m_getBuddiesSettings;
};

}

}
