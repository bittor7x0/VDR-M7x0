#
# Sudoku: A plugin for the Video Disk Recorder
#
# See the README file for copyright information and how to reach the author.
#
# $Id: Makefile 28 2006-04-25 00:09:14Z tom $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = sudoku

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

### The object files (add further files here):

OBJS = $(PLUGIN).o setup.o i18n.o bitmap.o menu.o \
       puzzle.o generator.o solver.o backtrack.o

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
	@cd tools && $(MAKE)

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) \
             --exclude debian --exclude CVS --exclude .svn $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

srcdoc: Doxyfile $(OBJS:%.o=%.cpp) $(OBJS:%.o=%.h)
	VERSION=$(VERSION) /usr/bin/doxygen

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so* *.tgz core* *~
	@-rm -rf srcdoc
	@cd tools && $(MAKE) clean
