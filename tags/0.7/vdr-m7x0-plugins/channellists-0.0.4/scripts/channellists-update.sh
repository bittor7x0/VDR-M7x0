#!/bin/sh

# channellists-update.sh
# by Marc Wernecke - www.zulu-entertainment.de
# 17.01.2008

RELOAD="off"
[ "$1" = "reload" ] && RELOAD="on"

CHANNELS_CONF_NAME="/var/lib/vdr/plugins/channellists/channels.conf.linowsat"

[ -f /tmp/channels.conf ] && rm -f /tmp/channels.conf

cd /tmp
wget --quiet --tries=1 http://www.linowsat.de/settings/vdr/0192/ca/channels.conf

if [ ! -f /tmp/channels.conf ]; then
#   echo "Update failed!"
   svdrpsend.pl mesg "Update failed!"
   exit 1
fi

mv -f /tmp/channels.conf "$CHANNELS_CONF_NAME"
[ "$RELOAD" = "on" ] && svdrpsend.pl plug channellists load "$CHANNELS_CONF_NAME"

#echo "Update done!"
svdrpsend.pl mesg "Update done!"
exit 0
