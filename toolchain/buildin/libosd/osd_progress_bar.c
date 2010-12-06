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
 * $Id: osd_progress_bar.c 201 2007-09-25 17:42:55Z andreas $
 *
 */
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "osd.h"
#include "osd_progress_bar.h"

#define BAR_BORDER_SIZE 4
#define BAR_BORDER_COLOR1 0xFF808080
#define BAR_BORDER_COLOR2 0xFFCCCCCC
#define MIN_BAR_WIDTH (BAR_BORDER_SIZE * 8)
#define MIN_BAR_HEIGHT (BAR_BORDER_SIZE * 4)


int osd_progress_init(struct osd_progress_bar *bar, int x, int y,
	int width, int height, int n_colors, uint8_t *colors, int max_value)
{
	uint8_t border_c1;
	uint8_t border_c2;
	if (width < MIN_BAR_WIDTH || height < MIN_BAR_HEIGHT || n_colors < 1 ||
			n_colors > MAX_BAR_COLORS || max_value < 0) {
		errno = EINVAL;
		return -1;
	}

	bar->x = x;
	bar->y = y;
	bar->width = width;
	bar->height = height;
	bar->n_colors = n_colors;
	memcpy(bar->colors, colors, n_colors);
	bar->max_value = max_value;
	bar->step = 1;
	bar->last_value = 0;

	border_c1 = osd_add_palette(BAR_BORDER_COLOR1);
	border_c2 = osd_add_palette(BAR_BORDER_COLOR2);

	osd_draw_rectangle(x, y, width, height, border_c1);
	osd_draw_rectangle(x + (BAR_BORDER_SIZE >> 1), y + (BAR_BORDER_SIZE >> 1),
		width - BAR_BORDER_SIZE, height - BAR_BORDER_SIZE, border_c2);

	osd_draw_rectangle(x + BAR_BORDER_SIZE, y + BAR_BORDER_SIZE,
		width - BAR_BORDER_SIZE * 2, height - BAR_BORDER_SIZE * 2, colors[0]);

	return 0;
}

int osd_progress_set_max_value(struct osd_progress_bar *bar, int max_value)
{
	if (max_value < 0) {
		errno = EINVAL;
		return -1;
	}

	bar->max_value = max_value;
	return 0;
}

int osd_progress_get_max_value(struct osd_progress_bar *bar)
{
	return bar->max_value;
}
int osd_progress_update(struct osd_progress_bar *bar, int step, int value)
{
	int64_t x;
	int64_t w;
	int64_t w_bar;
	if (bar->n_colors < step) {
		errno = EINVAL;
		return -1;
	}

	if (bar->step != step) {
		bar->last_value = 0;
		bar->step = step;
	}

	if (bar->max_value < value)
		value = bar->max_value;

	if (value <= bar->last_value)
		return 0;

	w_bar = bar->width - BAR_BORDER_SIZE * 2;
	x = (w_bar * bar->last_value) / bar->max_value;
	w = (w_bar * value) / bar->max_value;

	w -= x;
	x += bar->x + BAR_BORDER_SIZE;
	osd_draw_rectangle(x , bar->y + BAR_BORDER_SIZE, w,
		bar->height - BAR_BORDER_SIZE * 2, bar->colors[step]);
	bar->last_value = value;
	return 0;
}

int osd_progress_mark_range(struct osd_progress_bar *bar, int val1, int val2,
	int color_no)
{
	if (bar->n_colors < color_no) {
		errno = EINVAL;
		return -1;
	}

	if (bar->max_value <= val1) {
		return 0;
	}
	bar->step = color_no;
	bar->last_value = val1;
	return osd_progress_update(bar, color_no, val2);
}
