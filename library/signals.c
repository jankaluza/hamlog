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

#include "connection.h"
#include "signals.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "list.h"
#include "hashtable.h"

static HAMHashTable *signals;

static void free_signals() {
	ham_hash_table_destroy(signals);
}

typedef struct _pair {
	HAMFetchHandler handler;
	void *ui_data;
} pair;

void ham_signals_register_signal(const char *signal_name) {
	if (!signals) {
		signals = ham_hash_table_new();
		ham_hash_table_set_free_func(signals, (HAMListItemDataFree) ham_list_destroy);
		atexit(free_signals);
	}

	if (ham_hash_table_lookup(signals, signal_name, -1) == NULL) {
		HAMList *handlers = ham_list_new();
		ham_list_set_free_func(handlers, free);
		ham_hash_table_add(signals, signal_name, -1, (void *) handlers);
	}
}

void ham_signals_emit_signal(const char *signal_name, HAMConnection *connection, const char *data, int error) {
	HAMList *handlers = (HAMList *) ham_hash_table_lookup(signals, signal_name, -1);
	if (!handlers) {
		return;
	}

	HAMListItem *item = ham_list_get_first_item(handlers);
	while (item) {
		pair *p = ham_list_item_get_data(item);
		p->handler(connection, data, error, p->ui_data);
		item = ham_list_get_next_item(item);
	}	
}

void ham_signals_register_handler(const char *signal_name, HAMFetchHandler handler, void *ui_data) {
	HAMList *handlers = (HAMList *) ham_hash_table_lookup(signals, signal_name, -1);
	if (!handlers) {
		return;
	}

	pair *data = malloc(sizeof(pair));
	data->handler = handler;
	data->ui_data = ui_data;

	ham_list_insert_last(handlers, data);
}

void ham_signals_unregister_handler(const char *signal_name, HAMFetchHandler handler, void *ui_data) {
	HAMList *handlers = (HAMList *) ham_hash_table_lookup(signals, signal_name, -1);
	if (!handlers) {
		return;
	}

	HAMListItem *item = ham_list_get_first_item(handlers);
	while (item) {
		pair *p = ham_list_item_get_data(item);
		if (p->handler == handler && p->ui_data == ui_data) {
			ham_list_remove(handlers, p);
			free(p);
			return;
		}
	}	
}
