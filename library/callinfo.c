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

#include "callinfo.h"
#include "eventloop.h"
#include "parser.h"
#include "request.h"
#include "md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include "callinfo.h"

typedef struct _callInfo {
	int requests;
	char *data;
	char *call;
	HAMCallInfoHandler handler;
	void *ui_data;
} callInfo;

static HAMCallInfoUICallbacks *ui_callbacks;

void ham_callinfo_set_ui_callbacks(HAMCallInfoUICallbacks *callbacks) {
	ui_callbacks = callbacks;
}

static void ham_callinfo_response(HAMConnection *connection, HAMReply *reply, void *data) {
	callInfo *info = (callInfo *) data;
	info->requests--;

	if (ham_reply_get_status(reply) == 200) {
		if (info->data == 0) {
			info->data = strdup(ham_reply_get_content(reply));
		}
		else {
			char *new_data = ham_csv_merge(info->data, ham_reply_get_content(reply));
			free(info->data);
			info->data = new_data;
		}
	}
	else {
		// nothing?
	}

	// last reply received, fire the event
	if (info->requests == 0) {
		if (info->data) {
			if (info->handler) {
				info->handler(connection, info->data, 0, info->ui_data);
			}
			else if (ui_callbacks && ui_callbacks->fetched) {
				ui_callbacks->fetched(connection, data, info->data);
			}
		}

		free(info->call);
		free(info->data);
		free(info);
	}
}

void ham_callinfo_fetch(HAMConnection *connection, const char *call, HAMCallInfoHandler handler, void *ui_data) {
	callInfo *data = malloc(sizeof(callInfo));
	data->requests = 0;
	data->data = 0;
	data->call = strdup(call);
	data->ui_data = ui_data;
	data->handler = handler;

	// iterate over all CALLINFO modules and ask for the CALL information
	HAMList *modules = ham_connection_get_modules(connection);
	HAMListItem *item = ham_list_get_first_item(modules);
	while (item) {
		HAMModule *module = (HAMModule *) ham_list_item_get_data(item);
		if (module->type == CALLINFO) {
			data->requests++;
			HAMRequest *request = ham_request_new(module->uri, "POST", call, "hamlog");
			ham_connection_send_destroy(connection, request, ham_callinfo_response, data);
		}

		item = ham_list_get_next_item(item);
	}
	ham_list_destroy(modules);
}
