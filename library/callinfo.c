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
} callInfo;

static char *merge_responses(const char *first, const char *second) {
	// TODO: Create merge_csv() from this
	// get the first line from "first"
	HAMList *first_lines = ham_parse_csv(first);
	HAMList *parsed = first_lines;
	HAMListItem *first_line = ham_list_get_first_item(first_lines);

	// get the first line from "second"
	HAMList *second_lines = ham_parse_csv(second);
	HAMListItem *second_line = ham_list_get_first_item(second_lines);

	// add everything from "second" line to "first" line
	HAMListItem *field = ham_list_get_first_item(ham_list_item_get_data(second_line));
	while (field) {
		ham_list_insert_last(ham_list_item_get_data(first_line), ham_list_item_get_data(field));
		field = ham_list_get_next_item(field);
	}

	first_line = ham_list_get_next_item(first_line);
	second_line = ham_list_get_next_item(second_line);

	field = ham_list_get_first_item(ham_list_item_get_data(second_line));
	while (field) {
		ham_list_insert_last(ham_list_item_get_data(first_line), ham_list_item_get_data(field));
		field = ham_list_get_next_item(field);
	}

	// TODO: create ham_list_to_csv() from this
	unsigned long length = 0;
	HAMListItem *line = ham_list_get_first_item(parsed);
	while (line) {
		field = ham_list_get_first_item(ham_list_item_get_data(line));
		while (field) {
			length += strlen(ham_list_item_get_data(field)) + 1; // ';'
			field = ham_list_get_next_item(field);
		}
		length += 1; // '\n'
		line = ham_list_get_next_item(line);
	}

	length += 1; // '\0'

	char *res = malloc(sizeof(char) * length);
	char *ptr = res;
	line = ham_list_get_first_item(parsed);
	while (line) {
		field = ham_list_get_first_item(ham_list_item_get_data(line));
		while (field) {
			strcpy(ptr, (char *) ham_list_item_get_data(field));
			ptr += strlen(ham_list_item_get_data(field));
			*ptr++ = ';';
			field = ham_list_get_next_item(field);
		}
		*ptr++ = '\n';
		line = ham_list_get_next_item(line);
	}
	*ptr++ = 0;

	ham_list_destroy(parsed);

	return res;
}

static void ham_callinfo_response(HAMConnection *connection, HAMReply *reply, void *data) {
	callInfo *info = (callInfo *) data;
	info->requests--;

	if (ham_reply_get_status(reply) == 200) {
		if (info->data == 0) {
			info->data = strdup(ham_reply_get_content(reply));
		}
		else {
			char *new_data = merge_responses(info->data, ham_reply_get_content(reply));
			free(info->data);
			info->data = new_data;
		}
	}
	else {
		// nothing?
	}

	// last reply received, fire the event
	if (info->requests == 0) {
		printf("Everything received\n%s\n", info->data);
	}
}

void ham_callinfo_fetch(HAMConnection *connection, const char *call) {
	callInfo *data = malloc(sizeof(callInfo));
	data->requests = 0;
	data->data = 0;
	data->call = strdup(call);

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
