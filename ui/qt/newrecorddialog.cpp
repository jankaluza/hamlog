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
#include "qtlogbook.h"
#include "callinfo.h"
#include "iostream"

static void callinfo_handler(HAMConnection *connection, const char *data, int error, void *ui_data) {
	NewRecordDialog *widget = static_cast<NewRecordDialog *>(ui_data);
	if (!widget || error) {
		return;
	}

	std::string d(data);
	widget->setCSV(d);
}

NewRecordDialog::NewRecordDialog(HAMConnection *connection, QWidget *parent) : QDialog(parent), m_conn(connection), m_id("-1") {
	ui.setupUi(this);
	ui.myRST->setText("599");
	ui.hisRST->setText("599");
	ui.date->setDate(QDate::currentDate());
	ui.startTime->setTime(QTime::currentTime());
	ui.endTime->setTime(QTime::currentTime());

	ui.logbook->setConnection(m_conn);

	ui.call->setFocus();

	connect(ui.callLookUp, SIGNAL(clicked(bool)), this, SLOT(callLookUp(bool)));
}

void NewRecordDialog::callLookUp(bool unused) {
	std::string call = ui.call->text().toStdString();
	ui.logbook->fetch(call);

	ham_callinfo_fetch(m_conn, call.c_str(), callinfo_handler, this);
}

std::string NewRecordDialog::getCSV() {
	std::string data = "id;qth;continent;cq;itu;latitude;longitude;name;qsodate\n";

	data += m_id + ";";
	data += ui.qth->text().toStdString() + ";";
	data += ui.continent->text().toStdString() + ";";
	data += ui.cq->text().toStdString() + ";";
	data += ui.itu->text().toStdString() + ";";
	data += ui.latitude->text().toStdString() + ";";
	data += ui.longitude->text().toStdString() + ";";
	data += ui.name->text().toStdString() + ";";

	QDateTime dateTime;
	dateTime.setDate(ui.date->date());
	dateTime.setTime(ui.startTime->time());

	data += QString::number(dateTime.toTime_t()).toStdString() + "\n";

	return data;
}

void NewRecordDialog::setCSV(const std::string &data) {
	if (data.empty())
		return;

	std::vector<QStringList > tokens = QtLogBook::tokenize(data.c_str());

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

	if (indexes.find("callsign") != indexes.end()) {
		ui.logbook->fetch(tokens[1][indexes["callsign"]].toStdString());
		ui.call->setText(tokens[1][indexes["callsign"]]);
	}

	if (indexes.find("id") != indexes.end()) {
		m_id = tokens[1][indexes["id"]].toStdString();
	}
	else {
		m_id = "-1";
	}

#define SET_TEXT(WHERE, KEY) if(indexes.find(KEY) != indexes.end()) { WHERE->setText(tokens[1][indexes[KEY]]); }

	SET_TEXT(ui.qth, "qth");
	SET_TEXT(ui.continent, "continent");
	SET_TEXT(ui.cq, "cq");
	SET_TEXT(ui.itu, "itu");
	SET_TEXT(ui.latitude, "latitude");
	SET_TEXT(ui.longitude, "longitude");
	SET_TEXT(ui.longitude, "longitude");
	ui.name->setText(name);

#undef SET_TEXT
}
