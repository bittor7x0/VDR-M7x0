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
 */

#include <stdint.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "osd.h"
#include "font.h"

#define FB_DEV "/dev/fb0"
#define FB_WIDTH OSD_WIDTH
#define FB_HEIGHT OSD_HEIGHT


static struct {
	int fb_fd;
	uint8_t *fb_mem;
	struct fb_cmap palette;
	uint16_t red[256];
	uint16_t green[256];
	uint16_t blue[256];
	uint16_t trans[256];
	int palette_changed;
	int palette_used;
} osd_ctx;

static void osd_get_palette(void) {
	osd_ctx.palette_changed = 1;
	osd_ctx.palette_used = 1;
	int err = 0;
	if (ioctl(osd_ctx.fb_fd, FBIOGETCMAP, &osd_ctx.palette) < 0) {
		err = 1;
	}

	int i;
	for (i = 0; i < 255 ; i++) {
		osd_ctx.trans[i] = 0xFF - (osd_ctx.trans[i] >> 8);
		if ((osd_ctx.red[i] == 0 && osd_ctx.green[i] == 0 &&
				osd_ctx.blue[i] == 0) || osd_ctx.trans[i] >= 0xFF  || err) {
			osd_ctx.red[i] = 0;
			osd_ctx.green[i] = 0;
			osd_ctx.blue[i] = 0;
			osd_ctx.trans[i] = 0xFF00;
		} else {
			osd_ctx.trans[i] = osd_ctx.trans[i] << 8;
			osd_ctx.palette_used = i + 1;
		}
	}

	osd_ctx.red[255] = 0;
	osd_ctx.green[255] = 0;
	osd_ctx.blue[255] = 0;
	osd_ctx.trans[255] = 0xFF00;
}

int osd_init(int clear)
{
	int err;
	int r;
	struct fb_var_screeninfo fb_info;

	if (osd_ctx.fb_mem)
		return 0;

	osd_ctx.fb_mem = MAP_FAILED;

	osd_ctx.fb_fd = open(FB_DEV, O_RDWR);
	err = errno;
	if (osd_ctx.fb_fd < 0)
		goto err_out;

	memset(&fb_info, 0, sizeof(fb_info));
	r = ioctl(osd_ctx.fb_fd, FBIOGET_VSCREENINFO, &fb_info);
	err = errno;
	if (r < 0)
		goto err_out_close;

	if (fb_info.bits_per_pixel != 8 | clear) {
		fb_info.bits_per_pixel = 8;
		r = ioctl(osd_ctx.fb_fd, FBIOPUT_VSCREENINFO, &fb_info);
		err = errno;
		if (r < 0)
			goto err_out_close;

		memset(&fb_info, 0, sizeof(fb_info));
		r = ioctl(osd_ctx.fb_fd, FBIOGET_VSCREENINFO, &fb_info);
		err = errno;
		if (r < 0)
			goto err_out_close;
	}

	err = ENODEV;
	if (fb_info.xres != FB_WIDTH || fb_info.yres != FB_HEIGHT ||
			fb_info.bits_per_pixel != 8 ||
			!(fb_info.vmode & FB_VMODE_INTERLACED) )
		goto err_out_close;

	osd_ctx.fb_mem = mmap(NULL, FB_WIDTH * FB_HEIGHT,
			PROT_READ | PROT_WRITE, MAP_SHARED, osd_ctx.fb_fd, 0);
	err = errno;
	if (osd_ctx.fb_mem == MAP_FAILED)
		goto err_out_unmap;

	osd_ctx.palette.start = 0;
	osd_ctx.palette.len = 256;
	osd_ctx.palette.red = osd_ctx.red;
	osd_ctx.palette.green = osd_ctx.green;
	osd_ctx.palette.blue = osd_ctx.blue;
	osd_ctx.palette.transp = osd_ctx.trans;

	if (clear)
		osd_clear();
	else
		osd_get_palette();

	return 0;
err_out_unmap:
	munmap(osd_ctx.fb_mem, FB_WIDTH * FB_HEIGHT);
err_out_close:
	close(osd_ctx.fb_fd);
err_out:
	errno = err;
	return -1;
}

void osd_deinit(int clear)
{
	if (clear)
		memset(osd_ctx.fb_mem, 0xff, FB_WIDTH * FB_HEIGHT);
	munmap(osd_ctx.fb_mem, FB_WIDTH * FB_HEIGHT);
	osd_ctx.fb_mem = NULL;
	close(osd_ctx.fb_fd);
}

void osd_set_palette(uint8_t color_index, uint32_t argb_color)
{
	osd_ctx.blue[color_index] = (argb_color & 0xFF) << 8;
	osd_ctx.green[color_index] = (argb_color & 0xFF00);
	osd_ctx.red[color_index] = (argb_color >> 8) & 0xFF00;
	osd_ctx.trans[color_index] = (0xFF - (argb_color >> 24)) << 8;
	osd_ctx.palette_changed = 1;
}

