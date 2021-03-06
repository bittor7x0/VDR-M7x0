#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = vdrmanager

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'const char \*VERSION *=' $(PLUGIN).cpp | awk '{ print $$5 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual -fPIC -g
t_CXXFLAGS := $(CXXFLAGS)
CXXFLAGS = $(filter-out -fno-exceptions,$(t_CXXFLAGS))

### The directory environment:

DVBDIR ?= ../../../../DVB
VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### Conditionally definitions

VDRMANAGER_USE_SSL := 0
ifneq ($(VDRMANAGER_USE_SSL), 0)
ADDITIONAL_LIBS = $(shell pkg-config --libs openssl)
DEFINES += -DVDRMANAGER_USE_SSL=$(VDRMANAGER_USE_SSL)
endif

VDRMANAGER_USE_ZLIB := 0
ifneq ($(VDRMANAGER_USE_ZLIB), 0)
ADDITIONAL_LIBS += $(shell pkg-config --libs zlib)
DEFINES += -DVDRMANAGER_USE_ZLIB=$(VDRMANAGER_USE_ZLIB)
endif

VDRMANAGER_USE_GZIP := 1
ifneq ($(VDRMANAGER_USE_GZIP), 0)
DEFINES += -DVDRMANAGER_USE_GZIP=$(VDRMANAGER_USE_GZIP)
endif

### The object files (add further files here):

OBJS = $(PLUGIN).o sock.o serversock.o clientsock.o vdrmanagerthread.o select.o handler.o helpers.o compressor.o

### Implicit rules:

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.cpp) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) -o $@ $(ADDITIONAL_LIBS)
	@cp -f $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~

cppcheck:
	@cppcheck --enable=information,style,unusedFunction -v -f $(OBJS:%.o=%.cpp)
