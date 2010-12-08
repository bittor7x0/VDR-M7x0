PLUGIN    = games
VERSION   = 0.6.3
GAMES     = minesweeper \
            tictactoe   \
            tetris      \
            snake       \
            tron
OBJS      = launcher.o  \
            display.o   \
            screen.o    \
            bitmap.o    \
            text.o
VDROBJS   = plugin.o
SDLOBJS   = main.o
DEFINES   = -DVERSION=\"$(VERSION)\"
CFLAGS    = -g -pipe -O2 -Wall -Woverloaded-virtual
LFLAGS    = -pthread
VDRDIR    = ../../..
LIBDIR    = ../../lib
DVBDIR    = ../../../../DVB
PACKAGE   = vdr-games-$(VERSION)
ARCHIVES  = $(shell for GAME in $(GAMES); do echo $$GAME/$$GAME.a; done)
SDLGAMES  = $(shell test -f $(VDRDIR)/config.h || echo true )
VDRVER    = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | \
                    awk '{ print $$3 }' | sed -e 's/"//g' )
VDRNUM    = $(shell grep 'define VDRVERSNUM ' $(VDRDIR)/config.h | \
                    awk '{ print $$3 }' | sed -e 's/"//g' )

ifeq "$(SDLGAMES)" "true"

	TARGET    = sdl-games
	OBJS     += $(SDLOBJS)
	CFLAGS   += `sdl-config --cflags`
	LFLAGS   += `sdl-config --libs`
	DEFINES  += -DSDL -DVERBOSE

else

	# The version number of VDR's plugin API (taken from VDR's "config.h"):
	APIVERSION = $(shell sed -ne '/define APIVERSION/ { s/^.*"\(.*\)".*$$/\1/; p }' $(VDRDIR)/config.h)

	# This is required for VDRs older than 1.3.47
	ifeq ($(strip $(APIVERSION)),)
		APIVERSION = $(shell sed -ne '/define VDRVERSION/ { s/^.*"\(.*\)".*$$/\1/; p }' $(VDRDIR)/config.h)
	endif

	-include $(VDRDIR)/Make.config

	TARGET    = libvdr-games.so
	OBJS     += $(VDROBJS)
	CFLAGS   += -fPIC -I$(VDRDIR) -I$(DVBDIR)/include
	LFLAGS   += -shared --whole-archive
	DEFINES  += -D_GNU_SOURCE -DVDRVERSNUM=$(VDRNUM)

endif

all:
	@echo
	@echo -n "Building $(TARGET) "
	@for GAME in $(GAMES); do cd $$GAME; make -s all; cd -; done
	@rm -f $(TARGET)
	@make -s $(TARGET)
	@echo " done"
	@echo

libvdr-games.so: $(OBJS)
	@g++ $(OBJS) $(LFLAGS) $(ARCHIVES) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

sdl-games: $(OBJS)
	@g++ $(OBJS) $(LFLAGS) $(ARCHIVES) -o $@

install: all
	@cp $(LIBDIR)/libvdr-games.so.$(VDRVER) /mnt/vdr/lib/plugins/

tar: clean
	@echo -n "Creating archive ..."
	@cd .. ; cp -a vdr-games games-$(VERSION)
	@cd .. ; for GAME in $(GAMES); do rm -rf games-$(VERSION)/$$GAME/CVS; done
	@cd .. ; rm -rf games-$(VERSION)/CVS
	@cd .. ; tar cfz $(PACKAGE).tar.gz games-$(VERSION)
	@cd .. ; rm -rf games-$(VERSION)
	@echo " done"
	@echo

clean:
	@echo
	@echo -n "Cleaning source tree "
	@for GAME in $(GAMES); do cd $$GAME; make -s clean; cd -; done
	@rm -f *.o *.a $(TARGET)
	@echo " done"
	@echo

.cpp.o:
	@echo -n "."
	@$(COMPILE.cpp) $(DEFINES) $(CFLAGS) -c $< $(OUTPUT_OPTION)
