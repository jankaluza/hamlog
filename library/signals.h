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

#ifndef _HAMLOG_SIGNALS_H
#define _HAMLOG_SIGNALS_H

#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"
#include "hashtable.h"
#include "connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file signals.h Signals API
 * @ingroup core
 * @details Signals API provides basic way how to be informed about Hamlog
 * actions and incoming data.
 * 
 * From the UI developer perspective, you just
 * have to register to the proper signal using ham_signals_register_handler()
 * function. Once, the signal is emitted, your handler is called. Once, you
 * decide you don't want to listen to this signal, unregister it using
 * ham_signals_unregister_handler().
 *
 * Example:
 * 
 * @code
 * static void handle_logged_in(HAMConnection *connection, const char *data, int error, void *ui_data) {
 * 	if (!error)
 * 		printf("We're logged in!\n");
 * 	else
 * 		printf("Not logged in: %s\n", data);
 * 	}
 * }
 * 
 * static void register_signals_handlers() {
 * 	ham_signals_register_handler("account-logged-in", handle_logged_in, NULL);
 * }
 * 
 * static void unregister_signals_handlers() {
 *	ham_signals_unregister_handler("account-logged-in", handle_logged_in, NULL);
 * }
 * @endcode
 * 
 */

/**
 * Registers new signal.
 * @param signal_name Name of the signal. For example "connection-connected".
 * @see ham_signals_register_handler()
 * @see ham_signals_emit_signal()
 */
void ham_signals_register_signal(const char *signal_name);


/**
 * Emits the signals. Calls all handlers registered to this signal.
 * @param signal_name Name of the signal. For example "connection-connected".
 * @param connection Connection associated with the signal.
 * @param data Data passed to handler.
 * @param error Zero if the signal does not inform about error state.
 * @see ham_signals_register_handler()
 * @see ham_signals_register_signal()
 */
void ham_signals_emit_signal(const char *signal_name, HAMConnection *connection, const char *data, int error);

/**
 * Registers a handler which is called when the signal is emitted.
 * @param signal_name Name of the signal. For example "connection-connected".
 * @param handler Handler which is called when the signal is emitted.
 * @param ui_data Pointer to data object passed to handler to keep the context in the handler.
 * @see ham_signals_unregister_handler()
 */
void ham_signals_register_handler(const char *signal_name, HAMFetchHandler handler, void *ui_data);

/**
 * Unregisters the handler.
 * @param signal_name Name of the signal. For example "connection-connected".
 * @param handler Handler to be unregistered.
 * @param ui_data Pointer to data object passed to handler to keep the context in the handler.
 * @see ham_signals_register_handler()
 */
void ham_signals_unregister_handler(const char *signal_name, HAMFetchHandler handler, void *ui_data);

#ifdef __cplusplus
}
#endif

#endif
