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
#include <list>

namespace HamLog {

/// Abstract class defining storage backends.
class StorageBackend {
	public:

		struct User {
			unsigned long id;
			std::string name;
			std::string password;
		};

		class Column {
			public:
				enum {
					Integer,
					String,
					Datetime,
					Real,
				};

				Column(const std::string &name, int type, int size, bool not_null = false, bool primary_key = false)
				: m_name(name), m_type(type), m_not_null(not_null), m_primary_key(primary_key), m_unique(false), m_size(size) {
				}

				std::string m_name;
				int m_type;
				bool m_not_null;
				bool m_primary_key;
				bool m_unique;
				int m_size;
		};

		class Select {
			public:
				Select(const std::string &table) : m_table(table), m_row(0) {}
				void into(std::list<std::list<std::string> > *row) { m_row = row; }
				void where(const std::string &name, const std::string &value) { m_where[name] = value; }
				void what(const std::string &name) { m_what.push_back(name); }
			std::string m_table;
			std::list<std::list<std::string> > *m_row;
			std::list<std::string> m_what;
			std::map<std::string, std::string> m_where;
		};

		class Insert {
			public:
				Insert(const std::string &table) : m_table(table), m_row(0) {}
				void where(const std::string &name, const std::string &value) { m_where[name] = value; }
				void what(std::map<std::string, std::string> *row) { m_row = row; }
			std::string m_table;
			std::map<std::string, std::string> *m_row;
			std::map<std::string, std::string> m_where;
		};

		StorageBackend() {
			m_instance = this;
		}

		/// Virtual desctructor.
		virtual ~StorageBackend() {}

		static StorageBackend *getInstance() {
			return m_instance;
		}

		/// connect
		virtual bool connect() = 0;

		/// createDatabase
		virtual bool createDatabase() = 0;
		virtual bool createTable(const std::string &name, const std::list<Column> &columns) = 0;

		virtual bool select(Select &query) = 0;
		virtual bool insert(Insert &query) = 0;
		virtual bool update(Insert &query) = 0;
		virtual bool remove(Select &query) = 0;
		virtual long lastInsertedID() = 0;

		virtual bool addUser(const std::string &username, const std::string &password) = 0;
		virtual StorageBackend::User getUser(const std::string &username) = 0;

		virtual void beginTransaction() = 0;
		virtual void commitTransaction() = 0;

	private:
		static StorageBackend *m_instance;
};

}
