#!/bin/sh
#
# weatherng.sh
#
# source: vdr-plugin weatherng
#

# To use this examples of urls , you must have the permission of owner to download and
# use them. Otherwise you must change the URLs. See README or README.DE.

# Change SETUPDIR if you need.
SETUPDIR=/var/media/disk-volume-0/home/vdr
wget="$SETUPDIR/plugins/weatherng/busybox wget"

rm "$SETUPDIR/plugins/weatherng/pic0.xpm"
$wget "http://10.10.11.254/wetter/pic0.xpm" -O "$SETUPDIR/plugins/weatherng/pic0.xpm"
#wget "http://10.10.11.254/wetter/pic1.xpm" -O "$SETUPDIR/plugins/weatherng/pic1.xpm"
#wget "http://www.wetter24.de/automatic/brd_tag_dd00002.jpg" -O "$SETUPDIR/plugins/weatherng/pic2.xpm"
#wget "http://www.wetter24.de/automatic/brd_tag_tt00002.jpg" -O "$SETUPDIR/plugins/weatherng/pic3.xpm"
#wget "http://www.wetter24.de/automatic/brd_tag_rr00002.jpg" -O "$SETUPDIR/plugins/weatherng/pic4.xpm"

# Dont change or uncomment this and the following 5 lines. ;)
# Uncomment download only  if you have a permission from The Weather Channel (see README).
# otherwise get one ;) and do whatever you want.
# !!!! DONT FORGET TO CHANE YOUR STATIONIDs !!!!!! (See README or README.DE how to get them.
# Example for Herne,Germany [GMXX0057]with a permission from The weather channel:
# e.g :  wget -t 4 -T 20 "http://xoap.weather.com/weather/local/GMXX0057?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data1.xml"

rm "$SETUPDIR/plugins/weatherng/data1.xml"
$wget "http://xoap.weather.com/weather/local/GMXX0040?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data1.xml"
#wget "http://xoap.weather.com/weather/local/STATIONID2?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data2.xml"
#wget "http://xoap.weather.com/weather/local/STATIONID3?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data3.xml"

echo "Download abgeschlossen und Radarmaps aktualisiert"
