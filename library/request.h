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

#ifndef _HAMLOG_REQUEST_H
#define _HAMLOG_REQUEST_H

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

#define MAX_REPLY_SIZE 8192

typedef struct _RequestHeader {
	char *name;
	char *value;
} HAMRequestHeader;

typedef struct _Request {
	char *method;
	char *uri;
	char *content;
	char *content_type;
	HAMRequestHeader **headers;
	unsigned int headers_count;
} HAMRequest;


HAMRequest *ham_request_new(const char *uri, const char *method, const char *content, const char *content_type);
void ham_request_dump(HAMRequest *request);
void ham_request_destroy(HAMRequest *request);
char *ham_request_get_data(HAMRequest *request);

void ham_request_add_header(HAMRequest *request, const char *name, const char *value);


#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
