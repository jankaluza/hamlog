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

#include "account.h"
#include "eventloop.h"
#include "parser.h"
#include "request.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

static HAMAccountUICallbacks *ui_callbacks = NULL;

void ham_account_set_ui_callbacks(HAMAccountUICallbacks *callbacks) {
	ui_callbacks = callbacks;
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

void ham_account_register(HAMConnection *connection) {
	HAMRequest *request = ham_request_new("/register", "POST", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_register_handle_response, NULL);
}

void ham_account_unregister(HAMConnection *connection) {
	
}

static void ham_login_handle_get_response2(HAMConnection *connection, HAMReply *reply, void *data) {
	printf("login_handle_get_response: handling reply2\n");
	if (ham_reply_get_status(reply) == 401) {
		if (ui_callbacks && ui_callbacks->login_failed)
			ui_callbacks->login_failed(connection, "Bad username or password");
	}
	else {
		if (ui_callbacks && ui_callbacks->logged_in)
			ui_callbacks->logged_in(connection);
	}
}

static void ham_login_handle_get_response1(HAMConnection *connection, HAMReply *reply, void *data) {
	printf("login_handle_get_response: handling reply1\n");

	if (ham_reply_get_status(reply) == 401) {
		// TODO: compute authorization here
		HAMRequest *request = ham_request_new("/login", "GET", NULL, NULL);
		ham_request_add_header(request, "Authorization", "Something....");
		ham_connection_send_destroy(connection, request, ham_login_handle_get_response2, NULL);
	}
	else {
		if (ui_callbacks && ui_callbacks->logged_in)
			ui_callbacks->logged_in(connection);
	}
}

void ham_account_login(HAMConnection *connection) {
	HAMRequest *request = ham_request_new("/login", "GET", NULL, NULL);
	ham_connection_send_destroy(connection, request, ham_login_handle_get_response1, NULL);
}
