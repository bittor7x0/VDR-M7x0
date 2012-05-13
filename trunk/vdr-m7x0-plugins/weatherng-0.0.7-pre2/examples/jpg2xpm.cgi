#!/bin/sh
#
# Convert JPG from given URL to XPM and give it back to the requester
# 
#
#echo "Content-type:  text/plain"
echo "Content-type:  image/xpm"
echo ""
#echo "${1/\\?/?}"

/usr/bin/wget -q --output-document="/tmp/jpgxmp_tmp.jpg" "${1/\\?/?}"
/usr/bin/convert "/tmp/jpgxmp_tmp.jpg" XPM:"/tmp/jpgxmp_tmp.xpm"

/bin/cat "/tmp/jpgxmp_tmp.xpm"

/bin/rm "/tmp/jpgxmp_tmp.jpg"
/bin/rm "/tmp/jpgxmp_tmp.xpm"
