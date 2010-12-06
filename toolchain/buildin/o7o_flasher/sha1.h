/*
 * sha1.h: Header file for SHA1 hash calculation according to RFC 3174
 *
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * $Id: sha1.h 148 2007-08-30 15:58:15Z andreas $
 *
 */
#ifndef __SHA1_H
#define __SHA1_H 1
#include <stdint.h>

struct sha1_context {
	uint32_t message_length;
	uint32_t hash_todo;
	int pad_done;
	uint32_t hash_words[5];
};

void sha1_init_context(struct sha1_context *sha1_context);
int sha1_calc_hash_part(struct sha1_context *sha1_context,
	uint8_t *buf, uint32_t length, int end);
int check_sha1_hash(uint8_t *buf, uint32_t length,uint32_t hash[5]);
#endif
