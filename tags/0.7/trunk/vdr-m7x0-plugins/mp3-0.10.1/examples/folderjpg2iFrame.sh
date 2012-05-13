#!/bin/sh
FORCE=0
# Option '-o' => Bestehenden '.iFrame' überschreiben
[[ -z `echo $* | grep "\-o"` ]] || FORCE=1

IFS_SIK=$IFS     #originalen IFS sichern
IFS=$'\n' #set \n als trenner für for

for i in `find -type f -name 'folder.jpg' -true`
do
  echo "***************  covert $i ... ***************"
  if [ -f "${i/.jpg/.iFrame}" ] && [ "$FORCE" -eq 0 ]; then
    echo "skipping existing ${i/.jpg/.iFrame}"
  else
#   echo "ffmpeg -i '$i' -vcodec mpeg2video -s 720x576 -qscale 4 -intra -an -y '${i/.jpg/.m2v}'"
    `ffmpeg -i $i -vcodec mpeg2video -s 720x576 -qscale 4 -intra -an -y ${i/.jpg/.m2v}`
    `mv -f ${i/.jpg/.m2v} ${i/.jpg/.iFrame}`
  fi
done

IFS=$OLD_IFS

