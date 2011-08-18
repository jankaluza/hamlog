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

#include "qtdxcc.h"
#include "iostream"

QtDXCC* QtDXCC::m_instance = NULL;  


static void fetched(HAMConnection *connection, const char *call, const char *data) {
	std::cout << "fetched\n";
	QtDXCC::getInstance()->handleFetched(connection, call, data);
}

QtDXCC::QtDXCC() {
	m_uiCallbacks.fetched = fetched;
	ham_dxcc_set_ui_callbacks(&m_uiCallbacks);
}

QtDXCC *QtDXCC::getInstance() {
	if (!m_instance) {
		m_instance = new QtDXCC();
	}

	return m_instance;
}

void QtDXCC::handleFetched(HAMConnection *connection, const char *call, const char *data) {
	QString data_(data);
	QString call_(call);
	onDXCCFetched(connection, call_, data_);
}
