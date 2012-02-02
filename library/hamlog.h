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

#ifndef _HAMLOG_HAMLOG_H
#define _HAMLOG_HAMLOG_H

#include "account.h"
#include "callinfo.h"
#include "connection.h"
#include "dxcc.h"
#include "dxcluster.h"
#include "eventloop.h"
#include "hashtable.h"
#include "list.h"
#include "logbook.h"
#include "md5.h"
#include "parser.h"
#include "qrz.h"
#include "reply.h"
#include "request.h"
#include "signals.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup signals Signals    
 * List of all signals emitted by Hamlog. For the information how to use signals
 * read signals.h documentation.
 */

void ham_initialize();

void ham_initialize() {
	ham_account_register_signals();
	ham_callinfo_register_signals();
	ham_connection_register_signals();
	ham_dxcc_register_signals();
	ham_dxcluster_register_signals();
	ham_connection_register_signals();
	ham_qrz_register_signals();
	ham_logbook_register_signals();
}

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
