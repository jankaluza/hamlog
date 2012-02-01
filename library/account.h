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
 * @file account.h Account API
 * @ingroup core
 * @details Account API provides a way to login the Hamlog server
 * or register a new account.
 */

/**
 * Signal emitted when account has been registered.
 * If the error is true, data passed to handler contains error description.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_account_registered "account-registered"

/**
 * Signal emitted when login process finished.
 * If the error is true, data passed to handler contains error description.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_account_logged_in "account-logged-in"

/**
 * Registers new account on remote server. Emits ham_signal_account_registered signal.
 * @note This has to be called when when the connection is
 * connected (ham_signal_connection_connected signal has been emitted)
 * @param connection Connection used for registration.
 * @see ham_signal_account_registered
 * @see ham_signal_connection_connected
 * @see ham_connection_connect()
 */
void ham_account_register(HAMConnection *connection);

void ham_account_unregister(HAMConnection *connection);

/**
 * Logins user associated with the conection. Emits ham_signal_account_logged_in signal.
 * @note This has to be called when when the connection is
 * connected (ham_signal_connection_connected signal has been emitted)
 * Calls "logged_in" or "login_failed" UI callback when done.
 * @param connection Connection used for login.
 * @see ham_signal_account_logged_in
 * @see ham_signal_connection_connected
 * @see ham_connection_connect()
 */
void ham_account_login(HAMConnection *connection);

void ham_account_register_signals();

#ifdef __cplusplus
}
#endif

#endif
