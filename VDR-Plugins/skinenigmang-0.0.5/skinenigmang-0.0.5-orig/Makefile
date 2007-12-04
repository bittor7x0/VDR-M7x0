#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.16 2007/06/25 10:44:11 amair Exp $

# If you are using the epgsearch plugin and want to see the number of
# timer conflicts in the main menu's info area.
#SKINENIGMA_HAVE_EPGSEARCH = 1

# This turns usage of logos in the main menu complete. This might also
# improve the performance of the menus. EXPERIMENTAL!!!
#SKINENIGMA_NO_MENULOGO = 1

# Debugging on/off 
#SKINENIGMA_DEBUG = 1

# If you have installed ImageMagick and want to use
# images in event's and recording's details.
#HAVE_IMAGEMAGICK = 1

# If you use the mailbox plugin this will include support for it.
# NOTE: this can also be defined if you don't know if the mailbox
# plugin will be used because it has no compile time requirements.
SKINENIGMA_USE_PLUGIN_MAILBOX = 1

# EXPERIMENTAL!!! NOT YET OFFICIALY SUPPORTED!!!
#
# USE AT OWN RISC!!!
#SKINENIGMA_USE_PLUGIN_AVARDS = 1

# Disable any code that is used for scrolling or blinking text.
# NOTE: this is only useful if you want to save some bytes because you
# can disable them in the setup too.
#DISABLE_ANIMATED_TEXT = 1

# Set the descriptions for fonts you've patched in VDR. These fonts then
# can be selected in EnigmaNG setup.
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
-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef SKINENIGMA_HAVE_EPGSEARCH
DEFINES += -DSKINENIGMA_HAVE_EPGSEARCH
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
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
ifndef SKINENIGMA_DEBUG
	@$(STRIP) $@
endif
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude CVS -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
