#
# Copy FSK protection of the PIN plugin when cutting a protected recording
#

if [ $1 == "edited" ]; then
    target_recording="$2"
    source_recording=`echo $2 | sed s/"%"/""/ `
    protection_file="$source_recording/protection.fsk"
    
    if [ -e "$protection_file" ]; then
        cp "$protection_file" "$target_recording/"
    fi
fi

