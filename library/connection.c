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

#include "connection.h"
#include "eventloop.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h> 

static HAMConnectionUICallbacks *ui_callbacks = NULL;

void ham_connection_set_ui_callbacks(HAMConnectionUICallbacks *callbacks) {
	ui_callbacks = callbacks;
}

HAMConnection *ham_connection_new(const char *hostname, int port, const char *username, const char *password) {
	HAMConnection *connection = malloc(sizeof(HAMConnection));
	if (!connection)
		return connection;

	connection->fd = -1;
	connection->input_handle = NULL;
	connection->hostname = strdup(hostname);
	connection->port = port;
	connection->username = strdup(username);
	connection->password = strdup(password);
	connection->read_buffer = malloc(sizeof(char) * 8192);

	if (connection->hostname == NULL || connection->username == NULL ||
		connection->password == NULL || connection->read_buffer == NULL) {
		free(connection);
		return NULL;
	}

	return connection;
}

static void ham_connection_read_data(void * user_data, int fd, HAMInputCondition condition) {
	HAMConnection *connection = user_data;
	int len = read(connection->fd, connection->read_buffer, 8192);

	if (len == 0) {
		ui_callbacks->disconnected(connection, "Server closed the connection");
		ham_connection_disconnect(connection);
		return;
	}

	connection->read_buffer[len] = 0;

	printf("GOT DAT:%s\n", connection->read_buffer);
}

void ham_connection_connect(HAMConnection *connection) {
	if (connection->fd != -1)
		return;

	struct hostent *he;
	struct sockaddr_in addr;
	struct sockaddr_in host;

	// TODO: replace with non-blocking
	if ((he = gethostbyname(connection->hostname)) == NULL) {
		ui_callbacks->disconnected(connection, strerror(errno));
		return;
	}
    
	if ((connection->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		ui_callbacks->disconnected(connection, strerror(errno));
		return;
	}
    
	host.sin_family = AF_INET;
	host.sin_port = htons(connection->port);
	host.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(addr.sin_zero), '\0', 8);


    //connect to thy host
	if (connect(connection->fd, (struct sockaddr *)&host, sizeof(struct sockaddr)) < 0) {
		ui_callbacks->disconnected(connection, strerror(errno));
		return;
	}

	connection->input_handle = ham_input_add(connection->fd, HAM_INPUT_READ, ham_connection_read_data, connection);
}

void ham_connection_disconnect(HAMConnection *connection) {
	ham_input_remove(connection->input_handle);
	close(connection->fd);

	connection->fd = -1;
	connection->input_handle = NULL;
}

void ham_connection_destroy(HAMConnection *connection) {
	if (connection->input_handle) {
		ham_connection_disconnect(connection);
	}
	free(connection->hostname);
	free(connection->username);
	free(connection->password);
	free(connection);
}
