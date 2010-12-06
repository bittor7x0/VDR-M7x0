#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
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
# $Id: Makefile 201 2007-09-25 17:42:55Z andreas $
#
#

CC := gcc
CFLAGS := -Wall -O2
CPPFLAGS := -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
LDFLAGS :=
PREFIX := /usr/local
SBINDIR := $(PREFIX)/sbin
SPRGS := usbautomounter
RM := rm
CP := cp
MKDIR := mkdir

USBAUTOMOUNTER_OBJS := usbautomounter.o tools.o setup.o scsi_usb_scan.o \
   partitions_reader.o mount.o fs_id_reader.o fs_id_fat_reader.o \
   fs_id_ext2_3_reader.o display.o

# sort unifies (is the only reason for sorting)
ALL_OBJS := $(sort $(USBAUTOMOUNTER_OBJS))
.PHONY: all install clean

all: $(SPRGS)

.depend: $(ALL_OBJS:.o=.c)
	$(CC) $(CPPFLAGS) -M $(ALL_OBJS:.o=.c) > $@

-include .depend

%.o: %c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

usbautomounter: $(USBAUTOMOUNTER_OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o usbautomounter $(USBAUTOMOUNTER_OBJS) -losd


install: $(SPRGS)
	-@$(MKDIR) -p $(SBINDIR)
	@$(CP) $(SPRGS) $(SBINDIR)

clean:
	-$(RM) -f *~ *.o .depend

distclean: clean
	-$(RM) -f $(SPRGS)
