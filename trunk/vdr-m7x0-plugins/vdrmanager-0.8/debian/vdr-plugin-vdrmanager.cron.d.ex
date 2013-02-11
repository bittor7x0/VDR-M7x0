#
# Regular cron jobs for the vdr-plugin-vdrmanager package
#
0 4	* * *	root	[ -x /usr/bin/vdr-plugin-vdrmanager_maintenance ] && /usr/bin/vdr-plugin-vdrmanager_maintenance
