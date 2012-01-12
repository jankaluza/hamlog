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

#ifndef _HAMLOG_QRZ_H
#define _HAMLOG_QRZ_H

#include "connection.h"
#include "parser.h"
#include "reply.h"
#include "request.h"
#include "list.h"

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

/**
 * QRZ related UI callbacks.
 */
typedef struct _HAMQRZUICallbacks {
	/** Called when QRZ data is succesfully fetched from HAMLog server
	 */
	void (*fetched) (HAMConnection *connection, const char *call, const char *data);

	/** Called when user associated with this connection has been
	 *  succesfully registered.
	 */
	void (*registered) (HAMConnection *connection);

	/** Called when registration failed for some reason.
	 */
	void (*registration_failed) (HAMConnection *connection, const char *reason);
} HAMQRZUICallbacks;

void ham_qrz_set_ui_callbacks(HAMQRZUICallbacks *callbacks);

void ham_qrz_fetch(HAMConnection *connection, const char *call);

void ham_qrz_register(HAMConnection *connection, const char *username, const char *password);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
