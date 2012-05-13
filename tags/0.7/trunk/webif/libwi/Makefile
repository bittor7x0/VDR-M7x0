-include common.mk
-include ../vars.mk

# name of the library
LIB = wi

# list of source files
SRCS = basic_auth.c channels.c commands.c conf.c css.c epg.c fragment.c hooks.c i18n.c live.c
SRCS += message.c misc.c partitions.c playlist.c
SRCS += resource.c rsmedia.c rsprpfnd.c rsvdrrec.c rsvirt.c searches.c settings.c streaming.c
SRCS += svdrp_comm.c timers.c vdrwi.c

# include ${KLONE_CUSTOM_TC}
#DEBUG  = -g -ggdb -DDEBUG

# compilation flags
CFLAGS += $(COMMON_CFLAGS) $(KLONE_CFLAGS) -I$(KLONE_SRC)/libu/include/

-include lib.mk

clean:
	rm -f *.o libwi.a
