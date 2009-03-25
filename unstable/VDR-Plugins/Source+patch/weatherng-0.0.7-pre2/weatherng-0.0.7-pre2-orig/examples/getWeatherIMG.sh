#!/bin/sh
#
# weatherng.sh
#
# source: vdr-plugin weatherng
#

# To use this examples of urls , you must have the permission of owner to download and
# use them. Otherwise you must change the URLs. See README or README.DE.

# Change SETUPDIR if you need.
SETUPDIR=.

#LANGUAGE=( `grep -s ^OSDLanguage.*[0-9]$ "$SETUPDIR/setup.conf"` )
LANGUAGE="1"

case "${LANGUAGE}" in
	1) MESG='Download abgeschlossen und Radarmaps aktualisiert'
	   URLS=(\
		http://www.dwd.de/scripts/getimg.php?src=/wundk/Wetter.jpg \
		http://image.weather.com/images/sat/germany_sat_720x486.jpg \
#		http://www.wetter24.de/automatic/brd_tag_dd00002.jpg \
#		http://www.wetter24.de/automatic/brd_tag_tt00002.jpg \
#		http://www.wetter24.de/automatic/brd_tag_rr00002.jpg \
		)
	;;
	7) MESG='Téléchargement des données Météo OK'
	   URLS=(\
		http://meteo.france3.fr/img/f3-france-512x384-j0pm.jpg \
		http://meteo.france3.fr/img/f3-ouest-512x384-j0pm.jpg \
		http://meteo.france3.fr/img/f3-ouest-512x384-j1am.jpg \
		http://meteo.france3.fr/img/f3-ouest-512x384-j1pm.jpg \
		http://meteo.france3.fr/img/f3-ouest-512x384-j2am.jpg \
		)
	;;
	8) MESG='Sääkuvat päivitetty'
	   URLS=(\
		http://weather.fmi.fi/yle/suomineito_1.gif \
		http://www.yle.fi/saakartat/image1.jpg \
		http://www.yle.fi/saakartat/isokartta.jpg \
		http://www.yle.fi/saakartat/imgday1.jpg \
		http://www.yle.fi/saakartat/imgday2.jpg \
		)
	;;
	*) MESG='Weather pictures updated'
	   URLS=(\
		"" \
		"" \
		"" \
		"" \
		"" \
		)
	;;
esac

for i in $(seq 0 10) ; do
    eval ${URLS[$i]:+ wget -t 4 -T 20 "${URLS[$i]}" -O "$SETUPDIR/pic${i}.jpg"}
    eval ${URLS[$i]:+ convert "$SETUPDIR/pic${i}.jpg" XPM:pic${i}.xpm}
done

# Dont change or uncomment this and the following 5 lines. ;)
# Uncomment download only  if you have a permission from The Weather Channel (see README).
# otherwise get one ;) and do whatever you want.
# !!!! DONT FORGET TO CHANE YOUR STATIONIDs !!!!!! (See README or README.DE how to get them.
# Example for Herne,Germany [GMXX0057]with a permission from The weather channel:
# e.g :  wget -t 4 -T 20 "http://xoap.weather.com/weather/local/GMXX0057?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data1.xml"

#wget -t 4 -T 20 "http://xoap.weather.com/weather/local/STATIONID1?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data1.xml"
#wget -t 4 -T 20 "http://xoap.weather.com/weather/local/STATIONID2?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data2.xml"
#wget -t 4 -T 20 "http://xoap.weather.com/weather/local/STATIONID3?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494" -O "$SETUPDIR/plugins/weatherng/data3.xml"

echo $MESG
