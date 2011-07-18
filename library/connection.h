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

#ifndef _HAMLOG_CONNECTION_H
#define _HAMLOG_CONNECTION_H

#include "parser.h"

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
} HAMConnection;

typedef struct _HAMConnectionUICallbacks {
	void (*connected) (HAMConnection *connection);
	void (*disconnected) (HAMConnection *connection, const char *reason);
} HAMConnectionUICallbacks;

void ham_connection_set_ui_callbacks(HAMConnectionUICallbacks *callbacks);
HAMConnection *ham_connection_new(const char *hostname, int port, const char *username, const char *password);
void ham_connection_connect(HAMConnection *connection);
void ham_connection_disconnect(HAMConnection *connection);
void ham_connection_destroy(HAMConnection *connection);

#endif
