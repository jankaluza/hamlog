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
#include "iostream"

MainWindow::MainWindow()
	: m_eventLoop(QtEventLoop::getInstance()),
	m_connection(QtConnection::getInstance()),
	m_account(QtAccount::getInstance()),
	m_logbook(QtLogBook::getInstance()),
	m_conn(0),
	m_register(0),
	m_refetch(0) {
	ui.setupUi(this);

	connect(m_connection, SIGNAL(onConnected(HAMConnection *)), this, SLOT(handleConnected(HAMConnection *)));
	connect(m_connection, SIGNAL(onDisconnected(HAMConnection *, const QString &)), this, SLOT(handleDisconnected(HAMConnection *, const QString &)));

	connect(m_account, SIGNAL(onLoggedIn(HAMConnection *)), this, SLOT(handleLoggedIn(HAMConnection *)));
	connect(m_account, SIGNAL(onLoginFailed(HAMConnection *, const QString &)), this, SLOT(handleLoginFailed(HAMConnection *, const QString &)));

	connect(m_logbook, SIGNAL(onLogBookFetched(HAMConnection *, const QString &)), this, SLOT(handleLogBookFetched(HAMConnection *, const QString &)));
	connect(m_logbook, SIGNAL(onLogBookUpdated(HAMConnection *, const QString &)), this, SLOT(handleLogBookUpdated(HAMConnection *, const QString &)));
	connect(m_logbook, SIGNAL(onLogBookUpdateFailed(HAMConnection *, const QString &, const QString &)), this, SLOT(handleLogBookUpdateFailed(HAMConnection *, const QString &, const QString &)));

	connect(ui.connectServer, SIGNAL(clicked()), this, SLOT(connectServer()));
	connect(ui.registerAccount, SIGNAL(clicked()), this, SLOT(registerAccount()));
	connect(ui.addRecord, SIGNAL(clicked()), this, SLOT(addRecord()));

	ui.logbook->setContextMenuPolicy(Qt::CustomContextMenu);  
	connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
	connect(ui.logbook, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)));


	ui.stackedWidget->setCurrentIndex(0);
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

void MainWindow::addRecord() {
	disconnect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
	QTreeWidgetItem *item = new QTreeWidgetItem(ui.logbook);
	item->setText(2, "NEW");
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	connect(ui.logbook, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
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
	std::string data = "id;" + ui.logbook->headerItem()->text(col).toStdString() + "\n";
	if (item->text(0).isEmpty()) {
		data += "-1;" + item->text(col).toStdString();
		m_refetch = true;
	}
	else {
		data += item->text(0).toStdString() + ";" + item->text(col).toStdString();
		m_refetch = false;
	}
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

void MainWindow::handleLogBookUpdated(HAMConnection *connection, const QString &data) {
	ui.statusbar->showMessage("Record updated");
	if (m_refetch) {
		ham_logbook_fetch(connection);
	}
	else {
		for (int i = 0; i < ui.logbook->columnCount(); i++) {
			ui.logbook->resizeColumnToContents(i);
		}
	}
}

void MainWindow::handleLogBookUpdateFailed(HAMConnection *connection, const QString &data, const QString &reason) {
	ui.statusbar->showMessage(QString("Record updating error: ") + reason);
}

