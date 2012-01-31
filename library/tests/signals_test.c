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

#include "../signals.h"
#include "../connection.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>

static void signal_handler(HAMConnection *connection, const char *data, int error, void *ui_data) {
	printf("%p == 0x6\n", connection);
	printf("%s == ahoj\n", data);
	printf("%d == 1\n", error);
	printf("%p == 0x5\n", ui_data);
}

int main(int argc, char **argv) {
	ham_signals_register_signal("test-signal");

	ham_signals_register_handler("test-signal", signal_handler, (void *) 5);

	ham_signals_emit_signal("test-signal", (void *) 6, "ahoj", 1);

	ham_signals_unregister_handler("test-signal", signal_handler);

	ham_signals_emit_signal("test-signal", (void *) 6, "ahoj", 1);

	return 0;
}
