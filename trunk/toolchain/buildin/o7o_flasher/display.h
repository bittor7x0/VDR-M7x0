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
 * $Id: display.h 362 2008-08-17 17:14:52Z andreas $
 *
 */
#ifndef __DISPLAY_H
#define __DISPLAY_H 1
#include <libosd/libosd.h>

#define DISPLAY_KERNEL_BAR 0
#define DISPLAY_JFFS_BAR 1
#define DISPLAY_ROOTFS_BAR 2

#define DISPLAY_BAR_READ 2
#define DISPLAY_BAR_WRITTEN 3
#define DISPLAY_BAR_VERIFIED 4

int display_init(void);
void display_deinit(void);
int display_set_bar_max(int bar, int max);
int display_set_bar_free(int bar, int free_start);
int display_update_bar(int bar, int step, int value);
void display_msg(const char *txt);

#endif
