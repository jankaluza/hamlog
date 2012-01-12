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

#include "qtqrz.h"
#include "iostream"

QtQRZ* QtQRZ::m_instance = NULL;  


static void fetched(HAMConnection *connection, const char *call, const char *data) {
	QtQRZ::getInstance()->handleFetched(connection, call, data);
}

static void registered(HAMConnection *connection) {
	QtQRZ::getInstance()->handleRegistered(connection);
}

static void registration_failed(HAMConnection *connection, const char *reason) {
	QtQRZ::getInstance()->handleRegistrationFailed(connection, reason);
}

QtQRZ::QtQRZ() {
	m_uiCallbacks.fetched = fetched;
	m_uiCallbacks.registered = registered;
	m_uiCallbacks.registration_failed = registration_failed;
	ham_qrz_set_ui_callbacks(&m_uiCallbacks);
}

QtQRZ *QtQRZ::getInstance() {
	if (!m_instance) {
		m_instance = new QtQRZ();
	}

	return m_instance;
}

void QtQRZ::handleFetched(HAMConnection *connection, const char *call, const char *data) {
	QString data_(data);
	QString call_(call);
	onQRZFetched(connection, call_, data_);
}

void QtQRZ::handleRegistered(HAMConnection *connection) {
	onRegistered(connection);
}

void QtQRZ::handleRegistrationFailed(HAMConnection *connection, const char *reason) {
	QString reason_(reason);
	onRegistrationFailed(connection, reason_);
}
