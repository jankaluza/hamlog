/**
 * @file list.h List API
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

#ifndef _HAMLOG_LIST_H
#define _HAMLOG_LIST_H

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

typedef struct _HAMListItem {
	void *data;
	struct _HAMListItem *lptr;
	struct _HAMListItem *rptr;
} HAMListItem;

typedef struct _HAMList {
	HAMListItem *first;
	HAMListItem *last;
} HAMList;

/**
 * Creates new empy list.
 * @return Empty list which has to be destroyed by ham_list_destroy() method.
 */
HAMList *ham_list_new();

/**
 * Destroys the list and also frees the data.
 * @param list List.
 */
void ham_list_destroy(HAMList *list);

/**
 * Inserts new data as first element in list.
 * @param list List.
 * @param data Data.
 */
void ham_list_insert_first(HAMList *list, void *data);

/**
 * Inserts new data as last element in list.
 * @param list List.
 * @param data Data.
 */
void ham_list_insert_last(HAMList *list, void *data);

/**
 * Removes data, but does not free it.
 * @param list List.
 * @param data Data.
 */
void ham_list_remove(HAMList *list, void *data);

/**
 * Returns data of first element in list.
 * @param list List.
 * @return Data.
 */
void *ham_list_get_first(HAMList *list);

/**
 * Returns data of free element in list and removes it from list.
 * @param list List.
 * @return Data.
 */
void *ham_list_pop_first(HAMList *list);

/**
 * Returns data of last element in list.
 * @param list List.
 * @return Data.
 */
void *ham_list_get_last(HAMList *list);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
