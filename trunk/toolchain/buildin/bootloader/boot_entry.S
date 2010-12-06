/*
 * Copyright (C) 2007,2008 Andreas Koch - the open7x0.org group
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
 * $Id: boot_entry.S 362 2008-08-17 17:14:52Z andreas $
 *
 */
#include "regdef.h"
#include "bootdefs.h"

		.section .entry,"ax"
		.globl  boot_entry
                .align  2
                .type   boot_entry,@function
                .ent    boot_entry,0
boot_entry:     .frame  sp, 0, ra
		li	sp,STACK_START
		jal	init_loader

		mfc0	t0,CP0_STATUS
		li	t1,0xffbf0000
		and	t0,t1
		mtc0	t0,CP0_STATUS

		la	t0,boot
		jr	t0
		.end    boot_entry
                .size   boot_entry,.-boot_entry

