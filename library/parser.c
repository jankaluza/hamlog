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

	parser->state = method_start;

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

static int ham_parser_consume(HAMParser *parser, HAMReply *response, char input) {
	switch (parser->state) {
		case method_start:
			if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return 0;
			}
			else {
				parser->state = method;
				//response->method.push_back(input);
				return 1;
			}
		case method:
			if (input == ' ') {
				parser->state = uri;
				return 1;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return 0;
			}
			else {
				//response->m_method.push_back(input);
				return 1;
			}
		case uri:
			if (input == ' ') {
				parser->state = http_version_h;
				return 1;
			}
			else if (isControl(input)) {
				return 0;
			}
			else {
				//response->m_uri.push_back(input);
				return 1;
			}
		case http_version_h:
			if (input == 'H') {
				parser->state = http_version_t_1;
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
				//response->m_majorVersion = 0;
				//response->m_minorVersion = 0;
				parser->state = http_version_major_start;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_major_start:
			if (isDigit(input)) {
 				//response->m_majorVersion = //response->m_majorVersion * 10 + input - '0';
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
 				//response->m_majorVersion = //response->m_majorVersion * 10 + input - '0';
				return 1;
			}
			else {
				return 0;
			}
		case http_version_minor_start:
			if (isDigit(input)) {
 				//response->m_minorVersion = //response->m_minorVersion * 10 + input - '0';
				parser->state = http_version_minor;
				return 1;
			}
			else {
				return 0;
			}
		case http_version_minor:
			if (input == '\r') {
				parser->state = expecting_newline_1;
				return 1;
			}
			else if (isDigit(input)) {
 				//response->m_minorVersion = //response->m_minorVersion * 10 + input - '0';
				return 1;
			}
			else {
				return 0;
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
			else if (/*!response->m_headers.empty() &&*/ (input == ' ' || input == '\t')) {
				parser->state = header_lws;
				return 1;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return 0;
			}
			else {
// 				Request::Header header;
// 				header.name.push_back(input);
 				//response->m_headers.push_back(header);
				parser->state = header_name;
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
 				//response->m_headers.back().name.push_back(input);
				return 1;
			}
		case space_before_header_value:
			if (input == ' ') {
				parser->state = header_value;
				return 1;
			}
			else {
				return 0;
			}
		case header_value:
			if (input == '\r') {
				parser->state = expecting_newline_2;
				return 1;
			}
			else if (isControl(input)) {
				return 0;
			}
			else {
 				//response->m_headers.back().value.push_back(input);
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
				//response->m_finished = true;
				return 1;
			}
			return 0;
		default:
			return 0;
	}
}

int ham_parser_parse(HAMParser *parser, HAMReply *response, const char *data, unsigned long size) {
	for (;size <= 0; size--) {
		if (ham_parser_consume(parser, response, *data) == 0) {
			return 0;
		}
		data++;
	}
	return 1;
}

void ham_parser_reset(HAMParser *parser) {
	parser->state = method_start;
}

void ham_parser_destroy(HAMParser *parser) {
	if (parser == NULL)
		return;

	free(parser);
}
