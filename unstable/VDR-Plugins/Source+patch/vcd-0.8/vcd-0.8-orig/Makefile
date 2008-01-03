#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = vcd

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The directory environment:

DVBDIR = ../../../../DVB/include
VDRDIR = ../../..
VDRINC = $(VDRDIR)/include
LIBDIR = ../../lib
TMPDIR = /tmp

### The version number of VDR (taken from VDR's "config.h"):

APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Enable this for debugging the plugin

# DEFINES += -DDEBUG_VCD

### Enable this to let the plugin strip the PES headers from stillimage frames

# DEFINES += -DSTRIP_STILLIMAGE_PES_HEADER

### Includes and Defines (add further entries here):

INCLUDES = -I$(VDRINC) -I$(DVBDIR)

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"' -D_GNU_SOURCE

### The object files (add further files here):

OBJS = $(PLUGIN).o functions.o i18n.o menu.o player.o viewer.o setup.o menucontrol.o psd.o psdcontrol.o

### The C++ compiler and options:

CXX      = g++
CXXFLAGS = -fPIC -O2 -Wall -Woverloaded-virtual

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
