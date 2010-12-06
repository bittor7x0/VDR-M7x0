#
# Copyright (C) 2008 Andreas Koch - the open7x0.org group
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#
# $Id: Makefile 376 2008-09-01 19:15:23Z andreas $
#

PREFIX=/usr/local
INST_PREFIX=$(PREFIX)/boot
CC=mips-linux-gcc
OBJCOPY=mips-linux-objcopy
LD=mips-linux-ld

override CFLAGS=-Wall -O3 -G0 -mno-abicalls -fno-pic -mtune=vr4120 -march=mips2 -mfix-vr4120 -nostdinc -nostdlib -msoft-float
override LDFLAGS=-G0 -O3 -nostdlib
MKDIR=mkdir

OBJS = boot_entry.o init_loader.o cache_flush.o exception.o print_exception.o cache_func.o boot.o simple_serial.o LzmaDecode.o
SRCS = boot_entry.S init_loader.c cache_flush.S exception.S print_exception.c cache_func.c boot.c simple_serial.c LzmaDecode.c
.PHONY: clean all install

all: bootloader.bin

.dep: $(SRCS)
	$(CC) -M $(SRCS) > $@

include .dep
	
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c -o $@ $<

bootloader: $(OBJS) ld.script 
	$(LD) $(LDFLAGS) --script=ld.script -o bootloader $(OBJS)

bootloader.bin: bootloader
	$(OBJCOPY) --strip-all -O binary bootloader bootloader.bin 

clean:
	rm -rf *.o *~ bootloader bootloader.bin 	

install: bootloader.bin
	$(MKDIR) -p $(INST_PREFIX)
	cp bootloader.bin $(INST_PREFIX)
