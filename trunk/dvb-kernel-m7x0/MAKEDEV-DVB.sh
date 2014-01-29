#!/bin/sh
# Create device nodes for the Linux DVB API with DVB_API_VERSION 2.
# The devices created are suitable for most current PC DVB cards,
# i.e. cards having one frontend, one demux and optionally one
# MPEG decoder.
# The script creates devices for four cards by default.

if [ -e /dev/.devfsd ]; then
        echo "It seems you are using devfs. Good!"
	exit 0
fi

# get rid of old DVB API devices; do it twice for good measure...
rm -rf /dev/ost
rm -rf /dev/ost
rm -rf /dev/dvb
rm -rf /dev/dvb

mkdir /dev/dvb
chmod 755 /dev/dvb

for i in `seq 0 3`; do
	echo "Creating DVB devices in /dev/dvb/adapter$i"
	mkdir /dev/dvb/adapter$i
	chmod 755 /dev/dvb/adapter$i
	mknod -m 0660 /dev/dvb/adapter$i/video0    c 212   `expr 64 \* $i + 0`
	mknod -m 0660 /dev/dvb/adapter$i/audio0    c 212   `expr 64 \* $i + 1`
	mknod -m 0660 /dev/dvb/adapter$i/frontend0 c 212   `expr 64 \* $i + 3`
	mknod -m 0660 /dev/dvb/adapter$i/demux0    c 212   `expr 64 \* $i + 4`
	mknod -m 0660 /dev/dvb/adapter$i/dvr0      c 212   `expr 64 \* $i + 5`
	mknod -m 0660 /dev/dvb/adapter$i/ca0       c 212   `expr 64 \* $i + 6`
	mknod -m 0660 /dev/dvb/adapter$i/net0      c 212   `expr 64 \* $i + 7`
	mknod -m 0660 /dev/dvb/adapter$i/osd0      c 212   `expr 64 \* $i + 8`
	chown root.video /dev/dvb/adapter$i/*
done

