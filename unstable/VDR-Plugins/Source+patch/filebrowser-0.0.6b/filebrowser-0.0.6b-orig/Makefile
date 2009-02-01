#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile 35 2007-04-12 14:22:05Z gambler $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = filebrowser
DEBUG = 1
### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR ?= ../../../
VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

### Add Debug macro

ifdef DEBUG
	DEFINES += -DDEBUG
endif

### The the sources that we want to compile the plugin

DEFINES+= -DFILEBROWSER_PLUGIN_BUILD

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):

OBJS = $(PLUGIN).o menu-filebrowser.o commands.o threads.o menu-output.o statebag.o menu-threads.o menu-setup.o tools.o command-sources.o command-other.o commands-plugins.o menu-accesscode.o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o $@
	@cp -f $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@$(MAKE) clean -C developers/filebrowserdemo > /dev/null
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/filebrowser.{filelist,kdev*}
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/{*,*/*,*/*/*}~
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/Doxyfile
	@tar --numeric-owner -czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
