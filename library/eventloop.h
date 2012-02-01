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

#ifndef _HAMLOG_EVENTLOOP_H
#define _HAMLOG_EVENTLOOP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file eventloop.h EventLoop API
 * @ingroup core
 */

/** 
 * Callback called when timer timeouts.
 * @param data Data passed to callback from UI.
 */
typedef void (*HAMTimeoutCallback) (void *data);

/** 
 * Callback called when there are new data on file-descriptor.
 * @param data Data passed to callback from UI.
 * @param fd file-descriptor which has new data to be read.
 */
typedef void (*HAMInputCallback) (void *data, int fd);

/**
 * HAMEventLoop related UI callbacks.
 */
typedef struct _HAMEventLoopUICallbacks {
	/**
	 * Called when HAMLib wants to setup new timer. UI has to setup its eventloop
	 * to call callback function in particular interval.
	 * @param internval Interval in miliseconds.
	 * @param callback Callback called by UI in interval.
	 * @param user_data Data passed to callback.
	 * @return pointer to object owned by UI which represents the timer on UI's side
	 * and is used to remove the timer in the future.
	 */
	void * (*timeout_add) (int interval, HAMTimeoutCallback callback, void *user_data);

	/**
	 * Called when HAMLib wants to remove the timer. UI has to stop the timer
	 * and must not call callback anymore.
	 * @param handle Pointer to object owned by UI which represents the timer on UI's side
	 * and is used to remove the timer in the future.
	 */
	void   (*timeout_remove) (void *handle);

	/**
	 * Called when HAMLib wants to be informed about new data on file-descriptor.
	 * UI has to setup its eventloop to call callback function everytime when new
	 * data are available on socket.
	 * @param fd File-descriptor.
	 * @param callback Callback called by UI when new data arrives.
	 * @param user_data Data passed to callback.
	 * @return pointer to object owned by UI which represents the input on UI's side
	 * and is used to remove the input in the future.
	 */
	void * (*input_add) (int fd, HAMInputCallback callback, void *user_data);

	/**
	 * Called when HAMLib wants to remove the input. UI has to stop this input handling
	 * and must not call callback anymore.
	 * @param handle Pointer to object owned by UI which represents the input on UI's side
	 * and is used to remove the input in the future.
	 */
	void   (*input_remove) (void *handle);
} HAMEventLoopUICallbacks;

/**
 * Sets UI callbacks which are then called for particular events.
 * @param callbacks UI Callbacks.
 */
void ham_eventloop_set_ui_callbacks(HAMEventLoopUICallbacks *callbacks);

/**
 * Setups the timer, see timeout_add UI callback for more informations.
 * @param internval Interval in miliseconds.
 * @param callback Callback called by UI in interval.
 * @param user_data Data passed to callback.
 * @return pointer to object owned by UI which represents the timer on UI's side
 * and is used to remove the timer in the future.
 */
void *ham_timeout_add(int interval, HAMTimeoutCallback callback, void *user_data);

/**
 * Removes the timer, see timeout_remove UI callback for more informations.
 * @param internval Interval in miliseconds.
 * @param callback Callback called by UI in interval.
 * @param user_data Data passed to callback.
 * @return pointer to object owned by UI which represents the timer on UI's side
 * and is used to remove the timer in the future.
 */
void ham_timeout_remove(void *handle);

/**
 * Adds new input, see input_add UI callback for more informations.
 * @param fd File-descriptor.
 * @param callback Callback called by UI when new data arrives.
 * @param user_data Data passed to callback.
 * @return pointer to object owned by UI which represents the input on UI's side
 * and is used to remove the input in the future.
 */
void *ham_input_add(int fd, HAMInputCallback callback, void *user_data);

/**
 * Removes the input, see input_remove UI callback for more informations.
 * @param handle Pointer to object owned by UI which represents the input on UI's side
 * and is used to remove the input in the future.
 */
void ham_input_remove(void *handle);

#ifdef __cplusplus
}
#endif

#endif
