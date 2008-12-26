-include ../../linux-mips-uclibc.tc
#DEBUG  = -g -ggdb -DDEBUG 
CFLAGS  = $(UCLIBC_CFLAGS) ${DEBUG} -I$(KLONE_SRC) -I$(KLONE_SRC)/libu/include/

all: svdrp_comm.o svdrp_parse.o channels.o timers.o misc.o recordings.o epg.o i18n.o conf.o conf-i18n.o recording.o

%.o:%.c %.h
	${CC} ${CFLAGS} -c $<
	${STRIP} --strip-debug $@

clean:
	rm -f *.o
