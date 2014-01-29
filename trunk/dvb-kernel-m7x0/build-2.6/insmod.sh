#!/bin/sh
# insmod modules from current directory without having to install them first

sync

case "$1" in
    load)
	echo "Inserting DVB modules into kernel"
	# make sure input stuff is there for IR remote controls
	modprobe evdev
	# vide4linux for av7110 based "full featured" cards
	modprobe videodev
	modprobe v4l1-compat
	modprobe v4l2-common
	modprobe video-buf
	# DVB core
	insmod ./dvb-core.ko
	# frontend drivers
	insmod ./ves1x93.ko
	insmod ./alps_tdlb7.ko
	insmod ./alps_tdmb7.ko
	insmod ./stv0299.ko
	insmod ./ves1820.ko
	insmod ./tda1004x.ko
	insmod ./grundig_29504-401.ko
	insmod ./grundig_29504-491.ko
	insmod ./cx24110.ko
	insmod ./mt312.ko	
	# saa7146 based siemens/technotrend/hauppauge cards
	insmod ./saa7146.ko
	insmod ./saa7146_vv.ko
	insmod ./ttpci-eeprom.ko
	insmod ./dvb-ttpci.ko
	insmod ./budget-core.ko
	insmod ./budget.ko
	insmod ./budget-ci.ko
	insmod ./budget-av.ko
	# technotrend/hauppauge USB things
	insmod ./dvb-ttusb-budget.ko
	insmod ./ttusb_dec.ko
	# technisat skystar2
	insmod ./skystar2.ko
	echo
	;;
    debug)
	echo "Inserting DVB modules (debug) into kernel"
	modprobe videodev
	modprobe evdev
	modprobe v4l1-compat
	modprobe v4l2-common
	modprobe video-buf
	insmod ./dvb-core.ko
	insmod ./ves1x93.ko
	insmod ./alps_tdlb7.ko
	insmod ./alps_tdmb7.ko
	insmod ./stv0299.ko
	insmod ./ves1820.ko
	insmod ./tda1004x.ko
	insmod ./grundig_29504-401.ko
	insmod ./grundig_29504-491.ko
	insmod ./cx24110.ko
	insmod ./saa7146.ko
	insmod ./saa7146_vv.ko saa7146_debug=247
	insmod ./ttpci-eeprom.ko
	insmod ./dvb-ttpci.ko av7110_debug=247
	insmod ./budget-core.ko
	insmod ./budget.ko
	insmod ./budget-ci.ko
	insmod ./budget-av.ko
	insmod ./dvb-ttusb-budget.ko
	insmod ./mt312.ko
	insmod ./skystar2.ko debug=1
	insmod ./ttusb_dec.ko
	echo
	;;
    unload)
	echo "Deleting DVB modules from kernel"
	rmmod skystar2 mt312 dvb-ttpci budget budget-av budget-ci budget-core \
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
