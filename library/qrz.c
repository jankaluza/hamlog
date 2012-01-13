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

#include "qrz.h"
#include "eventloop.h"
#include "parser.h"
#include "request.h"
#include "md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

static HAMQRZUICallbacks *ui_callbacks = NULL;

void ham_qrz_set_ui_callbacks(HAMQRZUICallbacks *callbacks) {
	ui_callbacks = callbacks;
}

static void ham_qrz_response(HAMConnection *connection, HAMReply *reply, void *data) {
	if (ham_reply_get_status(reply) == 200) {
		if (ui_callbacks && ui_callbacks->fetched)
			ui_callbacks->fetched(connection, data, ham_reply_get_content(reply));
	}
	else {
		if (ui_callbacks && ui_callbacks->fetching_failed)
			ui_callbacks->fetching_failed(connection, data, ham_reply_get_content(reply));
	}
	free(data);
}

void ham_qrz_fetch(HAMConnection *connection, const char *call) {
	if (ham_connection_get_module(connection, "/qrz") == NULL) {
		return;
	}

	HAMRequest *request = ham_request_new("/qrz", "POST", call, "hamlog");
	ham_connection_send_destroy(connection, request, ham_qrz_response, strdup(call));
}

static void ham_username_handle_response(HAMConnection *connection, HAMReply *reply, void *data) {
	if (ham_reply_get_status(reply) == 200) {
		HAMModule *module = ham_connection_get_module(connection, "/qrz");
		if (module != NULL) {
			free(module->data);
			module->data = strdup(ham_reply_get_content(reply));
		}
	}
}

void ham_qrz_fetch_username(HAMConnection *connection) {
	HAMRequest *request = ham_request_new("/qrz/username", "GET", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_username_handle_response, NULL);
}

static void ham_register_handle_response(HAMConnection *connection, HAMReply *reply, void *data) {
	if (ham_reply_get_status(reply) == 200) {
		if (ui_callbacks && ui_callbacks->registered)
			ui_callbacks->registered(connection);
	}
	else {
		if (ui_callbacks && ui_callbacks->registration_failed)
			ui_callbacks->registration_failed(connection, ham_reply_get_content(reply));
	}
}

void ham_qrz_register(HAMConnection *connection, const char *username, const char *password) {
	HAMRequest *request = ham_request_new("/qrz/register", "GET", NULL, NULL);
	ham_request_add_header(request, "username", username);
	ham_request_add_header(request, "password", password);
	ham_connection_send_destroy(connection, request, ham_register_handle_response, NULL);
}
