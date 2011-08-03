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
		std::cout << str<< (sqlite3_errmsg(m_db) == NULL ? "" : sqlite3_errmsg(m_db)); \
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
		std::cout << NAME << (sqlite3_errmsg(m_db) == NULL ? "" : sqlite3_errmsg(m_db)) << "\n";\
			return false; \
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
// 		FINALIZE_STMT(m_addUser);
// 		FINALIZE_STMT(m_getUser);

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

// 	PREP_STMT(m_addUser, "INSERT INTO " + m_prefix + "users (name, password, last_login) VALUES (?, ?, DATETIME('NOW'))");
// 	PREP_STMT(m_getUser, "SELECT id, name, password FROM " + m_prefix + "users WHERE name=?");

	return true;
}

bool SQLite3::createTable(const std::string &name, const std::list<Column> &columns) {
	std::string sql = "CREATE TABLE IF NOT EXISTS " + m_prefix + name + "(";

	BOOST_FOREACH(Column c, columns) {
		sql += "  ";
		sql += c.m_name + " ";
		switch(c.m_type) {
			case Column::Integer:
				sql += "INTEGER ";
				break;
			case Column::String:
				sql += "VARCHAR(" + boost::lexical_cast<std::string>(c.m_size) + ") ";
				break;
			case Column::Datetime:
				sql += "datetime ";
				break;
			default:
				continue;
		}
		if (c.m_primary_key)
			sql += "PRIMARY KEY ";
		if (c.m_not_null)
			sql += "NOT NULL ";
		if (c.m_unique)
			sql += "UNIQUE ";
		sql += ",";
	}

	// remove last ','
	sql.erase(sql.end() - 1);

	sql += ");";
	return exec(sql);
}

bool SQLite3::select(Select &query) {
	std::string sql = "SELECT ";

	if (query.m_what.empty()) {
		sql += "* ";
	}
	else {
		BOOST_FOREACH(const std::string &what, query.m_what) {
			sql += what +",";
		}
		sql.erase(sql.end() - 1);
	}

	sql += " FROM " + m_prefix + query.m_table;

	if (query.m_where.empty()) {
		sql += ";";
	}
	else {
		sql += " WHERE";
		for(std::map<std::string, std::string>::const_iterator it = query.m_where.begin(); it != query.m_where.end(); it++) {
			sql += " " + (*it).first + "=? AND";
		}
		sql += " 1;";
	}

	sqlite3_stmt *stmt;
	PREP_STMT(stmt, sql.c_str());
	BEGIN(stmt);

	if (!query.m_where.empty()) {
		for(std::map<std::string, std::string>::const_iterator it = query.m_where.begin(); it != query.m_where.end(); it++) {
			BIND_STR(stmt, (*it).second);
		}
	}

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		BOOST_FOREACH(const std::string &what, query.m_what) {
			query.m_row->push_back(GET_STR(stmt));
		}
		FINALIZE_STMT(stmt);
		return true;
	}

	FINALIZE_STMT(stmt);
	return false;
}

bool SQLite3::insert(Insert &query) {
	std::string sql = "INSERT INTO " + m_prefix + query.m_table + " (";

	for(std::map<std::string, std::string>::const_iterator it = query.m_row->begin(); it != query.m_row->end(); it++) {
		sql += (*it).first + ",";
	}
	sql.erase(sql.end() - 1);
	sql += ") VALUES (";

	for(std::map<std::string, std::string>::const_iterator it = query.m_row->begin(); it != query.m_row->end(); it++) {
		sql += "?,";
	}
	sql.erase(sql.end() - 1);
	sql += ");";

	sqlite3_stmt *stmt;
	PREP_STMT(stmt, sql.c_str());
	BEGIN(stmt);

	for(std::map<std::string, std::string>::const_iterator it = query.m_row->begin(); it != query.m_row->end(); it++) {
		BIND_STR(stmt, (*it).second);
	}

	EXECUTE_STATEMENT(stmt, sql);
	FINALIZE_STMT(stmt);
	return true;
}

bool SQLite3::createDatabase() {
	int not_exist =	exec("CREATE TABLE IF NOT EXISTS " + m_prefix + "db_version ("
			"  ver INTEGER NOT NULL DEFAULT '" + boost::lexical_cast<std::string>(SQLITE_DB_VERSION) + "'"
			");");

	if (not_exist) {
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

bool SQLite3::addUser(const std::string &username, const std::string &password) {
	BEGIN(m_addUser);
	BIND_STR(m_addUser, username);
	BIND_STR(m_addUser, password);

	EXECUTE_STATEMENT(m_addUser, "add user");
	return true;
}

StorageBackend::User SQLite3::getUser(const std::string &username) {
	StorageBackend::User user;
	user.id = -1;

	BEGIN(m_getUser);
	BIND_STR(m_getUser, username);

	if(sqlite3_step(m_getUser) != SQLITE_ROW) {
		return user;
	}

	user.id = GET_INT(m_getUser);
	user.name = GET_STR(m_getUser);
	user.password = GET_STR(m_getUser);

	return user;
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
