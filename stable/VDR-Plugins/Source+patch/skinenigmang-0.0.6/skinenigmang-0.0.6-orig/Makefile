#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.24 2008/03/09 08:31:30 amair Exp $

# This turns usage of logos in the main menu complete. This might also
# improve the performance of the menus. EXPERIMENTAL!!!
#SKINENIGMA_NO_MENULOGO = 1

# Debugging on/off 
#SKINENIGMA_DEBUG = 1

# If you have installed ImageMagick and want to use
# images in event's and recording's details.
#HAVE_IMAGEMAGICK = 1

# If you are using the epgsearch plugin and want to see the number of
# timer conflicts in the main menu's info area.
SKINENIGMA_USE_PLUGIN_EPGSEARCH = 1

# If you use the mailbox plugin this will include support for it.
SKINENIGMA_USE_PLUGIN_MAILBOX = 1

# Include code to support the Avards plugin:
#  - Dynamic OSD size depending on current WSS mode.
#  - Display current WSS mode in channel info and replay OSDs.
SKINENIGMA_USE_PLUGIN_AVARDS = 1

# Disable any code that is used for scrolling or blinking text.
# NOTE: this is only useful if you want to save some bytes because you
# can disable them in the setup too.
#SKINENIGMA_DISABLE_ANIMATED_TEXT = 1

# Set the descriptions for fonts you've patched in VDR. These fonts then
# can be selected in EnigmaNG setup.
# This is NOT the path to TrueType fonts!
#SKINENIGMA_FONTS = "\"Test Font\", \"Test2 Font\""

# If you have installed FreeType2 and want to use TrueTypeFonts.
#HAVE_FREETYPE = 1

# Strip debug symbols?  Set eg. to /bin/true if not
#STRIP = strip
STRIP = /bin/true

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = skinenigmang

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char VERSION\[\] *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -g -O2 -Wall -Woverloaded-virtual

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:
#TODO
CLEAR_BUG_WORKAROUND = 1
ENABLE_COPYFONT = 1
-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### Test whether VDR has locale support
VDRLOCALE = $(shell grep '^LOCALEDIR' $(VDRDIR)/Makefile)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef SKINENIGMA_USE_PLUGIN_EPGSEARCH 
DEFINES += -DUSE_PLUGIN_EPGSEARCH
else
# for backwards compatibility only
ifdef SKINENIGMA_HAVE_EPGSEARCH
DEFINES += -DUSE_PLUGIN_EPGSEARCH
endif
endif

ifdef SKINENIGMA_DEBUG
DEFINES += -DDEBUG
endif

ifdef SKINENIGMA_NO_MENULOGO
DEFINES += -DSKINENIGMA_NO_MENULOGO
endif

ifdef SKINENIGMA_USE_PLUGIN_MAILBOX
DEFINES += -DUSE_PLUGIN_MAILBOX
endif

ifdef SKINENIGMA_USE_PLUGIN_AVARDS
DEFINES += -DUSE_PLUGIN_AVARDS
endif

ifdef HAVE_IMAGEMAGICK
DEFINES += -DHAVE_IMAGEMAGICK
endif
DEFINES += -DRECORDING_COVER='"Cover-Enigma"'

ifdef SKINENIGMA_DISABLE_ANIMATED_TEXT
DEFINES += -DDISABLE_ANIMATED_TEXT
endif

ifdef CLEAR_BUG_WORKAROUND
DEFINES += -DCLEAR_BUG_WORKAROUND
endif

ifdef ENABLE_COPYFONT
DEFINES += -DENABLE_COPYFONT
endif

DEFINES += -DSKINENIGMA_FONTS=$(SKINENIGMA_FONTS)

### The object files (add further files here):

OBJS = $(PLUGIN).o enigma.o config.o logo.o i18n.o tools.o status.o texteffects.o setup.o

ifdef HAVE_IMAGEMAGICK
OBJS += bitmap.o
LIBS += -lMagick++
endif

ifdef HAVE_FREETYPE
	ifneq ($(shell which freetype-config),)
		INCLUDES += $(shell freetype-config --cflags)
		LIBS += $(shell freetype-config --libs)
	else
		INCLUDES += -I/usr/include/freetype -I/usr/local/include/freetype
		LIBS += -lfreetype
	endif
	DEFINES += -DHAVE_FREETYPE
	OBJS += font.o
endif

### The main target:

all: libvdr-$(PLUGIN).so i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
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

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(subst i18n.c,,$(wildcard *.c))
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<andreas@vdr-developer.org>' -o $@ $(subst i18n.c,,$(wildcard *.c))

$(I18Npo): $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<

ifneq ($(strip $(VDRLOCALE)),)
### do gettext based i18n stuff

i18n: $(I18Nmo)
	@mkdir -p $(LOCALEDIR)
	for i in $(I18Ndirs); do\
	    mkdir -p $(LOCALEDIR)/$$i/LC_MESSAGES;\
	    cp $(PODIR)/$$i.mo $(LOCALEDIR)/$$i/LC_MESSAGES/$(I18Nvdrmo);\
	    done

i18n.c: i18n-template.c
	@cp i18n-template.c i18n.c

else ### do i18n.c based i18n stuff

i18n:
	@### nothing to do

#i18n compatibility generator:
i18n.c: i18n-template.c buildtools/po2i18n.pl $(I18Npo)
	buildtools/po2i18n.pl < i18n-template.c > i18n.c

endif


### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
ifndef SKINENIGMA_DEBUG
	@$(STRIP) $@
endif
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean i18n.c
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude CVS --exclude '.#*' --exclude '*.bak' -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) i18n.c *.so *.tgz core* *~ $(PODIR)/*.mo $(PODIR)/*.pot
