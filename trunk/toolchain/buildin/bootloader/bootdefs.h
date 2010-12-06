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
 * $Id: bootdefs.h 362 2008-08-17 17:14:52Z andreas $
 *
 */
#ifndef _BOOTDEFS_H
#define _BOOTDEFS_H

#define KSEG0 0x80000000
#define MEM_SIZE (32 * 1024 * 1024)
#define STACK_START (KSEG0 + MEM_SIZE - 128) 

#define FLASH_START (KSEG0 + 0x1fc00000 + 64 * 1024)
#define HEAD_SYS_TYPE (FLASH_START + 0x3EC)
#define HEAD_KERNEL_LOAD_ADDR (FLASH_START + 0x3F0)
#define HEAD_KERNEL_ENTRY (FLASH_START + 0x3F4)
#define HEAD_KERNEL_COMPRESSED (FLASH_START + 0x3F8)
#define HEAD_KERNEL_COMPRESSED_SIZE (FLASH_START + 0x3FC)
#define MAX_KERNEL_SIZE (16 * 1024 * 1024)
#define BOOT_LOADER_START (FLASH_START + 0x400)
#define SYS_TYPE_M740 1
#define SYS_TYPE_M750 2
#endif
