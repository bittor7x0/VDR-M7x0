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
# The project's page is at http://www.assembla.com/wiki/show/VDR-M7x0
#

CC := gcc
CFLAGS := -Wall -O2
CPPFLAGS := -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
LDFLAGS :=
PREFIX := /usr
SBINDIR := $(PREFIX)/usr/bin
SPRGS := vdr2ts
RM := rm
CP := cp
MKDIR := mkdir

VDR2TS_OBJS := vdr2ts.o

# sort unifies (is the only reason for sorting)
ALL_OBJS := $(sort $(VDR2TS_OBJS))
.PHONY: all install clean

all: $(SPRGS)

.depend: $(ALL_OBJS:.o=.c)
	$(CC) $(CPPFLAGS) -M $(ALL_OBJS:.o=.c) > $@

-include .depend

%.o: %c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

vdr2ts: $(VDR2TS_OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o vdr2ts $(VDR2TS_OBJS)


install: $(SPRGS)
	$(CP) $(SPRGS) $(SBINDIR)

clean:
	-$(RM) -f *~ *.o .depend

distclean: clean
	-$(RM) -f $(SPRGS)
