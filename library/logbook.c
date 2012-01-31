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

#include "logbook.h"
#include "eventloop.h"
#include "parser.h"
#include "request.h"
#include "md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

typedef struct _logbookClosure {
	HAMLoogbookHandler handler;
	void *ui_data;
} logbookClosure;

static HAMLogBookUICallbacks *ui_callbacks = NULL;

void ham_logbook_set_ui_callbacks(HAMLogBookUICallbacks *callbacks) {
	ui_callbacks = callbacks;
}

static void ham_logbook_fetch_response(HAMConnection *connection, HAMReply *reply, void *_data) {
	const char *content = ham_reply_get_content(reply);

	logbookClosure *data = (logbookClosure *) _data;
	if (data->handler) {
		data->handler(connection, content, ham_reply_get_status(reply) != 200, data->ui_data);
	}
	else {
		if (ui_callbacks && ui_callbacks->fetched) {
			ui_callbacks->fetched(connection, content);
		}
	}
	free(data);
}

void ham_logbook_fetch(HAMConnection *connection, HAMLoogbookHandler handler, void *ui_data) {
	logbookClosure *data = malloc(sizeof(logbookClosure));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/logbook", "GET", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_logbook_fetch_response, data);
}

void ham_logbook_fetch_with_call(HAMConnection *connection, const char *call, HAMLoogbookHandler handler, void *ui_data) {
	logbookClosure *data = malloc(sizeof(logbookClosure));
	data->handler = handler;
	data->ui_data = ui_data;
	
	char uri[80] = "/logbook/call/";
	strncpy(uri + 14, call, 40);
	HAMRequest *request = ham_request_new(uri, "GET", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_logbook_fetch_response, data);
}

static void ham_logbook_add_response(HAMConnection *connection, HAMReply *reply, void *_data) {
	const char *content = ham_reply_get_content(reply);

	logbookClosure *data = (logbookClosure *) _data;
	if (data->handler) {
		data->handler(connection, content, ham_reply_get_status(reply) != 200, data->ui_data);
	}
	free(data);
}

void ham_logbook_add(HAMConnection *connection, const char *payload, HAMLoogbookHandler handler, void *ui_data) {
	logbookClosure *data = malloc(sizeof(logbookClosure));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/logbook/add", "POST", payload, "hamlog");
	ham_connection_send_destroy(connection, request, ham_logbook_add_response, data);
}

static void ham_logbook_remove_response(HAMConnection *connection, HAMReply *reply, void *data) {
	if (ham_reply_get_status(reply) == 200) {
		if (ui_callbacks && ui_callbacks->removed)
			ui_callbacks->removed(connection, data);
	}
	else {
		const char *error = ham_reply_get_content(reply);
		if (ui_callbacks && ui_callbacks->remove_failed)
			ui_callbacks->remove_failed(connection, data, error);
	}
	free(data);
}

void ham_logbook_remove(HAMConnection *connection, const char *data) {
	HAMRequest *request = ham_request_new("/logbook/remove", "POST", data, "hamlog");
	ham_connection_send_destroy(connection, request, ham_logbook_remove_response, (void *) strdup(data));
}
