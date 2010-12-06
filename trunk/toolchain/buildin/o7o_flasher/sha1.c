/*
 * sha1.c: Implementation of SHA1 hash calculation according to RFC 3174
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
 * $Id: sha1.c 148 2007-08-30 15:58:15Z andreas $
 *
 */
#include <endian.h>
#include <byteswap.h>
#include <stdint.h>
#include <string.h>

#include "sha1.h"
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ROTATE_LEFT(a,n) (((a) << (n)) | ((a) >> (32 - (n))))

void sha1_init_context(struct sha1_context *sha1_context)
{
	sha1_context->message_length = 0;
	sha1_context->hash_todo =  64;
	sha1_context->pad_done = 0;
	sha1_context->hash_words[0] = 0x67452301;
	sha1_context->hash_words[1] = 0xEFCDAB89;
	sha1_context->hash_words[2] = 0x98BADCFE;
	sha1_context->hash_words[3] = 0x10325476;
	sha1_context->hash_words[4] = 0xC3D2E1F0;
}

int sha1_calc_hash_part(struct sha1_context *sha1_context,
	uint8_t *buf, uint32_t length, int end)
{
	uint32_t process_words[80];
	uint32_t hw1;
	uint32_t hw2;
	uint32_t hw3;
	uint32_t hw4;
	uint32_t hw5;
	int todo;
	int i;
	int r;

	if (!sha1_context->hash_todo && end) {
		return -1;
	}

	r = 0;
	while (sha1_context->hash_todo) {
		todo = 64;
		if (length < 64) {
			if (!end)
				return r;
			todo = length;
		}


		memcpy(process_words, buf, todo);
		sha1_context->message_length += todo;
		r += todo;
		buf += todo;
		length -= todo;
		if (todo < 64) {
			uint8_t *p = (uint8_t *)process_words;
			sha1_context->hash_todo = ((todo + 1 + 8 + 63)  & ~63) - 64;
			if (!sha1_context->pad_done) {
				p[todo] = 0x80;
				memset(&p[todo + 1], 0, 64 - 1 - todo);
			} else {
				memset(&p[todo], 0, 64 - todo);
			}
			if (todo < 56) {
				process_words[14] = (sha1_context->message_length >> 29);
				process_words[15] = (sha1_context->message_length << 3);
#if BYTE_ORDER != BIG_ENDIAN
				/* For reversing the rest need not take care of the special case */
				process_words[14] = bswap_32(process_words[14]);
				process_words[15] = bswap_32(process_words[15]);
#endif
			}
			sha1_context->pad_done += 64 - todo;
		}

#if BYTE_ORDER != BIG_ENDIAN
		for (i = 0 ; i < 16; i++)
			process_words[i] = bswap_32(process_words[i]);
#endif

		for (i = 16; i < 80; i++)
			process_words[i] = ROTATE_LEFT(process_words[i - 3] ^
					process_words[i - 8] ^ process_words[i - 14] ^
					process_words[i - 16], 1);

		hw1 = sha1_context->hash_words[0];
		hw2 = sha1_context->hash_words[1];
		hw3 = sha1_context->hash_words[2];
		hw4 = sha1_context->hash_words[3];
		hw5 = sha1_context->hash_words[4];

		for (i = 0; i < 80; i++) {
			uint32_t t;
			if (i < 20) {
				t = 0x5A827999 + ((hw2 & hw3) | ((~hw2) & hw4));
			} else if (i < 40) {
				t = 0x6ED9EBA1 + (hw2 ^ hw3 ^ hw4);
			} else if (i < 60) {
				t = 0x8F1BBCDC + ((hw2 & (hw3 | hw4)) | (hw3 & hw4));
			} else {
				t = 0xCA62C1D6 + (hw2 ^ hw3 ^ hw4);
			}
			t += hw5;
			hw5 = hw4;
			hw4 = hw3;
			hw3 = ROTATE_LEFT(hw2,30);
			hw2 = hw1;
			hw1 = t + ROTATE_LEFT(hw1,5) + process_words[i];
		}

		sha1_context->hash_words[0] += hw1;
		sha1_context->hash_words[1] += hw2;
		sha1_context->hash_words[2] += hw3;
		sha1_context->hash_words[3] += hw4;
		sha1_context->hash_words[4] += hw5;
	}
	return r;
}

int check_sha1_hash(uint8_t *buf, uint32_t length, uint32_t hash[5])
{
	struct sha1_context sha1_context;
	int r;
	sha1_init_context(&sha1_context);

	r = sha1_calc_hash_part(&sha1_context, buf, length, 1);

	return (r == length && sha1_context.hash_words[0] == hash[0] &&
			sha1_context.hash_words[1] == hash[1] &&
			sha1_context.hash_words[2] == hash[2] &&
			sha1_context.hash_words[3] == hash[3] &&
			sha1_context.hash_words[4] == hash[4]);
}

