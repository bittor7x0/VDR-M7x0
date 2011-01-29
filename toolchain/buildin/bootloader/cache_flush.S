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
 */
#include "regdef.h"
#include "bootdefs.h"
#include "cache.h"

		.section .init,"ax"
		.globl  _flush_dcache
                .align  2
                .type   _flush_dcache,@function
                .ent    _flush_dcache,0
_flush_dcache:  .frame  sp, 0, ra
		li	t0,KSEG0
		addiu	t1,t0,DCACHE_SIZE
1:
		.set	mips3
		cache	1,0(t0)
		addiu	t0,DCACHE_LSIZE
		slt	t2,t0,t1
		bnez	t2,1b
		
		jr	ra
		.end    _flush_dcache
                .size   _flush_dcache,.-flush_dcache

		.section .init,"ax"
		.globl  _flush_icache
                .align  2
                .type   _flush_icache,@function
                .ent    _flush_icache,0
_flush_icache:  .frame  sp, 0, ra
		li	t0,KSEG0
		addiu	t1,t0,ICACHE_SIZE
1:
		.set	mips3
		cache	0,0(t0)
		addiu	t0,ICACHE_LSIZE
		slt	t2,t0,t1
		bnez	t2,1b
		
		jr	ra
		.end    _flush_icache
                .size   _flush_icache,.-flush_icache
