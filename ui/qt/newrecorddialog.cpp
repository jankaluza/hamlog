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

#include "newrecorddialog.h"
#include "qteventloop.h"
#include "qtconnection.h"
#include "qtaccount.h"
#include "qtlogbook.h"
#include "callinfo.h"
#include "iostream"

NewRecordDialog::NewRecordDialog(HAMConnection *connection, QWidget *parent) : QDialog(parent), m_conn(connection) {
	ui.setupUi(this);

	ui.logbook->setConnection(m_conn);

	connect(ui.callLookUp, SIGNAL(clicked(bool)), this, SLOT(callLookUp(bool)));
}

void NewRecordDialog::callLookUp(bool unused) {
	std::string call = ui.call->text().toStdString();
	ui.logbook->fetch(call);

	ham_callinfo_fetch(m_conn, call.c_str(), callinfo_handler, this);
}
