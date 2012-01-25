/**
 * @file callinfo.h CALLINFO module asker
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

#ifndef _HAMLOG_CALLINFO_H
#define _HAMLOG_CALLINFO_H

#include "connection.h"
#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

typedef void (*HAMCallInfoHandler) (HAMConnection *connection, const char *callinfo_data, int error, void *ui_data);

typedef struct _HAMCallInfoUICallbacks {
	/** Called when Call info data is succesfully fetched from HAMLog server
	 */
	void (*fetched) (HAMConnection *connection, const char *call, const char *data);
} HAMCallInfoUICallbacks;

void ham_callinfo_set_ui_callbacks(HAMCallInfoUICallbacks *callbacks);

void ham_callinfo_fetch(HAMConnection *connection, const char *call, HAMCallInfoHandler handler, void *ui_data);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