uint8_t osd_add_palette(uint32_t argb_color)
{
	uint16_t b;
	uint16_t g;
	uint16_t r;
	uint16_t t;
	int i;

	b = (argb_color & 0xFF) << 8;
	g = (argb_color & 0xFF00);
	r = (argb_color >> 8) & 0xFF00;
	t = (0xFF - (argb_color >> 24)) << 8;

	for (i = 1; i < osd_ctx.palette_used; i++) {
		if (osd_ctx.blue[i] == b && osd_ctx.green[i] == g &&
				osd_ctx.red[i] == r && osd_ctx.trans[i] == t)
			return i;
	}
	if (i < 255) {
		osd_ctx.palette_used++;
		osd_ctx.blue[i] = b;
		osd_ctx.green[i] = g;
		osd_ctx.red[i] = r;
		osd_ctx.trans[i] = t;
		osd_ctx.palette_changed = 1;
		return i;
	}
	return 0;
}

void osd_flush(void)
{
	if (osd_ctx.palette_changed) {
		osd_ctx.palette_changed = 0;
		if (ioctl(osd_ctx.fb_fd, FBIOPUTCMAP, &osd_ctx.palette) < 0) {
			osd_ctx.palette_changed = 1;
		}
	}

	msync(osd_ctx.fb_mem, FB_WIDTH * FB_HEIGHT, MS_SYNC);
}

void osd_clear(void)
{
	memset(osd_ctx.fb_mem, 0x00, FB_WIDTH * FB_HEIGHT);
	memset(osd_ctx.red, 0, 256 * sizeof(uint16_t));
	memset(osd_ctx.green, 0, 256 * sizeof(uint16_t));
	memset(osd_ctx.blue, 0, 256 * sizeof(uint16_t));
	memset(osd_ctx.trans, 0xFF, 256 * sizeof(uint16_t));
	osd_ctx.palette_changed = 1;
	osd_ctx.palette_used = 1;
	osd_flush();
}

void osd_draw_pixel(int x, int y, uint8_t color)
{
	int offset;
	if (x < 0 || x >= FB_WIDTH)
		return;

	if (y < 0 || y >= FB_HEIGHT)
		return;


	offset = (y >> 1) * FB_WIDTH + x;
	if (y & 1) {
		offset += FB_WIDTH * (FB_HEIGHT >> 1);
	}
	osd_ctx.fb_mem[offset] = color;
}

static void osd_draw_pixel_line(int x, int y, int length, uint8_t color)
{
	int offset;
	if (x < 0)
		return;

	if (y < 0 || y >= FB_HEIGHT)
		return;

	if (x + length > FB_WIDTH) {
		length = FB_WIDTH - x;
		if (length <= 0)
			return;
	}

	offset = (y >> 1) * FB_WIDTH + x;
	if (y & 1) {
		offset += FB_WIDTH * (FB_HEIGHT >> 1);
	}
	memset(osd_ctx.fb_mem + offset, color, length);
}

void osd_draw_rectangle(int x, int y, int width, int height, uint8_t color)
{
	for (;height > 0; height--,y++) {
		osd_draw_pixel_line(x, y, width, color);
	}
}

void osd_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
	uint8_t color)
{
	int x_tmp;
	int y_tmp;
	int y;
	int d_y1_3;
	int d_y1_2;
	int d_y2_3;
	int d_x1_3;
	int d_x1_2;
	int d_x2_3;

	if (y2 < y1 || (y2 == y1 && x2 < x1)) {
		x_tmp = x1; y_tmp = y1;
		x1 = x2; y1 = y2;
		x2 = x_tmp; y2 = y_tmp;
	}

	if (y3 < y1 || (y3 == y1 && x3 < x1)) {
		x_tmp = x1; y_tmp = y1;
		x1 = x3; y1 = y3;
		x3 = x_tmp; y3 = y_tmp;
	}

	if (y3 < y2 || (y3 == y2 && x3 < x2)) {
		x_tmp = x2; y_tmp = y2;
		x2 = x3; y2 = y3;
		x3 = x_tmp; y3 = y_tmp;
	}


	d_y1_3 = y3 - y1;
	if (d_y1_3 <= 0)
		d_y1_3 = 1;
	d_x1_3 = x3 - x1;

	d_y1_2 = y2 - y1;
	if (d_y1_2 <= 0)
		d_y1_2 = 1;
	d_x1_2 = x2 - x1;

	d_y2_3 = y3 - y2;
	if (d_y2_3 <= 0)
		d_y2_3 = 1;
	d_x2_3 = x3 - x2;

	for (y = y1; y <= y3; y++) {
		int x_d1;
		int x_d2;

		x_d1 = ((d_x1_3 * (y - y1) + (d_y1_3 >> 1)) / d_y1_3)  + x1;

		if (y < y2) {
			x_d2 = ((d_x1_2 * (y - y1) + (d_y1_2 >> 1)) / d_y1_2)  + x1;
		} else {
			x_d2 = ((d_x2_3 * (y - y2) + (d_y2_3 >> 1)) / d_y2_3)  + x2;
		}

		if (x_d1 <= x_d2) {
			osd_draw_pixel_line(x_d1, y, x_d2 - x_d1 + 1, color);
		} else {
			osd_draw_pixel_line(x_d2, y, x_d1 - x_d2 + 1, color);
		}
	}
}

