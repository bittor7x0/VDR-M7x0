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

CC := gcc
CFLAGS := -Wall -O2
CPPFLAGS := -D_GNU_SOURCE
LDFLAGS := -static
LIBS := -losd
PREFIX := /usr/local
SBINDIR := $(PREFIX)/sbin
SPRGS := o7o_flasher
RM := rm
CP := cp
MKDIR := mkdir

.SECONDEXPANSION:

o7o_flasher_OBJS := o7o_flasher.o sha1.o display.o flash.o image.o tools.o

# sort unifies (is the only reason for sorting)
ALL_OBJS := $(sort $(foreach bin,$(SPRGS),$($(bin)_OBJS)))

.PHONY: all install clean distclean

all: $(SPRGS)

.depend: $(ALL_OBJS:.o=.c)
	echo $(ALL_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -M $(ALL_OBJS:.o=.c) > $@

-include .depend

%.o: %c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(SPRGS): $$($$@_OBJS)
	$(CC) -o $@ $($@_OBJS) $(LDFLAGS) $(LIBS)

install: $(SPRGS)
	-@$(MKDIR) -p $(SBINDIR)
	@$(CP) $(SPRGS) $(SBINDIR)

clean:
	-$(RM) -f *~ *.o .depend

distclean: clean
	-$(RM) -f $(SPRGS)
