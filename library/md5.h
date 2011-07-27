/**
 * @file md5.h MD5 API
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

#ifndef _HAMLOG_MD5_H
#define _HAMLOG_MD5_H

#ifdef __cplusplus                                                                                                                                                      
extern "C" {
#endif

/**
 * Computes MD5 hash from string ended by zero.
 * @param str String.
 * @return MD5 hash, size is always 16 bytes. This has to be freed by free().
 */
char *md5_get_hash(const char *str);

/**
 * Computes MD5 hash from string ended by zero and returns hexified hash.
 * @param str String.
 * @return Hexified MD5 hash ended by zero. This has to be freed by free().
 */
char *md5_get_hash_hex(const char *str);

#ifdef __cplusplus                                                                                                                                                      
}
#endif

#endif
