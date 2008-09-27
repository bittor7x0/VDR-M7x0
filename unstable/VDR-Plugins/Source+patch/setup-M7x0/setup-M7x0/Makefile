#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile 43 2007-05-15 06:20:51Z hape $

# Debugging on/off 
#OPEN7x0_Setup_DEBUG = 1
#DEBUG = -DDEBUG 


# Strip debug symbols?  Set eg. to /bin/true if not
STRIP = mips-linux-uclibc-strip


# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = setup

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).cpp | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -g -O2 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR ?= ../../..
LIBDIR ?= ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

# The version number of VDR's plugin API (taken from VDR's "config.h"):
APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include


#just for TESTING VDR SUBmenu
INCLUDES += -I$(VDRDIR)

DEFINES += $(DEBUG) -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"' -DVDRVERSION="\"$(VDRVERSION)\"" 


ifdef BIGPATCH_INSTALLED
        DEFINES += -DBIGPATCH_INSTALLED
endif

ifdef LIVEBUFFER_PATCH_INSTALLED
        DEFINES += -DLIVEBUFFER_PATCH_INSTALLED
endif

ifdef OPEN7x0_Setup_DEBUG
	DEFINES += -DDEBUG
endif

### The object files (add further files here):
#OBJS     = $(PLUGIN).o  i18n.o menus.o setupmenu.o setupsetup.o config.o plugins.o sysconfig.o util.o setupsystemmenu.o
# removed setupsetup as we do not need it
OBJS     = $(PLUGIN).o  i18n.o menus.o setupmenu.o config.o plugins.o sysconfig.o util.o setupsystemmenu.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o

### Implicit rules:

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE):  setupsystemmenu.cpp Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.cpp)   > $@

-include $(DEPFILE)

### Targets:


all: libvdr-$(PLUGIN).so


libvdr-$(PLUGIN).so: setupsystemmenu.cpp $(OBJS) 
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -o $@
#	ifndef OPEN7x0_Setup_DEBUG
#		@$(STRIP) $@
#	endif
	@cp -f $@ $(LIBDIR)/$@.$(APIVERSION)

setupsystemmenu.cpp:  $(VDRDIR)/menu.c
#	@echo "Extracting class definitions from VDR's menu.c for standard Setup"
#	@chmod +x ./getVdrSetupMenu.pl
#	@./getVdrSetupMenu.pl $(VDRDIR)/menu.c

	
dist: clean
	@rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@rm -f $(TMPDIR)/$(ARCHIVE)/$(PLUGIN).kdevelop
	@rm -f $(TMPDIR)/$(ARCHIVE)/$(PLUGIN).kdevelop.filelist
	@rm -f $(TMPDIR)/$(ARCHIVE)/$(PLUGIN).kdevelop.pcs
	@rm -f $(TMPDIR)/$(ARCHIVE)/$(PLUGIN).kdevses
	@rm -rf $(TMPDIR)/$(ARCHIVE)/CVS
	@rm -rf $(TMPDIR)/$(ARCHIVE)/Examples/CVS
	@rm -rf $(TMPDIR)/$(ARCHIVE)/Examples/help/CVS
	@rm -rf $(TMPDIR)/$(ARCHIVE)/Patch/CVS
	@ln -s $(ARCHIVE) $(TMPDIR)/$(PLUGIN)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE) $(PLUGIN)
	@rm -rf $(TMPDIR)/$(ARCHIVE) $(TMPDIR)/$(PLUGIN)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(MAIN) $(DEPFILE) *.so *.tgz core* *~