static int osd_draw_char(int x, int y, int x2, char c, uint8_t fgColor)
{
	const uint32_t *char_data;
	int i;
	int j;

	char_data = font_get_char_data(c);

	if (x2 < x + char_data[0])
		return -1;

	for (i = 0; i < char_data[1]; i++) {
		uint32_t line_data = char_data[2 + i];
		for (j = char_data[0] - 1; j >= 0; j--) {
			if (line_data & 1) {
				osd_draw_pixel(x + j, y + i, fgColor);
			}
			line_data >>= 1;
		}
	}
   return x + char_data[0];
}

int osd_draw_text_line(int x, int y, int width, int height, const char *text,
	uint8_t bgColor, uint8_t fgColor, int align)
{
	int text_width;
	int text_height;
	int x2;

	text_width = font_get_text_line_width(text);
	text_height = font_get_line_height();
	if (!width) {
		width = text_width;
		align &= ~3;
	}
	if (!height) {
		height = text_height;
		align &= ~12;
	}
	if (x + width > FB_WIDTH) {
		width = FB_WIDTH - x;
	}
	if (width <= 0)
		return 0;
	if (y + height > FB_HEIGHT) {
		height = FB_HEIGHT - y;
	}
	if (height < text_height)
		return 0;

	if (bgColor != COLOR_INDEX_TRANSPARENT) {
		osd_draw_rectangle(x, y, width, height, bgColor);
	}

	if (align & TEXT_ALIGN_BOTTOM) {
		y += height - text_height;
	} else if (align & TEXT_ALIGN_MIDDLE) {
		y += (height - text_height) >> 1;
	}

	x2 = x + width;
	if (width > text_width) {
		if (align & TEXT_ALIGN_RIGHT) {
			x += width - text_width;
		} else if (align & TEXT_ALIGN_CENTER) {
			x += (width - text_width) >> 1;
		}
	}

	int c = 0;
	while (text[0]) {
		x = osd_draw_char(x, y, x2, text[0], fgColor);
		if (x < 0)
         return c;
		c++;
		text++;
	}
	return c;
}

void osd_move_region(int xsrc, int ysrc, int xdest, int ydest, int width,
	 int height)
{
	// Parameter checking
	if (xsrc == xdest && ysrc == ydest)
		return;
	if (xsrc < 0)
		xsrc = 0;
	if (xdest < 0)
		xdest = 0;
	if (xsrc + width > FB_WIDTH)
		width = FB_WIDTH - xsrc;
	if (xdest + width > FB_WIDTH)
		width = FB_WIDTH - xdest;
	if (width <= 0)
		return;

	if (ysrc < 0)
		ysrc = 0;
	if (ydest < 0)
		ydest = 0;
	if (ysrc + height > FB_HEIGHT)
		height = FB_HEIGHT - ysrc;
	if (ydest + height > FB_HEIGHT)
		height = FB_HEIGHT - ysrc;
	if (height <= 0)
		return;

	int x_overlap = ysrc == ydest &&
		((xdest >= xsrc && xdest < xsrc + width) ||
		(xsrc >= xdest && xsrc < xdest + width));
	int i;
   int foff[2] = {0, FB_WIDTH * (FB_HEIGHT >> 1)};
	if (ysrc >= ydest) {
		if (!x_overlap) {
			for (i = 0; i < height; i++) {
				int y1 = ydest + i;
				int y2 = ysrc + i;
				memcpy(osd_ctx.fb_mem + (y1 / 2) * FB_WIDTH + foff[y1&1] + xdest,
						osd_ctx.fb_mem + (y2 / 2) * FB_WIDTH + foff[y2&1] + xsrc, width);
			}
		} else {
			for (i = 0; i < height; i++) {
				int y1 = ydest + i;
				int y2 = ysrc + i;
				memmove(osd_ctx.fb_mem + (y1 / 2) * FB_WIDTH + foff[y1&1] + xdest,
						osd_ctx.fb_mem + (y2 / 2) * FB_WIDTH + foff[y2&1] + xsrc, width);
			}
		}
	} else {
		if (!x_overlap) {
			for (i = height - 1; i >= 0; i--) {
				int y1 = ydest + i;
				int y2 = ysrc + i;
				memcpy(osd_ctx.fb_mem + (y1 / 2) * FB_WIDTH + foff[y1&1] + xdest,
						osd_ctx.fb_mem + (y2 / 2) * FB_WIDTH + foff[y2&1] + xsrc, width);
			}
		} else {
			for (i = height - 1; i >= 0; i--) {
				int y1 = ydest + i;
				int y2 = ysrc + i;
				memmove(osd_ctx.fb_mem + (y1 / 2) * FB_WIDTH + foff[y1&1] + xdest,
						osd_ctx.fb_mem + (y2 / 2) * FB_WIDTH + foff[y2&1] + xsrc, width);
			}
		}
	}
}
