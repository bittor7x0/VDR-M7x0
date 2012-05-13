#!/bin/sh
# This script creates .iFrame files from .jpg files using ffmpeg
# !! It DOSEN'T respect the ratio of the jpg files !!
# If you can better use the pic2iFrame.sh from Klaus
# as this perl script keeps the ratio of the jpg when
# it resizes the images

FORCE=0
# Option '-o' => Bestehenden '.iFrame' überschreiben
[[ -z `echo $* | grep "\-o"` ]] || FORCE=1

IFS_SIK=$IFS     #originalen IFS sichern
IFS=$'\n' #set \n als trenner für for

for i in `find -type f -name '*.[Jj][Pp][Gg]' -true`
do
  echo "***************  covert $i ... ***************"
  if [ -f "${i/.[Jj][Pp][Gg]/.iFrame}" ] && [ "$FORCE" -eq 0 ]; then
    echo "skipping existing ${i/.jpg/.iFrame}"
  else
#   echo "ffmpeg -i '$i' -vcodec mpeg2video -s 720x576 -qscale 4 -intra -an -y '${i%.*}.m2v'"
    `ffmpeg -i $i -vcodec mpeg2video -aspect 16:9 -qscale 1 -intra -an -y ${i%.*}.m2v`
    `mv -f ${i%.*}.m2v ${i%.*}.iFrame`
  fi
done

IFS=$OLD_IFS
