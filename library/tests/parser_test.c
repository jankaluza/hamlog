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

#include "../parser.h"
#include "../reply.h"
#include "string.h"
#include "stdio.h"

int main(int argc, char **argv) {
	const char req1[] = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 116\r\n"
						"\r\n"
						"<html><head></head><body>This is HamLog server. Download HamLog and setup your username to connect it.</body></html>\r\n";
	const char req2[] = "HTTP/1.1 440 NOT FOUND\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 116\r\n"
						"\r\n"
						"<html><head></head><body>This is HamLog server. Download HamLog and setup your username to connect it.</body></html>\r\n";
	int ret;

	HAMParser *parser = ham_parser_new();
	HAMReply *reply = ham_reply_new();

	ret = ham_parser_parse(parser, reply, req1, strlen(req1));
	printf("parser returned %d\n", ret);
	ham_reply_dump(reply);

	ret = ham_parser_parse(parser, reply, req2, strlen(req2));
	printf("parser returned %d\n", ret);
	ham_reply_dump(reply);

	ham_reply_destroy(reply);
	ham_parser_destroy(parser);
	return 0;
}
