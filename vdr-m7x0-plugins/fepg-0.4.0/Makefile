#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = fepg

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char\* VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -g -O2 -Wall -Woverloaded-virtual

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### Test whether VDR has locale support

VDRLOCALE = $(shell grep '^LOCALEDIR' $(VDRDIR)/Makefile)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include 

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):

OBJS = $(PLUGIN).o fepgOsd.o menu.o tools.o screenMenu.o colorMenu.o i18n.o textWrapper.o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(subst i18n.c,,$(OBJS:%.o=%.c)) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Ndirs  = $(notdir $(foreach file, $(I18Npo), $(basename $(file))))
I18Npot   = $(PODIR)/$(PLUGIN).pot
I18Nvdrmo = vdr-$(PLUGIN).mo
ifeq ($(strip $(APIVERSION)),1.5.7)
  I18Nvdrmo = $(PLUGIN).mo
endif

ifneq ($(strip $(VDRLOCALE)),)

### do gettext based i18n stuff

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(subst i18n.c,,$(wildcard *.c))
	xgettext -C -cTRANSLATORS --no-wrap -F -k -ktr -ktrNOOP --msgid-bugs-address='yourmailaddress' -o $@ $(subst i18n.c,,$(wildcard *.c))
 
$(I18Npo): $(I18Npot)
	msgmerge -U --no-wrap -F --backup=none -q $@ $<

i18n: $(I18Npot) $(I18Nmo)
	@mkdir -p $(LOCALEDIR)
	for i in $(I18Ndirs); do\
	    mkdir -p $(LOCALEDIR)/$$i/LC_MESSAGES;\
	    cp $(PODIR)/$$i.mo $(LOCALEDIR)/$$i/LC_MESSAGES/$(I18Nvdrmo);\
	    done
	    
i18n.c: po2i18n/i18n-template.c 
	@cp po2i18n/i18n-template.c i18n.c

else ### do i18n.c based i18n stuff

i18n:
	@### nothing to do

#i18n compatibility generator:
i18n.c: po2i18n/i18n-template.c po2i18n/po2i18n.pl $(I18Npo)
	po2i18n/po2i18n.pl < po2i18n/i18n-template.c > i18n.c

endif

### Targets:

all: libvdr-$(PLUGIN).so i18n

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
	@-rm -f $(PODIR)/*.mo $(PODIR)/*~
	@-rm -f $(OBJS) $(DEPFILE) i18n.c *.so *.tgz core* *~
