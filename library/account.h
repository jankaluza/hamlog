/**
 * @file account.h Account API
 * @ingroup core
 */

/*
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

#ifndef _HAMLOG_ACCOUNT_H
#define _HAMLOG_ACCOUNT_H

#include "connection.h"
#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

/**
 * Account related UI callbacks.
 */
typedef struct _HAMAccountUICallbacks {
	/** Called when user associated with this connection has been
	 *  succesfully registered.
	 */
	void (*registered) (HAMConnection *connection);

	/** Called when registration failed for some reason.
	 */
	void (*registration_failed) (HAMConnection *connection, const char *reason);

	/** Called when user associated with this connection logged in.
	 */
	void (*logged_in) (HAMConnection *connection);

	/** Called when login process failed for some reason.
	 */
	void (*login_failed) (HAMConnection *connection, const char *reason);
} HAMAccountUICallbacks;

/**
 * Sets UI callbacks which are then called for particular event.
 * @param callbacks UI Callbacks.
 */
void ham_account_set_ui_callbacks(HAMAccountUICallbacks *callbacks);

/**
 * Registers new account on remote server. Calls "registered" or
 * "registration_failed" UI callback when done.
 * @code
 * // ham_account_register has to be called when the connection is connected
 * static void handle_connection_connected(HAMConnection *connection) {
 * 	ham_account_register(connection);
 * }
 * 
 * // register handle_connection_connected callback
 * HAMConnectionUICallbacks callbacks;
 * callbacks.connected = handle_connection_connected;
 * ham_connection_set_ui_callbacks(callbacks);
 * 
 * // create connection and connect it
 * HAMConnection *connection = ham_connection_new("localhost", 8888, "user", "password");
 * ham_connection_connect(connection);
 * @endcode
 * @param connection Connection used for registration.
 */
void ham_account_register(HAMConnection *connection);

void ham_account_unregister(HAMConnection *connection);

/**
 * Logins user associated with the conection once the connection is connected.
 * Calls "logged_in" or "login_failed" UI callback when done.
 * @code
 * // ham_account_login has to be called when the connection is connected 
 * static void handle_connection_connected(HAMConnection *connection) {
 * 	ham_account_login(connection);
 * }
 * 
 * // register handle_connection_connected callback
 * HAMConnectionUICallbacks callbacks;
 * callbacks.connected = handle_connection_connected;
 * ham_connection_set_ui_callbacks(callbacks);
 * 
 * // create connection and connect it
 * HAMConnection *connection = ham_connection_new("localhost", 8888, "user", "password");
 * ham_connection_connect(connection);
 * @endcode
 * @param connection Connection used for login.
 */
void ham_account_login(HAMConnection *connection);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
