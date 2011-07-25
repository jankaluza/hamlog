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

#include "reply.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

HAMReplyHeader *ham_reply_header_new(const char *name, const char *value) {
	HAMReplyHeader *header = malloc(sizeof(HAMReplyHeader));
	if (header == NULL)
		return NULL;

	header->name = strdup(name);
	header->value = strdup(value);

	if (header->name == NULL || header->value == name) {
		free(header->name);
		free(header->value);
		free(header);
		return NULL;
	}

	return header;
}

void ham_reply_header_destroy(HAMReplyHeader *header) {
	if (header == NULL)
		return;
	free(header->name);
	free(header->value);
	free(header);
}

const char *ham_reply_header_get_name(HAMReplyHeader *header) {
	return header->name;
}

const char *ham_reply_header_get_value(HAMReplyHeader *header) {
	return header->value;
}

HAMReply *ham_reply_new() {
	HAMReply *reply = malloc(sizeof(HAMReply));
	if (reply == NULL)
		return NULL;

	reply->status = 0;
	reply->headers_count = 0;
	reply->headers = NULL;
	reply->finished = 0;
	reply->content = malloc(sizeof(char) * MAX_REPLY_SIZE); // TODO: realloc in the future
	if (reply->content == NULL) {
		free(reply);
		return NULL;
	}

	return reply;
}

void ham_reply_destroy(HAMReply *reply) {
	int i;
	if (reply == NULL)
		return;

	free(reply->content);
	for (i = 0; i < reply->headers_count; i++) {
		ham_reply_header_destroy(reply->headers[i]);
	}
	free(reply->headers);

	free(reply);
}

void ham_reply_dump(HAMReply *reply) {
	int i;
	printf("reply dump:\n");
	printf("   status=%d\n", reply->status);
	printf("   content=%s\n", reply->content);
	printf("   Headers:\n");
	for (i = 0; i < reply->headers_count; i++) {
		HAMReplyHeader *header = reply->headers[i];
		printf("       %s: %s\n", header->name, header->value);
	}
}

void ham_reply_add_header(HAMReply *reply, HAMReplyHeader *header) {
	reply->headers_count++;
	reply->headers = realloc(reply->headers, sizeof(HAMReplyHeader*) * reply->headers_count);

	if (reply->headers == NULL) {
		reply->headers_count = 0;
		return;
	}

	reply->headers[reply->headers_count - 1] = header;
}

const char *ham_reply_get_header(HAMReply *reply, const char *name) {
	int i;
	for (i = 0; i < reply->headers_count; i++) {
		HAMReplyHeader *header = reply->headers[i];
		if (strcmp(header->name, name) == 0) {
			return header->value;
		}
	}

	return NULL;
}

const char *ham_reply_get_content(HAMReply *reply) {
	return reply->content;
}

int ham_reply_get_status(HAMReply *reply) {
	return reply->status;
}
