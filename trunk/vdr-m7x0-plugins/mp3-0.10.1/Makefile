#
# MP3/MPlayer plugin to VDR
#
# (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
#
# This code is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html

# You can change the compile options here or create a Make.config
# in the VDR directory an set them there.

### uncomment the following line, if you want to include debug symbols
#DBG=1

### The C++ compiler and options:
CXX      ?= g++
CXXFLAGS ?= -O2 -fPIC -Wall -Woverloaded-virtual

###############################################
###############################################
#
# no user configurable options below this point
#
###############################################
###############################################

### The directory environment:

VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN  = mp3

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'define PLUGIN_VERSION' version.h | awk '{ print $$3 }' | sed -e 's/[";]//g')

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell sed -ne '/define VDRVERSION/ s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
APIVERSION = $(shell sed -ne '/define APIVERSION/ s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
ifeq ($(strip $(APIVERSION)),)
   APIVERSION = $(VDRVERSION)
endif
VDRVERSNUM = $(shell sed -ne '/define VDRVERSNUM/ s/^.[a-zA-Z ]*\([0-9]*\) .*$$/\1/p' $(VDRDIR)/config.h)
APIVERSNUM = $(shell sed -ne '/define APIVERSNUM/ s/^.[a-zA-Z ]*\([0-9]*\) .*$$/\1/p' $(VDRDIR)/config.h)
ifeq ($(strip $(APIVERSNUM)),)
   APIVERSNUM = $(VDRVERSNUM)
endif

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include
DEFINES  += -D_GNU_SOURCE -DAPIVERSNUM=$(APIVERSNUM)

### The object files (add further files here):

ALL += libvdr-$(PLUGIN).so
ifneq ($(shell grep -l 'Phrases' $(VDRDIR)/i18n.c),$(VDRDIR)/i18n.c)
  ALL += i18n-$(PLUGIN)
endif

COM_OBJS = i18n.o data.o menu.o

OBJS     = $(PLUGIN).o $(COM_OBJS)\
            data-mp3.o setup-mp3.o player-mp3.o stream.o network.o\
            decoder.o decoder-mp3.o decoder-mp3-stream.o 
LIBS     = 

ifdef DBG
  CXXFLAGS += -g
endif

### Internationalization (I18N):

PODIR     = po
I18Npot   = $(PODIR)/mp3-mplayer.pot
I18Npots  = $(notdir $(foreach file, $(wildcard $(PODIR)/*.po), $(basename $(file))))
I18Nmsgs  = $(addprefix $(LOCALEDIR)/,$(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo,$(I18Npots)))
I18Nmsgs2 = $(addprefix $(LOCALEDIR)/,$(addsuffix /LC_MESSAGES/vdr-$(PLUGIN2).mo,$(I18Npots)))
LOCALEDIR = $(VDRDIR)/locale

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) $(OBJS2:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: $(ALL)
.PHONY: i18n-$(PLUGIN) i18n-$(PLUGIN2)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

libvdr-$(PLUGIN2).so: $(OBJS2)
	$(CXX) $(CXXFLAGS) -shared $(OBJS2) $(LIBS2) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

$(I18Npot): $(shell grep -rl '\(tr\|trNOOP\)(\".*\")' *.c $(SYSDIR))
	xgettext -C -cTRANSLATORS --no-wrap -F -k -ktr -ktrNOOP --msgid-bugs-address='<s.huelswitt@gmx.de>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap -F --backup=none -q $@ $<
	@touch $@

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

i18n-$(PLUGIN): $(I18Nmsgs)

$(I18Nmsgs2): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN2).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

i18n-$(PLUGIN2): $(I18Nmsgs2)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tar.gz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tar.gz

clean:
	@-rm -f $(OBJS) $(OBJS2) $(DEPFILE) libvdr-*.so $(PACKAGE).tar.gz core* *~
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
