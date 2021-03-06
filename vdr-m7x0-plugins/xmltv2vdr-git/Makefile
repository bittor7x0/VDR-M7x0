#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
# IMPORTANT: the presence of this macro is important for the Make.config
# file. So it must be defined, even if it is not used here!
#
PLUGIN = xmltv2vdr

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).h | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -g -O2 -Wall -Wextra -Woverloaded-virtual -Wno-parentheses
PKG-CONFIG ?= pkg-config

### The directory environment:

VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

### Make sure that necessary options are included:

-include $(VDRDIR)/Make.global

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

PKG-LIBS += libpcrecpp sqlite3
PKG-INCLUDES += libpcrecpp sqlite3

INCLUDES += -I$(VDRDIR)/include

DEFINES += -D_GNU_SOURCE -D_XOPEN_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

INCLUDES += $(shell $(PKG-CONFIG) --cflags $(PKG-INCLUDES))
LIBS    += $(shell $(PKG-CONFIG) --libs $(PKG-LIBS))

### The object files (add further files here):

OBJS = $(PLUGIN).o i18n.o compatibility.o soundex.o extpipe.o parse.o source.o import.o event.o setup.o maps.o

### The main target:

all: libvdr-$(PLUGIN).so

### Implicit rules:

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.cpp) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.cpp *.h)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<see README>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

.PHONY: i18n
i18n: $(I18Nmsgs) $(I18Npot)

generate-i18n: i18n-template.h $(I18Npot) $(I18Npo) buildutil/pot2i18n.pl
	@chmod +x buildutil/pot2i18n.pl
	buildutil/pot2i18n.pl $(I18Npot) i18n-template.h > i18n-generated.h

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: generate-i18n clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a *.cpp *.h HISTORY COPYING Makefile README po $(TMPDIR)/$(ARCHIVE)
	@mkdir -p $(TMPDIR)/$(ARCHIVE)/dist/epgdata2xmltv
	@cp -a dist/epgdata2xmltv/*.cpp dist/epgdata2xmltv/*.h dist/epgdata2xmltv/Makefile dist/epgdata2xmltv/INSTALL dist/epgdata2xmltv/COPYING dist/epgdata2xmltv/epgdata2xmltv.dist dist/epgdata2xmltv/epgdata2xmltv.xsl $(TMPDIR)/$(ARCHIVE)/dist/epgdata2xmltv 
	@mkdir -p $(TMPDIR)/$(ARCHIVE)/dist/patches
	@cp -a dist/patches/* $(TMPDIR)/$(ARCHIVE)/dist/patches
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) --exclude debian --exclude CVS --exclude .svn $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ $(PODIR)/*.mo $(PODIR)/*.pot pod2*.tmp

cppcheck:
	@cppcheck --enable=information,style,unusedFunction -v -f $(OBJS:%.o=%.cpp)
