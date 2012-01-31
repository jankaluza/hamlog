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
#include "signals.h"

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

typedef struct {
	HAMReplyHandler handler;
	void *ui_data;
} HandlerTuple;

void ham_connection_set_ui_callbacks(HAMConnectionUICallbacks *callbacks) {
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
	connection->read_buffer = malloc(sizeof(char) * 65535);
	connection->parser = ham_parser_new();
	connection->reply = ham_reply_new();
	connection->handlers = ham_list_new();
	connection->modules = ham_hash_table_new();
	ham_hash_table_set_free_func(connection->modules, free);

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
	if (connection->fd == -1) {
		return;
	}

	int len = read(connection->fd, connection->read_buffer, 65535);
	
	if (len == 0) {
		ham_signals_emit_signal("connection-disconnected", connection, "Server closed the connection", 0);
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
				char *dump = ham_reply_get_dump(connection->reply);
				ham_signals_emit_signal("connection-reply-received", connection, dump, 0);

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

			}
		}
		else {
			ham_signals_emit_signal("connection-disconnected", connection, "Server sent mallformed data", 0);
			printf("PARSING ERROR\nBUFFER_TO_PARSER='%s'\nCOMPLETE_BUFFER='%s'\n", connection->read_buffer + parsed_total, connection->read_buffer);
			ham_connection_disconnect(connection);
			return;
		}

		ham_reply_destroy(connection->reply);
		connection->reply = ham_reply_new();
	}
}

static void ham_connection_parse_modules(HAMHashTable *table, const char *modules) {
	HAMModule *module = NULL;
	HAMList *lines = ham_csv_parse(modules);
	HAMListItem *line = ham_list_get_first_item(lines);

	// header
	int uri = -1;
	int name = -1;
	int desc = -1;
	int need_auth = -1;
	int type = -1;
	int i = 0;

	// 1st line is header
	HAMListItem *field = ham_list_get_first_item(ham_list_item_get_data(line));
	while (field) {
		if (strcmp((char *) field->data, "uri") == 0) {
			uri = i;
		}
		else if (strcmp((char *) field->data, "name") == 0) {
			name = i;
		}
		else if (strcmp((char *) field->data, "desc") == 0) {
			desc = i;
		}
		else if (strcmp((char *) field->data, "need_auth") == 0) {
			need_auth = i;
		}
		else if (strcmp((char *) field->data, "type") == 0) {
			type = i;
		}
		
		i++;
		field = ham_list_get_next_item(field);
	}

	// 2nd line
	line = ham_list_get_next_item(line);
	while (line) {
		module = calloc(1, sizeof(HAMModule));
		
		int i = 0;
		field = ham_list_get_first_item(ham_list_item_get_data(line));
		while (field) {
			if (i == uri) {
				module->uri = strdup((char *) ham_list_item_get_data(field));
			}
			else if (i == name) {
				module->name = strdup((char *) ham_list_item_get_data(field));
			}
			else if (i == desc) {
				module->desc = strdup((char *) ham_list_item_get_data(field));
			}
			else if (i == need_auth) {
				module->need_auth = *((char *) ham_list_item_get_data(field)) == '1';
			}
			else if (i == type) {
				module->type = atoi((char *) ham_list_item_get_data(field));
			}
			i++;
			field = ham_list_get_next_item(field);
		}

		ham_hash_table_add(table, module->uri, -1, (void *) module);

		line = ham_list_get_next_item(line);
	}

	ham_list_destroy(lines);
}

static void ham_connect_handle_response(HAMConnection *connection, HAMReply *reply, void *data) {
	if (ham_reply_get_status(reply) == 200) {
		// parse modules
		ham_connection_parse_modules(connection->modules, ham_reply_get_content(reply));

		ham_signals_emit_signal("connection-connected", connection, "", 0);
	}
	else {
		ham_signals_emit_signal("connection-disconnected", connection, "Error response from server while fetching modules", 0);
	}
}

void ham_connection_connect(HAMConnection *connection) {
	if (connection->fd != -1)
		return;

	struct hostent *he;
	struct sockaddr_in addr;
	struct sockaddr_in host;

	// TODO: replace with non-blocking
	if ((he = gethostbyname(connection->hostname)) == NULL) {
		ham_signals_emit_signal("connection-disconnected", connection, strerror(errno), 0);
		return;
	}
    
	if ((connection->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		ham_signals_emit_signal("connection-disconnected", connection, strerror(errno), 0);
		return;
	}
    
	host.sin_family = AF_INET;
	host.sin_port = htons(connection->port);
	host.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(addr.sin_zero), '\0', 8);


    //connect to thy host
	if (connect(connection->fd, (struct sockaddr *)&host, sizeof(struct sockaddr)) < 0) {
		ham_signals_emit_signal("connection-disconnected", connection, strerror(errno), 0);
		return;
	}

	connection->input_handle = ham_input_add(connection->fd, ham_connection_read_data, connection);

	ham_connection_fetch_available_modules(connection, ham_connect_handle_response, NULL);
}

void ham_connection_disconnect(HAMConnection *connection) {
	if (connection->fd == -1) {
		return;
	}

	connection->fd = -1;

	ham_input_remove(connection->input_handle);
	close(connection->fd);

	connection->input_handle = NULL;
}

void ham_connection_send(HAMConnection *connection, HAMRequest *request, HAMReplyHandler handler, void *ui_data) {
	if (connection->fd == -1) {
		return;
	}

	char *data = ham_request_get_data(request);
	ham_signals_emit_signal("connection-request-sent", connection, data, 0);
	
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

void ham_connection_fetch_available_modules(HAMConnection *connection, HAMReplyHandler handler, void *ui_data) {
	HAMRequest *request = ham_request_new("/modules", "GET", NULL, NULL);
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
	free(connection->modules);
	ham_parser_destroy(connection->parser);
	free(connection);
}

HAMList *ham_connection_get_modules(HAMConnection *connection) {
	return ham_hash_table_to_list(connection->modules);
}

HAMModule *ham_connection_get_module(HAMConnection *connection, char *name) {
	return (HAMModule *) ham_hash_table_lookup(connection->modules, name, -1);
}

void ham_connection_register_signals() {
	ham_signals_register_signal("connection-request-sent");
	ham_signals_register_signal("connection-reply-received");
	ham_signals_register_signal("connection-connected");
	ham_signals_register_signal("connection-disconnected");
}

