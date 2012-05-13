/*
 *  libzvbi - Error correction functions
 *
 *  Copyright (C) 2001, 2002, 2003, 2004 Michael H. Schimek
 *
 *  Based on code from AleVT 1.5.1
 *  Copyright (C) 1998, 1999 Edgar Toernig <froese@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Id: hamm.c,v 1.15 2005/06/28 01:00:59 mschimek Exp $ */

#include <limits.h>		/* CHAR_BIT */
#include "hamm.h"

const uint8_t
_vbi3_bit_reverse [256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,	
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,	
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,

	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,	
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,	
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,

	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,	
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,	
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,

	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,	
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,	
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,

	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,	
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,	
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,

	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,	
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,	
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,

	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,	
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,	
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,

	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,	
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,	
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

/**
 * @ingroup Error
 *
 * @param p Array of unsigned bytes.
 * @param n Size of array.
 * 
 * Of each byte of the array, changes the most significant
 * bit to make the number of set bits odd.
 */
void
vbi3_par				(uint8_t *		p,
				 unsigned long		n)
{
	while (n-- > 0) {
		uint8_t c = *p;

		/* if 0 == (inv_par[] & 32) change msb of *p. */
		*p++ = c ^ (128 & ~(_vbi3_hamm24_inv_par[0][c] << 2));
	}
}

/**
 * @ingroup Error
 * @param p Array of unsigned bytes.
 * @param n Size of array.
 * 
 * Tests the parity and clears the most significant bit of
 * each byte of the array.
 *
 * @return
 * A negative value if any byte of the array had even
 * parity (sum of bits modulo 2 is 0).
 */
int
vbi3_unpar			(uint8_t *		p,
				 unsigned long		n)
{
	int r = 0;

	while (n-- > 0) {
		uint8_t c = *p;

		/* if 0 == (inv_par[] & 32) set msb of r. */
		r |= ~ _vbi3_hamm24_inv_par[0][c]
			<< (sizeof (int) * CHAR_BIT - 1 - 5);

		*p++ = c & 127;
	}

	return r;
}

/* ETS 300 706, Section 8.2 Hamming 8/4 */

const uint8_t
_vbi3_hamm8_fwd [16] = {
	0x15, 0x02, 0x49, 0x5e, 0x64, 0x73, 0x38, 0x2f,
	0xd0, 0xc7, 0x8c, 0x9b, 0xa1, 0xb6, 0xfd, 0xea
};

const int8_t
_vbi3_hamm8_inv [256] = {
	0x01,   -1, 0x01, 0x01,   -1, 0x00, 0x01,   -1, 
	  -1, 0x02, 0x01,   -1, 0x0a,   -1,   -1, 0x07, 
	  -1, 0x00, 0x01,   -1, 0x00, 0x00,   -1, 0x00, 
	0x06,   -1,   -1, 0x0b,   -1, 0x00, 0x03,   -1, 

	  -1, 0x0c, 0x01,   -1, 0x04,   -1,   -1, 0x07, 
	0x06,   -1,   -1, 0x07,   -1, 0x07, 0x07, 0x07, 
	0x06,   -1,   -1, 0x05,   -1, 0x00, 0x0d,   -1, 
	0x06, 0x06, 0x06,   -1, 0x06,   -1,   -1, 0x07, 

	  -1, 0x02, 0x01,   -1, 0x04,   -1,   -1, 0x09, 
	0x02, 0x02,   -1, 0x02,   -1, 0x02, 0x03,   -1, 
	0x08,   -1,   -1, 0x05,   -1, 0x00, 0x03,   -1, 
	  -1, 0x02, 0x03,   -1, 0x03,   -1, 0x03, 0x03, 

	0x04,   -1,   -1, 0x05, 0x04, 0x04, 0x04,   -1, 
	  -1, 0x02, 0x0f,   -1, 0x04,   -1,   -1, 0x07, 
	  -1, 0x05, 0x05, 0x05, 0x04,   -1,   -1, 0x05, 
	0x06,   -1,   -1, 0x05,   -1, 0x0e, 0x03,   -1, 

	  -1, 0x0c, 0x01,   -1, 0x0a,   -1,   -1, 0x09, 
	0x0a,   -1,   -1, 0x0b, 0x0a, 0x0a, 0x0a,   -1, 
	0x08,   -1,   -1, 0x0b,   -1, 0x00, 0x0d,   -1, 
	  -1, 0x0b, 0x0b, 0x0b, 0x0a,   -1,   -1, 0x0b, 

	0x0c, 0x0c,   -1, 0x0c,   -1, 0x0c, 0x0d,   -1, 
	  -1, 0x0c, 0x0f,   -1, 0x0a,   -1,   -1, 0x07, 
	  -1, 0x0c, 0x0d,   -1, 0x0d,   -1, 0x0d, 0x0d, 
	0x06,   -1,   -1, 0x0b,   -1, 0x0e, 0x0d,   -1, 

	0x08,   -1,   -1, 0x09,   -1, 0x09, 0x09, 0x09, 
	  -1, 0x02, 0x0f,   -1, 0x0a,   -1,   -1, 0x09, 
	0x08, 0x08, 0x08,   -1, 0x08,   -1,   -1, 0x09, 
	0x08,   -1,   -1, 0x0b,   -1, 0x0e, 0x03,   -1, 

	  -1, 0x0c, 0x0f,   -1, 0x04,   -1,   -1, 0x09, 
	0x0f,   -1, 0x0f, 0x0f,   -1, 0x0e, 0x0f,   -1, 
	0x08,   -1,   -1, 0x05,   -1, 0x0e, 0x0d,   -1, 
	  -1, 0x0e, 0x0f,   -1, 0x0e, 0x0e,   -1, 0x0e
};

/* ETS 300 706, Section 8.3 Hamming 18/24 (code from AleVT) */

/* This table generates the parity checks for hamm24/18 decoding.
   Bit 0 is for test A, 1 for B, ...
   Thanks to R. Gancarz for this fine table *g* */
const int8_t
_vbi3_hamm24_inv_par [3][256] = {
    {
        /* Parities of first byte */
	 0, 33, 34,  3, 35,  2,  1, 32, 36,  5,  6, 39,  7, 38, 37,  4,
	37,  4,  7, 38,  6, 39, 36,  5,  1, 32, 35,  2, 34,  3,  0, 33,
	38,  7,  4, 37,  5, 36, 39,  6,  2, 35, 32,  1, 33,  0,  3, 34,
	 3, 34, 33,  0, 32,  1,  2, 35, 39,  6,  5, 36,  4, 37, 38,  7,

	39,  6,  5, 36,  4, 37, 38,  7,  3, 34, 33,  0, 32,  1,  2, 35,
	 2, 35, 32,  1, 33,  0,  3, 34, 38,  7,  4, 37,  5, 36, 39,  6,
	 1, 32, 35,  2, 34,  3,  0, 33, 37,  4,  7, 38,  6, 39, 36,  5,
	36,  5,  6, 39,  7, 38, 37,  4,  0, 33, 34,  3, 35,  2,  1, 32,

	40,  9, 10, 43, 11, 42, 41,  8, 12, 45, 46, 15, 47, 14, 13, 44,
	13, 44, 47, 14, 46, 15, 12, 45, 41,  8, 11, 42, 10, 43, 40,  9,
	14, 47, 44, 13, 45, 12, 15, 46, 42, 11,  8, 41,  9, 40, 43, 10,
	43, 10,  9, 40,  8, 41, 42, 11, 15, 46, 45, 12, 44, 13, 14, 47,

	15, 46, 45, 12, 44, 13, 14, 47, 43, 10,  9, 40,  8, 41, 42, 11,
	42, 11,  8, 41,  9, 40, 43, 10, 14, 47, 44, 13, 45, 12, 15, 46,
	41,  8, 11, 42, 10, 43, 40,  9, 13, 44, 47, 14, 46, 15, 12, 45,
	12, 45, 46, 15, 47, 14, 13, 44, 40,  9, 10, 43, 11, 42, 41,  8
    }, { 
        /* Parities of second byte */
	 0, 41, 42,  3, 43,  2,  1, 40, 44,  5,  6, 47,  7, 46, 45,  4,
	45,  4,  7, 46,  6, 47, 44,  5,  1, 40, 43,  2, 42,  3,  0, 41,
	46,  7,  4, 45,  5, 44, 47,  6,  2, 43, 40,  1, 41,  0,  3, 42,
	 3, 42, 41,  0, 40,  1,  2, 43, 47,  6,  5, 44,  4, 45, 46,  7,

	47,  6,  5, 44,  4, 45, 46,  7,  3, 42, 41,  0, 40,  1,  2, 43,
	 2, 43, 40,  1, 41,  0,  3, 42, 46,  7,  4, 45,  5, 44, 47,  6,
	 1, 40, 43,  2, 42,  3,  0, 41, 45,  4,  7, 46,  6, 47, 44,  5,
	44,  5,  6, 47,  7, 46, 45,  4,  0, 41, 42,  3, 43,  2,  1, 40,

	48, 25, 26, 51, 27, 50, 49, 24, 28, 53, 54, 31, 55, 30, 29, 52,
	29, 52, 55, 30, 54, 31, 28, 53, 49, 24, 27, 50, 26, 51, 48, 25,
	30, 55, 52, 29, 53, 28, 31, 54, 50, 27, 24, 49, 25, 48, 51, 26,
	51, 26, 25, 48, 24, 49, 50, 27, 31, 54, 53, 28, 52, 29, 30, 55,

	31, 54, 53, 28, 52, 29, 30, 55, 51, 26, 25, 48, 24, 49, 50, 27,
	50, 27, 24, 49, 25, 48, 51, 26, 30, 55, 52, 29, 53, 28, 31, 54,
	49, 24, 27, 50, 26, 51, 48, 25, 29, 52, 55, 30, 54, 31, 28, 53,
	28, 53, 54, 31, 55, 30, 29, 52, 48, 25, 26, 51, 27, 50, 49, 24
    }, { 
        /* Parities of third byte, xor 0x3F */
	63, 14, 13, 60, 12, 61, 62, 15, 11, 58, 57,  8, 56,  9, 10, 59,
	10, 59, 56,  9, 57,  8, 11, 58, 62, 15, 12, 61, 13, 60, 63, 14,
	 9, 56, 59, 10, 58, 11,  8, 57, 61, 12, 15, 62, 14, 63, 60, 13,
	60, 13, 14, 63, 15, 62, 61, 12,  8, 57, 58, 11, 59, 10,  9, 56,

	 8, 57, 58, 11, 59, 10,  9, 56, 60, 13, 14, 63, 15, 62, 61, 12,
	61, 12, 15, 62, 14, 63, 60, 13,  9, 56, 59, 10, 58, 11,  8, 57,
	62, 15, 12, 61, 13, 60, 63, 14, 10, 59, 56,  9, 57,  8, 11, 58,
	11, 58, 57,  8, 56,  9, 10, 59, 63, 14, 13, 60, 12, 61, 62, 15,

	31, 46, 45, 28, 44, 29, 30, 47, 43, 26, 25, 40, 24, 41, 42, 27,
	42, 27, 24, 41, 25, 40, 43, 26, 30, 47, 44, 29, 45, 28, 31, 46,
	41, 24, 27, 42, 26, 43, 40, 25, 29, 44, 47, 30, 46, 31, 28, 45,
	28, 45, 46, 31, 47, 30, 29, 44, 40, 25, 26, 43, 27, 42, 41, 24,

	40, 25, 26, 43, 27, 42, 41, 24, 28, 45, 46, 31, 47, 30, 29, 44,
	29, 44, 47, 30, 46, 31, 28, 45, 41, 24, 27, 42, 26, 43, 40, 25,
	30, 47, 44, 29, 45, 28, 31, 46, 42, 27, 24, 41, 25, 40, 43, 26,
	43, 26, 25, 40, 24, 41, 42, 27, 31, 46, 45, 28, 44, 29, 30, 47
    }
};

/* Table to extract the lower 4 bits from first hamm24/18
   encoded byte (P4 D4 D3 D2 P3 D1 P2 P1) */
static const int8_t
_vbi3_hamm24_val [128] = {
	 0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0,  1,  1,  1,  1,
	 2,  2,  2,  2,  3,  3,  3,  3,  2,  2,  2,  2,  3,  3,  3,  3,
	 4,  4,  4,  4,  5,  5,  5,  5,  4,  4,  4,  4,  5,  5,  5,  5,
	 6,  6,  6,  6,  7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  7,  7,

	 8,  8,  8,  8,  9,  9,  9,  9,  8,  8,  8,  8,  9,  9,  9,  9,
	10, 10, 10, 10, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11,
	12, 12, 12, 12, 13, 13, 13, 13, 12, 12, 12, 12, 13, 13, 13, 13,
	14, 14, 14, 14, 15, 15, 15, 15, 14, 14, 14, 14, 15, 15, 15, 15
};

/* Correction of single bit error in decoded word,
   according to test A ... F in table vbi3_hamm24_inv_tst.
   MSB indicates double bit error. */
static const int32_t
_vbi3_hamm24_corr [64] = {
	0x00000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,

	0x80000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000001,
	0x00000000, 0x00000002, 0x00000004, 0x00000008,
	0x00000000, 0x00000010, 0x00000020, 0x00000040,
	0x00000080, 0x00000100, 0x00000200, 0x00000400,

	0x00000000, 0x00000800, 0x00001000, 0x00002000,
	0x00004000, 0x00008000, 0x00010000, 0x00020000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,
	0x80000000, 0x80000000, 0x80000000, 0x80000000,
};

/**
 * @ingroup Error
 * @param p Pointer to a Hamming 24/18 protected 24 bit word,
 *   last significant byte first, lsb first transmitted.
 * 
 * Decodes a Hamming 24/18 protected byte triplet
 * as specified in ETS 300 706, Section 8.3.
 * 
 * @return
 * Triplet data bits D18 [msb] ... D1 [lsb] or a negative value
 * if the triplet contained incorrectable errors.
 */
int
vbi3_unham24p			(const uint8_t *	p)
{
	int e = _vbi3_hamm24_inv_par[0][p[0]]
		^ _vbi3_hamm24_inv_par[1][p[1]]
		^ _vbi3_hamm24_inv_par[2][p[2]];

	int x = _vbi3_hamm24_val[p[0] & 127]
		+ (p[1] & 127) * 16
		+ (p[2] & 127) * 2048;

	return x ^ (int) _vbi3_hamm24_corr[e];
}
