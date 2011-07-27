/**
 * @file md5.h Reply API
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

#ifndef _HAMLOG_REPLY_H
#define _HAMLOG_REPLY_H

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

#define MAX_REPLY_SIZE 8192

typedef struct _Header {
	char *name;
	char *value;
} HAMReplyHeader;

typedef struct _Reply {
	unsigned int status;
	char *content;
	HAMReplyHeader **headers;
	unsigned int headers_count;
	int finished;
} HAMReply;

HAMReplyHeader *ham_reply_header_new(const char *name, const char *value);
void ham_reply_header_destroy(HAMReplyHeader *header);

const char *ham_reply_header_get_name(HAMReplyHeader *header);
const char *ham_reply_header_get_value(HAMReplyHeader *header);

HAMReply *ham_reply_new();
void ham_reply_dump(HAMReply *reply);
void ham_reply_destroy(HAMReply *reply);

int ham_reply_is_finished(HAMReply *reply);

void ham_reply_add_header(HAMReply *reply, HAMReplyHeader *header);
int ham_reply_get_status(HAMReply *reply);
const char *ham_reply_get_content(HAMReply *reply);
const char *ham_reply_get_header(HAMReply *reply, const char *name);


#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
