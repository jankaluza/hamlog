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

#include "../hashtable.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	HAMHashTable *table = ham_hash_table_new();

	printf("add 'ahoj'=1\n");
	ham_hash_table_add(table, "ahoj", -1, (void *) 1);
	
	printf("    lookup 'ahoj1': %s\n", (char *) ham_hash_table_lookup(table, "ahoj1", -1));
	printf("    lookup 'ahoj': %d\n", (int) ham_hash_table_lookup(table, "ahoj", -1));

	printf("get_keys:\n");
	void **keys = NULL;
	unsigned long count = ham_hash_table_get_keys(table, &keys);
	unsigned long i;
	for (i = 0; i < count; i++) {
		printf("   %s\n", (char *) keys[i]);
	}
	free(keys);

	printf("remove 'ahoj'\n");
	ham_hash_table_remove(table, "ahoj", -1);
	printf("    lookup 'ahoj': %s\n", (char *) ham_hash_table_lookup(table, "ahoj", -1));

	ham_hash_table_set_free_func(table, free);
	ham_hash_table_add(table, "ahoj", -1, (void *) strdup("this must be freed"));
	
	ham_hash_table_destroy(table);
	return 0;
}
