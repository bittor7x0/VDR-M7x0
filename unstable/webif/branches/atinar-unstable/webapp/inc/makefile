-include ../../linux-mips-uclibc.tc
DEBUG  = -g -ggdb -DDEBUG 
COMMON_CFLAGS = -I$(KLONE_SRC) $(UCLIBC_CFLAGS) -D_GNU_SOURCE $(DEBUG)
CFLAGS  = $(COMMON_CFLAGS) -I$(KLONE_SRC)/libu/include/

all: browse.o channels.o conf.o epg.o i18n.o misc.o partitions.o recording.o recordings.o searches.o svdrp_comm.o timers.o

%.o:%.c %.h
	${CC} ${CFLAGS} -c $<
	# ${STRIP} --strip-debug $@

clean:
	rm -f *.o
