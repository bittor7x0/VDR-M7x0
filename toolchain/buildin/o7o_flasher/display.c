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
#include "display.h"
#include "tools.h"

#define TEXT_COLOR 0xfffcfcfc
#define LINE_COLOR 0xfff07800
#define INIT_COLOR 0xffb2b2b2
#define FREE_COLOR 0xff2e4099
#define READ_COLOR 0xffe6d345
#define WRITTEN_COLOR 0xffb32d2d
#define VERIFIED_COLOR 0xff268035
#define BAR_WIDTH (OSD_WIDTH - (2 * (OSD_HKEEPOUT + 20)))
#define BAR_HEIGHT 30
#define BAR_XPOS ((OSD_WIDTH - BAR_WIDTH) / 2)

/* Texts for flasher */
static const i18n_text_t flasher_texts[] = {
	{
		"Open7x0.org Flash Tool",
		"Open7x0.org Flasher",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Utilidad Flash Open7x0.org",
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
		"Kernel",
		"Kernel",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Kernel",
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
		"JFFS",
		"JFFS",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"JFFS",
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
		"RootFS",
		"RootFS",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"RootFS",
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
		"Read",
		"Gelesen",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Leído",
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
		"Written",
		"Geschrieben",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escrito",
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
		"Verified",
		"Geprüft",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Verificado",
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
		"Unchanged",
		"Unverändert",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Sin cambios",
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
		"Free",
		"Frei",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Libre",
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
		"Update successfully completed!",
		"Update erfolgreich",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"¡Actualización completada correctamente!",
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
		"Reading Firmware Image ...",
		"Lese Fimware Image ...",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Leyendo imagen firmware ...",
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
		"Writing Firmware Image ...",
		"Schreibe Fimware Image ...",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escribiendo imagen firmware ...",
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
		"Verifying Written Firmware Image...",
		"Prüfe Geschriebene Fimware ... ",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Verificando imagen firmware escrita ...",
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
		"Error reading firmware image",
		"Fehler beim lesen des Fimware Image",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Error leyendo imagen firmware",
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
		"Error writing firmware image",
		"Fehler beim Schreiben des Fimware Image",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Error escribiendo imagen firmware",
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
		"Firmware not correctly written",
		"Fimware Image nicht korrekt geschrieben",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Firmware no escrito correctamente",
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
		"Initialization error",
		"Fehler bei der Initialisierung",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Error de inicialización",
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
		"Image checksum wrong",
		"Fehlerhafte Prüfsumme",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Checksum de imagen erróneo",
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
		"Image is not for this system",
		"Image ist nicht für dieses System",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"La imagen no es para éste sistema",
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
		"Image exceeds flash size",
		"Image ist größer als der Flash",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"La imagen excede el tamaño flash",
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
		"Rebooting ...",
		"Neustart",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Reiniciando ...",
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
	uint8_t text_color;
	struct osd_progress_bar bars[3];
} display_ctx;

int display_init(void)
{
	const char *legend_texts[5];
	int legend_texts_width[5];

	int text_height;
	int y;
	int y_legend;
	int x_legend;
	int y_spaceing;

	int r;
	uint8_t text_color;
	uint8_t line_color;
	uint8_t bar_colors[5];


	if (libosd_init(1)) {
		PRINT_ERROR("Cannot init libosd");
		return -1;
	}

	if (i18n_register(flasher_texts)) {
		PRINT_ERROR("Cannot init translation");
		return -1;
	}

	text_height = font_get_line_height();
	y = OSD_VKEEPOUT + text_height;

	text_color = osd_add_palette(TEXT_COLOR);
	display_ctx.text_color = text_color;
	osd_draw_text_line(0, 0, OSD_WIDTH, y, tr("Open7x0.org Flash Tool"),
		COLOR_INDEX_TRANSPARENT, text_color,
		TEXT_ALIGN_BOTTOM | TEXT_ALIGN_CENTER);

	y += 2;
	line_color = osd_add_palette(LINE_COLOR);
	osd_draw_rectangle(0, y, OSD_WIDTH, 3, line_color);
	y += 3;

	y_legend = OSD_HEIGHT - OSD_VKEEPOUT - text_height * 2;


	y_spaceing = (y_legend - y - (text_height + 2 + BAR_HEIGHT) * 3 - text_height) / 4;
	if (y_spaceing < 0) {
		y_spaceing = 0;
		PRINT_WARNING("Not enough space on screen");
	}

	/* leave space for message texts */
	display_ctx.y_msg = y + (y_spaceing /2);
	y += text_height + y_spaceing;

	/* Draw progress bars */
	bar_colors[0] = osd_add_palette(INIT_COLOR);
	bar_colors[1] = osd_add_palette(FREE_COLOR);
	bar_colors[2] = osd_add_palette(READ_COLOR);
	bar_colors[3] = osd_add_palette(WRITTEN_COLOR);
	bar_colors[4] = osd_add_palette(VERIFIED_COLOR);

	osd_draw_text_line(BAR_XPOS, y, 0, 0, tr("Kernel"),
		COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);
	y += 2 + text_height;
	r = osd_progress_init(&display_ctx.bars[0], BAR_XPOS, y, BAR_WIDTH,
		BAR_HEIGHT, 5, bar_colors, 1);
	if (r < 0) {
		PRINT_ERROR("Cannot init progress bar");
		return -1;
	}

	y += BAR_HEIGHT + y_spaceing;
	osd_draw_text_line(BAR_XPOS, y, 0, 0, tr("JFFS"),
		COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);
	y += 2 + text_height;
	r = osd_progress_init(&display_ctx.bars[1], BAR_XPOS,	y, BAR_WIDTH,
		BAR_HEIGHT, 5, bar_colors, 1);
	if (r < 0) {
		PRINT_ERROR("Cannot init progress bar");
		return -1;
	}

	y += BAR_HEIGHT + y_spaceing;
	osd_draw_text_line(BAR_XPOS, y, 0, 0, tr("RootFS"),
		COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);
	y += 2 + text_height;
	r = osd_progress_init(&display_ctx.bars[2], BAR_XPOS, y, BAR_WIDTH,
		BAR_HEIGHT, 5, bar_colors, 1);
	if (r < 0) {
		PRINT_ERROR("Cannot init progress bar");
		return -1;
	}

	legend_texts[0] = tr("Unchanged");
	legend_texts[1] = tr("Free");
	legend_texts[2] = tr("Read");
	legend_texts[3] = tr("Written");
	legend_texts[4] = tr("Verified");

	legend_texts_width[0] = font_get_text_line_width(legend_texts[0]);
	legend_texts_width[1] = font_get_text_line_width(legend_texts[1]);
	legend_texts_width[2] = font_get_text_line_width(legend_texts[2]);
	legend_texts_width[3] = font_get_text_line_width(legend_texts[3]);
	legend_texts_width[4] = font_get_text_line_width(legend_texts[4]);

	/* Calc line width */
	x_legend = legend_texts_width[0] + legend_texts_width[1];
	x_legend += 10 * 2 + 2 * 2 + 5; /* Color + Spacing */

	if (x_legend > OSD_WIDTH - OSD_HKEEPOUT * 2) {
		int x_cut = (x_legend - (OSD_WIDTH - OSD_HKEEPOUT * 2) + 1) / 2;
		legend_texts_width[0] -= x_cut;
		legend_texts_width[1] -= x_cut;
		/* Recalc */
		x_legend = legend_texts_width[0] + legend_texts_width[1];
		x_legend += 10 * 2 + 2 * 2 + 5;
	}

	x_legend = (OSD_WIDTH - x_legend) / 2;

	osd_draw_rectangle(x_legend, y_legend + (text_height - 10) / 2, 10, 10,
		bar_colors[0]);
	x_legend += 10 + 2;
	osd_draw_text_line(x_legend, y_legend, legend_texts_width[0], text_height,
		legend_texts[0], COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);
	x_legend += 5 + legend_texts_width[0];

	osd_draw_rectangle(x_legend, y_legend + (text_height - 10) / 2, 10, 10,
		bar_colors[1]);
	x_legend += 10 + 2;
	osd_draw_text_line(x_legend, y_legend, legend_texts_width[1], text_height,
		legend_texts[1], COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);

	/* Calc line width */
	x_legend = legend_texts_width[2] + legend_texts_width[3] +
		legend_texts_width[4];
	x_legend += 10 * 3 + 2 * 3 + 5 * 2; /* Color + Spacing */

	if (x_legend > OSD_WIDTH - OSD_HKEEPOUT * 2) {
		int x_cut = (x_legend - (OSD_WIDTH - OSD_HKEEPOUT * 2) + 2) / 3;
		legend_texts_width[2] -= x_cut;
		legend_texts_width[3] -= x_cut;
		legend_texts_width[4] -= x_cut;
		/* Recalc */
		x_legend = legend_texts_width[2] + legend_texts_width[3] +
			legend_texts_width[4];
		x_legend += 10 * 3 + 2 * 3 + 5 * 2;
	}

	y_legend += text_height;
	x_legend = (OSD_WIDTH - x_legend) / 2;

	osd_draw_rectangle(x_legend, y_legend + (text_height - 10) / 2, 10, 10,
		bar_colors[2]);
	x_legend += 10 + 2;
	osd_draw_text_line(x_legend, y_legend, legend_texts_width[2], text_height,
		legend_texts[2], COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);
	x_legend += 5 + legend_texts_width[2];

	osd_draw_rectangle(x_legend, y_legend + (text_height - 10) / 2, 10, 10,
		bar_colors[3]);
	x_legend += 10 + 2;
	osd_draw_text_line(x_legend, y_legend, legend_texts_width[3], text_height,
		legend_texts[3],COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);
	x_legend += 5 + legend_texts_width[3];

	osd_draw_rectangle(x_legend, y_legend + (text_height - 10) / 2, 10, 10,
		bar_colors[4]);
	x_legend += 10 + 2;
	osd_draw_text_line(x_legend, y_legend, legend_texts_width[4], text_height,
		legend_texts[4],COLOR_INDEX_TRANSPARENT, text_color, TEXT_ALIGN_DEFAULT);

	osd_flush();
	return 0;
}

void display_deinit(void)
{
	libosd_deinit(1,1);
}

int display_set_bar_max(int bar, int max)
{
	return osd_progress_set_max_value(&display_ctx.bars[bar], max);
}

int display_set_bar_free(int bar, int free_start)
{
	int r;
	r = osd_progress_mark_range(&display_ctx.bars[bar], free_start,
		 osd_progress_get_max_value(&display_ctx.bars[bar]), 1);
	osd_flush();
	return r;
}


int display_update_bar(int bar, int step, int value)
{
	int r;
	r = osd_progress_update(&display_ctx.bars[bar], step, value);
	osd_flush();
	return r;
}

void display_msg(const char *txt)
{
	int text_height;
	text_height = font_get_line_height();
	osd_draw_text_line(0, display_ctx.y_msg, OSD_WIDTH, text_height, txt,
		COLOR_INDEX_BACKGROUND, display_ctx.text_color,
		TEXT_ALIGN_TOP | TEXT_ALIGN_CENTER);
	osd_flush();
}
