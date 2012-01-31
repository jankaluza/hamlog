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
#include "signals.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

typedef struct _logbookClosure {
	HAMFetchHandler handler;
	void *ui_data;
} logbookClosure;

static void ham_logbook_fetch_response(HAMConnection *connection, HAMReply *reply, void *_data) {
	const char *content = ham_reply_get_content(reply);

	logbookClosure *data = (logbookClosure *) _data;
	if (data->handler) {
		data->handler(connection, content, ham_reply_get_status(reply) != 200, data->ui_data);
	}

	ham_signals_emit_signal("logbook-fetched", connection, content, ham_reply_get_status(reply) != 200);

	free(data);
}

void ham_logbook_fetch(HAMConnection *connection, HAMFetchHandler handler, void *ui_data) {
	logbookClosure *data = malloc(sizeof(logbookClosure));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/logbook", "GET", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_logbook_fetch_response, data);
}

void ham_logbook_fetch_with_call(HAMConnection *connection, const char *call, HAMFetchHandler handler, void *ui_data) {
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

	ham_signals_emit_signal("logbook-added", connection, content, ham_reply_get_status(reply) != 200);

	free(data);
}

void ham_logbook_add(HAMConnection *connection, const char *payload, HAMFetchHandler handler, void *ui_data) {
	logbookClosure *data = malloc(sizeof(logbookClosure));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/logbook/add", "POST", payload, "hamlog");
	ham_connection_send_destroy(connection, request, ham_logbook_add_response, data);
}

static void ham_logbook_remove_response(HAMConnection *connection, HAMReply *reply, void *_data) {
	const char *content = ham_reply_get_content(reply);

	logbookClosure *data = (logbookClosure *) _data;
	if (data->handler) {
		data->handler(connection, content, ham_reply_get_status(reply) != 200, data->ui_data);
	}

	ham_signals_emit_signal("logbook-removed", connection, content, ham_reply_get_status(reply) != 200);

	free(data);
}

void ham_logbook_remove(HAMConnection *connection, const char *id, HAMFetchHandler handler, void *ui_data) {
	logbookClosure *data = malloc(sizeof(logbookClosure));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/logbook/remove", "POST", id, "hamlog");
	ham_connection_send_destroy(connection, request, ham_logbook_remove_response, data);
}

void ham_logbook_register_signals() {
	ham_signals_register_signal("logbook-added");
	ham_signals_register_signal("logbook-removed");
	ham_signals_register_signal("logbook-fetched");
}
