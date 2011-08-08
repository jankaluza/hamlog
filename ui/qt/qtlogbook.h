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
#include "logbook.h"

class QtLogBook : public QObject {
	Q_OBJECT

	public:
		static QtLogBook *getInstance();

		void handleFetched(HAMConnection *connection, const char *logbook);
		void handleUpdated(HAMConnection *connection);
		void handleUpdateFailed(HAMConnection *connection, const char *reason);

		static std::vector<QStringList> tokenize(const QString &str);

	signals:
		void onLogBookFetched(HAMConnection *connection, const QString &logbook);
		void onLogBookUpdated(HAMConnection *connection);
		void onLogBookUpdateFailed(HAMConnection *connection, const QString &reason);

	private:
		QtLogBook();

		HAMLogBookUICallbacks m_uiCallbacks;
		static QtLogBook* m_instance;
};
