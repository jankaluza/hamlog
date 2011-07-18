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

#ifndef _HAMLOG_PARSER_H
#define _HAMLOG_PARSER_H

typedef struct _Header {
	char *name;
	char *value;
} HAMReplyHeader;

typedef struct _Reply {
	unsigned int status;
	char *content;
	HAMReplyHeader **headers;
	int finished;
} HAMReply;

// TODO: better namespace
typedef enum _HAMParserState {
	method_start,
	method,
	uri_start,
	uri,
	http_version_h,
	http_version_t_1,
	http_version_t_2,
	http_version_p,
	http_version_slash,
	http_version_major_start,
	http_version_major,
	http_version_minor_start,
	http_version_minor,
	expecting_newline_1,
	header_line_start,
	header_lws,
	header_name,
	space_before_header_value,
	header_value,
	expecting_newline_2,
	expecting_newline_3
} HAMParserState;

typedef struct _HAMParser {
	HAMParserState state;
} HAMParser;

HAMParser *ham_parser_new();
int ham_parser_parse(HAMParser *parser, HAMReply *response, const char *data, unsigned long size);
void ham_parser_reset(HAMParser *parser);
void ham_parser_destroy(HAMParser *parser);

#endif
