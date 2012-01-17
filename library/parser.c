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

#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

HAMParser *ham_parser_new() {
	HAMParser *parser = malloc(sizeof(HAMParser));
	if (parser == NULL)
		return NULL;

	ham_parser_reset(parser);

	return parser;
}

static int isChar(int c) {
	return c >= 0 && c <= 127;
}

static int isControl(int c) {
	return (c >= 0 && c <= 31) || c == 127;
}

static int isSpecial(int c) {
	switch (c) {
		case '(':
		case ')':
		case '<':
		case '>':
		case '@':
		case ',':
		case ';':
		case ':':
		case '\\':
		case '"':
		case '/':
		case '[':
		case ']':
		case '?': 
		case '=':
		case '{':
		case '}':
		case ' ':
		case '\t':
			return 1;
		default:
			return 0;
	}
}

static int isDigit(int c) {
	return c >= '0' && c <= '9';
}

static int ham_parser_consume(HAMParser *parser, HAMReply *reply, char input) {
	switch (parser->state) {
		case http_version_h:
			if (input == 'H') {
				parser->state = http_version_t_1;
				return 1;
			}
			/*HACK: FIX ME*/
			else if (input == 10 || input == 13) {
				return 1;
			}
			else {
				return 0;
			}
		case http_version_t_1:
			if (input == 'T') {
				parser->state = http_version_t_2;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_t_2:
			if (input == 'T') {
				parser->state = http_version_p;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_p:
			if (input == 'P') {
				parser->state = http_version_slash;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_slash:
			if (input == '/') {
				parser->state = http_version_major_start;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_major_start:
			if (isDigit(input)) {
				parser->state = http_version_major;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_major:
			if (input == '.') {
				parser->state = http_version_minor_start;
				return 1;
			}
			else if (isDigit(input)) {
				return 1;
			}
			else {
				return 0;
			}
		case http_version_minor_start:
			if (isDigit(input)) {
				parser->state = http_version_minor;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_minor:
			if (input == ' ') {
				parser->state = status_start;
				return 1;
			}
			else if (isDigit(input)) {
				return 1;
			}
			else {
				return 0;
			}
		case status_start:
			if (isDigit(input)) {
				reply->status = 100 * (input - '0');
				parser->state = status_1;
				return 1;
			}
			else {
				return 0;
			}
		case status_1:
			if (isDigit(input)) {
				reply->status += 10 * (input - '0');
				parser->state = status_2;
				return 1;
			}
			else {
				return 0;
			}
		case status_2:
			if (isDigit(input)) {
				reply->status += input - '0';
				parser->state = status_text;
				return 1;
			}
			else {
				return 0;
			}
		case status_text:
			/* we skip this one */
			if (isDigit(input)) {
				return 0;
			}
			else if (input == '\r') {
				parser->state = expecting_newline_1;
				return 1;
			}
			else {
				return 1;
			}
		case expecting_newline_1:
			if (input == '\n') {
				parser->state = header_line_start;
				return 1;
			}
			else {
				return 0;
			}
		case header_line_start:
			if (input == '\r') {
				parser->state = expecting_newline_3;
				return 1;
			}
			else if (reply->headers_count == 0 && (input == ' ' || input == '\t')) {
				parser->state = header_lws;
				return 1;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return 0;
			}
			else {
				parser->state = header_name;
				parser->ptr = parser->header_name;
				*parser->ptr++ = input;
				return 1;
			}
		case header_lws:
			if (input == '\r') {
				parser->state = expecting_newline_2;
				return 1;
			}
			else if (input == ' ' || input == '\t') {
				return 1;
			}
			else if (isControl(input)) {
				return 0;
			}
			else {
				parser->state = header_value;
				*parser->ptr++ = input;
				//response->m_headers.back().value.push_back(input);
				return 1;
			}
		case header_name:
			if (input == ':') {
				parser->state = space_before_header_value;
				return 1;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return 0;
			}
			else {
				*parser->ptr++ = input;
				return 1;
			}
		case space_before_header_value:
			if (input == ' ') {
				parser->state = header_value;
				*parser->ptr = 0;
				parser->ptr = parser->header_value;
				return 1;
			}
			else {
				return 0;
			}
		case header_value:
			if (input == '\r') {
				parser->state = expecting_newline_2;
				*parser->ptr = 0;
				HAMReplyHeader *header = ham_reply_header_new(parser->header_name, parser->header_value);
				ham_reply_add_header(reply, header);
				return 1;
			}
			else if (isControl(input)) {
				return 0;
			}
			else {
 				*parser->ptr++ = input;
				return 1;
			}
		case content_start:
			if (input == '\r') {
				parser->state = expecting_newline_3;
				return 1;
			}
			else {
				parser->state = content;
				parser->ptr = reply->content;
				*parser->ptr++ = input;
				return 1;
			}
		case content:
			if (input == '\r') {
				*parser->ptr = 0;
				parser->state = expecting_newline_4;
				return 1;
			}
			else {
 				*parser->ptr++ = input;
				return 1;
			}
		case expecting_newline_2:
			if (input == '\n') {
				parser->state = header_line_start;
				return 1;
			}
			else {
				return 0;
			}
		case expecting_newline_3:
			if (input == '\n') {
				if (ham_reply_get_header(reply, "Content-Length") && strcmp(ham_reply_get_header(reply, "Content-Length"), "0") != 0) {
					parser->state = content_start;
				}
				else {
					*reply->content = 0;
					reply->finished = 1;
					ham_parser_reset(parser);
				}
				return 1;
			}
			return 0;
		case expecting_newline_4:
			if (input == '\n') {
				reply->finished = 1;
				ham_parser_reset(parser);
				return 1;
			}
			return 0;
		default:
			return 0;
	}
}

unsigned long ham_parser_parse(HAMParser *parser, HAMReply *response, const char *data, unsigned long size) {
	unsigned long old_size = size;
	for (;size > 0; size--) {
		if (ham_parser_consume(parser, response, *data++) == 0) {
			ham_parser_reset(parser);
			return old_size - size;
		}
		if (response->finished)
			return old_size - size;
	}
	return old_size - size;
}

void ham_parser_reset(HAMParser *parser) {
	parser->state = http_version_h;
	parser->ptr = NULL;
}

void ham_parser_destroy(HAMParser *parser) {
	if (parser == NULL)
		return;

	free(parser);
}

HAMList *ham_parse_csv(const char *str) {
	HAMList *tokens = ham_list_new();
	ham_list_set_free_func(tokens, ham_list_destroy);

	unsigned int pos = 0;
	int quotes = 0;
	char field[65535];
	char *ptr = field;

	HAMList *line = ham_list_new();
	ham_list_set_free_func(line, free);
     
    while(pos < strlen(str) && str[pos] != 0) {
		char c = str[pos];
		if (!quotes && c == '"' ) {
			quotes = 1;
		}
		else if (quotes && c== '"' ) {
			if (pos + 1 < strlen(str) && str[pos+1]== '"' ) {
				*ptr++ = c;
				pos++;
			}
			else {
				quotes = 0;
			}
		}
		else if (!quotes && c == ';') {
			*ptr++ = 0;
			ham_list_insert_last(line, strdup(field));
			ptr = field;
			*ptr = 0;
		}
		else if (!quotes && ( c == '\n' || c == '\r' )) {
			*ptr++ = 0;
			ham_list_insert_last(line, strdup(field));
			ptr = field;
			*ptr = 0;

			ham_list_insert_last(tokens, line);
			line = ham_list_new();
			ham_list_set_free_func(line, free);
		}
		else {
			*ptr++ = c;
		}
		pos++;
	}

	if (strlen(field) != 0) {
		ham_list_insert_last(line, strdup(field));
		ham_list_insert_last(tokens, line);
	}
	else {
		ham_list_destroy(line);
	}

	return tokens;
}
