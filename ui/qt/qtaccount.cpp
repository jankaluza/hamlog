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

#include "qtaccount.h"

QtAccount* QtAccount::m_instance = NULL;  


static void logged_in(HAMConnection *connection) {
	QtAccount::getInstance()->handleLoggedIn(connection);
}

static void login_failed(HAMConnection *connection, const char *reason) {
	QtAccount::getInstance()->handleLoginFailed(connection, reason);
}

QtAccount::QtAccount() {
	m_uiCallbacks.logged_in = logged_in;
	m_uiCallbacks.login_failed = login_failed;
	ham_account_set_ui_callbacks(&m_uiCallbacks);
}

QtAccount *QtAccount::getInstance() {
	if (!m_instance) {
		m_instance = new QtAccount();
	}

	return m_instance;
}

void QtAccount::handleLoggedIn(HAMConnection *connection) {
	onLoggedIn(connection);
}

void QtAccount::handleLoginFailed(HAMConnection *connection, const char *reason) {
	QString reason_(reason);
	onLoginFailed(connection, reason_);
}

