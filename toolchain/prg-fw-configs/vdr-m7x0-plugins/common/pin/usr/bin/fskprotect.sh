
fskcheck check

if [ $? != 1 ]; then
     fskcheck info "Please enable pin protection first"
     exit 1
fi

if [ "$1" == "protect" ]; then

   if [ -f $2/protection.fsk ]; then 
      fskcheck info "Recording was already protected!"
      exit 1
   fi

   touch $2/protection.fsk
   fskcheck info "Recording now protected!"

fi

if [ "$1" == "unprotect" ]; then

   if [ ! -f $2/protection.fsk ]; then
      echo fskcheck info "Recording was not protected!"
      exit 1
   fi

   rm -f $2/protection.fsk
   fskcheck info "Removed protection of recording!"

fi

exit 0

