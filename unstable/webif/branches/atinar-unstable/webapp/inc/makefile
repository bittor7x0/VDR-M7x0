-include ../../linux-mips-uclibc.tc
#DEBUG  = -g -ggdb -DDEBUG 
COMMON_CFLAGS = -I$(KLONE_SRC) $(UCLIBC_CFLAGS) -D_GNU_SOURCE $(DEBUG)
CFLAGS  = $(COMMON_CFLAGS) -I$(KLONE_SRC)/libu/include/

all: svdrp_comm.o channels.o timers.o misc.o recordings.o epg.o i18n.o conf.o recording.o browse.o

%.o:%.c %.h
	${CC} ${CFLAGS} -c $<
	${STRIP} --strip-debug $@

clean:
	rm -f *.o
