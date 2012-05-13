#!/bin/sh
#
# jpg2xpm.sh based on weatherng.sh
#
# source: vdr-plugin weatherng
#

# To use this examples of urls , you must have the permission of owner to download and
# use them. Otherwise you must change the URLs. See README or README.DE.

# Change SETUPDIR to the www path of you httpd task.
SETUPDIR=/home/notes/data/domino/html/wetter

UTCDATE=`date -u +%Y%m%d`
UTCHOUR=`echo "scale=0; if (\`date -u +%H%M\`<1014 && \`date -u +%H%M\`>0014) print 0; \
  if (\`date -u +%M\`/15==0) print (\`date +%H\`)-1 else print (\`date +%H\`)" | bc`
UTCHOUR=$UTCHOUR`echo "scale=0; if (\`date -u +%M\`/15==0) print 45 else \
  if (\`date -u +%M\`/15==1) print 0,0 else print (\`date -u +%M\`/15)*15-15" | bc`
 # find other images following link "Regionalradar" http://www.wetter.com/v2/?SID=&LANG=DE&LOC=0280&LOCFROM=0280&region=BB
 # this image also needs clipping, see below
 # recommended settings in plugin-config: radar image left/top/width/height 160/80/400/400 

URLS=(\
	http://www.dwd.de/scripts/getimg.php?src=/wundk/Wetter.jpg \
	http://www.tagesschau.de/import/wetter/de-vs-tn.jpg \
	http://www.tagesschau.de/import/wetter/de-vs-tt.jpg \
	http://www.tagesschau.de/import/wetter/de-vs-3t.jpg \
	http://imgs.www.wetter.com/v2/img/wx/RADAR_w30/radar_DE/v2/l/radar_DE__$UTCDATE$UTCHOUR.gif \
	)

REGEX='s/gray\([0-9][0-9]\)\"/#\1\1\1\"/;s/gray\([0-9]\)\"/#0\10\10\1\"/;s/gray100/#A0A0A0/;s/DimGray/#707070/'
echo $REGEX

for i in $(seq 0 10) ; do
    eval ${URLS[$i]:+ echo "Download: ${URLS[$i]} to $SETUPDIR/pic${i}.xpm"}	
    eval ${URLS[$i]:+ wget -t 4 -T 20 "${URLS[$i]}" -O "/tmp/jpgxmp_tmp.${URLS[$i]##*.}"}
    eval ${URLS[$i]:+ convert "/tmp/jpgxmp_tmp.${URLS[$i]##*.}" XPM:"$SETUPDIR/pic${i}.xpm"}
    eval ${URLS[$i]:+ cp "$SETUPDIR/pic${i}.xpm" "$SETUPDIR/pic${i}a.xpm"}
    eval ${URLS[$i]:+ `sed -i -e "s/gray\([0-9][0-9]\)\"/#\1\1\1\"/;s/gray\([0-9]\)\"/#0\10\10\1\"/;s/gray100/#A0A0A0/;s/DimGray/#707070/" "$SETUPDIR/pic${i}.xpm"`}
done

rm "/tmp/jpgxmp_tmp.jpg"
rm "/tmp/jpgxmp_tmp.gif"