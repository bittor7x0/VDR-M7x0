#!/bin/sh

opts=
for opt in $@ ; do
   case $opt in
       -mcpu=*)
           opts="$opts -mtune=${opt##-mcpu=}"
           ;;
       *)
          opts="$opts $opt"
   esac
done
`dirname $0`/as.orig $opts


