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
#include <string.h>
#include "font.h"
#include "font-iso8859-1.c"
#include "font-iso8859-2.c"
#include "font-iso8859-5.c"
#include "font-iso8859-7.c"
#include "font-iso8859-13.c"
#include "font-iso8859-15.c"

#define FONT_COUNT 6
static const struct {
	const char *encoding;
	const uint32_t *data;
} fonts[FONT_COUNT] = {
	{"iso-8859-1", (const uint32_t *) font_iso8859_1},
	{"iso-8859-2", (const uint32_t *) font_iso8859_2},
	{"iso-8859-5", (const uint32_t *) font_iso8859_5},
	{"iso-8859-7", (const uint32_t *) font_iso8859_7},
	{"iso-8859-13", (const uint32_t *) font_iso8859_13},
	{"iso-8859-15", (const uint32_t *) font_iso8859_15}
};

static struct {
	const uint32_t *font_data;
	int char_height;
} font_ctx;

void font_init(const char *encoding)
{
	int i;
	for (i = 0; i < FONT_COUNT; i++) {
		if (!strcasecmp(fonts[i].encoding,  encoding)) {
			font_ctx.font_data = fonts[i].data;
			font_ctx.char_height = fonts[i].data[1];
			return;
		}
	}
	font_ctx.font_data = fonts[0].data;
	font_ctx.char_height = fonts[0].data[1];
}

const uint32_t *font_get_char_data(char c)
{
	int uc;
   int offset;

	uc = ((uint8_t) c) - 32;
	if (uc < 0)
		uc = 0;

	offset = uc * (font_ctx.char_height + 2);

	return (font_ctx.font_data + offset);
}

int font_get_char_width(char c)
{
	const uint32_t *c_data;

	c_data = font_get_char_data(c);
	return c_data[0];
}

int font_get_text_line_width(const char *text)
{
	int width;

	width = 0;
	while (text[0]) {
		width += font_get_char_width(text[0]);
		text++;
	}
	return width;
}

int font_get_line_height(void)
{
	return font_ctx.char_height;
}
