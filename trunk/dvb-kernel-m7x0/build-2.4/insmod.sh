#!/bin/sh
# insmod modules from current directory without having to install them first

sync

case "$1" in
    load)
	echo -n -e "Inserting DVB modules into kernel"
	# make sure input stuff is there for IR remote controls
	modprobe input
	modprobe evdev
	# vide4linux for av7110 based "full featured" cards
	modprobe videodev
	insmod ./v4l1-compat.o
	insmod ./v4l2-common.o
	insmod ./video-buf.o
	# DVB core
	insmod ./dvb-core.o
	# frontend drivers
	insmod ./ves1x93.o
	insmod ./alps_tdlb7.o
	insmod ./alps_tdmb7.o
	insmod ./stv0299.o
	insmod ./ves1820.o
	insmod ./tda1004x.o
	insmod ./grundig_29504-401.o
	insmod ./grundig_29504-491.o
	insmod ./cx24110.o
	insmod ./mt312.o	
	# saa7146 based siemens/technotrend/hauppauge cards
	insmod ./saa7146.o
	insmod ./saa7146_vv.o
	insmod ./ttpci-eeprom.o
	insmod ./dvb-ttpci.o
	insmod ./dvb-ttpci-budget.o
	insmod ./dvb-ttpci-budget-ci.o
	insmod ./dvb-ttpci-budget-av.o
	# technotrend/hauppauge USB things
	insmod ./dvb-ttusb-budget.o
	insmod ./ttusb_dec.o
	# technisat skystar2
	insmod ./skystar2.o
	echo
	;;
    debug)
	echo -n -e "Inserting DVB modules (debug) into kernel"
	modprobe videodev
	insmod ./v4l1-compat.o
	insmod ./v4l2-common.o
	insmod ./video-buf.o
	insmod ./dvb-core.o
	insmod ./ves1x93.o
	insmod ./alps_tdlb7.o
	insmod ./alps_tdmb7.o
	insmod ./stv0299.o
	insmod ./ves1820.o
	insmod ./tda1004x.o
	insmod ./grundig_29504-401.o
	insmod ./grundig_29504-491.o
	insmod ./cx24110.o
	insmod ./saa7146.o
	insmod ./saa7146_vv.o saa7146_debug=247
	insmod ./ttpci-eeprom.o
	insmod ./dvb-ttpci.o av7110_debug=247
	insmod ./dvb-ttpci-budget.o
	insmod ./dvb-ttpci-budget-ci.o
	insmod ./dvb-ttpci-budget-av.o
	insmod ./dvb-ttusb-budget.o
	insmod ./mt312.o
	insmod ./skystar2.o debug=1
	insmod ./ttusb_dec.o
	echo
	;;
    unload)
	echo -n -e "Deleting DVB modules from kernel"
	rmmod skystar2 mt312 dvb-ttpci dvb-ttpci-budget dvb-ttpci-budget-av \
		dvb-ttpci-budget-ci \
		ttusb_dec dvb-ttusb-budget ttpci-eeprom \
		ves1x93 alps_tdmb7 alps_tdlb7 stv0299 ves1820 \
		tda1004x grundig_29504-401 grundig_29504-491 cx24110 \
		saa7146_vv saa7146 \
		video-buf v4l2-common v4l1-compat dvb-core videodev 
	echo
	;;
    reload)
	$0 unload && $0 load
	;;
    *)
	echo "Usage$0 {load|unload|debug|reload}"
	exit 1
esac

sync
