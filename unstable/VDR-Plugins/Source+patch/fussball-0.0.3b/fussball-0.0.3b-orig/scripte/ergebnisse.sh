#!/bin/sh

BULIURL="http://linpop.zdf.de/sport/buli/head.php"
SORT=/usr/local/bin/sort.pl

GET="wget -O - -q"

CONTENT=`$GET $BULIURL |sed  -e '/.* - .*/!d' -e '/.* - .*/{n;!p;}' |sed -e 's/.*<td.*0">//' -e 's/<\/a><\/td>//'  -e 's/.*<td.*">//' -e 's/<.*\/td>//' -e 's/\(.*:.*\)/ \1\n/' |grep -v "<.*>"` 
IFS=""

for i in $CONTENT
do
  START=`echo "$i"|grep \-`
  if [ "$START"x != "x" ]
  then
    echo ""
    echo "$i"
  else
    echo "$i"
    echo ""
  fi
done

echo $CONTENT | $SORT > /tmp/ergebnisse.txt