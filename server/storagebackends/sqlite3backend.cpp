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

#include "sqlite3backend.h"
#include "session.h"
#include "config.h"
#include "boost/lexical_cast.hpp"

#define SQLITE_DB_VERSION 1
#define CHECK_DB_RESPONSE(stmt) \
	if(stmt) { \
		sqlite3_exec(m_db, "ROLLBACK;", NULL, NULL, NULL); \
		return 0; \
	}

// Prepare the SQL statement
#define PREP_STMT(sql, str) \
	if(sqlite3_prepare_v2(m_db, std::string(str).c_str(), -1, &sql, NULL)) { \
		LOG4CXX_ERROR(logger, str<< (sqlite3_errmsg(m_db) == NULL ? "" : sqlite3_errmsg(m_db))); \
		return false; \
	}

// Finalize the prepared statement
#define FINALIZE_STMT(prep) \
	if(prep != NULL) { \
		sqlite3_finalize(prep); \
	}
	
#define BEGIN(STATEMENT) 	sqlite3_reset(STATEMENT);\
							int STATEMENT##_id = 1;\
							int STATEMENT##_id_get = 0;\
							(void)STATEMENT##_id_get;

#define BIND_INT(STATEMENT, VARIABLE) sqlite3_bind_int(STATEMENT, STATEMENT##_id++, VARIABLE)
#define BIND_STR(STATEMENT, VARIABLE) sqlite3_bind_text(STATEMENT, STATEMENT##_id++, VARIABLE.c_str(), -1, SQLITE_STATIC)
#define RESET_GET_COUNTER(STATEMENT)	STATEMENT##_id_get = 0;
#define GET_INT(STATEMENT)	sqlite3_column_int(STATEMENT, STATEMENT##_id_get++)
#define GET_STR(STATEMENT)	(const char *) sqlite3_column_text(STATEMENT, STATEMENT##_id_get++)
#define EXECUTE_STATEMENT(STATEMENT, NAME) 	if(sqlite3_step(STATEMENT) != SQLITE_DONE) {\
		LOG4CXX_ERROR(logger, NAME<< (sqlite3_errmsg(m_db) == NULL ? "" : sqlite3_errmsg(m_db)));\
			}

namespace HamLog {

namespace Storage {

SQLite3::SQLite3(Config *config) {
	m_config = config;
	m_db = NULL;
	m_prefix = CONFIG_STRING(m_config, "database.prefix");
}

SQLite3::~SQLite3(){
	if (m_db) {
		// Would be nice to use this:
		//
		//   sqlite3_stmt *pStmt;
		//   while((pStmt = sqlite3_next_stmt(db, 0)) != 0 ) {
		//    sqlite3_finalize(pStmt);
		//   }
		//
		// But requires SQLite3 >= 3.6.0 beta

		sqlite3_close(m_db);
	}
}

bool SQLite3::connect() {
	if (sqlite3_open(CONFIG_STRING(m_config, "database.database").c_str(), &m_db)) {
		sqlite3_close(m_db);
		return false;
	}

	if (createDatabase() == false)
		return false;

	return true;
}

bool SQLite3::createDatabase() {
	int not_exist = exec("CREATE TABLE IF NOT EXISTS " + m_prefix + "users ("
					"  id INTEGER PRIMARY KEY NOT NULL,"
					"  name varchar(255) NOT NULL,"
					"  password varchar(255) NOT NULL,"
					"  last_login datetime"
					");");

	if (not_exist) {
		exec("CREATE UNIQUE INDEX IF NOT EXISTS jid ON " + m_prefix + "users (name);");

		exec("CREATE TABLE IF NOT EXISTS " + m_prefix + "db_version ("
			"  ver INTEGER NOT NULL DEFAULT '" + boost::lexical_cast<std::string>(SQLITE_DB_VERSION) + "'"
			");");

		exec("REPLACE INTO " + m_prefix + "db_version (ver) values(" + boost::lexical_cast<std::string>(SQLITE_DB_VERSION) + ")");
	}
	return true;
}

void SQLite3::beginTransaction() {
	exec("BEGIN TRANSACTION;");
}

void SQLite3::commitTransaction() {
	exec("COMMIT TRANSACTION;");
}

bool SQLite3::exec(const std::string &query) {
	char *errMsg = 0;
	int rc = sqlite3_exec(m_db, query.c_str(), NULL, 0, &errMsg);
	if (rc != SQLITE_OK) {
		std::cerr << errMsg << " during statement " << query;
		sqlite3_free(errMsg);
		return false;
	}
	return true;
}

}

}
