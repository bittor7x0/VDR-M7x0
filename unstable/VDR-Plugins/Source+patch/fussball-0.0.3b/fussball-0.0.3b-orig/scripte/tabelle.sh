#! /bin/sh

wget -O /tmp/blitztabelle.htm http://linpop.zdf.de/sport/buli/blitztabelle.htm
sed -e \
's/&nbsp;9/09/' -e \
's/&nbsp;8/08/' -e \
's/&nbsp;7/07/' -e \
's/&nbsp;6/06/' -e \
's/&nbsp;18/18/' -e \
's/&nbsp;17/17/' -e \
's/&nbsp;16/16/' -e \
's/&nbsp;15/15/' -e \
's/&nbsp;14/14/' -e \
's/&nbsp;13/13/' -e \
's/&nbsp;12/12/' -e \
's/&nbsp;11/11/' -e \
's/&nbsp;10/10/' -e \
's/&nbsp;5/05/' -e \
's/&nbsp;4/04/' -e \
's/&nbsp;3/03/' -e \
's/&nbsp;2/02/' -e \
's/&nbsp;1/01/' -e \
's/Schalke 04/Schalke 04............./' -e \
's/MSV Duisburg/MSV Duisburg............../' -e \
's/Stuttgart/Stuttgart............./' -e \
's/ln/ln................./' -e \
's/Bremen/Bremen............./' -e \
's/nchen/nchen........./' -e \
's/Hamburger SV/Hamburger SV............../' -e \
's/Leverkusen/Leverkusen......./' -e \
's/Dortmund/Dortmund........./' -e \
's/Wolfsburg/Wolfsburg............./' -e \
's/Bielefeld/Bielefeld........./' -e \
's/Hannover 96/Hannover 96.............../' -e \
's/nberg/nberg............./' -e \
's/Mainz 05/Mainz 05............/' -e \
's/Bochum/Bochum......./' -e \
's/Rostock/Rostock..../' -e \
's/Freiburg/Freiburg....../' -e \
's/Berlin/Berlin........./' -e \
's/Kaiserslautern/Kaiserslautern......./' -e \
's/Frankfurt/Frankfurt......./' -e \
's/gladbach/gladbach......./' -e \
's/Aachen/Aachen....../' -e \
's/Cottbus/Cottbus......../' -e \
's/Bochum/Bochum........./' -e \
's/<img.*.">//' -e 's/<td.*.">//' -e 's/<td>//' -e 's/<.*.td>//' -e 's/.*<tr.*">//' -e 's/<.*\/tr>//' -e \
's/<tr>//' -e 's/<table.*>//' -e 's/<*.table>//' -e 's/<meta.*>//' -e 's/<!DOCTYPE HTML.*>//' -e 's/<html.*>//' -e 's/<*.head.*>//' -e \
's/<link.*>//' -e 's/<body.*>//' -e 's/-Ticker.*>//' -e 's/<title.*>//' -e 's/Champion.*e//' -e 's/CL-Qualif.*n//' -e 's/UEFA-Cup//' -e \
's/Abstieg//' -e 's/<*.body>//' -e 's/<*.html>//' -e 's/Blitztabelle//' -e 's/Mannschaft//' -e 's/Sp//' -e 's/Tore//' -e 's/P//' -e \
's/ZD.*l//g' /tmp/blitztabelle.htm > /tmp/test2.txt 

sed -e '/^[^0-9a-zA-Z!-\/]*$/d' /tmp/test2.txt > /tmp/test3.txt

if [ -s /tmp/test3.txt ]; then
 rm /tmp/tabelle.txt > /dev/null
 a5="GO"
 line=1
 while [ ! -z "$a5" ]
 do
 a1=`sed -n "$line"p /tmp/test3.txt` # Platzierung
 line=`expr "$line" + 1`
 a2=`sed -n "$line"p /tmp/test3.txt` # Club
 line=`expr "$line" + 1`
 a3=`sed -n "$line"p /tmp/test3.txt` # Spiele
 line=`expr "$line" + 1`
 a4=`sed -n "$line"p /tmp/test3.txt` # Torverhältnis
 line=`expr "$line" + 1`
 a5=`sed -n "$line"p /tmp/test3.txt` # Punkte
 line=`expr $line + 1`
 echo "$a1 $a2 $a3 $a4 $a5" >> /tmp/test4.txt
 done
rm /tmp/blitztabelle.htm
rm /tmp/test2.txt
rm /tmp/test3.txt
 sed '19,20d' /tmp/test4.txt > /tmp/tabelle.txt
rm /tmp/test4.txt
fi
cat /tmp/tabelle.txt
