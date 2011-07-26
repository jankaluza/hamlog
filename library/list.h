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

HAMList *ham_list_new();
void ham_list_destroy(HAMList *list);

void ham_list_insert_first(HAMList *list, void *data);
void ham_list_insert_last(HAMList *list, void *data);

void ham_list_remove(HAMList *list, void *data);

void *ham_list_get_first(HAMList *list);
void *ham_list_pop_first(HAMList *list);
void *ham_list_get_last(HAMList *list);


#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
