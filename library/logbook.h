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
 * @file logbook.h Logbook API
 * @ingroup core
 * @details Logbook API provides basic way how maintain the logbook.
 * It allows to fetch the logbook and add or remove items from it.
 * 
 */

/**
 * Signal emitted when records are fetched.
 * Data passed by this signal are in CSV format where the first row is header.
 * Read \ref csv "Hamlog CSV description" for more information.
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_logbook_fetched "logbook-fetched"

/**
 * Signal emitted when the record is added/updated.
 * If there is no error, data passed by this signal are in CSV format and contains "id" and "user_id"
 * column associated to added/updated record.
 * 
 * It is useful to handle this signal to get the ID of the newly created record.
 * 
 * On error, error description is passed as a data.
 * 
 * @ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_logbook_added "logbook-added"

/**
 * Signal emitted when the record is removed.
 * \ingroup signals
 * @see ham_signals_register_handler()
 */
#define ham_signal_logbook_removed "logbook-removed"


/**
 * Fetches the logbook from the server.
 * @param connection Connection associated with the request.
 * @param handler Handler called when the data is fetched, or NULL.
 * For the CSV data passed to the handler, check the \ref ham_signal_logbook_fetched signal description.
 * @param ui_data Pointer to data passed to handler or NULL.
 * @see ham_logbook_fetch_with_call()
 * @see ham_signal_logbook_fetched
 */
void ham_logbook_fetch(HAMConnection *connection, HAMFetchHandler handler, void *ui_data);

/**
 * Fetches only the records with particular Call from the server.
 * @param connection Connection associated with the request.
 * @param call Call name to be used as filter.
 * @param handler Handler called when the data is fetched, or NULL.
 * For the CSV data passed to the handler, check the \ref ham_signal_logbook_fetched signal description.
 * @param ui_data Pointer to data passed to handler or NULL.
 * @see ham_logbook_fetch()
 * @see ham_signal_logbook_fetched
 */
void ham_logbook_fetch_with_call(HAMConnection *connection, const char *call, HAMFetchHandler handler, void *ui_data);

/**
 * Adds or updates the record.
 * @param connection Connection associated with the request.
 * @param data CSV data representing the record.
 * Read \ref csv "Hamlog CSV description" for more information. "id" column has to be always included.
 * @param handler Handler called when the record is added/updated, or NULL.
 * For the CSV data passed to the handler, check the \ref ham_signal_logbook_added signal description.
 * @param ui_data Pointer to data passed to handler or NULL.
 * @see ham_logbook_fetch()
 * @see ham_signal_logbook_added
 */
void ham_logbook_add(HAMConnection *connection, const char *data, HAMFetchHandler handler, void *ui_data);

/**
 * Removes the record.
 * @param connection Connection associated with the request.
 * @param data CSV data representing the record. Only "id" column is needed.
 * Read \ref csv "Hamlog CSV description" for more information.
 * @param handler Handler called when the record is removed, or NULL.
 * @param ui_data Pointer to data passed to handler or NULL.
 * @see ham_logbook_fetch()
 * @see ham_signal_logbook_removed
 */
void ham_logbook_remove(HAMConnection *connection, const char *data, HAMFetchHandler handler, void *ui_data);

void ham_logbook_register_signals();

#ifdef __cplusplus
}
#endif

#endif
