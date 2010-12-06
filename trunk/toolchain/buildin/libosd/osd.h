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
 * $Id: osd.h 316 2008-02-19 19:36:28Z andreas $
 *
 */
#ifndef __LIBOSD_OSD_H
#define __LIBOSD_OSD_H 1
#include <stdint.h>
/* Text alignment */
#define OSD_WIDTH 720
#define OSD_HEIGHT 576
#define OSD_HKEEPOUT 40
#define OSD_VKEEPOUT 30
#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_CENTER 1
#define TEXT_ALIGN_RIGHT 2
#define TEXT_ALIGN_TOP 0
#define TEXT_ALIGN_MIDDLE 4
#define TEXT_ALIGN_BOTTOM 8
#define TEXT_ALIGN_DEFAULT (TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP)

/* Reserved Color Indexes */
#define COLOR_INDEX_BACKGROUND 0
#define COLOR_INDEX_TRANSPARENT 255

int osd_init(int clear);
void osd_deinit(int clear);
void osd_set_palette(uint8_t color_index, uint32_t argb_color);
uint8_t osd_add_palette(uint32_t argb_color);
void osd_flush(void);
void osd_clear(void);
void osd_draw_pixel(int x, int y, uint8_t color);
void osd_draw_rectangle(int x, int y, int width, int height, uint8_t color);
void osd_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
	uint8_t color);
int osd_draw_text_line(int x, int y, int width, int height, const char *text,
	uint8_t bgColor, uint8_t fgColor, int align);
void osd_move_region(int xsrc, int ysrc, int xdest, int ydest, int width,
	 int height);
#endif
