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
 * $Id: exception.S 373 2008-08-30 16:30:20Z andreas $
 *
 */
#include "regdef.h"
#include "bootdefs.h"

		.section .ex_entry,"ax"
		.globl  exception_entry
                .align  2
                .type   exception_entry,@function
                .ent    exception_entry,0
	        .frame  sp, 0, ra
exception_entry:
		j 	exception_save
		.end    exception_entry
                .size   exception_entry,.-exception_entry

		.section .text,"ax"
		.globl  exception_save
                .align  2
                .type   exception_save,@function
                .ent    exception_save,0
	        .frame  sp, 0, ra
exception_save:
		.set	noat
		sw	sp,29*4-(32*2*4+8)(sp)
		addiu	sp,-(32*2*4+8)
		sw	$0,0*4(sp)
		sw	$1,1*4(sp)
		sw	$2,2*4(sp)
		mfhi	$1
		sw	$3,3*4(sp)
		sw	$4,4*4(sp)
		sw	$5,5*4(sp)
		sw	$6,6*4(sp)
		sw	$1,32*4(sp)
		sw	$7,7*4(sp)
		mflo	$1
		sw	$8,8*4(sp)
		sw	$9,9*4(sp)
		sw	$10,10*4(sp)
		sw	$11,11*4(sp)
		sw	$12,12*4(sp)
		sw	$13,13*4(sp)
		sw	$1,33*4(sp)
		sw	$14,14*4(sp)
		sw	$15,15*4(sp)
		sw	$16,16*4(sp)
		sw	$17,17*4(sp)
		sw	$18,18*4(sp)
		sw	$19,19*4(sp)
		sw	$20,20*4(sp)
		sw	$21,21*4(sp)
		sw	$22,22*4(sp)
		sw	$23,23*4(sp)
		sw	$24,24*4(sp)
		sw	$25,25*4(sp)
		sw	$26,26*4(sp)
		sw	$27,27*4(sp)
		sw	$28,28*4(sp)
		sw	$30,30*4(sp)
		sw	$31,31*4(sp)
		.set	at
		mfc0	a0,CP0_CONTEXT
		sw	a0,(4+34)*4(sp)
		mfc0	a1,CP0_BADVADDR
		sw	a1,(8+34)*4(sp)
		mfc0	a0,CP0_STATUS
		sw	a0,(12+34)*4(sp)
		mfc0	a1,CP0_CAUSE
		sw	a1,(13+34)*4(sp)
		mfc0	a0,CP0_EPC
		sw	a0,(14+34)*4(sp)
		mfc0	a1,CP0_CONFIG
		sw	a1,(16+34)*4(sp)
		mfc0	a0,CP0_XCONTEXT
		sw	a0,(20+34)*4(sp)
		mfc0	a1,CP0_ERROREPC
		sw	a1,(30+34)*4(sp)
		la	t0,print_exception
		move	a0,sp
		jalr	t0
		lw	t0,32*4(sp)
		.set 	noat
		lw	$1,1*4(sp)
		mthi	t0
		lw	$2,2*4(sp)
		lw	$3,3*4(sp)
		lw	$4,4*4(sp)
		lw	$5,5*4(sp)
		lw	t7,33*4(sp)
		lw	$6,6*4(sp)
		mtlo	t7
		lw	$7,7*4(sp)
		lw	$8,8*4(sp)
		lw	$9,9*4(sp)
		lw	$10,10*4(sp)
		lw	$11,11*4(sp)
		lw	$12,12*4(sp)
		lw	$13,13*4(sp)
		lw	$14,14*4(sp)
		lw	$15,15*4(sp)
		lw	$16,16*4(sp)
		lw	$17,17*4(sp)
		lw	$18,18*4(sp)
		lw	$19,19*4(sp)
		lw	$20,20*4(sp)
		lw	$21,21*4(sp)
		lw	$22,22*4(sp)
		lw	$23,23*4(sp)
		lw	$24,24*4(sp)
		lw	$25,25*4(sp)
		lw	$26,26*4(sp)
		lw	$27,27*4(sp)
		lw	$28,28*4(sp)
		lw	$30,30*4(sp)
		lw	$31,31*4(sp)
		lw	$29,29*4(sp)
		.set mips3
		eret
		.end    exception_save
                .size   exception_save,.-exception_save
