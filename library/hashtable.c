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

#include "hashtable.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

static unsigned long ham_hash_table_do_hash(void *key, long key_len) {
	char c;
	char *ptr = (char *) key;
	unsigned long hash = 0;

	if (key_len == -1) {
		key_len = strlen(key);
	}

	// sdbm hashing algorithm (used also in gawk)
	while ((c = *ptr++) != 0) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
    return hash % HAM_HASH_LEN;
}

HAMHashTableItem *ham_hash_table_item_new() {
	HAMHashTableItem *item = calloc(1, sizeof(HAMHashTableItem));
	return item;
}

void ham_hash_table_item_destroy(HAMHashTable *table, HAMHashTableItem *item) {
	if (table->free_func) {
		table->free_func(item->data);
	}
	free(item->key);
	free(item);
}

HAMHashTable *ham_hash_table_new() {
	HAMHashTable *table = calloc(1, sizeof(HAMHashTable));
	return table;
}

void ham_hash_table_destroy(HAMHashTable *table) {
	unsigned long i;
	for (i = 0; i < HAM_HASH_LEN; i++) {
		while (table->items[i]) {
			HAMHashTableItem *tmp = table->items[i];
			table->items[i] = table->items[i]->next;
			ham_hash_table_item_destroy(table, tmp);
		}
	}
	free(table);
}

void ham_hash_table_set_free_func(HAMHashTable *table, HAMHashTableItemDataFree func) {
	table->free_func = func;
}

int ham_hash_table_add(HAMHashTable *table, void *key, long key_len, void *value) {
	if (key_len == -1) {
		key_len = strlen(key);
	}
	unsigned long hash = ham_hash_table_do_hash(key, key_len);

	HAMHashTableItem *item = ham_hash_table_item_new();
	if (!item) {
		return -1;
	}

	item->key = malloc(key_len);
	if (!item->key) {
		free(item);
		return -1;
	}

	memcpy(item->key, key, key_len);
	item->data = value;
	item->key_len = key_len;
    item->next = NULL;

	if (!table->items[hash]) {
		table->items[hash] = item;
		table->count++;
	}
	else {
		HAMHashTableItem *tmp = table->items[hash];
		while(tmp->next) {
			while(tmp->next && tmp->next->key_len != key_len) {
				tmp = tmp->next;
			}

			if(tmp->next) {
				if (!memcmp(tmp->next->key, key, key_len)) {
					// replace
					HAMHashTableItem *to_delete = tmp->next;
					tmp->next = item;
					item->next = to_delete->next;
					ham_hash_table_item_destroy(table, to_delete);
					return 0;
				}
				else
				{
					tmp = tmp->next;
				}
			}
		}
		tmp->next = item;
		table->count++;
	}
	return 0;
}

int ham_hash_table_remove(HAMHashTable *table, void *key, long key_len) {
	if (key_len == -1) {
		key_len = strlen(key);
	}
	unsigned long hash = ham_hash_table_do_hash(key, key_len);

	if (!table->items[hash]) {
		return -1;
	}
	HAMHashTableItem *tmp = table->items[hash];
	HAMHashTableItem *prev = tmp;
	while(tmp) {
		while(tmp && tmp->key_len != key_len) {
			prev = tmp;
			tmp = tmp->next;
		}

		if(tmp) {
			if (!memcmp(tmp->key, key, key_len)) {
				// previous item is the first record in items list
				if (prev == table->items[hash]) {
					table->items[hash] = tmp->next;
				}
				else {
					prev->next = tmp->next;
				}
				ham_hash_table_item_destroy(table, tmp);
				table->count--;
				return 0;
			}
			tmp=tmp->next;
		}
	}
	return -1;
}

void *ham_hash_table_lookup(HAMHashTable *table, void *key, long key_len) {
	if (key_len == -1) {
		key_len = strlen(key);
	}
	unsigned long hash = ham_hash_table_do_hash(key, key_len);

	if (!table->items[hash]) {
		return NULL;
	}
	
	HAMHashTableItem *tmp = table->items[hash];
	while(tmp) {
		while(tmp && tmp->key_len != key_len) {
			tmp = tmp->next;
		}

		if(tmp) {
			if (!memcmp(tmp->key, key, key_len)) {
				return tmp->data;
			}
			else {
				tmp = tmp->next;
			}
		}
	}
	return NULL;
}
