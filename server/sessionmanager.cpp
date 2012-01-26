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

#include "sessionmanager.h"
#include <boost/bind.hpp>
#include <iostream>

namespace HamLog {

SessionManager::SessionManager() {
	
}

void SessionManager::start(Session::ref session) {
	m_sessions.push_back(session);
	session->onStopped.connect(boost::bind(&SessionManager::handleStopped, this, session));
	session->start();
}

void SessionManager::stop(Session::ref session) {
	session->stop();
}

void SessionManager::handleStopped(Session::ref session) {
	session->onStopped.disconnect(boost::bind(&SessionManager::handleStopped, this, session));
	m_sessions.remove(session);
}

}
