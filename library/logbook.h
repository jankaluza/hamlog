/**
 * @file logbook.h LogBook API
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

#ifndef _HAMLOG_LOGBOOK_H
#define _HAMLOG_LOGBOOK_H

#include "connection.h"
#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

/**
 * LogBook related UI callbacks.
 */
typedef struct _HAMLogBookUICallbacks {
	/** Called when logbook is succesfully fetched from HAMLog server
	 */
	void (*fetched) (HAMConnection *connection, const char *logbook);

	void (*updated) (HAMConnection *connection);
	void (*update_failed) (HAMConnection *connection, const char *reason);

	void (*removed) (HAMConnection *connection);
	void (*remove_failed) (HAMConnection *connection, const char *reason);
} HAMLogBookUICallbacks;

void ham_logbook_set_ui_callbacks(HAMLogBookUICallbacks *callbacks);

void ham_logbook_fetch(HAMConnection *connection);

void ham_logbook_add(HAMConnection *connection, const char *data);
void ham_logbook_remove(HAMConnection *connection, const char *data);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
