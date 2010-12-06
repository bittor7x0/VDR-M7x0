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
 * $Id: osd_progress_bar.h 148 2007-08-30 15:58:15Z andreas $
 *
 */
#ifndef __LIBOSD_OSD_PROGRESS_BAR_H
#define __LIBOSD_OSD_PROGRESS_BAR_H 1
#include <stdint.h>

#define MAX_BAR_COLORS 10
struct osd_progress_bar {
	int x;
	int y;
	int width;
	int height;
	int n_colors;
	uint8_t colors[MAX_BAR_COLORS];
	int step;
	int last_value;
	int max_value;
};

int osd_progress_init(struct osd_progress_bar *bar, int x, int y,
	int width, int height, int n_colors, uint8_t *colors, int max_value);
int osd_progress_set_max_value(struct osd_progress_bar *bar, int max_value);
int osd_progress_get_max_value(struct osd_progress_bar *bar);
int osd_progress_update(struct osd_progress_bar *bar, int step, int value);
int osd_progress_mark_range(struct osd_progress_bar *bar, int val1, int val2,
	int color_no);
#endif
