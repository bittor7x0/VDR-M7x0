CC = gcc
LD = ld
CFLAGS = -MD -g -Wall -O2 -nostdinc -fno-builtin -I. -I./include -D__KERNEL__ -DCONFIG_DVB_DEVFS_ONLY

dvb-core-objs = dvb_compat.o \
	dmxdev.o \
	dvb_demux.o \
	dvb_filter.o \
	dvb_i2c.o \
	dvb_ringbuffer.o \
	dvbdev.o \
	dvb_frontend.o \
	\
	alps_bsrv2.o \
	alps_tdmb7.o \
	at76c651.o \
	cx24110.o \
	grundig_29504-401.o \
	grundig_29504-491.o \
	nxt6000.o \
	stv0299.o \
	ves1820.o \
	#dvb_net.o \

all: dvb-core.o

dvb-core.o: $(dvb-core-objs)
	$(LD) -r -o $@ $(dvb-core-objs)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.d
	rm -f `find . -type l`


-include $(wildcard *.d) dummy

