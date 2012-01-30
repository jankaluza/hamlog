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
#include "qtlogbook.h"
#include "qtdxcc.h"
#include "iostream"
#include "connectdialog.h"
#include "modulesdialog.h"
#include "qrzregisterdialog.h"
#include "qtqrz.h"
#include "qtcallinfo.h"
#include "callinfo.h"

#include <QMessageBox>

MainWindow::MainWindow()
	: m_eventLoop(QtEventLoop::getInstance()),
	m_connection(QtConnection::getInstance()),
	m_account(QtAccount::getInstance()),
	m_logbook(QtLogBook::getInstance()),
	m_dxcc(QtDXCC::getInstance()),
	m_conn(0),
	m_register(0),
	m_refetch(0),
	m_askDXCC(0),
	m_connectDialog(0) {
	ui.setupUi(this);

	connect(m_connection, SIGNAL(onConnected(HAMConnection *)), this, SLOT(handleConnected(HAMConnection *)));
	connect(m_connection, SIGNAL(onDisconnected(HAMConnection *, const QString &)), this, SLOT(handleDisconnected(HAMConnection *, const QString &)));

	connect(m_account, SIGNAL(onLoggedIn(HAMConnection *)), this, SLOT(handleLoggedIn(HAMConnection *)));
	connect(m_account, SIGNAL(onLoginFailed(HAMConnection *, const QString &)), this, SLOT(handleLoginFailed(HAMConnection *, const QString &)));
	connect(m_account, SIGNAL(onRegistered(HAMConnection *)), this, SLOT(handleRegistered(HAMConnection *)));
	connect(m_account, SIGNAL(onRegistrationFailed(HAMConnection *, const QString &)), this, SLOT(handleRegistrationFailed(HAMConnection *, const QString &)));

	connect(ui.addRecord, SIGNAL(clicked()), ui.logbook, SLOT(addRecord()));

	connect(ui.dxcluster, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(handleDXClusterDoubleClicked(QTreeWidgetItem *, int)));
	connect(ui.dxcluster, SIGNAL(onItemAdded(const std::string &)), ui.earth, SLOT(showCall(const std::string &)));

	connect(ui.actionAvailable_modules, SIGNAL(triggered(bool)), this, SLOT(showAvailableModules(bool)));
	connect(ui.actionRegister_QRZ_account, SIGNAL(triggered(bool)), this, SLOT(showQRZRegisterDialog(bool)));

	connect(ui.zoom, SIGNAL(toggled(bool)), ui.earth, SLOT(setZoom(bool)));
	connect(ui.earthLat, SIGNAL(valueChanged(int)), ui.earth, SLOT(setLatitude(int)));
	connect(ui.earthLon, SIGNAL(valueChanged(int)), ui.earth, SLOT(setLongitude(int)));
	connect(ui.earthRadius, SIGNAL(valueChanged(int)), ui.earth, SLOT(setRadius(int)));
	connect(ui.earth, SIGNAL(onLocationChanged(double, double, double)), this, SLOT(handleEarthLocationChanged(double, double, double)));

	ui.stackedWidget->setCurrentIndex(1);
	showConnectDialog();
}

MainWindow::~MainWindow() {
	if (m_connectDialog) {
		m_connectDialog->deleteLater();
	}
}

int MainWindow::findColumnWithName(const std::string &name) {
	QTreeWidgetItem *item = ui.logbook->headerItem();
	for (int i = 0; i < item->columnCount(); i++) {
		if (item->text(i).toStdString() == name) {
			return i;
		}
	}

	return -1;
}

void MainWindow::handleEarthLocationChanged(double lat, double lon, double radius) {
	ui.zoom->setChecked(true);
	ui.earthLat->setValue((int)lat);
	ui.earthLon->setValue((int)lon);
	ui.earthRadius->setValue((int)radius);
}

void MainWindow::handleDXClusterDoubleClicked(QTreeWidgetItem *item, int col) {
	ui.logbook->addRecord(ui.dxcluster->getItemCall(item));
}

void MainWindow::showConnectDialog() {
	if (m_connectDialog) {
		m_connectDialog->reject();
		m_connectDialog->deleteLater();
	}
	m_connectDialog = new ConnectDialog(this);
	connect(m_connectDialog, SIGNAL(onConnectServer(const QString &, int, const QString &, const QString&)),
			this, SLOT(connectServer(const QString &, int, const QString &, const QString&)));
	connect(m_connectDialog, SIGNAL(onRegisterAccount(const QString &, int, const QString &, const QString&)),
			this, SLOT(registerAccount(const QString &, int, const QString &, const QString&)));
	m_connectDialog->setModal(true);
	m_connectDialog->show();
}

void MainWindow::showAvailableModules(bool checked) {
	ModulesDialog dialog(m_conn, this);
	dialog.exec();
}

void MainWindow::showQRZRegisterDialog(bool checked) {
	QRZRegisterDialog dialog(m_conn, this);
	dialog.exec();
}

void MainWindow::connectServer(const QString &server, int port, const QString &username, const QString &password) {
	m_register = false;
	if (m_conn) {
		ham_connection_destroy(m_conn);
	}

	m_conn = ham_connection_new(server.toStdString().c_str(),
								port,
								username.toStdString().c_str(),
								password.toStdString().c_str());
	ham_connection_connect(m_conn);
	ui.statusbar->showMessage("Connection in progress");
}

void MainWindow::registerAccount(const QString &server, int port, const QString &username, const QString &password) {
	connectServer(server, port, username, password);
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
	ui.infoLabel->setText(QString("Username: ") + connection->username + "     ");

	ui.logbook->setConnection(connection);
	ui.logbook->fetch();

	ui.earth->setConnection(connection);

	ui.dxcluster->setConnection(connection);
	ui.dxcluster->fetch();
}

void MainWindow::handleLoginFailed(HAMConnection *connection, const QString &reason) {
	ui.statusbar->showMessage(QString("Login failed: ") + reason);
	QMessageBox::critical(this, "Login failed!", reason);
}

void MainWindow::handleRegistered(HAMConnection *connection) {
	ui.statusbar->showMessage("Registered!");
	ui.statusbar->showMessage("Logging in");
	ham_account_login(connection);
}

void MainWindow::handleRegistrationFailed(HAMConnection *connection, const QString &reason) {
	ui.statusbar->showMessage(QString("Registration failed: ") + reason);
	QMessageBox::critical(this, "Registration failed!", reason);
}


