#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.34 2010/03/08 14:31:57 amair Exp $

# This turns usage of logos in the main menu complete. This might also
# improve the performance of the menus. EXPERIMENTAL!!!
#SKINENIGMA_NO_MENULOGO = 1

# Debugging on/off 
#SKINENIGMA_DEBUG = 1

# If you have installed ImageMagick and want to use images in events'
# and recordings' details.  Set to GRAPHICS to use GraphicsMagick.
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

# Disable code to show signal strength and signal-to-noise ratio
# in channel info OSD.
#SKINENIGMA_DISABLE_SIGNALINFO = 1

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
CXXFLAGS ?= -g -O2 -Wall -Woverloaded-virtual -Wno-parentheses

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Make sure that necessary options are included:

-include $(VDRDIR)/Make.global

### Allow user defined options to overwrite defaults:
-include $(VDRDIR)/Make.config

#CXXFLAGS += -Wall -W -Wconversion -Wshadow -Wpointer-arith -Wcast-align -Woverloaded-virtual -Wwrite-strings
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

ifdef SKINENIGMA_DISABLE_SIGNALINFO
DEFINES += -DDISABLE_SIGNALINFO
endif

ifdef HAVE_IMAGEMAGICK
DEFINES += -DHAVE_IMAGEMAGICK
endif
DEFINES += -DRECORDING_COVER='"Cover-Enigma"'

# This is a simple workaround if one wants to use
# softdevice plugin without a single 8bpp area,
# because this combination shows some false colored
# areas in menu OSD.
ifdef CLEAR_BUG_WORKAROUND
DEFINES += -DCLEAR_BUG_WORKAROUND
endif

### The object files (add further files here):

OBJS = $(PLUGIN).o enigma.o config.o logo.o tools.o status.o texteffects.o setup.o font.o

ifdef HAVE_IMAGEMAGICK
OBJS += bitmap.o
ifneq ($(HAVE_IMAGEMAGICK), GRAPHICS)
LIBS += -lMagick++
INCLUDES += -I/usr/include/ImageMagick -I/usr/local/include/ImageMagick
else
LIBS += $(shell pkg-config --libs GraphicsMagick++)
INCLUDES += $(shell pkg-config --cflags GraphicsMagick++)
endif
endif

ifneq ($(shell which freetype-config),)
	INCLUDES += $(shell freetype-config --cflags)
	LIBS += $(shell freetype-config --libs)
else
	INCLUDES += -I/usr/include/freetype -I/usr/local/include/freetype
	LIBS += -lfreetype
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
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<andreas@vdr-developer.org>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none --no-fuzzy-matching -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

.PHONY: i18n
i18n: $(I18Nmsgs) $(I18Npot)

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) $(LIBS) -o $@
ifndef SKINENIGMA_DEBUG
	@$(STRIP) $@
endif
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude CVS --exclude '.#*' --exclude '*.bak' -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ $(PODIR)/*.mo $(PODIR)/*.pot
