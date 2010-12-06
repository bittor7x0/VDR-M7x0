/*
 * Copyright (C) 2008 Andreas Koch - the open7x0.org group
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
 * $Id: simple_serial.h 362 2008-08-17 17:14:52Z andreas $
 *
 */
#ifndef _SIMPLE_SERIAL_H
#define _SIMPLE_SERIAL_H

void serial_put_char(u8 c);

void serial_put_string(const char *s);

void serial_put_num_dec(long n, int len, int sign);

void serial_put_num_hex(unsigned long n, int len);

#endif
