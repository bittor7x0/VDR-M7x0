#
# Spider-Arachnid: A plugin for the Video Disk Recorder
#
# Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# $Id: Makefile 96 2007-09-23 13:18:41Z tom $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
# IMPORTANT: the presence of this macro is important for the Make.config
# file. So it must be defined, even if it is not used here!
#
PLUGIN = spider

### The version number of this plugin (taken from the main source file):

VERSION = $(shell sed -ne '/static .* VERSION *=/s/^.*"\(.*\)".*$$/\1/p' \
                      $(PLUGIN).cpp)

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -g -O2 -Wall -Woverloaded-virtual

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' \
                         $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include
DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'
CXXFLAGS += $(INCLUDES) $(DEFINES)

### The source and object files

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

### The main target:

all: libvdr-$(PLUGIN).so i18n

### Dependencies:

DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(CXX) -MM -MG $(SRCS) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(notdir $(wildcard $(PODIR)/*.po))
I18Npot   = $(PODIR)/$(PLUGIN).pot

$(I18Npot): $(SRCS)
	xgettext -C -cTRANSLATORS --no-wrap -F -k -ktr -ktrNOOP \
	         --msgid-bugs-address='<tom@toms-cafe.de>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap -F --backup=none -q $@ $<
	@touch $@

$(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.po
	@mkdir -p $(dir $@)
	msgfmt -c -o $@ $<

.PHONY: i18n
i18n: $(I18Npo:%.po=$(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo)

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $^ -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) \
	     --exclude debian --exclude CVS --exclude .svn $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(DEPFILE) *.o *.so $(I18Npot) *.tgz core* *~
