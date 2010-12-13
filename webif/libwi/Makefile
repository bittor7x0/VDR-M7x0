-include common.mk
-include ../vars.mk

# name of the library
LIB = wi

# list of source files
SRCS = basic_auth.c browse.c channels.c conf.c epg.c hooks.c i18n.c misc.c partitions.c recording.c recordings.c searches.c svdrp_comm.c timers.c

# include ${KLONE_CUSTOM_TC}
#DEBUG  = -g -ggdb -DDEBUG

# compilation flags
CFLAGS += $(COMMON_CFLAGS) $(KLONE_CFLAGS) -I$(KLONE_SRC)/libu/include/ -DIGNORE_TABS

-include lib.mk

clean:
	rm -f *.o libwi.a
