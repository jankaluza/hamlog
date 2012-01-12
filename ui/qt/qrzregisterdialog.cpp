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

#include "qrzregisterdialog.h"
#include "qteventloop.h"
#include "qtconnection.h"
#include "qtaccount.h"
#include "qtlogbook.h"
#include "dxcc.h"
#include "qrz.h"
#include "qtqrz.h"
#include "iostream"

QRZRegisterDialog::QRZRegisterDialog(HAMConnection *connection, QWidget *parent) : QDialog(parent), m_conn(connection) {
	ui.setupUi(this);

	connect(ui.registerAccount, SIGNAL(clicked()), this, SLOT(registerAccount()));

	connect(QtQRZ::getInstance(), SIGNAL(onRegistered(HAMConnection *)), this, SLOT(handleRegistered(HAMConnection *)));
	connect(QtQRZ::getInstance(), SIGNAL(onRegistrationFailed(HAMConnection *, const QString &)), this, SLOT(handleRegistrationFailed(HAMConnection *, const QString &)));
}

QRZRegisterDialog::~QRZRegisterDialog() {
	disconnect(QtQRZ::getInstance(), SIGNAL(onRegistered(HAMConnection *)), this, SLOT(handleRegistered(HAMConnection *)));
	disconnect(QtQRZ::getInstance(), SIGNAL(onRegistrationFailed(HAMConnection *, const QString &)), this, SLOT(handleRegistrationFailed(HAMConnection *, const QString &)));
}

void QRZRegisterDialog::handleRegistered(HAMConnection *connection) {
	accept();
}

void QRZRegisterDialog::handleRegistrationFailed(HAMConnection *connection, const QString &reason) {
	
}

void QRZRegisterDialog::registerAccount() {
	ham_qrz_register(m_conn, ui.username->text().toStdString().c_str(), ui.password->text().toStdString().c_str());
}
