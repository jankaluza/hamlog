/**
 * @file connection.h Connection API
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

#ifndef _HAMLOG_SIGNALS_H
#define _HAMLOG_SIGNALS_H

#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"
#include "hashtable.h"
#include "connection.h"

#ifdef __cplusplus
extern "C" {
#endif

void ham_signals_register_signal(const char *signal_name);

void ham_signals_emit_signal(const char *signal_name, HAMConnection *connection, const char *data, int error);

void ham_signals_register_handler(const char *signal_name, HAMFetchHandler handler, void *ui_data);

void ham_signals_unregister_handler(const char *signal_name, HAMFetchHandler handler);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
