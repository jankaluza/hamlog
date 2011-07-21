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

#include "../list.h"
#include "string.h"
#include "stdio.h"

int main(int argc, char **argv) {
	HAMList *list = ham_list_new();
	char *ahoj = strdup("ahoj");
	char *ahoj_first = strdup("ahoj_first");
	char *ahoj_last = strdup("ahoj_last");

	printf("insert_first 'ahoj'\n");
	ham_list_insert_first(list, ahoj);
	printf("    first element: %s\n", (char *) ham_list_get_first(list));
	printf("    last  element: %s\n", (char *) ham_list_get_last(list));

	printf("insert_first 'ahoj_first'\n");
	ham_list_insert_first(list, ahoj_first);
	printf("first element: %s\n", (char *) ham_list_get_first(list));
	printf("last  element: %s\n", (char *) ham_list_get_last(list));

	printf("insert_last 'ahoj_last'\n");
	ham_list_insert_last(list, ahoj_last);
	printf("first element: %s\n", (char *) ham_list_get_first(list));
	printf("last  element: %s\n", (char *) ham_list_get_last(list));

	printf("remove 'ahoj_last'\n");
	ham_list_remove(list, ahoj_last);
	printf("first element: %s\n", (char *) ham_list_get_first(list));
	printf("last  element: %s\n", (char *) ham_list_get_last(list));

	printf("remove 'ahoj_first'\n");
	ham_list_remove(list, ahoj_first);
	printf("first element: %s\n", (char *) ham_list_get_first(list));
	printf("last  element: %s\n", (char *) ham_list_get_last(list));

	printf("remove 'ahoj'\n");
	ham_list_remove(list, ahoj);
	printf("first element: %p\n", ham_list_get_first(list));
	printf("last  element: %p\n", ham_list_get_last(list));
	
	ham_list_destroy(list);
	return 0;
}
