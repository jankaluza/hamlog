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

#include "qtlogbook.h"

QtLogBook::QtLogBook() {
}

std::vector<QStringList> QtLogBook::tokenize(const QString &str) {
	std::vector<QStringList> tokens;
     
	unsigned int pos = 0;
	bool quotes = false;
	QString field = "";

	tokens.resize(tokens.size() + 1);
     
    while(pos < str.length() && str[pos] != 0) {
		QChar c = str[pos];
		if (!quotes && c == '"' ) {
			quotes = true;
		}
		else if (quotes && c== '"' ) {
			if (pos + 1 <str.length() && str[pos+1]== '"' ) {
				field.push_back(c);
				pos++;
			}
			else {
				quotes = false;
			}
		}
		else if (!quotes && c == ';') {
			tokens.back().push_back(field);
			field.clear();
		}
		else if (!quotes && ( c == '\n' || c == '\r' )) {
			tokens.back().push_back(field);
			field.clear();
			tokens.resize(tokens.size() + 1);
		}
		else {
			field.push_back(c);
		}
		pos++;
	}

	if (!field.isEmpty()) {
		tokens.back().push_back(field);
	}

	return tokens;
}

