#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = cinebars

### VERSNUM for i18n (taken from VDR's "config.h"):
VERSNUM = $(shell grep 'define VDRVERSNUM ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
VERS157 = $(shell if [ $(VERSNUM) -ge 10507 ]; then echo "1"; else echo "0"; fi)

### The version number of this plugin (taken from the main source file):
VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:
CXX      ?= g++
CXXFLAGS ?= -fPIC -O2 -Wall -Woverloaded-virtual

### The directory environment:
#DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:
-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):
APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
# need for vdr <= 1.3.47
ifeq ($(strip $(APIVERSION)),)
    APIVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
endif

### The version number of VDR (taken from VDR's "config.h"):
VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:
ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):
INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include
DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):
OBJS = $(PLUGIN).o
ifneq ($(VERS157),1)
OBJS += i18n.o
endif

### The main target:
all: libvdr-$(PLUGIN).so
	if [ $(VERS157) = 1 ]; then make i18n; fi

### Implicit rules:
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:
MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):
PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Ndirs  = $(notdir $(foreach file, $(I18Npo), $(basename $(file))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap -F -k -ktr -ktrNOOP --msgid-bugs-address='<egal@egal-vdr.de>' -o $@ $(wildcard *.c)

$(I18Npo): $(I18Npot)
	msgmerge -U --no-wrap -F --backup=none -q $@ $<

i18n: $(I18Nmo)
	@mkdir -p $(LOCALEDIR)
	for i in $(I18Ndirs); do\
	    mkdir -p $(LOCALEDIR)/$$i/LC_MESSAGES;\
	    cp $(PODIR)/$$i.mo $(LOCALEDIR)/$$i/LC_MESSAGES/vdr-$(PLUGIN).mo;\
	    done

### Targets:
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
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
