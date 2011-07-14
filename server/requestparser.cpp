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

#include "requestparser.h"
#include <boost/bind.hpp>
#include <iostream>

namespace HamLog {
	
RequestParser::RequestParser()
	: m_state(method_start) {
	
}

void RequestParser::reset() {
	m_state = method_start;
}

bool RequestParser::handleChar(Request::ref req, char input) {
	switch (m_state) {
		case method_start:
			if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return false;
			}
			else {
				m_state = method;
				req->m_method.push_back(input);
				return true;;
			}
		case method:
			if (input == ' ') {
				m_state = uri;
				return true;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return false;
			}
			else {
				req->m_method.push_back(input);
				return true;
			}
		case uri:
			if (input == ' ') {
				m_state = http_version_h;
				return true;
			}
			else if (isControl(input)) {
				return false;
			}
			else {
				req->m_uri.push_back(input);
				return true;
			}
		case http_version_h:
			if (input == 'H') {
				m_state = http_version_t_1;
				return true;
			}
			else {
				return false;
			}
		case http_version_t_1:
			if (input == 'T') {
				m_state = http_version_t_2;
				return true;
			}
			else {
				return false;
			}
		case http_version_t_2:
			if (input == 'T') {
				m_state = http_version_p;
				return true;
			}
			else {
				return false;
			}
		case http_version_p:
			if (input == 'P') {
				m_state = http_version_slash;
				return true;
			}
			else {
				return false;
			}
		case http_version_slash:
			if (input == '/') {
				req->m_majorVersion = 0;
				req->m_minorVersion = 0;
				m_state = http_version_major_start;
				return true;
			}
			else {
				return false;
			}
		case http_version_major_start:
			if (isDigit(input)) {
 				req->m_majorVersion = req->m_majorVersion * 10 + input - '0';
				m_state = http_version_major;
				return true;
			}
			else {
				return false;
			}
		case http_version_major:
			if (input == '.') {
				m_state = http_version_minor_start;
				return true;
			}
			else if (isDigit(input)) {
 				req->m_majorVersion = req->m_majorVersion * 10 + input - '0';
				return true;
			}
			else {
				return false;
			}
		case http_version_minor_start:
			if (isDigit(input)) {
 				req->m_minorVersion = req->m_minorVersion * 10 + input - '0';
				m_state = http_version_minor;
				return true;
			}
			else {
				return false;
			}
		case http_version_minor:
			if (input == '\r') {
				m_state = expecting_newline_1;
				return true;
			}
			else if (isDigit(input)) {
 				req->m_minorVersion = req->m_minorVersion * 10 + input - '0';
				return true;
			}
			else {
				return false;
			}
		case expecting_newline_1:
			if (input == '\n') {
				m_state = header_line_start;
				return true;
			}
			else {
				return false;
			}
		case header_line_start:
			if (input == '\r') {
				m_state = expecting_newline_3;
				return true;
			}
			else if (!req->m_headers.empty() && (input == ' ' || input == '\t')) {
				m_state = header_lws;
				return true;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return false;
			}
			else {
				Request::Header header;
				header.name.push_back(input);
 				req->m_headers.push_back(header);
				m_state = header_name;
				return true;
			}
		case header_lws:
			if (input == '\r') {
				m_state = expecting_newline_2;
				return true;
			}
			else if (input == ' ' || input == '\t') {
				return true;
			}
			else if (isControl(input)) {
				return false;
			}
			else {
				m_state = header_value;
				req->m_headers.back().value.push_back(input);
				return true;
			}
		case header_name:
			if (input == ':') {
				m_state = space_before_header_value;
				return true;
			}
			else if (!isChar(input) || isControl(input) || isSpecial(input)) {
				return false;
			}
			else {
 				req->m_headers.back().name.push_back(input);
				return true;
			}
		case space_before_header_value:
			if (input == ' ') {
				m_state = header_value;
				return true;
			}
			else {
				return false;
			}
		case header_value:
			if (input == '\r') {
				m_state = expecting_newline_2;
				return true;
			}
			else if (isControl(input)) {
				return false;
			}
			else {
 				req->m_headers.back().value.push_back(input);
				return true;
			}
		case expecting_newline_2:
			if (input == '\n') {
				m_state = header_line_start;
				return true;
			}
			else {
				return false;
			}
		case expecting_newline_3:
			if (input == '\n') {
				req->m_finished = true;
				return true;
			}
			return false;
		default:
			return false;
	}
}

bool RequestParser::isChar(int c) {
	return c >= 0 && c <= 127;
}

bool RequestParser::isControl(int c) {
	return (c >= 0 && c <= 31) || c == 127;
}

bool RequestParser::isSpecial(int c) {
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
			return true;
		default:
			return false;
	}
}

bool RequestParser::isDigit(int c) {
	return c >= '0' && c <= '9';
}

}
