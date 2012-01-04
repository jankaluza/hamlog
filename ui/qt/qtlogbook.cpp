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

QtLogBook* QtLogBook::m_instance = NULL;  


static void fetched(HAMConnection *connection, const char *data) {
	QtLogBook::getInstance()->handleFetched(connection, data);
}

static void updated(HAMConnection *connection, const char *data, const char *response) {
	QtLogBook::getInstance()->handleUpdated(connection, data, response);
}

static void update_failed(HAMConnection *connection, const char *data, const char *reason) {
	QtLogBook::getInstance()->handleUpdateFailed(connection, data, reason);
}

QtLogBook::QtLogBook() {
	m_uiCallbacks.fetched = fetched;
	m_uiCallbacks.updated = updated;
	m_uiCallbacks.update_failed = update_failed;
	m_uiCallbacks.removed = NULL;
	m_uiCallbacks.remove_failed = NULL;
	ham_logbook_set_ui_callbacks(&m_uiCallbacks);
}

QtLogBook *QtLogBook::getInstance() {
	if (!m_instance) {
		m_instance = new QtLogBook();
	}

	return m_instance;
}

void QtLogBook::handleFetched(HAMConnection *connection, const char *data) {
	QString data_(data);
	onLogBookFetched(connection, data_);
}

void QtLogBook::handleUpdated(HAMConnection *connection, const char *data, const char *response) {
	QString d(data);
	QString r(response);
	onLogBookUpdated(connection, d, r);
}

void QtLogBook::handleUpdateFailed(HAMConnection *connection, const char *data, const char *reason) {
	QString r(reason);
	QString d(data);
	onLogBookUpdateFailed(connection, d, r);
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

