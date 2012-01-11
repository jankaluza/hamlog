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

#include "modulesdialog.h"
#include "qteventloop.h"
#include "qtconnection.h"
#include "qtaccount.h"
#include "qtlogbook.h"
#include "dxcc.h"
#include "iostream"

ModulesDialog::ModulesDialog(HAMConnection *connection, QWidget *parent) : QDialog(parent), m_connection(connection) {
	ui.setupUi(this);

	HAMList *modules = ham_connection_get_modules(m_connection);

	QStringList labels;
	labels << "Name" << "URI" << "Description";
	ui.modules->setHeaderLabels(labels);

	HAMListItem *item = ham_list_get_first_item(modules);
	while (item) {
		HAMModule *module = (HAMModule *) ham_list_item_get_data(item);

		QTreeWidgetItem *i = new QTreeWidgetItem(ui.modules);
		i->setText(0, module->name);
		i->setText(1, module->uri);
		i->setText(2, module->desc);

		item = ham_list_get_next_item(item);
	}

	ham_list_destroy(modules);

	for (int i = 0; i < ui.modules->columnCount(); i++) {
		ui.modules->resizeColumnToContents(i);
	}
}
