
./send ask -s "Confirmation test?"

res=$?

if [ $res -eq 0 ]; then 
    echo Failed
fi

if [ $res -eq 1 ]; then 
  echo success
fi

if [ $res -eq 2 ]; then 
  echo Confirmed
fi

if [ $res -eq 3 ]; then 
    echo Aborted
fi

