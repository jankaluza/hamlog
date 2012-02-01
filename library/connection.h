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

#ifndef _HAMLOG_CONNECTION_H
#define _HAMLOG_CONNECTION_H

#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"
#include "hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file connection.h Connection API
 * @ingroup core
 * @details Connection API provides a way how to connect the Hamlog
 * server and the low-level functions to send requests and receives
 * responses.
 */

/** 
 * Represents one Connection to server.
 */
typedef struct _HAMConnection {
	int fd;
	void *input_handle;

	char *hostname;
	int port;
	char *username;
	char *password;

	HAMReply *reply;
	HAMParser *parser;
	char *read_buffer;

	HAMList *handlers;
	HAMHashTable *modules;
} HAMConnection;

typedef enum {
	UNKNOWN,
	CALLINFO,
	DXCLUSTER,
} ModuleType;

typedef struct _HAMModule {
	char *uri;
	char *name;
	char *desc;
	int need_auth;
	ModuleType type;
	char *data;
} HAMModule;

/** 
 * Low-level handler for incoming replies.
 */
typedef void (*HAMReplyHandler) (HAMConnection *connection, HAMReply *reply, void *ui_data);

/** 
 * High-level handler for the parsed replies.
 */
typedef void (*HAMFetchHandler) (HAMConnection *connection, const char *data, int error, void *ui_data);

/**
 * Signal emitted when Hamlog connected the server.
 * If the error is true, data passed to handler contains error description.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_connection_connected "connection-connected"

/**
 * Signal emitted when Hamlog disconnects the server for some reason.
 * Data passed to handler contains the disconnect reason.
 * @note This signal is \em not called as result of ham_connection_disconnect() call.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_connection_disconnected "connection-disconnected"

/**
 * Signal emitted when request is sent to server.
 * Data passed to the handler contains dump of the request.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_connection_request_sent "connection-request-sent"

/**
 * Signal emitted when new reply from the server is received.
 * Data passed to the handler contains dump of the reply.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_connection_reply_received "connection-reply_received"

/**
 * Creates new connection. You have to use ham_connection_connect() in order to connect the server.
 * @param hostname Hostname of server.
 * @param port Port.
 * @param username Username.
 * @param password Password.
 * @return Newly created connection. The connection has to be destroyed by ham_connection_destroy() later.
 * @see ham_connection_destroy()
 * @see ham_connection_connect()
 */
HAMConnection *ham_connection_new(const char *hostname, int port, const char *username, const char *password);

/**
 * Connects the server. Emits ham_signal_connection_connected when connected.
 * @param connection Connection.
 * @see ham_signal_connection_connected
 * @see ham_connection_disconnect()
 */
void ham_connection_connect(HAMConnection *connection);

/**
 * Disconnects from the server. You still have to destroy the connection later to free it.
 * @param connection Connection.
 * @see ham_connection_destroy()
 */
void ham_connection_disconnect(HAMConnection *connection);

/**
 * Sends request over connected connection and calls callback when reply is received.
 * @param connection Connection.
 * @param request Request.
 * @param handler Handler called once reply for this request is received.
 * @param ui_data Data passed to handler.
 * @see ham_connection_connect()
 * @see ham_connection_send_destroy()
 */
void ham_connection_send(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data);

/**
 * Sends request over connected connection, destroys the request, and calls callback when reply is received.
 * @param connection Connection.
 * @param request Request.
 * @param handler Handler called once reply for this request is received.
 * @param ui_data Data passed to handler.
 * @see ham_connection_connect()
 */
void ham_connection_send_destroy(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data);

/**
 * Fetches the list of server-side modules.
 * @note You don't have to call this function manually. It is called automatically during the connecting process.
 * @param connection Connection.
 * @param handler Handler called once reply for this request is received.
 * @param ui_data Data passed to handler.
 * @see ham_connection_connect()
 */
void ham_connection_fetch_available_modules(HAMConnection *connection, HAMReplyHandler handler, void *ui_data);

/**
 * Returns the list of all server-side modules.
 * @param connection Connection.
 * @return List of all server-side modules.
 * @see ham_connection_connect()
 */
HAMList *ham_connection_get_modules(HAMConnection *connection);

/**
 * Returns the server-side module according to its name.
 * @param connection Connection.
 * @param name Name of the module to return
 * @return Pointer to server-side module or NULL if there is not module with that name.
 * @see ham_connection_connect()
 */
HAMModule *ham_connection_get_module(HAMConnection *connection, char *name);

/**
 * Destroys the connection.
 * @param connection Connection.
 * @see ham_connection_disconnect()
 */
void ham_connection_destroy(HAMConnection *connection);

void ham_connection_register_signals();

#ifdef __cplusplus
}
#endif

#endif
