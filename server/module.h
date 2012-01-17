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

namespace HamLog {

class Module {
	public:
		typedef enum {
			UNKNOWN,
			CALLINFO,
		} Type;
		
		Module(const std::string &name, Type type, const std::string &description = "") : m_name(name), m_type(type), m_description(description) {}

		virtual const std::string &getName() {
			return m_name;
		}

		virtual const std::string &getDescription() {
			return m_description;
		}

		virtual Type getType() {
			return m_type;
		}

	private:
		std::string m_name;
		Type m_type;
		std::string m_description;
};

}
