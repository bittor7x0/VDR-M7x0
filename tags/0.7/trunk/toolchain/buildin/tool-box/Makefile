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
LDFLAGS :=
PREFIX := /usr/local
SBINDIR := $(PREFIX)/sbin
BINDIR := $(PREFIX)/bin
SPRGS := pic_tool avswctl
PRGS := osddrawtext svdrpsend genindex io-test vdron
RM := rm
CP := cp
MKDIR := mkdir

.SECONDEXPANSION:

pic_tool_OBJS := pic_tool.o tools.o
pic_tool_LDADD :=
osddrawtext_OBJS := osddrawtext.o tools.o
osddrawtext_LDADD := -losd
svdrpsend_OBJS := svdrpsend.o
svdrpsend_LDADD :=
avswctl_OBJS := avswctl.o
avswctl_LDADD :=
genindex_OBJS := genindex.o
genindex_LDADD :=
io-test_OBJS := io-test.o
io-test_LDADD :=
vdron_OBJS := vdron.o
vdron_LDADD :=

# sort unifies (is the only reason for sorting)
ALL_OBJS := $(sort $(foreach bin,$(SPRGS) $(PRGS),$($(bin)_OBJS)))

.PHONY: all install clean distclean

all: $(SPRGS) $(PRGS)

.depend: $(ALL_OBJS:.o=.c)
	$(CC) -M $(ALL_OBJS:.o=.c) > $@

-include .depend

%.o: %c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(SPRGS) $(PRGS): $$($$@_OBJS)
	$(CC) $(LDFLAGS) -o $@ $($@_OBJS) $($@_LDADD)

install: $(SPRGS) $(PRGS)
	-@$(MKDIR) -p $(SBINDIR)
	@$(CP) $(SPRGS) $(SBINDIR)
	-@$(MKDIR) -p $(BINDIR)
	@$(CP) $(PRGS) $(BINDIR)

clean:
	-$(RM) -f *~ *.o .depend

distclean: clean
	-$(RM) -f $(SPRGS)
	-$(RM) -f $(PRGS)
