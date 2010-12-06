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
 * $Id: osd_test.c 263 2007-10-22 20:04:12Z gambler $
 *
 */

#include <stdio.h>
#include <unistd.h>
#include "tools.h"
#include "../libosd.h"

static const i18n_text_t osd_test_texts[] = {
	{
	"OSD Test En",
	"OSD Test De",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Prueba OSD Es",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	},{
	NULL
	}
};

int main(int argc, char **argv)
{
	uint8_t bar_colors[5];
	uint8_t text_color;
	struct osd_progress_bar bar;
	int i;
	int val;
	if (libosd_init()) {
		PRINT_ERROR("Cannot init libosd");
		return 1;
	}
	if (i18n_register(osd_test_texts)) {
		PRINT_ERROR("Cannot register i18n texts");
		return 1;
	}
	bar_colors[0] = osd_add_palette(0xFFFFFFFF);
	bar_colors[1] = osd_add_palette(0xFF3535B3);
	bar_colors[2] = osd_add_palette(0xFF991F1F);
	bar_colors[3] = osd_add_palette(0xFFD9D900);
	bar_colors[4] = osd_add_palette(0xFF61B312);
	text_color = osd_add_palette(0xFFFFFFFF);
	osd_draw_text_line(0,30,720,40, tr("OSD Test En"),
		COLOR_INDEX_TRANSPARENT, text_color,TEXT_ALIGN_MIDDLE|TEXT_ALIGN_CENTER);
	if (osd_progress_init(&bar, 50, 280, 620, 30, 5, bar_colors, 6 * 1024 * 1024)) {
		PRINT_ERROR("Cannot init bar");
		return 1;
	}

	if (osd_progress_mark_range(&bar, 5312 * 1024, 6 * 1024 * 1024, 1)) {
		PRINT_ERROR("Cannot mark bar");
		return 1;
	}

	osd_flush();

	for (i = 2; i < 5; i++) {
		for (val = 0; val <= 5312 * 1024; val += 64 * 1024) {
			osd_progress_update(&bar, i, val);
			osd_flush();
			usleep(100000);
		}
	}
	sleep(10);
	libosd_deinit();
	return 0;
}
