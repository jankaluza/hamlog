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

#pragma once

#include <QtCore>
#include <QApplication>
#include <QTreeWidget>
#include "ui_qrzregisterdialog.h"
#include "connection.h"

class LogbookTreeWidget : public QTreeWidget {
	Q_OBJECT

	public:
		LogbookTreeWidget(QWidget *parent);
		~LogbookTreeWidget();

		void setConnection(HAMConnection *connection);

		int findColumnWithName(const std::string &name);
		
		void tryAskDXCC();

		std::string itemToCSV(QTreeWidgetItem *item);
		QTreeWidgetItem *itemFromCSV(const std::string &csv);
		QTreeWidgetItem *itemFromCSV(const QStringList &tokens, std::map<std::string, int> &indexes);

	public slots:
		void fetch();
		void fetch(const std::string &call);
		void addRecord();
		void addRecord(const std::string &call);
		void removeRecord();

	private slots:
		void handleItemChanged(QTreeWidgetItem *item, int col);
		void handleItemChanged(QTreeWidgetItem *item);
		void handleContextMenu(const QPoint &p);
		void handleItemDoubleClicked(QTreeWidgetItem *item, int col);

	private:
		HAMConnection *m_conn;
		bool m_askDXCC;
};
