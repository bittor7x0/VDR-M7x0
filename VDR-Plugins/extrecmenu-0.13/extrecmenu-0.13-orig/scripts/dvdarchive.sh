#!/bin/bash
#
# Version 2.1 2006-06-02
#
# Author:	Mike Constabel
# VDR-Portal:	vejoun
# EMail:	vejoun @ toppoint.de
# WWW:          http://www.constabel.net/vdr/scripts.htm
#
# ---> CONFIGURATION AT LINE 79 <---
#
# MANUAL:
# -------
# 1.
# If you run VDR as user, you need in /etc/fstab the option "user" for your DVD mountpoint.
#
# Example:
# /dev/hdc /media/cdrom iso9660 defaults,ro,user,noauto 0 0
#
# 2.
# If you use a VFAT partition for your video-data, you must configure sudo to allow the
# VDR-user to execute mount and umount as root. To edit sudoers run 'visudo' command as root.
#
# Example:
# # Cmnd alias specification
# Cmnd_Alias SYSTEM = /bin/mount, /bin/umount
# # User privilege specification
# vdr	ALL=(root) NOPASSWD: SYSTEM
#
# With a vfat partition some things doesn't work: resume, marks
#
# 3.
# For dvd-in-drive detection compile isodetect.c and put it into the $PATH,
# usually /usr/local/bin/
# You find isodetect.c in the extrecmenu plugin source package.
#
# 4.
# Tools needed:   mount, awk, find, test, stat
# Optional tools: isodetect
#
# 5.
# If you have some DVDs with the index.vdr only on DVD, you don't use vfat and you
# want to see the recording length in the menu, you can switch GETLENGTH to 1 and
# the script will create a length.vdr for you.
#
# 6.
# If something went wrong, set DEBUG=1 and send me the file which is defined in $DEBUGLOG.
# Use VDR-Portal or EMail.
#
# 7.
# Exitcodes:
#
# exit 0 - no error
# exit 1 - mount/umount error
# exit 2 - no dvd in drive
# exit 3 - wrong dvd in drive / recording not found
# exit 4 - error while linking [0-9]*.vdr
# exit 5 - sudo or mount --bind / umount error (vfat system)
#
# HISTORY:
# --------
# 2.1 - Fixed bug in testing if dvd is mounted
#     - more DEBUG=1 output
#
# 2.0 - more logging
#     - check if mountpoint and device exists
#     - Debug Log in file $DEBUGLOG if $DEBUG is 1, for easier error reporting
#
# 1.9 - use "sudo mount --bind" for mounting if filesystem is vfat
#     - automatic fallback to 'sudo' and 'mount --bind' if filesystem is vfat
#     - mounting more failure tolerant
#     - added MANUAL part in script
#     - length.vdr creation, you must not use it ;)
#
# 1.8 - remove sudo, is not necessary
#     - on mount, if already mounted try to umount first
#     - logging per syslog, see $SYSLOG
#
# CONFIGURATION
# -------------
#<Configuration>

# Mountpoint, the same as in fstab
MOUNTPOINT="/media/cdrom" # no trailing '/'

# Eject DVD for exit-codes 2 and 3 (no or wrong dvd). 1 = yes, 0 = no.
EJECTWRONG=0

# Eject DVD after unmounting. 1 = yes, 0 = no.
EJECTUMOUNT=0

# Log warnings/errors in syslog. 1 = yes, 0 = no.
SYSLOG=0

# Create a length.vdr after mounting the dvd for the played recording. 1 = yes, 0 = no.
# Only for non-vfat and with index.vdr only on dvd.
GETLENGTH=0

# Put debug infos in file $DEBUGLOG. Only if $DEBUG=1.
DEBUG=0
DEBUGLOG="/tmp/dvdarchive.sh-debug.log"

#</Configuration>
#
# No changes needed after this mark

# Remove trailing slash
MOUNTPOINT=${MOUNTPOINT/%\/}
if [ -L "$MOUNTPOINT" ]; then
	MOUNTPOINTT="$(find "$MOUNTPOINT" -printf "%l")"
else
	MOUNTPOINTT="$MOUNTPOINT"
