#CC	    = mips-linux-uclibc-gcc
#STRIP   = mips-linux-uclibc-strip
-include ../../linux-mips-uclibc.tc
CFLAGS  = $(UCLIBC_CFLAGS) -I$(KLONE_SRC)/libu/include/

all: svdrp_comm.o svdrp_parse.o channels.o timers.o misc.o recordings.o epg.o i18n.o conf.o conf-i18n.o

svdrp_comm.o: svdrp_comm.h svdrp_comm.c
	${CC} ${CFLAGS} -c svdrp_comm.c
	${STRIP} --strip-debug svdrp_comm.o

svdrp_parse.o: svdrp_parse.h svdrp_parse.c
	${CC} ${CFLAGS} -c svdrp_parse.c
	${STRIP} --strip-debug svdrp_parse.o
  
channels.o: channels.h channels.c
	${CC} ${CFLAGS} -c channels.c
	${STRIP} --strip-debug channels.o
	
timers.o: timers.h timers.c
	${CC} ${CFLAGS} -c timers.c
	${STRIP} --strip-debug timers.o
	
recordings.o: recordings.h recordings.c
	${CC} ${CFLAGS} -c recordings.c
	${STRIP} --strip-debug recordings.o
  
epg.o: epg.h epg.c
	${CC} ${CFLAGS} -c epg.c
	${STRIP} --strip-debug epg.o

misc.o: misc.h misc.c
	${CC} ${CFLAGS} -c misc.c
	${STRIP} --strip-debug misc.o
	
i18n.o: i18n.h i18n.c
	${CC} ${CFLAGS} -c i18n.c
	${STRIP} --strip-debug i18n.o
	
conf.o: conf.h conf.c
	${CC} ${CFLAGS} -c conf.c
	${STRIP} --strip-debug conf.o
	
conf.o: conf-i18n.h conf-i18n.c
	${CC} ${CFLAGS} -c conf-i18n.c
	${STRIP} --strip-debug conf-i18n.o
	
clean:
	rm -f *.o
