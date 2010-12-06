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
 * $Id: simple_serial.c 362 2008-08-17 17:14:52Z andreas $
 *
 */
#include "typedefs.h"
#include "bootdefs.h"
#include "simple_serial.h"

#define UART_BASE (KSEG0 + 0x32001000)
#define UART_TX ((volatile u32 *)UART_BASE)
#define UART_RX ((volatile u32 *)UART_BASE)
#define UART_LINE_STATUS ((volatile u32 *)(UART_BASE + 0x50))

void serial_put_char(u8 c) 
{
  while (((*UART_LINE_STATUS) & 0x20) == 0)
    ;
    
  *UART_TX = c;
}

void serial_put_string(const char *s) 
{
  while (*s) {
    if (*s != '\n') {
       serial_put_char(*s);
    } else {
      serial_put_char('\r');
      serial_put_char('\n');
    }
    s++;
  }
}

static inline void serial_put_num(unsigned long n, int len, int base) 
{
  char digits[] = "0123456789ABCDEF";
  char str[33];
  int i;
  str[32] = 0;
  i = 31;
  
  if (len <= 0)
    len = 1;
  while (n) {
    str[i] = digits[n % base];
    n /= base;
    i--;
    len--;
  }
  
  while (len > 0) {
    str[i] = '0';
    len --;
    i--;
    }
    
  serial_put_string(str + i + 1);
}

void serial_put_num_dec(long n, int len, int sign) {
  if (sign && n < 0) {
    n = -n;
    serial_put_char('-');
    len--;
  }
  serial_put_num((unsigned long)n, len, 10);
}

void serial_put_num_hex(unsigned long n, int len) {
  serial_put_num(n, len, 16);
}
