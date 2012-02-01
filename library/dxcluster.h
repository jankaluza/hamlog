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

#ifndef _HAMLOG_DXCLUSTER_H
#define _HAMLOG_DXCLUSTER_H

#include "connection.h"
#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file dxcluster.h DXCluster API
 * @ingroup core
 * @details DXCluster API provides a way to get information
 * from DXCluster cluster.
 */

/**
 * Signal emitted when DXCluster data have been fetched.
 * Data passed by this signal are in CSV format where the first row is header.
 * Read \ref csv "Hamlog CSV description" for more information.
 * If the error is true, data passed to handler contains error description.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_dxcluster_fetched "dxcluster-fetched"

/**
 * Fetches new data from DXCluster if any. Emits \ref ham_signal_dxcluster_fetched when done.
 * You should call this method repeateadly (probably by timer) to get new data.
 * @param connection Connection associated with the request.
 * @param handler Handler called when the data is fetched, or NULL.
 * For the CSV data passed to the handler, check the \ref ham_signal_dxcluster_fetched signal description.
 * @param ui_data Pointer to data passed to handler or NULL.
 * @see ham_signal_dxcluster_fetched
 */
void ham_dxcluster_fetch(HAMConnection *connection, HAMFetchHandler handler, void *ui_data);

void ham_dxcluster_register_signals();

#ifdef __cplusplus
}
#endif

#endif
