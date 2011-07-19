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

#include "qtconnection.h"

QtConnection* QtConnection::m_instance = NULL;  


static void connected(HAMConnection *connection) {
	QtConnection::getInstance()->handleConnected(connection);
}

static void disconnected(HAMConnection *connection, const char *reason) {
	QtConnection::getInstance()->handleDisconnected(connection, reason);
}

QtConnection::QtConnection() {
	m_uiCallbacks.connected = connected;
	m_uiCallbacks.disconnected = disconnected;
	ham_connection_set_ui_callbacks(&m_uiCallbacks);
}

QtConnection *QtConnection::getInstance() {
	if (!m_instance) {
		m_instance = new QtConnection();
	}

	return m_instance;
}

void QtConnection::handleConnected(HAMConnection *connection) {
	onConnected(connection);
}

void QtConnection::handleDisconnected(HAMConnection *connection, const char *reason) {
	QString reason_(reason);
	onDisconnected(connection, reason_);
}

