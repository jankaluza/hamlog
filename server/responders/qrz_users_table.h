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

#define CREATE_QRZ_USERS_TABLE() {\
	std::list<StorageBackend::Column> columns;\
	\
	columns.push_back(StorageBackend::Column("id", StorageBackend::Column::Integer, -1, true, true));\
	columns.push_back(StorageBackend::Column("user_id", StorageBackend::Column::Integer, -1, 255, true));\
	columns.back().m_unique = true;\
	columns.push_back(StorageBackend::Column("name", StorageBackend::Column::String, 255, true));\
	columns.push_back(StorageBackend::Column("password", StorageBackend::Column::String, 255, true));\
	\
	StorageBackend::getInstance()->createTable("qrz_users", columns);\
	}

