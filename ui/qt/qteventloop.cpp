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

#include "qteventloop.h"

QtEventLoop* QtEventLoop::m_instance = NULL;  

static void *timeout_add(int interval, HAMTimeoutCallback callback, void *user_data) {
	return NULL;
}

static void timeout_remove(void *handle) {
}

static void *input_add(int fd, HAMInputCallback callback, void *user_data) {
	return QtEventLoop::getInstance()->handleInputAdd(fd, callback, user_data);
}

static void input_remove(void *handle) {
	QtEventLoop::getInstance()->handleInputRemove(handle);
}

QtEventLoop::QtEventLoop() {
	m_uiCallbacks.timeout_add = timeout_add;
	m_uiCallbacks.timeout_remove = timeout_remove;
	m_uiCallbacks.input_add = input_add;
	m_uiCallbacks.timeout_remove = input_remove;
	ham_eventloop_set_ui_callbacks(&m_uiCallbacks);
}

QtEventLoop *QtEventLoop::getInstance() {
	if (!m_instance) {
		m_instance = new QtEventLoop();
	}

	return m_instance;
}

void QtEventLoop::handleInputActivated(int fd) {
	QSocketNotifier *notifier = dynamic_cast<QSocketNotifier *>(sender());
	InputClosure &closure = m_notifiers[notifier];
	closure.callback(closure.user_data, fd);
}

void *QtEventLoop::handleInputAdd(int fd, HAMInputCallback callback, void *user_data) {
	QSocketNotifier *notifier = new QSocketNotifier(fd, QSocketNotifier::Read);
	connect(notifier, SIGNAL(activated(int)), this, SLOT(handleInputActivated(int)));

	InputClosure closure;
	closure.callback = callback;
	closure.user_data = user_data;

	m_notifiers[notifier] = closure;
	return notifier;
}

void QtEventLoop::handleInputRemove(void *handle) {
	QSocketNotifier *notifier = (QSocketNotifier *) handle;
	m_notifiers.erase(notifier);

	notifier->setEnabled(false);
	notifier->deleteLater();
}
