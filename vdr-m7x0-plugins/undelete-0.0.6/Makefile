#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile 0.4 2005/11/07 16:47:18 hflor Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = undelete

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -O2 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
VDRVERSNUM = $(shell grep 'define VDRVERSNUM ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### Test SVDRP
ifeq ($(shell grep -q ReplyCode $(VDRDIR)/plugin.h ; echo $$?),0)
  DEFINES += -DHAVE_SVDRP
endif

### Test Elchi
ifeq ($(shell test -f $(VDRDIR)/theme.h ; echo $$?),0)
  DEFINES += -DHAVE_ELCHI
endif

### Test wareagle-patch
ifeq ($(shell test -f $(VDRDIR)/iconpatch.h ; echo $$?),0)
  DEFINES += -DHAVE_ICONPATCH
endif

#for more debug lines
#DEFINES += -DUND_Debug1 #
#DEFINES += -DUND_Debug2 # log all functions with parameters
#DEFINES += -DUND_Debug3 # log all ProcessKey calls without kNone

### The object files (add further files here):

OBJS = $(PLUGIN).o i18n.o menuundelete.o menusetup.o vdrtools.o menueditkeys.o menudispkey.o
ifeq ($(shell test $(VDRVERSNUM) -lt 10308 ; echo $$?),0)
  OBJS += menutext.o
endif

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
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
