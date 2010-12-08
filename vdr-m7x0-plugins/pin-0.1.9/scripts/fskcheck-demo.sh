

./fskcheck check

case "$?" in
  0)
     echo "check went wrong"
     ;;

  1)
     echo "not protected"
     ;;

  2)
     echo "protected"
     ./fskcheck info "Can't execute script due to pin protection"
     ;;

  *)
     echo "unexpected return code"
     ;;

esac
