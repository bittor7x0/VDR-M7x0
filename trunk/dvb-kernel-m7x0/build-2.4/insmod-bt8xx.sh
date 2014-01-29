#!/bin/sh

case "$1" in

    start|load)
        echo -n -e "Inserting DVB modules into kernel"
        # Linux video
        insmod videodev.o
        # bttv is required for bt878 and dvb-bt8xx
	# card=0x68 is vital FOR THE DST CARD
        insmod bttv.o card=0x68 i2c_hw=1
        # core DVB
        insmod ./dvb-core.o
        insmod ./v4l1-compat.o
        insmod ./v4l2-common.o
        insmod ./video-buf.o
	# Choose one of:
        # Nebula DVB-T card
        insmod ./nxt6000.o
        # or Pinnacle PCTV
        insmod ./cx24110.o
	# Avermedia DVB-T
	insmod ./sp887x.o
	# or DST
	insmod ./dst.o
        insmod ./bt878.o
        insmod ./dvb-bt8xx.o
        ;;
    stop|unload)
        echo -n -e "Removing DVB modules from kernel"
	# Nebula DVB-T card
        rmmod dvb-bt8xx bt878 nxt6000 cx24110 sp887x dst
        # core DVB
        rmmod dvb-core
        # bttv (required for bt878 and dvb-bt8xx)
        rmmod tuner bttv
        # Linux video
        rmmod videodev video-buf
        echo
        ;;
    restart|reload)
        $0 stop && $0 start
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
esac

exit 0

