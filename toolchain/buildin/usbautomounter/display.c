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
 * $Id: display.c 321 2008-02-22 17:21:34Z andreas $
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "display.h"
#include "tools.h"

#define TEXT_COLOR 0xfffcfcfc
#define LINE_COLOR 0xfff07800
#define INIT_COLOR 0xffb2b2b2
#define CHECKED_COLOR 0xff268035
#define BAR_WIDTH (OSD_WIDTH - (2 * (OSD_HKEEPOUT + 20)))
#define BAR_HEIGHT 30
#define BAR_XPOS ((OSD_WIDTH - BAR_WIDTH) / 2)

/* Texts for flasher */
static const i18n_text_t mounter_texts[] = {
	{
		"Filesystem Check",
		"Dateisystem Test",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Comprobar sistema de archivos",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	},{
		"Check Status",
		"Test Status",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Comprobar estado",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	},{
		"Disk %s unclean unmounted",
		"Platte %s inkorrekt entfernt",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Disco %s desmontado incorrectamente",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	},{
		"Disk %s too long unchecked",
		"Platte %s zu lange ungetestet",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Mucho tiempo sin comprobar disco %s",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	},{
		"Check successful",
		"Test erfolgreich",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Comprobación correcta",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	},{
		"Check failed",
		"Test fehlgeschlagen",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Comprobación errónea",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	},
	{
	NULL
	}
};

static struct {
	int y_msg;
	int autoboot;
	uint8_t text_color;
	int init;
	struct osd_progress_bar bar;
} display_ctx;

static void get_vdr_config(void)
{
	char *conf_path;
	FILE *setup;
	char *line;
	size_t line_size;

	conf_path = getenv("vdr_confdir");
	setup = NULL;
	if (conf_path) {
		char *set_name;
		if (asprintf(&set_name, "%s/setup.conf", conf_path) >= 0) {
			setup = fopen(set_name,"r");
			if (!setup)
				SYSLOG_ERR("Cannot open '%s'",set_name);
			free(set_name);
		} else
			SYSLOG_ERR("Cannot setup vdr config path");
	}

	if (!setup) {
		setup = fopen("/etc/vdr/setup.conf","r");
		if (!setup) {
			SYSLOG_ERR("Cannot open '/etc/vdr/setup.conf'");
			return;
		}
	}

	line = NULL;
	line_size = 0;
	while (getline(&line, &line_size, setup) > 0 ) {
		char *key;
		char *val;
		char *p;

		p = strchr(line,'#');
		if (p)
			*p = 0;

		key = line;
		val = strchr(line,'=');
		if (!val)
			continue;
		*val = 0;
		p = val - 1;
		val++;

		while (p - key >= 0 && isspace(*p)) {
			*p = 0;
			p--;
		}

		while (*key && isspace(*key))
			key++;

		while (*val && isspace(*val))
			val++;

		if (!(*key) || !(*val))
			continue;

		p = val + strlen(val) -1;
		while (p - val >= 0 && isspace(*p)) {
			*p = 0;
			p--;
		}
		if (!strcasecmp(key,"OSDLanguage")) {
			setenv("VDR_LANG",val,1);
		} else if (!strcasecmp(key,"TvMode")) {
			setenv("VDR_TV_MODE",val,1);
		}
	}
	free(line);
	fclose(setup);
}

int display_init(int autoboot)
{
	int text_height;
	int y;

	int r;
	uint8_t text_color;
	uint8_t line_color;
	uint8_t bar_colors[2];

	display_ctx.autoboot = autoboot;
	if (display_ctx.init) {
		osd_clear();
   } else {
		get_vdr_config();
		if (libosd_init(1)) {
			SYSLOG_ERR("Cannot init libosd");
			return -1;
		}

		if (i18n_register(mounter_texts)) {
			SYSLOG_ERR("Cannot init translation");
			return -1;
		}
		display_ctx.init = 1;
   }
	text_height = font_get_line_height();
	y = OSD_VKEEPOUT + text_height;

	text_color = osd_add_palette(TEXT_COLOR);
	display_ctx.text_color = text_color;
	osd_draw_text_line(0, 0, OSD_WIDTH, y, tr("Filesystem Check"),
		COLOR_INDEX_TRANSPARENT, text_color,
		TEXT_ALIGN_BOTTOM | TEXT_ALIGN_CENTER);

	y += 2;
	line_color = osd_add_palette(LINE_COLOR);
	osd_draw_rectangle(0, y, OSD_WIDTH, 3, line_color);
   y += 3 + 2;

	display_ctx.y_msg = y;
	y += text_height;

	y = (OSD_HEIGHT - 2 - text_height - BAR_HEIGHT) / 2;

	osd_draw_text_line(BAR_XPOS, y, OSD_WIDTH - BAR_XPOS, text_height,
		tr("Check Status"), COLOR_INDEX_TRANSPARENT, text_color,
		TEXT_ALIGN_TOP | TEXT_ALIGN_LEFT);
	y += text_height + 2;

	/* Draw progress bar */
	bar_colors[0] = osd_add_palette(INIT_COLOR);
	bar_colors[1] = osd_add_palette(CHECKED_COLOR);

	r = osd_progress_init(&display_ctx.bar, BAR_XPOS, y, BAR_WIDTH,
		BAR_HEIGHT, 2, bar_colors, 100);
	if (r < 0) {
		SYSLOG_ERR("Cannot init progress bar");
		return -1;
	}

	osd_flush();
	return 0;
}

void display_deinit(void)
{
	if (display_ctx.init)
		libosd_deinit(display_ctx.autoboot,1);
}

int display_update_bar(int value)
{
	int r;
	r = osd_progress_update(&display_ctx.bar, 1, value);
	osd_flush();
	return r;
}

void display_msg(const char *txt)
{
	int text_height;
	text_height = font_get_line_height();
	osd_draw_text_line(0, display_ctx.y_msg, OSD_WIDTH , text_height, txt,
		COLOR_INDEX_BACKGROUND, display_ctx.text_color,
		TEXT_ALIGN_CENTER | TEXT_ALIGN_TOP);
	osd_flush();
}
