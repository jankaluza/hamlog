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

#include "dxclusterwidget.h"
#include "qteventloop.h"
#include "qtconnection.h"
#include "qtaccount.h"
#include "qtlogbook.h"
#include "dxcc.h"
#include "callinfo.h"
#include "qrz.h"
#include "qtqrz.h"
#include "iostream"
#include <QtGui>

#include "dxcluster.h"

#include "newrecorddialog.h"

static void fetch_handler(HAMConnection *connection, const char *data, int error, void *ui_data) {
	LogbookTreeWidget *widget = static_cast<LogbookTreeWidget *>(ui_data);
	if (!widget || error) {
		return;
	}

	std::vector<QStringList > tokens = QtLogBook::tokenize(data);

	std::cout << "DATA:" << data << "\n";

}


DXClusterWidget::DXClusterWidget(QWidget *parent) : QTreeWidget(parent), m_conn(NULL), m_timer(new QTimer(this)) {
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)));
	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(handleItemDoubleClicked(QTreeWidgetItem *, int)));

	connect(m_timer, SIGNAL(timeout()), this, SLOT(fetch()));
}

DXClusterWidget::~DXClusterWidget() {
	m_timer->stop();
}

void DXClusterWidget::setConnection(HAMConnection *connection) {
	m_conn = connection;

	if (m_conn) {
		m_timer->start(3000);
	}
	else {
		m_timer->stop();
	}
}

void DXClusterWidget::fetch() {
	if (!m_conn)
		return;

	ham_dxcluster_fetch(m_conn, fetch_handler, this);
}

int DXClusterWidget::findColumnWithName(const std::string &name) {
	QTreeWidgetItem *item = headerItem();
	for (int i = 0; i < item->columnCount(); i++) {
		if (item->text(i).toStdString() == name) {
			return i;
		}
	}

	return -1;
}

void DXClusterWidget::handleContextMenu(const QPoint &p) {
// 	QTreeWidgetItem *item = itemAt(p);
// 	if (!item)
// 		return;
// 
//     QMenu *menu = new QMenu;
//     menu->addAction(QString("Remove"), this, SLOT(removeRecord()));
//     menu->exec(mapToGlobal(p));
}

void DXClusterWidget::handleItemDoubleClicked(QTreeWidgetItem *item, int col) {
	
}
