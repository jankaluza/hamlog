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

#include "logbooktreewidget.h"
#include "qtlogbook.h"
#include "dxcc.h"
#include "callinfo.h"
#include "qrz.h"
#include "iostream"
#include <QtGui>

#include "newrecorddialog.h"

static void fetch_handler(HAMConnection *connection, const char *logbook, int error, void *ui_data) {
	LogbookTreeWidget *widget = static_cast<LogbookTreeWidget *>(ui_data);
	if (!widget || error) {
		return;
	}

	std::vector<QStringList > tokens = QtLogBook::tokenize(logbook);

	widget->clear();
	widget->setHeaderLabels(tokens.front());
	tokens.erase(tokens.begin());

	QObject::disconnect(widget, SIGNAL(itemChanged( QTreeWidgetItem *, int)), widget, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	Q_FOREACH(const QStringList &row, tokens) {
		if (row.size() > 1) {
			QTreeWidgetItem *item = new QTreeWidgetItem(widget, row);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
		}
	}

	for (int i = 0; i < widget->columnCount(); i++) {
		widget->resizeColumnToContents(i);
	}

	widget->setColumnHidden(0, true);
	widget->setColumnHidden(1, true);

	QObject::connect(widget, SIGNAL(itemChanged( QTreeWidgetItem *, int)), widget, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
}

static void callinfo_handler(HAMConnection *connection, const char *data, int error, void *ui_data) {
	LogbookTreeWidget *widget = static_cast<LogbookTreeWidget *>(ui_data);
	if (!widget || error) {
		return;
	}

	std::vector<QStringList > tokens = QtLogBook::tokenize(data);

	if (tokens[0].size() < 4)
		return;

	std::map<std::string, int> indexes;
	int i = 0;
	Q_FOREACH(const QString &header, tokens[0]) {
		indexes[header.toStdString()] = i++;
	}

	QString name;

	if (indexes.find("fname") != indexes.end()) {
		name += tokens[1][indexes["fname"]];
	}

	if (indexes.find("name") != indexes.end()) {
		if (!name.isEmpty()) {
			name += " ";
		}
		name += tokens[1][indexes["name"]];
	}

	QString text = "Do you want to use following Call Info data for this record?<br/>";
	text += "<i>";
	text += "QTH: " + tokens[1][indexes["qth"]] + "<br/>";
	text += "Continent: " + tokens[1][indexes["continent"]] + "<br/>";
	text += "Lat: " + tokens[1][indexes["latitude"]] + "<br/>";
	text += "Lon: " + tokens[1][indexes["longitude"]] + "<br/>";
	text += "CQ: " + tokens[1][indexes["cq"]] + "<br/>";
	text += "ITU: " + tokens[1][indexes["itu"]] + "<br/>";

	if (!name.isEmpty()) {
		text += "Name: " + name + "<br/>";
	}

	text += "</i>";

	QMessageBox::StandardButton b = QMessageBox::question(widget, "Use Call Info data?", text,
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (b == QMessageBox::Yes) {
		QTreeWidgetItem *item = widget->currentItem();

		QObject::disconnect(widget, SIGNAL(itemChanged( QTreeWidgetItem *, int)), widget, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

		item->setText(widget->findColumnWithName("qth"), tokens[1][indexes["qth"]]);
		item->setText(widget->findColumnWithName("continent"), tokens[1][indexes["continent"]]);
		item->setText(widget->findColumnWithName("cq"), tokens[1][indexes["cq"]]);
		item->setText(widget->findColumnWithName("itu"), tokens[1][indexes["itu"]]);
		item->setText(widget->findColumnWithName("latitude"), tokens[1][indexes["latitude"]]);
		item->setText(widget->findColumnWithName("longitude"), tokens[1][indexes["longitude"]]);
		QObject::connect(widget, SIGNAL(itemChanged( QTreeWidgetItem *, int)), widget, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

		item->setText(widget->findColumnWithName("name"), name);

		
	}
}

static void add_handler(HAMConnection *connection, const char *response, int error, void *ui_data) {
	LogbookTreeWidget *widget = static_cast<LogbookTreeWidget *>(ui_data);
	if (!widget || error) {
		return;
	}

	std::vector<QStringList > tokens = QtLogBook::tokenize(response);

	QTreeWidgetItem *item = widget->currentItem();
	item->setText(0, tokens[0][0]);
	item->setText(1, tokens[0][1]);

	widget->tryAskDXCC();

	for (int i = 0; i < widget->columnCount(); i++) {
		widget->resizeColumnToContents(i);
	}
}

LogbookTreeWidget::LogbookTreeWidget(QWidget *parent) : QTreeWidget(parent), m_conn(NULL), m_askDXCC(false) {
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)));
	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(handleItemDoubleClicked(QTreeWidgetItem *, int)));
}

LogbookTreeWidget::~LogbookTreeWidget() {
}

void LogbookTreeWidget::setConnection(HAMConnection *connection) {
	m_conn = connection;
}

void LogbookTreeWidget::fetch() {
	if (!m_conn)
		return;

	ham_logbook_fetch(m_conn, fetch_handler, this);
}

void LogbookTreeWidget::fetch(const std::string &call) {
	if (!m_conn)
		return;

	ham_logbook_fetch_with_call(m_conn, call.c_str(), fetch_handler, this);
}

void LogbookTreeWidget::tryAskDXCC() {
	if (m_askDXCC) {
		m_askDXCC = false;
		QTreeWidgetItem *item = currentItem();
		ham_callinfo_fetch(m_conn, item->text(findColumnWithName("callsign")).toStdString().c_str(), callinfo_handler, this);
	}
}

int LogbookTreeWidget::findColumnWithName(const std::string &name) {
	QTreeWidgetItem *item = headerItem();
	for (int i = 0; i < item->columnCount(); i++) {
		if (item->text(i).toStdString() == name) {
			return i;
		}
	}

	return -1;
}

void LogbookTreeWidget::handleItemChanged(QTreeWidgetItem *item, int col) {
	if (col < 2) {
		return;
	}

	std::string data = "id;" + headerItem()->text(col).toStdString() + "\n";
	if (item->text(0).isEmpty()) {
		data += "-1;" + item->text(col).toStdString();
	}
	else {
		data += item->text(0).toStdString() + ";" + item->text(col).toStdString();
	}
	data += "\n";

	ham_logbook_add(m_conn, data.c_str(), add_handler, this);

	// CALL changed, so ask dxcc
	if (col == findColumnWithName("callsign")) {
		m_askDXCC = true;
	}
}

QTreeWidgetItem *LogbookTreeWidget::itemFromCSV(const std::string &data) {
	if (data.empty())
		return NULL;

	std::vector<QStringList > tokens = QtLogBook::tokenize(data.c_str());

	std::map<std::string, int> indexes;
	int i = 0;
	Q_FOREACH(const QString &header, tokens[0]) {
		indexes[header.toStdString()] = i++;
	}

	disconnect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	QTreeWidgetItem *item;

	QList<QTreeWidgetItem *> items = findItems(tokens[1][indexes["id"]], Qt::MatchExactly);
	if (items.empty()) {
		item = new QTreeWidgetItem(this);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
	else {
		item = items.front();
	}

	setCurrentItem(item);

	item->setText(findColumnWithName("qth"), tokens[1][indexes["qth"]]);
	item->setText(findColumnWithName("continent"), tokens[1][indexes["continent"]]);
	item->setText(findColumnWithName("cq"), tokens[1][indexes["cq"]]);
	item->setText(findColumnWithName("itu"), tokens[1][indexes["itu"]]);
	item->setText(findColumnWithName("latitude"), tokens[1][indexes["latitude"]]);
	item->setText(findColumnWithName("longitude"), tokens[1][indexes["longitude"]]);
	item->setText(findColumnWithName("name"), tokens[1][indexes["name"]]);

	connect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	return item;
}

std::string LogbookTreeWidget::itemToCSV(QTreeWidgetItem *item) {
	std::string data;
	for (int i = 0; i < headerItem()->columnCount(); i++) {
		data += headerItem()->text(i).toStdString() + ";";
	}

	data.erase(data.end() - 1);
	data += "\n";

	for (int i = 0; i < headerItem()->columnCount(); i++) {
		if (i == 0 && item->text(0).isEmpty()) {
			data += "-1;";
			continue;
		}

		data += item->text(i).toStdString() + ";";
	}

	data.erase(data.end() - 1);
	data += "\n";

	return data;
}

void LogbookTreeWidget::handleItemChanged(QTreeWidgetItem *item) {
	std::string data = itemToCSV(item);
	ham_logbook_add(m_conn, data.c_str(), add_handler, this);
}

void LogbookTreeWidget::addRecord() {
	NewRecordDialog dialog(m_conn, this);
	if (dialog.exec()) {
		std::string csv = dialog.getCSV();
		itemFromCSV(csv);
		ham_logbook_add(m_conn, csv.c_str(), add_handler, this);
	}

// 	disconnect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
// 	QTreeWidgetItem *item = new QTreeWidgetItem(this);
// 	item->setText(findColumnWithName("callsign"), "UNNAMED");
// 	item->setFlags(item->flags() | Qt::ItemIsEditable);
// 	connect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
// 
// 	this->setCurrentItem(item);
// 	this->editItem(item, findColumnWithName("callsign"));
}

void LogbookTreeWidget::addRecord(const std::string &call) {
	NewRecordDialog dialog(m_conn, this);
	dialog.setCSV(std::string("callsign\n") + call);
	dialog.callLookUp();
	if (dialog.exec()) {
		std::string csv = dialog.getCSV();
		itemFromCSV(csv);
		ham_logbook_add(m_conn, csv.c_str(), add_handler, this);
	}
}

void LogbookTreeWidget::removeRecord() {
	disconnect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));

	QTreeWidgetItem *item = this->currentItem();
	std::string data = item->text(0).toStdString() + "\n";

	ham_logbook_remove(m_conn, data.c_str(), NULL, NULL);

	item = this->takeTopLevelItem(this->indexOfTopLevelItem(item));
	delete item;

	connect(this, SIGNAL(itemChanged( QTreeWidgetItem *, int)), this, SLOT(handleItemChanged( QTreeWidgetItem *, int)));
}

void LogbookTreeWidget::handleContextMenu(const QPoint &p) {
	QTreeWidgetItem *item = itemAt(p);
	if (!item)
		return;

    QMenu *menu = new QMenu;
    menu->addAction(QString("Remove"), this, SLOT(removeRecord()));
    menu->exec(mapToGlobal(p));
}

void LogbookTreeWidget::handleItemDoubleClicked(QTreeWidgetItem *item, int col) {
	NewRecordDialog dialog(m_conn, this);
	dialog.setCSV(itemToCSV(item));
	if (dialog.exec()) {
		itemFromCSV(dialog.getCSV());
		ham_logbook_add(m_conn, dialog.getCSV().c_str(), add_handler, this);
	}
}
