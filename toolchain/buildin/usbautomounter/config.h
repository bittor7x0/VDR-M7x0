/*
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * $Id: config.h 138 2007-07-17 08:32:24Z andreas $
 *
 */
#ifndef __CONFIG_H
#define __CONFIG_H 1
#define CONFIG_MTAB "/proc/mounts"
#define CONFIG_DEFAULT_MOUNT_OPTS "noatime,nodiratime,nosuid,nodev"
#define CONFIG_CONFIG_FILE "/etc/usbautomounter/usbam.conf"
#define CONFIG_DEFAULT_TABLE_FILE "/etc/usbautomounter/usbam_mtable.conf"
#define CONFIG_DEFAULT_MOUNT_PREFIX "/var/media"
#define CONFIG_DEFAULT_MOUNT_UNKNOWN_PREFIX "disk-volume-"
#define USB_MOUNTER_VERSION_STR "0.0.1"
#endif
