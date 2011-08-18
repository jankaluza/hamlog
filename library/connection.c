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
#include "parser.h"

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

typedef struct {
	HAMReplyHandler handler;
	void *ui_data;
} HandlerTuple;

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
	connection->parser = ham_parser_new();
	connection->reply = ham_reply_new();
	connection->handlers = ham_list_new();

	if (connection->hostname == NULL || connection->username == NULL ||
		connection->password == NULL || connection->read_buffer == NULL) {
		free(connection->hostname);
		free(connection->username);
		free(connection->password);
		free(connection->read_buffer);
		ham_parser_destroy(connection->parser);
		ham_list_destroy(connection->handlers);
		free(connection);
		return NULL;
	}

	return connection;
}

static void ham_connection_read_data(void * user_data, int fd) {
	HAMConnection *connection = user_data;
	int len = read(connection->fd, connection->read_buffer, 8192);
	
	if (len == 0) {
		ui_callbacks->disconnected(connection, "Server closed the connection");
		ham_connection_disconnect(connection);
		return;
	}

	connection->read_buffer[len] = 0;
	unsigned long parsed = 0;
	unsigned long parsed_total = 0;


	while (parsed_total != len - 1) {
		parsed = ham_parser_parse(connection->parser, connection->reply, connection->read_buffer + parsed_total, len - parsed_total);

		if (parsed) {
			parsed_total += parsed;
			if (connection->reply->finished) {
				ham_reply_dump(connection->reply);

				HandlerTuple *tuple = ham_list_get_first(connection->handlers);
				if (tuple) {
					HAMReplyHandler handler = tuple->handler;
					void *ui_data = tuple->ui_data;
					ham_list_remove(connection->handlers, tuple);
					free(tuple);
					if (handler) {
						handler(connection, connection->reply, ui_data);
					}
					
				}

				ham_reply_destroy(connection->reply);
				connection->reply = ham_reply_new();
			}
		}
		else {
			// TODO: ERROR	
		}
	}
}

static void ham_connect_handle_response(HAMConnection *connection, HAMReply *reply, void *data) {
	ui_callbacks->connected(connection);
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

	connection->input_handle = ham_input_add(connection->fd, ham_connection_read_data, connection);

	HAMRequest *request = ham_request_new("/", "GET", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_connect_handle_response, NULL);
}

void ham_connection_disconnect(HAMConnection *connection) {
	ham_input_remove(connection->input_handle);
	close(connection->fd);

	connection->fd = -1;
	connection->input_handle = NULL;
}

void ham_connection_send(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data) {
	char *data = ham_request_get_data(request);
	printf("Request:\n    %s\n", data);
	write(connection->fd, data, strlen(data));
	free(data);

	HandlerTuple *tuple = malloc(sizeof(HandlerTuple));
	if (tuple == NULL)
		return;

	tuple->handler = handler;
	tuple->ui_data = ui_data;
	ham_list_insert_last(connection->handlers, tuple);
}

void ham_connection_send_destroy(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data) {
	ham_connection_send(connection, request, handler, ui_data);
	ham_request_destroy(request);
}

void ham_connection_destroy(HAMConnection *connection) {
	if (connection == NULL)
		return;

	if (connection->input_handle) {
		ham_connection_disconnect(connection);
	}
	free(connection->hostname);
	free(connection->username);
	free(connection->password);
	free(connection->read_buffer);
	ham_parser_destroy(connection->parser);
	free(connection);
}
