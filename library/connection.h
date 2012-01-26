/**
 * @file connection.h Connection API
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
 * Handler for incoming replies.
 */
typedef void (*HAMReplyHandler) (HAMConnection *connection, HAMReply *reply, void *data);

/**
 * HAMConnection related UI callbacks.
 */
typedef struct _HAMConnectionUICallbacks {
	/**
	 * Called when the connection is connected to server.
	 */
	void (*connected) (HAMConnection *connection);

	/**
	 * Called when the connection is disconnected.
	 */
	void (*disconnected) (HAMConnection *connection, const char *reason);

	/**
	 * Called when the reply from server is received and there was no handler
	 * to handle it.
	 */
	void (*reply_received) (HAMConnection *connection, HAMReply *reply);
} HAMConnectionUICallbacks;

/**
 * Sets UI callbacks which are then called for particular events.
 * @param callbacks UI Callbacks.
 */
void ham_connection_set_ui_callbacks(HAMConnectionUICallbacks *callbacks);

/**
 * Creates new connection. You have to use ham_connection_connect in order to connect the server.
 * @param hostname Hostname of server.
 * @param port Port.
 * @param username Username.
 * @param password Password.
 * @return Newly created connection. The connection has to be destroyed by ham_connection_destroy later.
 */
HAMConnection *ham_connection_new(const char *hostname, int port, const char *username, const char *password);

/**
 * Connects the server. Calls "connected" or "disconnected" UI callbacks when done.
 * @code
 * // ham_account_register has to be called when the connection is connected
 * static void handle_connection_connected(HAMConnection *connection) {
 * 	printf("connected\n");
 * }
 * 
 * static void handle_connection_disconnected(HAMConnection *connection, const char *reason) {
 * 	printf("disconnected: %s\n", reason);
 * }
 * 
 * // register UI callbacks
 * HAMConnectionUICallbacks callbacks;
 * callbacks.connected = handle_connection_connected;
 * callbacks.disconnected = handle_connection_disconnected;
 * ham_connection_set_ui_callbacks(callbacks);
 * 
 * // create connection and connect it
 * HAMConnection *connection = ham_connection_new("localhost", 8888, "user", "password");
 * ham_connection_connect(connection);
 * @endcode
 * @param connection Connection.
 */
void ham_connection_connect(HAMConnection *connection);

/**
 * Disconnects the server.
 * @param connection Connection.
 */
void ham_connection_disconnect(HAMConnection *connection);

/**
 * Sends request over connected connection and calls callback when reply is received.
 * @param connection Connection.
 * @param request Request.
 * @param handler Handler called once reply for this request is received.
 * @param ui_data Data passed to handler.
 */
void ham_connection_send(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data);

/**
 * Sends request over connected connection, destroys the request, and calls callback when reply is received.
 * @param connection Connection.
 * @param request Request.
 * @param handler Handler called once reply for this request is received.
 * @param ui_data Data passed to handler.
 */
void ham_connection_send_destroy(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data);

void ham_connection_fetch_available_modules(HAMConnection *connection, HAMReplyHandler handler, void *ui_data);

HAMList *ham_connection_get_modules(HAMConnection *connection);

HAMModule *ham_connection_get_module(HAMConnection *connection, char *name);

/**
 * Destroys the connection.
 * @param connection Connection.
 */
void ham_connection_destroy(HAMConnection *connection);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
