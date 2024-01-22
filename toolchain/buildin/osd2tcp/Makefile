# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2012-2024 VDR-NG-EM Project
#
# More information can be found in the files COPYING and README.
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
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

CC := gcc
CFLAGS := -Wall -O2
CPPFLAGS := -D_GNU_SOURCE
LDFLAGS :=
LIBS := -lz
PREFIX := /usr
BINDIR := $(PREFIX)/usr/bin
SPRGS := osd2tcp
RM := rm
CP := cp
MKDIR := mkdir

OSD2TCP_OBJS := osd2tcp.o

# sort unifies (is the only reason for sorting)
ALL_OBJS := $(sort $(OSD2TCP_OBJS))
.PHONY: all install clean

all: $(SPRGS)

.depend: $(ALL_OBJS:.o=.c)
	$(CC) $(CPPFLAGS) -M $(ALL_OBJS:.o=.c) > $@

-include .depend

%.o: %c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

osd2tcp: $(OSD2TCP_OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o osd2tcp $(OSD2TCP_OBJS) $(LIBS)


install: $(SPRGS)
	-@$(MKDIR) -p $(BINDIR)
	$(CP) $(SPRGS) $(BINDIR)

clean:
	-$(RM) -f *~ *.o .depend

distclean: clean
	-$(RM) -f $(SPRGS)
