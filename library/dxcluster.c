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

#include "dxcluster.h"
#include "eventloop.h"
#include "parser.h"
#include "request.h"
#include "md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

typedef struct _dxclusterInfo {
	int requests;
	char *data;
	HAMDXClusterHandler handler;
	void *ui_data;
} dxclusterInfo;

static void ham_dxcluster_response(HAMConnection *connection, HAMReply *reply, void *data) {
	return;
	dxclusterInfo *info = (dxclusterInfo *) data;
	info->requests--;

	if (ham_reply_get_status(reply) == 200) {
		if (info->data == 0) {
			info->data = strdup(ham_reply_get_content(reply));
		}
		else {
			// TODO
		}
	}
	else {
		// nothing?
	}

	// last reply received, fire the event
	if (info->requests == 0) {
		if (info->handler) {
			info->handler(connection, info->data, 0, info->ui_data);
		}

		free(info->data);
		free(info);
	}
}

void ham_dxcluster_fetch(HAMConnection *connection, HAMDXClusterHandler handler, void *ui_data) {
	dxclusterInfo *data = malloc(sizeof(dxclusterInfo));
	data->requests = 0;
	data->data = 0;
	data->ui_data = ui_data;
	data->handler = handler;

	HAMList *modules = ham_connection_get_modules(connection);
	HAMListItem *item = ham_list_get_first_item(modules);
	while (item) {
		HAMModule *module = (HAMModule *) ham_list_item_get_data(item);
		if (module->type == DXCLUSTER) {
			data->requests++;
			HAMRequest *request = ham_request_new(module->uri, "GET", NULL, NULL);
			ham_connection_send_destroy(connection, request, ham_dxcluster_response, data);
		}

		item = ham_list_get_next_item(item);
	}
	ham_list_destroy(modules);
}
