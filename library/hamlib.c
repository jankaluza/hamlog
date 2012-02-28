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

#include "hamlib.h"
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

typedef struct _dxccInfo {
	HAMFetchHandler handler;
	void *ui_data;
} dxccInfo;

static void ham_hamlib_frequency_set_response(HAMConnection *connection, HAMReply *reply, void *_data) {
	dxccInfo *data = (dxccInfo *) _data;
	const char *content = ham_reply_get_content(reply);
	if (data->handler) {
		data->handler(connection, content, ham_reply_get_status(reply) != 200, data->ui_data);
	}

	ham_signals_emit_signal("hamlib-frequency-set", connection, content, ham_reply_get_status(reply) != 200);

	free(data);
}

void ham_hamlib_set_frequency(HAMConnection *connection, const char *freq, HAMFetchHandler handler, void *ui_data) {
	if (ham_connection_get_module(connection, "/hamlib") == NULL) {
		return;
	}

	dxccInfo *data = malloc(sizeof(dxccInfo));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/hamlib", "POST", freq, "hamlog");
	ham_connection_send_destroy(connection, request, ham_hamlib_frequency_set_response, data);
}

static void ham_hamlib_frequency_fetch_response(HAMConnection *connection, HAMReply *reply, void *_data) {
	dxccInfo *data = (dxccInfo *) _data;
	const char *content = ham_reply_get_content(reply);
	if (data->handler) {
		data->handler(connection, content, ham_reply_get_status(reply) != 200, data->ui_data);
	}

	ham_signals_emit_signal("hamlib-frequency-fetched", connection, content, ham_reply_get_status(reply) != 200);

	free(data);
}

void ham_hamlib_fetch_frequency(HAMConnection *connection, HAMFetchHandler handler, void *ui_data) {
	if (ham_connection_get_module(connection, "/hamlib") == NULL) {
		return;
	}

	dxccInfo *data = malloc(sizeof(dxccInfo));
	data->handler = handler;
	data->ui_data = ui_data;

	HAMRequest *request = ham_request_new("/hamlib", "GET", NULL, "hamlog");
	ham_connection_send_destroy(connection, request, ham_hamlib_frequency_fetch_response, data);
}

void ham_hamlib_register_signals() {
	ham_signals_register_signal("hamlib-frequency-set");
	ham_signals_register_signal("hamlib-frequency-fetched");
}
