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
# $Id: Makefile 203 2007-09-27 11:07:18Z andreas $
#

AR := ar
ARFLAGS := r
CC := gcc
CFLAGS := -Wall -O2
CFLAGS_SHARED := $(CFLAGS) -fpic
CPPFLAGS := -D_GNU_SOURCE
LDFLAGS :=
TEST_PRGS := osd_test
LIBS := libosd
RM := rm
CP := cp
MKDIR := mkdir
PREFIX := /usr/local
LIBDIR := $(PREFIX)/lib
INCDIR := $(PREFIX)/include

.SECONDEXPANSION:

libosd_OBJS := libosd.o i18n.o font.o osd.o osd_progress_bar.o
libosd_OBJS_SHARED := $(libosd_OBJS:.o=.o_shared)
osd_test_OBJS := tests/osd_test.o libosd.a


# sort unifies (is the only reason for sorting)
ALL_OBJS := $(sort $(foreach bin,$(TEST_PRGS) $(LIBS),$($(bin)_OBJS))) \
   $(sort $(foreach bin, $(LIBS),$($(bin)_OBJS_SHARED)))

.PHONY: all clean distclean

all: $(addsuffix .a,$(LIBS)) $(addsuffix .so,$(LIBS))

.depend: $(filter %.c,$(ALL_OBJS:.o=.c))
	$(CC) -M $(filter %.c,$(ALL_OBJS:.o=.c)) > $@

-include .depend

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o_shared: %.c
	$(CC) $(CFLAGS_SHARED) $(CPPFLAGS) -c -o $@ $<

%.a: $$(%_OBJS)
	$(AR) $(ARFLAGS) $@ $+

%.so: $$(%_OBJS_SHARED)
	$(CC) $(CFLAGS_SHARED) $(CPPFLAGS) -shared $+ -o $@

install: $(addsuffix .a,$(LIBS)) $(addsuffix .so,$(LIBS))
	-@$(MKDIR) -p $(LIBDIR)
	-@$(MKDIR) -p $(INCDIR)/libosd
	@$(CP) *.h $(INCDIR)/libosd
	@$(CP) $(addsuffix .a,$(LIBS)) $(LIBDIR)
	@$(CP) $(addsuffix .so,$(LIBS)) $(LIBDIR)

$(TEST_PRGS): $$($$@_OBJS)
	$(CC) $(LDFLAGS) -o $@ $($@_OBJS)

clean:
	-$(RM) -f *~ *.o tests/*~ tests/*.o .depend

distclean: clean
	-$(RM) -f $(TEST_PRGS) $(addsuffix .a,$(LIBS))
