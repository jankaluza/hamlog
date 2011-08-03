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
#include "md5.h"

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
	HAMRequest *request = ham_request_new("/register", "GET", NULL, NULL);
	ham_request_add_header(request, "username", connection->username);
	ham_request_add_header(request, "password", connection->password);
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

/* Inspiration taken from libgsasl */
static HAMList* ham_login_digest_md5_parse(const char *digest) {
	const char *token_start, *val_start, *val_end, *cur;

	HAMList *ret = ham_list_new();

	cur = digest;
	while(*cur != '\0') {
		/* Find the end of the token */
		int in_quotes = 0;
		char *name, *value = NULL;
		token_start = cur;
		while(*cur != '\0' && (in_quotes || (!in_quotes && *cur != ','))) {
			if (*cur == '"')
				in_quotes = !in_quotes;
			cur++;
		}

		/* Find start of value.  */
		val_start = strchr(token_start, '=');
		if (val_start == NULL || val_start > cur)
			val_start = cur;

		if (token_start != val_start) {
			name = strndup(token_start, val_start - token_start);

			if (val_start != cur) {
				val_start++;
				while (val_start != cur && (*val_start == ' ' || *val_start == '\t'
						|| *val_start == '\r' || *val_start == '\n'
						|| *val_start == '"'))
					val_start++;

				val_end = cur;
				while (val_end >= val_start && (*val_end == ' ' || *val_end == ',' || *val_end == '\t'
						|| *val_end == '\r' || *val_end == '\n'
						|| *val_end == '"'  || *val_end == '\0'))
					val_end--;

				if (val_end - val_start + 1 >= 0)
					value = strndup(val_start, val_end - val_start + 1);
			}

			ham_list_insert_first(ret, value);
			ham_list_insert_first(ret, name);
		}

		/* Find the start of the next token, if there is one */
		if (*cur != '\0') {
			cur++;
			while (*cur == ' ' || *cur == ',' || *cur == '\t'
					|| *cur == '\r' || *cur == '\n')
				cur++;
		}
	}

	return ret;
}

static void ham_login_handle_get_response1(HAMConnection *connection, HAMReply *reply, void *data) {
	printf("login_handle_get_response: handling reply1\n");

	if (ham_reply_get_status(reply) == 401) {
		char *name;
		char *value;
		char *qop, *realm, *nonce, *opaque = NULL;
		const char *auth = ham_reply_get_header(reply, "WWW-Authenticate");
		HAMRequest *request = ham_request_new("/login", "GET", NULL, NULL);

		/* Parse digest-md5 into HAMList */
		HAMList *list = ham_login_digest_md5_parse(auth + 7);
		while ((name = ham_list_pop_first(list)) != NULL &&
				(value = ham_list_pop_first(list)) != NULL) {
			if (strcmp(name, "qop") == 0)
				qop = value;
			else if (strcmp(name, "realm") == 0)
				realm = value;
			else if (strcmp(name, "nonce") == 0)
				nonce = value;
			else if (strcmp(name, "opaque") == 0)
				opaque = value;
			else
				free(value);
			free(name);
		}
		ham_list_destroy(list);

		/* Stop if server does not send everything we need */
		if (!qop || !realm || !nonce || !opaque) {
			free(nonce); free(opaque); free(realm); free(qop);
			ham_request_destroy(request);
			return;
		}

		/* Compute "response" */
		char *a1 = malloc(sizeof(char) * (strlen(connection->username) + strlen(connection->password) + strlen(realm) + 1 + 2)); // twice ':'
		sprintf(a1, "%s:%s:%s", connection->username, realm, connection->password);
		char *ha1 = md5_get_hash_hex(a1);
		char *ha2 = md5_get_hash_hex("GET:/login");

		char *a3 = malloc(sizeof(char) * (strlen(ha1) + strlen(nonce) + strlen(ha2) + 1 + 2)); // twice ':'
		sprintf(a3, "%s:%s:%s", ha1, nonce, ha2);
		char *response = md5_get_hash_hex(a3);

		/* Generate Authorization header */
		char authorization[512];
		snprintf(authorization, 512, "Digest username=\"%s\","
								"realm=\"%s\","
								"nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
								"uri=\"/login\","
								"qop=auth,"
								"response=\"%s\","
								"opaque=\"%s\")",
								connection->username,
								realm,
								response,
								opaque);

		free(nonce); free(opaque); free(realm); free(qop);
		free(a1); free(ha1); free(ha2); free(a3); free(response);

		/* fire this bastard */
		ham_request_add_header(request, "Authorization", authorization);
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
