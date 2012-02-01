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

#ifndef _HAMLOG_DXCC_H
#define _HAMLOG_DXCC_H

#include "connection.h"
#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file dxcc.h DXCC API
 * @ingroup core
 * @details DXCC API provides a way to get DXCC related 
 * informations about operator
 */

/**
 * Signal emitted when DXCC data have been fetched.
 * Data passed by this signal are in CSV format where the first row is header.
 * Read \ref csv "Hamlog CSV description" for more information.
 * If the error is true, data passed to handler contains error description.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_dxcc_fetched "dxcc-fetched"

/**
 * Fetches DXCC information about particular operator. Emits ham_signal_dxcc_fetched when done.
 * @note If you want to get all information about particular operator (not just DXCC), use 
 * ham_callinfo_fetch() instead.
 * @param connection Connection associated with the request.
 * @param call Call sign.
 * @param handler Handler called when the data is fetched, or NULL.
 * For the CSV data passed to the handler, check the \ref ham_signal_dxcc_fetched signal description.
 * @param ui_data Pointer to data passed to handler or NULL.
 * @see ham_signal_dxcc_fetched
 */
void ham_dxcc_fetch(HAMConnection *connection, const char *call, HAMFetchHandler handler, void *ui_data);

void ham_dxcc_register_signals();

#ifdef __cplusplus
}
#endif

#endif
