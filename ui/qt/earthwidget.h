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
#include <QLabel>
#include <QString>
#include "connection.h"

class EarthWidget : public QLabel {
	Q_OBJECT

	public:
		EarthWidget(QWidget *parent);
		~EarthWidget();

		void setConnection(HAMConnection *connection);

	public slots:
		void setZoom(bool zoom) {
			m_zoom = zoom;
			reloadEarth();
		}

		void setLatitude(double latitude) {
			m_latitude = latitude;
			reloadEarth();
		}

		void setLatitude(int lat) { setLatitude((double)lat); }

		void setLongitude(double longitude) {
			m_longitude = longitude;
			reloadEarth();
		}

		void setLongitude(int lat) { setLongitude((double)lat); }

		void setRadius(double radius) {
			m_radius = radius;
			reloadEarth();
		}

		void setRadius(int radius) { setRadius((double)radius); }

	public slots:
		void showCall(const std::string &call);
		void showCall(const std::string &call, double lat, double lon);

		void showEurope();

	protected:
		void paintEvent(QPaintEvent *event);

	private slots:
		void handleEarthReloaded(int exitcode, QProcess::ExitStatus status);
		void handleContextMenu(const QPoint &p);

	private:
		void reloadEarth();

	private:
		HAMConnection *m_conn;
		QProcess *m_xplanet;
		QString m_markers;
		bool m_zoom;
		double m_latitude;
		double m_longitude;
		double m_radius;
};
