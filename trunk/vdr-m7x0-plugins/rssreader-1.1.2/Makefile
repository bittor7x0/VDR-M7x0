#
# Makefile for a Video Disk Recorder RSS Reader plugin
#
# $Id$

# Debugging on/off 
#RSSREADER_DEBUG = 1

# Strip debug symbols?  Set eg. to /bin/true if not
STRIP = /bin/true

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = rssreader

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char VERSION\[\] *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -g -O2 -Wall -Woverloaded-virtual

### The directory environment:

VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include 


DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef RSSREADER_DEBUG
DEFINES += -DDEBUG
endif

### The object files (add further files here):

OBJS =  http_error_codes.o http_fetcher.o ezxml.o rssreader.o parser.o menu.o config.o tools.o compatibility.o i18n.o

### The main target:

all: libvdr-$(PLUGIN).so 
#i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<


#i18n compatibility generator:
#i18n.c: i18n-template.c po2i18n.pl
#	./po2i18n.pl < i18n-template.c > i18n.c


# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):

#PODIR     = po
#LOCALEDIR = $(VDRDIR)/locale
#I18Npo    = $(wildcard $(PODIR)/*.po)
#I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
#I18Ndirs  = $(notdir $(foreach file, $(I18Npo), $(basename $(file))))
#I18Npot   = $(PODIR)/$(PLUGIN).pot
#
#%.mo: %.po
#	msgfmt -c -o $@ $<
#
#$(I18Npot): $(wildcard *.c)
#	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='Rolf Ahrenberg' -o $@ $(wildcard *.c)
#
#$(I18Npo): $(I18Npot)
#	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
#
#i18n: $(I18Npot) $(I18Nmo)
#	@mkdir -p $(LOCALEDIR)
#	for i in $(I18Ndirs); do\
#	    mkdir -p $(LOCALEDIR)/$$i/LC_MESSAGES;\
#	    cp $(PODIR)/$$i.mo $(LOCALEDIR)/$$i/LC_MESSAGES/vdr-$(PLUGIN).mo;\
#	    done

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
ifndef RSSREADER_DEBUG
	@$(STRIP) $@
endif
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
