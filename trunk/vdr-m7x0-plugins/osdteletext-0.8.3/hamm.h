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

/* $Id: hamm.h,v 1.16 2005/06/28 01:00:22 mschimek Exp $ */

#ifndef __ZVBI3_HAMM_H__
#define __ZVBI3_HAMM_H__

#include <inttypes.h>		/* uintN_t */
#include "macros.h"

VBI3_BEGIN_DECLS

extern const uint8_t		_vbi3_bit_reverse [256];
extern const uint8_t		_vbi3_hamm8_fwd [16];
extern const int8_t		_vbi3_hamm8_inv [256];
extern const int8_t		_vbi3_hamm24_inv_par [3][256];

/**
 * @addtogroup Error Error correction functions
 * @ingroup Raw
 * @brief Helper functions to decode sliced VBI data.
 * @{
 */

/**
 * @param c Unsigned byte.
 * 
 * Reverses the bits of the argument.
 * 
 * @returns
 * Data bits 0 [msb] ... 7 [lsb].
 */
vbi3_inline unsigned int
vbi3_rev8			(unsigned int		c)
{
	return _vbi3_bit_reverse[(uint8_t) c];
}

/**
 * @param c Unsigned 16 bit word.
 * 
 * Reverses the bits of the argument.
 * 
 * @returns
 * Data bits 0 [msb] ... 15 [lsb].
 */
vbi3_inline unsigned int
vbi3_rev16			(unsigned int		c)
{
	return _vbi3_bit_reverse[(uint8_t) c] * 256
		+ _vbi3_bit_reverse[(uint8_t)(c >> 8)];
}

/**
 * @param p Pointer to a 16 bit word, last significant
 *   byte first.
 * 
 * Reverses the bits of the argument.
 * 
 * @returns
 * Data bits 0 [msb] ... 15 [lsb].
 */
vbi3_inline unsigned int
vbi3_rev16p			(const uint8_t *	p)
{
	return _vbi3_bit_reverse[p[0]] * 256
		+ _vbi3_bit_reverse[p[1]];
}

/**
 * @param c Unsigned byte.
 *
 * @returns
 * Changes the most significant bit of the byte
 * to make the number of set bits odd.
 */
vbi3_inline unsigned int
vbi3_par8			(unsigned int		c)
{
	c &= 255;

	/* if 0 == (inv_par[] & 32) change bit 7 of c. */
	c ^= 128 & ~(_vbi3_hamm24_inv_par[0][c] << 2);

	return c;
}

/**
 * @param c Unsigned byte. 
 * 
 * @returns
 * If the byte has odd parity (sum of bits modulo 2 is 1) the
 * byte AND 127, otherwise a negative value.
 */
vbi3_inline int
vbi3_unpar8			(unsigned int		c)
{
	if (_vbi3_hamm24_inv_par[0][(uint8_t) c] & 32) {
		return c & 127;
	} else {
		/* The idea is to OR results together to find a parity
		   error in a sequence, rather than a test and branch on
		   each byte. */
		return -1;
	}
}

extern void
vbi3_par				(uint8_t *		p,
				 unsigned long		n);
extern int
vbi3_unpar			(uint8_t *		p,
				 unsigned long		n);

/**
 * @param c Integer between 0 ... 15.
 * 
 * Encodes a nibble with Hamming 8/4 protection
 * as specified in EN 300 706, Section 8.2.
 * 
 * @returns
 * Hamming encoded unsigned byte, lsb first transmitted.
 */
vbi3_inline unsigned int
vbi3_ham8			(unsigned int		c)
{
	return _vbi3_hamm8_fwd[c & 15];
}

/**
 * @param c Hamming 8/4 protected byte, lsb first transmitted.
 * 
 * Decodes a Hamming 8/4 protected byte
 * as specified in EN 300 706, Section 8.2.
 * 
 * @returns
 * Data bits (D4 [msb] ... D1 [lsb]) or a negative
 * value if the byte contained incorrectable errors.
 */
vbi3_inline int
vbi3_unham8			(unsigned int		c)
{
	return _vbi3_hamm8_inv[(uint8_t) c];
}

/**
 * @param p Pointer to a Hamming 8/4 protected 16 bit word,
 *   last significant byte first, lsb first transmitted.
 * 
 * Decodes a Hamming 8/4 protected byte pair
 * as specified in EN 300 706, Section 8.2.
 * 
 * @returns
 * Data bits D4 [msb] ... D1 of first byte and D4 ... D1 [lsb]
 * of second byte, or a negative value if any of the bytes
 * contained incorrectable errors.
 */
vbi3_inline int
vbi3_unham16p			(const uint8_t *	p)
{
	return ((int) _vbi3_hamm8_inv[p[0]])
	  | (((int) _vbi3_hamm8_inv[p[1]]) << 4);
}

extern int
vbi3_unham24p			(const uint8_t *	p)
  __attribute__ ((_vbi3_pure));

/** @} */

VBI3_END_DECLS

#endif /* __ZVBI3_HAMM_H__ */
