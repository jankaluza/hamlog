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

namespace HamLog {

/// Abstract class defining storage backends.
class StorageBackend {
	public:
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

		virtual bool addUser(const std::string &username, const std::string &password) = 0;

		virtual void beginTransaction() = 0;
		virtual void commitTransaction() = 0;

	private:
		static StorageBackend *m_instance;
};

}
