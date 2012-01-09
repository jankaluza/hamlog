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

	connect(m_logbook, SIGNAL(onLogBookFetched(HAMConnection *, const QString &)), this, SLOT(handleLogBookFetched(HAMConnection *, const QString &)));
	connect(m_logbook, SIGNAL(onLogBookUpdated(HAMConnection *, const QString &, const QString &)), this, SLOT(handleLogBookUpdated(HAMConnection *, const QString &, const QString &)));
	connect(m_logbook, SIGNAL(onLogBookUpdateFailed(HAMConnection *, const QString &, const QString &)), this, SLOT(handleLogBookUpdateFailed(HAMConnection *, const QString &, const QString &)));

	connect(m_dxcc, SIGNAL(onDXCCFetched(HAMConnection *, const QString &, const QString &)), this, SLOT(handleDXCCFetched(HAMConnection *, const QString &, const QString &)));

	connect(ui.addRecord, SIGNAL(clicked()), this, SLOT(addRecord()));

	ui.logbook->setContextMenuPolicy(Qt::CustomContextMenu);  
	connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
	connect(ui.logbook, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)));


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

void MainWindow::addRecord() {
	disconnect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
	QTreeWidgetItem *item = new QTreeWidgetItem(ui.logbook);
	item->setText(findColumnWithName("callsign"), "UNNAMED");
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	ui.logbook->setCurrentItem(item);
	ui.logbook->editItem(item, findColumnWithName("callsign"));
}

void MainWindow::removeRecord() {
	disconnect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	QTreeWidgetItem *item = ui.logbook->currentItem();
	std::string data = item->text(0).toStdString() + "\n";
	ui.statusbar->showMessage("Removing record");

	ham_logbook_remove(m_conn, data.c_str());

	item = ui.logbook->takeTopLevelItem(ui.logbook->indexOfTopLevelItem(item));
	delete item;

	connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
}

void MainWindow::handleItemChanged(QTreeWidgetItem *item, int col) {
	if (col < 2) {
		return;
	}

	std::string data = "id;" + ui.logbook->headerItem()->text(col).toStdString() + "\n";
	if (item->text(0).isEmpty()) {
		data += "-1;" + item->text(col).toStdString();
	}
	else {
		data += item->text(0).toStdString() + ";" + item->text(col).toStdString();
	}
	data += "\n";

	ui.statusbar->showMessage("Updating record");
	ham_logbook_add(m_conn, data.c_str());

	// CALL changed, so ask dxcc
	if (col == findColumnWithName("callsign")) {
		m_askDXCC = true;
	}
}

void MainWindow::handleItemChanged(QTreeWidgetItem *item) {
	std::string data;
	for (int i = 0; i < ui.logbook->headerItem()->columnCount(); i++) {
		data += ui.logbook->headerItem()->text(i).toStdString() + ";";
	}

	data.erase(data.end() - 1);
	data += "\n";

	for (int i = 0; i < ui.logbook->headerItem()->columnCount(); i++) {
		if (i == 0 && item->text(0).isEmpty()) {
			data += "-1;";
			continue;
		}

		data += item->text(i).toStdString() + ";";
	}

	data.erase(data.end() - 1);
	data += "\n";

	ui.statusbar->showMessage("Updating record");
	ham_logbook_add(m_conn, data.c_str());
}

void MainWindow::handleContextMenu(const QPoint &p) {
	QTreeWidgetItem *item = ui.logbook->itemAt(p);
	if (!item)
		return;

    QMenu *menu = new QMenu;
    menu->addAction(QString("Remove"), this, SLOT(removeRecord()));
    menu->exec(ui.logbook->mapToGlobal(p));
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

void MainWindow::handleLogBookFetched(HAMConnection *connection, const QString &logbook) {
	std::vector<QStringList > tokens = QtLogBook::tokenize(logbook);

	ui.logbook->clear();
	ui.logbook->setHeaderLabels(tokens.front());
	tokens.erase(tokens.begin());

	disconnect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	Q_FOREACH(const QStringList &row, tokens) {
		if (row.size() > 1) {
			QTreeWidgetItem *item = new QTreeWidgetItem(ui.logbook, row);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
		}
	}

	for (int i = 0; i < ui.logbook->columnCount(); i++) {
		ui.logbook->resizeColumnToContents(i);
	}

	ui.logbook->setColumnHidden(0, true);
	ui.logbook->setColumnHidden(1, true);

	ui.stackedWidget->setCurrentIndex(1);
	ui.statusbar->showMessage("Logbook fetched!");

	connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
}

void MainWindow::handleLogBookUpdated(HAMConnection *connection, const QString &data, const QString &response) {
	ui.statusbar->showMessage("Record updated");
	std::vector<QStringList > tokens = QtLogBook::tokenize(response);

	QTreeWidgetItem *item = ui.logbook->currentItem();
	item->setText(0, tokens[0][0]);
	item->setText(1, tokens[0][1]);

	if (m_askDXCC) {
		m_askDXCC = false;
		ham_dxcc_fetch(m_conn, item->text(findColumnWithName("callsign")).toStdString().c_str());
	}

	for (int i = 0; i < ui.logbook->columnCount(); i++) {
		ui.logbook->resizeColumnToContents(i);
	}
}

void MainWindow::handleLogBookUpdateFailed(HAMConnection *connection, const QString &data, const QString &reason) {
	ui.statusbar->showMessage(QString("Record updating error: ") + reason);
}

void MainWindow::handleDXCCFetched(HAMConnection *connection, const QString &call, const QString &data) {
	std::vector<QStringList > tokens = QtLogBook::tokenize(data);

	if (tokens[0].size() < 4)
		return;

	QString text = "Do you want to use following DXCC data for this record?<br/>";
	text += "<i>";
	text += "Country: " + tokens[0][0] + "<br/>";
	text += "Continent: " + tokens[0][1] + "<br/>";
	text += "Lat: " + tokens[0][4] + "<br/>";
	text += "Lon: " + tokens[0][5] + "<br/>";
	text += "CQ: " + tokens[0][2] + "<br/>";
	text += "ITU: " + tokens[0][3] + "<br/>";
	text += "</i>";

	QMessageBox::StandardButton b = QMessageBox::question(this, "Use DXCC data?", text,
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (b == QMessageBox::Yes) {
		QTreeWidgetItem *item = ui.logbook->currentItem();

		disconnect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

		item->setText(findColumnWithName("qth"), tokens[0][0]);
		item->setText(findColumnWithName("continent"), tokens[0][1]);
		item->setText(findColumnWithName("latitude"), tokens[0][2]);
		item->setText(findColumnWithName("longitude"), tokens[0][3]);
		item->setText(findColumnWithName("cq"), tokens[0][4]);
		item->setText(findColumnWithName("itu"), tokens[0][5]);
		handleItemChanged(item);

		connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
	}
}

