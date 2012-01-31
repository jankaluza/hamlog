/**
 * @file hashtable.h Hash table API
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

#ifndef _HAMLOG_HASHTABLE_H
#define _HAMLOG_HASHTABLE_H

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAM_HASH_LEN 256

typedef void (*HAMHashTableItemDataFree) (void *data);

typedef struct _HAMHashTableItem {
	const void *key;
	void *data;
	unsigned long key_len;
	struct _HAMHashTableItem *next;
} HAMHashTableItem;

typedef struct _HAMHashTable {
	HAMHashTableItem *items[HAM_HASH_LEN];
	unsigned long count;
	HAMHashTableItemDataFree free_func;
} HAMHashTable;

/**
 * Creates new empy Hash table item.
 * @return Empty Hash table item which has to be destroyed by ham_hash_table_item_destroy() method.
 */
HAMHashTableItem *ham_hash_table_item_new();

/**
 * Destroys the Hash table item and also frees the data if the hash table has set Free function.
 * @param table Hash Table.
 * @param item Hash Table Item to be freed.
 */
void ham_hash_table_item_destroy(HAMHashTable *table, HAMHashTableItem *item);

/**
 * Creates new empy Hash table.
 * @return Empty Hash table which has to be destroyed by ham_hash_table_destroy() method.
 */
HAMHashTable *ham_hash_table_new();

/**
 * Destroys the Hash table and also frees the data if there is Free function set.
 * @param table Hash Table.
 */
void ham_hash_table_destroy(HAMHashTable *table);

/**
 * Sets the function called for every item in Hash Table to free its data. The data
 * are not freed when the Free function is not set.
 * @param table Hash Table.
 * @param func Function to be called for every data stored in Hash table
 * when the Hash Table Item is removed.
 */
void ham_hash_table_set_free_func(HAMHashTable *table, HAMHashTableItemDataFree func);

/**
 * Adds or replaces item in hash table.
 * @param table Hash table.
 * @param key Key.
 * @param key_len Key length or -1 when the key is string.
 * @param value Value.
 * @return -1 on malloc error.
 */
int ham_hash_table_add(HAMHashTable *table, const void *key, long key_len, void *value);

/**
 * Removes item from hash table.
 * @param table Hash table.
 * @param key Key.
 * @param key_len Key length or -1 when the key is string.
 * @return -1 when item is not in hash table.
 */
int ham_hash_table_remove(HAMHashTable *table, const void *key, long key_len);

/**
 * Finds the item with particular key in Hash Table and returns pointer to its data.
 * @param table Hash table.
 * @param key Key.
 * @param key_len Key length or -1 when the key is string.
 * @return Pointer to the data associated with the key.
 */
void *ham_hash_table_lookup(HAMHashTable *table, const void *key, long key_len);

/**
 * Returns the number of items stored in hash table.
 * @param table Hash table.
 * @return Number of items stored in the hash table.
 */
unsigned long ham_hash_table_get_size(HAMHashTable *table);

/**
 * Stores all keys in array and returns the number of keys.
 * @param table Hash table.
 * @param keys pointer to array where the keys are stored. Array is allocated by this function
 * and has to be freed later.
 * @return Number of keys stored in the hash table.
 */
int ham_hash_table_get_keys(HAMHashTable *table, void **keys[]);

/**
 * Returns the list of HAMHashTableItems.
 * @param table Hash table.
 * @return List of HAMHashTableItems. This list has to be freed later.
 */
HAMList *ham_hash_table_to_list(HAMHashTable *table);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
