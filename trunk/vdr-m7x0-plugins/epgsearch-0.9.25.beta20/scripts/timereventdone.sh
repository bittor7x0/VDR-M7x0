#!/bin/bash
#
# Put the event of a timer to epgsearch's done file
#
# Use it as command in timercmds.conf
#
# Example:
#
# Mark as already recorded :  /usr/local/bin/timereventdone.sh
#
# 2008-02-09 Christian Wieninger
# Version 0.1
#

#<Configuration>

# path where epgsearch stores its data
CONFDIR="/etc/vdr/plugins/epgsearch"
SVDRPSEND="/usr/local/src/VDR/svdrpsend.pl"
#</Configuration>

DONEFILE=$CONFDIR/epgsearchdone.data
CHANNELID="$2"
START="$3"
STOP="$4"
TITLE="$5"
SUBTITLE="$6"
let DURATION=$STOP-$START

echo "" >> $DONEFILE
echo R $START $DURATION -1 >> $DONEFILE
echo C $CHANNELID >> $DONEFILE
echo T $TITLE >> $DONEFILE
echo S $SUBTITLE >> $DONEFILE
echo r >> $DONEFILE

# Tell epgsearch that done-file was changed
echo "$SVDRPSEND PLUG epgsearch UPDD >/dev/null 2>&1" | at now >/dev/null 2>&1;
