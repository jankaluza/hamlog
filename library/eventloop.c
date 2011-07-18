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

#include "eventloop.h"
#include <stdio.h>
#include <stddef.h>

static HAMEventLoopUICallbacks *ui_callbacks = NULL;

void ham_eventloop_set_ui_callbacks(HAMEventLoopUICallbacks *callbacks) {
	ui_callbacks = callbacks;
}

void *ham_timeout_add(int interval, HAMTimeoutCallback callback, void *user_data) {
	if (ui_callbacks == NULL)
		return NULL;

	return ui_callbacks->timeout_add(interval, callback, user_data);
}

void ham_timeout_remove(void *handle) {
	if (ui_callbacks == NULL)
		return;

	ui_callbacks->timeout_remove(handle);
}

void *ham_input_add(int fd, HAMInputCallback callback, void *user_data) {
	if (ui_callbacks == NULL)
		return NULL;

	return ui_callbacks->input_add(fd, callback, user_data);
}

void ham_input_remove(void *handle) {
	if (ui_callbacks == NULL)
		return;

	ui_callbacks->input_remove(handle);
}
