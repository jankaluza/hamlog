/**
 * @file parser.h HTTP Parser API
 * @ingroup core
 */

/*
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

#include "reply.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: better namespace
typedef enum _HAMParserState {
	http_version_h,
	http_version_t_1,
	http_version_t_2,
	http_version_p,
	http_version_slash,
	http_version_major_start,
	http_version_major,
	http_version_minor_start,
	http_version_minor,
	status_start,
	status_1,
	status_2,
	status_text,
	content_start,
	content,
	expecting_newline_1,
	header_line_start,
	header_lws,
	header_name,
	space_before_header_value,
	header_value,
	expecting_newline_2,
	expecting_newline_3,
	expecting_newline_4,
} HAMParserState;

typedef struct _HAMParser {
	HAMParserState state;
	char header_name[512];
	char header_value[512];
	char *ptr;
} HAMParser;

/**
 * Creates new HTTP parser.
 * @return HTTP parser. Use ham_parser_destroy to destroy it.
 */
HAMParser *ham_parser_new();

/**
 * Destroyes HTTP parser.
 * @param parser HTTP parser.
 */
void ham_parser_destroy(HAMParser *parser);

/**
 * Parses data. Data are stored into reply and once whole reply has been parsed,
 * ham_reply_is_finished(reply) returns 1.
 * @param parser HTTP parser.
 * @param reply Reply which contains parsed data.
 * @param data Data received from server.
 * @param size Size of received data.
 * @return size
 */
unsigned long ham_parser_parse(HAMParser *parser, HAMReply *reply, const char *data, unsigned long size);

/**
 * Resets the parser and starts with empty data.
 * @param parser HTTP parser.
 */
void ham_parser_reset(HAMParser *parser);

HAMList *ham_csv_parse(const char *str);

char *ham_csv_from_list(HAMList *list);

char *ham_csv_merge(const char *first, const char *second);



#ifdef __cplusplus
}
#endif

#endif
