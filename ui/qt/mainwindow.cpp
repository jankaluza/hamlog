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

#include "mainwindow.h"
#include "qteventloop.h"
#include "qtconnection.h"
#include "qtaccount.h"
#include "logbook.h"

MainWindow::MainWindow()
	: m_eventLoop(QtEventLoop::getInstance()),
	m_connection(QtConnection::getInstance()),
	m_account(QtAccount::getInstance()),
	m_conn(0),
	m_register(0) {
	ui.setupUi(this);

	connect(m_connection, SIGNAL(onConnected(HAMConnection *)), this, SLOT(handleConnected(HAMConnection *)));
	connect(m_connection, SIGNAL(onDisconnected(HAMConnection *, const QString &)), this, SLOT(handleDisconnected(HAMConnection *, const QString &)));

	connect(m_account, SIGNAL(onLoggedIn(HAMConnection *)), this, SLOT(handleLoggedIn(HAMConnection *)));
	connect(m_account, SIGNAL(onLoginFailed(HAMConnection *, const QString &)), this, SLOT(handleLoginFailed(HAMConnection *, const QString &)));

	connect(ui.connectServer, SIGNAL(clicked()), this, SLOT(connectServer()));
	connect(ui.registerAccount, SIGNAL(clicked()), this, SLOT(registerAccount()));
}

void MainWindow::connectServer() {
	m_register = false;
	if (m_conn) {
		ham_connection_destroy(m_conn);
	}

	m_conn = ham_connection_new(ui.server->text().toStdString().c_str(),
								ui.port->value(),
								ui.username->text().toStdString().c_str(),
								ui.password->text().toStdString().c_str());
	ham_connection_connect(m_conn);
	ui.statusbar->showMessage("Connection in progress");
}

void MainWindow::registerAccount() {
	connectServer();
	m_register = true;
}

void MainWindow::handleConnected(HAMConnection *connection) {
	if (m_register) {
		ham_account_register(connection);
		ui.statusbar->showMessage("Registering account");
	}
	else {
		ui.statusbar->showMessage("Logging in");
		ham_account_login(connection);
	}
}

void MainWindow::handleDisconnected(HAMConnection *connection, const QString &reason) {
	ui.statusbar->showMessage(QString("Connection error: ") + reason);
}

void MainWindow::handleLoggedIn(HAMConnection *connection) {
	ui.statusbar->showMessage("Logged in!");
	ui.statusbar->showMessage("Fetching logbook");
	ham_logbook_fetch(connection);
}

void MainWindow::handleLoginFailed(HAMConnection *connection, const QString &reason) {
	ui.statusbar->showMessage(QString("Login failed: ") + reason);
}