fi
# determine dvd-device, used by eject and isodetect if exists
DEVICE="$(awk '( $1 !~ /^#/ ) && ( $2 == "'$MOUNTPOINT'" ) { printf("%s", $1); exit; }' /etc/fstab)"
if [ -L "$DEVICE" ]; then
	DEVICET="$(find "$DEVICE" -printf "%l")"
else
	DEVICET="$DEVICE"
fi

ACTION="$1"
REC="$2"
NAME="$3"

# function to print help
call() {
	echo -e "\nScript $0 needs three parameters for mount and two for umount. The first must be mount or umount, the second is the full path.\n"
	echo -e "Only for mounting the script needs a third parameter, the last part of the recording path.\n"
	echo -e "Example: dvdarchive.sh mount '/video0/Music/%Riverdance/2004-06-06.00:10.50.99.rec' '2004-06-06.00:10.50.99.rec'\n"
	echo -e "Example: dvdarchive.sh umount '/video0/Music/%Riverdance/2004-06-06.00:10.50.99.rec'\n"
	echo -e "For more information read the MANUAL part inside this script.\n"
}

# function to log messages
log() {
	case "$1" in
	      info)
	   	echo -e "INFO: $2"
	   	[ $SYSLOG -eq 1 ] && logger -t "$0" "INFO: $2"
	   	;;
	   warning)
	   	echo -e "WARNING: $2"
	   	[ $SYSLOG -eq 1 ] && logger -t "$0" "WARNING: $2"
	   	;;
	     error)
	     	echo -e "ERROR: $2"
	     	[ $SYSLOG -eq 1 ] && logger -t "$0" "ERROR: $2"
	     	if [ $DEBUG -eq 1 ]; then
	     		echo "-------" >> $DEBUGLOG
	 	     	echo -e "Parameters: $ACTION $REC $NAME\n" >> $DEBUGLOG
	 	     	echo -e "ERROR: $2\n\n" >> $DEBUGLOG
	 	     	echo -e "Mountpoint: $MOUNTPOINT\nDevice: $DEVICE\n" >> $DEBUGLOG
	 	     	echo -e "MountpointT: $MOUNTPOINTT\nDeviceT: $DEVICET\n" >> $DEBUGLOG
	 	     	FSTAB="$(awk '( $1 !~ /^#/ ) && ( $2 == "'$MOUNTPOINT'" || $2 == "'$MOUNTPOINTT'" ) { printf("%s", $0); }' /etc/fstab)"
	 	     	echo -e "fstab: ${FSTAB}\n" >>$DEBUGLOG
	 	     	echo -e "Filesystem: $(stat -f -c %T "$REC")\n" >> $DEBUGLOG
		     	mount >> $DEBUGLOG
		     	echo >> $DEBUGLOG
		     	cat /proc/mounts >> $DEBUGLOG
		     	echo >> $DEBUGLOG
		     	sudo -l >> $DEBUGLOG
		fi
	     	;;
	esac		
}

# Some checks before doing something
[ "$ACTION" = "mount" -o "$ACTION" = "umount" ] || { call; exit 10; }
[ -z "$REC" ] && { call; exit 10; }
[ "$ACTION" = mount -a -z "$NAME" ] && { call; exit 10; }
[ ! -d "$MOUNTPOINT" ] && { log error "Mountpoint $MOUNTPOINT doesn't exist"; exit 10; }
[ ! -e "$DEVICE" ] && { log error "Device $DEVICE doesn't exist"; exit 10; }

