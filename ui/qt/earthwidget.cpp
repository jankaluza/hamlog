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

#include "earthwidget.h"
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

static void fetch_handler(HAMConnection *connection, const char *data, int error, void *ui_data) {
	EarthWidget *widget = static_cast<EarthWidget *>(ui_data);
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

	double lat = tokens[1][indexes["latitude"]].toDouble();
	double lon = tokens[1][indexes["longitude"]].toDouble();
	std::string call = tokens[1][indexes["call"]].toStdString();

	widget->showCall(call, lat, lon * -1);
}

EarthWidget::EarthWidget(QWidget *parent) : QLabel(parent), m_conn(NULL), m_xplanet(new QProcess()),
	m_zoom(false), m_latitude(0), m_longitude(0), m_radius(40) {
	connect(m_xplanet, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleEarthReloaded(int, QProcess::ExitStatus)));

	QFile file("./xplanet/markers/hamlog_marker");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		file.close();
	}

	reloadEarth();
}

EarthWidget::~EarthWidget() {
	delete m_xplanet;
}

void EarthWidget::setConnection(HAMConnection *conn) {
	m_conn = conn;
}

void EarthWidget::reloadEarth() {
	m_xplanet->close();
	m_xplanet->kill();

	QStringList args;
	args << "-searchdir" << "./xplanet" << "-config" << "hamlog";
	if (!m_zoom) {
		args << "-projection" << "rectangular";
	}
	args << "-fontsize" << "12";
	args << "-window-id" << QString::number(winId());
	args << "-wait" << "1";

	if (m_zoom) {
		args << "-latitude" << QString::number(m_latitude) << "-longitude" << QString::number(m_longitude);
		args << "-radius" << QString::number(m_radius);
	}
	m_xplanet->start("xplanet", args);
}

void EarthWidget::handleEarthReloaded(int exitcode, QProcess::ExitStatus status) {
	
}

void EarthWidget::paintEvent(QPaintEvent *event) {
	QLabel::paintEvent(event);

	reloadEarth();
}

void EarthWidget::showCall(const std::string &call, double lat, double lon) {
	m_markers += QString::number(lat) + QString("\t");
	m_markers += QString::number(lon) + QString("\t\"") + QString::fromStdString(call) + QString("\"");
	m_markers += "\tcolor=Green\n";

	QFile file("./xplanet/markers/hamlog_marker");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	out << m_markers;
	file.close();

	reloadEarth();
}

void EarthWidget::showCall(const std::string &call) {
	if (m_conn) {
		ham_dxcc_fetch(m_conn, call.c_str(), fetch_handler, this);
	}
}
