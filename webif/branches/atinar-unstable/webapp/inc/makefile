#CC	    = mips-linux-uclibc-gcc
#STRIP   = mips-linux-uclibc-strip
-include ../../linux-mips-uclibc.tc
CFLAGS  = $(UCLIBC_CFLAGS) -I$(KLONE_SRC)/libu/include/

all: svdrp_comm.o svdrp_parse.o channels.o timers.o misc.o recordings.o epg.o i18n.o conf.o conf-i18n.o

%.o:%.c %.h
	${CC} ${CFLAGS} -c $<
	${STRIP} --strip-debug $@

clean:
	rm -f *.o
