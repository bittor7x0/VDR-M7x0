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
 * $Id: libosd.c 316 2008-02-19 19:36:28Z andreas $
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "libosd.h"


#define AVSWCMD_TV_FBAS    0x80
#define AVSWCMD_TV_SVIDEO  0x81
#define AVSWCMD_TV_VCR     0x82
#define AVSWCMD_TV_OFF     0x84
#define AVSWCMD_VCR_FBAS   0x85
#define AVSWCMD_VCR_SVIDEO 0x86
#define AVSWCMD_MODE_4_3   0x90

static void libosd_setup_mode(void)
{
	int avsw_fd;
	char *vdr_tv_mode;
	int vdr_tv_mode_val = 0;

	vdr_tv_mode = getenv("VDR_TV_MODE");
	if (vdr_tv_mode) {
		vdr_tv_mode_val = atoi(vdr_tv_mode);
	}

	avsw_fd = open("/dev/avswitch", O_WRONLY);
	if (avsw_fd < 0)
		return;

	if (vdr_tv_mode_val == 1) {
		ioctl(avsw_fd, AVSWCMD_TV_SVIDEO, 0);
		ioctl(avsw_fd, AVSWCMD_VCR_SVIDEO, 0);
	} else {
		ioctl(avsw_fd, AVSWCMD_TV_FBAS, 0);
		ioctl(avsw_fd, AVSWCMD_VCR_FBAS, 0);
	}
	ioctl(avsw_fd, AVSWCMD_MODE_4_3, 0);
	close(avsw_fd);
}

static void libosd_setup_off(void)
{
	int avsw_fd;

	avsw_fd = open("/dev/avswitch", O_WRONLY);
	if (avsw_fd < 0)
		return;

	ioctl(avsw_fd, AVSWCMD_TV_OFF, 0);
	ioctl(avsw_fd, AVSWCMD_TV_VCR, 0);
	close(avsw_fd);
}

int libosd_init(int clear)
{
	libosd_setup_mode();
	i18n_init(-1);
	font_init(i18n_encoding());
	return osd_init(clear);
}

void libosd_deinit(int tvoff,int clear)
{
   if (tvoff)
	   libosd_setup_off();
	i18n_deinit();
	osd_deinit(clear);
}