case "$ACTION" in
mount)
	# check if dvd is in drive, only if isodetect exists
	if [ -n "$(which isodetect)" -a -n "$DEVICE" ]; then
		isodetect -d "$DEVICE" >/dev/null 2>&1
		if [ $? -ne 0 ]; then
			log warning "no dvd in drive"
			[ $EJECTWRONG -eq 1 ] && { eject "$DEVICE"; }
			exit 2
		fi
	fi
	# check if not mounted
	if mount | egrep -q " $MOUNTPOINTT "; then
		# check if dvd is in use
		if mount | egrep -q "^$DEVICET"; then
			log error "dvd in use (at: check if dvd is in use)"
			exit 1
		fi
		# if already mountet, try to umount
		log warning "dvd already mounted, try to umount"
		umount "$MOUNTPOINT" || { log error "dvd umount error (at: dvd already mounted, try to umount)"; exit 1; }
		# unlink broken existing links
		for LINK in "${REC}/"*.vdr; do
			if [ -L "$LINK" -a ! -s "$LINK" ]; then
				rm "$LINK"
			fi
		done
	fi
	# mount dvd
 	mount "$MOUNTPOINT" || { log error "dvd mount error (at: mount dvd)"; exit 1; }
 	# is mounted, find recording on dvd
	DIR="$(find "${MOUNTPOINT}/" -name "$NAME")"
	# if not found, umount
	if [ -z "$DIR" ]; then
		log error "wrong dvd in drive / recording not found on dvd"
		umount "$MOUNTPOINT" || { log error "dvd umount error (at: wrong dvd in drive / recording not found on dvd)"; exit 1; }
		# If wanted, eject dvd
		[ $EJECTWRONG -eq 1 ] && { eject "$DEVICE"; }
		exit 3
	fi
	# check if video partition is vfat
	if [ "$(stat -f -c %T "$REC")" != "vfat" ]; then
		# link index.vdr if not exist
		if [ ! -e "${REC}/index.vdr" ]; then
			cp -s "${DIR}/index.vdr" "${REC}/" || { log error "could not link index.vdr (at: link index.vdr from dvd to disk)"; }
		fi
		# link [0-9]*.vdr files
		cp -s "${DIR}"/[0-9]*.vdr "${REC}/"
		# error while linking [0-9]*.vdr files?
		if [ $? -ne 0 ]; then
			log error "error while linking [0-9]*.vdr"
			# umount dvd bevor unlinking
			umount "$MOUNTPOINT" || { log error "dvd umount error (at: error while linking)"; exit 1; }
			# unlink broken links
			for LINK in "${REC}/"*.vdr; do
				if [ -L "$LINK" -a ! -s "$LINK" ]; then
					rm "$LINK"
				fi
			done
			exit 4
		fi
		# If wanted, create length.vdr
		if [ $GETLENGTH -eq 1 -a ! -s "${REC}/length.vdr" -a -L "${REC}/index.vdr" ]; then
			 echo $(( $(stat -L -c %s "${REC}/index.vdr")/12000 )) > "${REC}/length.vdr"
		fi
	else
		if [ ! "$(sudo -l | egrep "\(root\) NOPASSWD: /bin/mount")" -o ! "$(sudo -l | egrep "\(root\) NOPASSWD: /bin/umount")" ]; then
			log error "you must configure sudo and allow $(whoami) to use mount/umount!"
			log info "$(sudo -l)"
			umount "$MOUNTPOINT" || { log error "dvd umount error (at: you must configure sudo)"; exit 1; }
			exit 5
		fi
		# mount recording
		sudo mount --bind "$DIR" "$REC"
		if [ $? -ne 0 ]; then
			log error "sudo mount --bind $DIR $REC"
			umount "$MOUNTPOINT" || { log error "dvd umount error (at: sudo mount --bind)"; exit 1; }
			exit 5
		fi
	fi
	;;
umount)
	# check if dvd is mounted
	mount | egrep -q " $MOUNTPOINTT " || { log error "dvd not mounted (at: check if dvd is mounted)"; exit 1; }
	# check if video partition is vfat
	if [ "$(stat -f -c %T "$REC")" != "vfat" ]; then
		# is mounted, umount dvd bevor unlinking
		umount "$MOUNTPOINT" || { log error "dvd umount error (at: is mounted, umount dvd bevor unlinking)"; exit 1; }
		# unlink broken links
		for LINK in "${REC}/"*.vdr; do
			if [ -L "$LINK" -a ! -s "$LINK" ]; then
				rm "$LINK"
			fi
		done
	else
		# umount recording
		sudo umount "$REC" || { log error "sudo umount $REC"; exit 5; }
		# umount dvd at umount
		umount "$MOUNTPOINT" || { log error "dvd umount error (at: umount dvd at umount)"; exit 1; }
	fi
	# If wanted, eject dvd
	[ $EJECTUMOUNT -eq 1 ] && { eject "$DEVICE"; }
	;;
     *)
     	# Output help
        log error "\nWrong action $ACTION."
        call
        ;;
esac

exit 0
