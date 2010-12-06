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
 * $Id: osddrawtext.c 321 2008-02-22 17:21:34Z andreas $
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libosd/libosd.h>
#include <ctype.h>
#include "tools.h"

static struct {
	int clear;
	int x;
	int y;
	int width;
	int height;
	uint32_t bgColor;
	uint32_t fgColor;
	uint8_t bgColorIndex;
	uint8_t fgColorIndex;
	int align;
	int noscroll;
	int usesyslog;
	char *text;
} odt_ctx;

static int drawtext(void)
{
	int text_height = font_get_line_height();
	if (odt_ctx.height < text_height) {
		ERROR_LOG_NE("Height to small for drawing one text line");
		return -1;
	}

	int y_pos = odt_ctx.y;
	int y_end = y_pos + odt_ctx.height;
	if (odt_ctx.align & TEXT_ALIGN_MIDDLE) {
		y_pos += (odt_ctx.height - text_height) / 2;
	} else if (odt_ctx.align & TEXT_ALIGN_BOTTOM) {
		y_pos += odt_ctx.height - (odt_ctx.noscroll ? text_height : 0 );
	}

	char *line = NULL;
	size_t len = 0;
	char *p = odt_ctx.text;
	int ret = 0;
	do {
		if (!odt_ctx.text) {
			int r = getline(&line,&len,stdin);
			if (r < 0)
				break;
			if (r && line[r-1] == '\n')
				line[r-1] = 0;
		} else {
			if (!p)
				break;
			line = p;
			p = strchr(line,'\n');
			if (p) {
				*p++ = 0;
				if (!*p)
					p = NULL;
			}
		}

		int c = 0;
		do {
			if (y_pos + text_height > y_end) {
				if (odt_ctx.noscroll) {
					y_pos = odt_ctx.y;
				} else {
					y_pos -= text_height;
					osd_move_region(odt_ctx.x, odt_ctx.y + text_height, odt_ctx.x,
							odt_ctx.y, odt_ctx.width, y_pos - odt_ctx.y);
					if (odt_ctx.bgColorIndex == COLOR_INDEX_TRANSPARENT)
						osd_draw_rectangle(odt_ctx.x, y_pos, odt_ctx.width,
								text_height,odt_ctx.bgColorIndex);
				}
			}
			c += osd_draw_text_line(odt_ctx.x, y_pos, odt_ctx.width, text_height,
					line + c, odt_ctx.bgColorIndex, odt_ctx.fgColorIndex,
					odt_ctx.align);
			if (!c && line[0]) {
				ERROR_LOG_NE("Width too small to draw one char");
				ret = -1;
				break;
			}
			y_pos += text_height;
			osd_flush();
		} while (line[c]);
	} while (!ret);
	if (!odt_ctx.text)
		free(line);
	return ret;
}

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
				ERROR_LOG("Cannot open '%s'",set_name);
			free(set_name);
		} else
			ERROR_LOG("Cannot setup vdr config path");
	}

	if (!setup) {
		setup = fopen("/etc/vdr/setup.conf","r");
		if (!setup) {
			ERROR_LOG("Cannot open '/etc/vdr/setup.conf'");
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
		if (!strcasecmp(key,"TvMode")) {
			setenv("VDR_TV_MODE",val,1);
			break;
		}
	}
	free(line);
	fclose(setup);
}

static int do_init(void)
{
	if (odt_ctx.usesyslog)
		init_syslog("osddrawtext");

	get_vdr_config();
	if (libosd_init(odt_ctx.clear == 1 || odt_ctx.clear == 3)) {
		ERROR_LOG_NE("Cannot init libosd");
		return -1;
	}
	if (odt_ctx.bgColor >> 24) {
		odt_ctx.bgColorIndex = osd_add_palette(odt_ctx.bgColor);
	} else {
		odt_ctx.bgColorIndex =  COLOR_INDEX_TRANSPARENT;
	}
	if (odt_ctx.fgColor >> 24) {
		odt_ctx.fgColorIndex = osd_add_palette(odt_ctx.fgColor);
	} else {
		odt_ctx.fgColorIndex =  COLOR_INDEX_TRANSPARENT;
	}
	return 0;
}

static void do_deinit(void)
{
	if (odt_ctx.usesyslog)
		close_syslog();

	libosd_deinit(odt_ctx.clear == 2 || odt_ctx.clear == 3,
		odt_ctx.clear == 2 || odt_ctx.clear == 3);
	free(odt_ctx.text);
}

static void print_usage(void)
{
	fprintf(stderr,"osddrawtext [--help] [-c|--clear <clear mode>] "
					"[-x <x pos>] [-y <y pos>] [-w|--width <width>] "
					"[-h|--height <height>] [-b|--bgColor <color>] "
					"[-f|--fgColor <color>] [-a|--align <alignment>] "
					"[-noscroll] [-s|--syslog] [<text>..]\n");
	fprintf(stderr,"draws <text> on osd. If not given reads text from"
					" stdin.\n\n");
	fprintf(stderr,"Options:\n");
	fprintf(stderr,"-c|--clear <clear mode>\t\t0 := Never, 1 := On start, 2 := On exit, 3 := both (default 0)\n");
	fprintf(stderr,"-x <x pos>\t\tx position of text (default %d)\n",OSD_HKEEPOUT);
	fprintf(stderr,"-y <y pos>\t\ty position of text (default %d)\n",OSD_VKEEPOUT);
	fprintf(stderr,"-w|--width <width>\t\twidth of text area (default %d)\n",
					OSD_WIDTH - OSD_HKEEPOUT * 2);
	fprintf(stderr,"-h|--height <height>\t\theight of text area (default %d)\n",
					OSD_HEIGHT - OSD_VKEEPOUT * 2);
	fprintf(stderr,"-b|--bgColor <color>\t\tARGB background color (default 0)\n");
	fprintf(stderr,"-f|--fgColor <color>\t\tARGB text color (default 0xFFFFFFFF)\n");
	fprintf(stderr,"-a|--align <alignment>\t\ttext-alignment (default %d):\n",
					TEXT_ALIGN_DEFAULT);
	fprintf(stderr,"\t\t%d\talign left\n",TEXT_ALIGN_LEFT);
	fprintf(stderr,"\t\t%d\talign center\n",TEXT_ALIGN_CENTER);
	fprintf(stderr,"\t\t%d\talign right\n",TEXT_ALIGN_RIGHT);
	fprintf(stderr,"\t\t%d\talign top\n",TEXT_ALIGN_TOP);
	fprintf(stderr,"\t\t%d\talign middle\n",TEXT_ALIGN_MIDDLE);
	fprintf(stderr,"\t\t%d\talign bottom\n",TEXT_ALIGN_BOTTOM);
	fprintf(stderr,"--noscroll\t\tdo not scroll text\n");
	fprintf(stderr,"-s|--syslog\t\tuse syslog\n");
}

static int parse_cmdline(int argc, char **argv)
{
	odt_ctx.clear = 0;
	odt_ctx.x = OSD_HKEEPOUT;
	odt_ctx.y = OSD_VKEEPOUT;
	odt_ctx.width = OSD_WIDTH - OSD_HKEEPOUT * 2;
	odt_ctx.height = OSD_HEIGHT - OSD_VKEEPOUT * 2;
	odt_ctx.align = TEXT_ALIGN_DEFAULT;
	odt_ctx.noscroll = 0;
	odt_ctx.bgColor = 0;
	odt_ctx.fgColor = 0xFFFFFFFFUL;
	odt_ctx.usesyslog = 0;
	odt_ctx.text = NULL;

	int i;
	for (i=1; i < argc; i++) {
		uint32_t *val = NULL;
		uint32_t minv = 0;
		uint32_t maxv = 0;
		if (!strcasecmp("--help",argv[i])) {
			print_usage();
			return -1;
		} else if (!strcasecmp("-c",argv[i]) || !strcasecmp("--clear",argv[i])) {
			val = (uint32_t *) &odt_ctx.clear;
			minv = 0;
			maxv = 3;
		} else if (!strcasecmp("-x",argv[i])) {
			val = &odt_ctx.x;
			minv = 0;
			maxv = OSD_WIDTH - 1;
		} else if (!strcasecmp("-y",argv[i])) {
			val = &odt_ctx.y;
			minv = 0;
			maxv = OSD_HEIGHT - 1;
		} else if (!strcasecmp("-w",argv[i]) || !strcasecmp("--width",argv[i])) {
			val = (uint32_t *) &odt_ctx.width;
			minv = 0;
			maxv = OSD_WIDTH -1;
		} else if (!strcasecmp("-h",argv[i]) || !strcasecmp("--height",argv[i])) {
			val = (uint32_t *) &odt_ctx.height;
			minv = 0;
			maxv = OSD_HEIGHT -1;
		} else if (!strcasecmp("-b",argv[i]) || !strcasecmp("--bgColor",argv[i])) {
			val = (uint32_t *) &odt_ctx.bgColor;
			minv = 0;
			maxv = 0xFFFFFFFFUL;
		} else if (!strcasecmp("-f",argv[i]) || !strcasecmp("--fgColor",argv[i])) {
			val = (uint32_t *) &odt_ctx.fgColor;
			minv = 0;
			maxv = 0xFFFFFFFFUL;
		} else if (!strcasecmp("-a",argv[i]) || !strcasecmp("--align",argv[i])) {
			val = (uint32_t *) &odt_ctx.align;
			minv = 0;
			maxv = 10;
		} else if (!strcasecmp("--noscroll",argv[i])) {
			odt_ctx.noscroll = 1;
		} else if (!strcasecmp("-s",argv[i]) || !strcasecmp("--syslog",argv[i])) {
			odt_ctx.usesyslog = 1;
		} else {
			break;
		}
		if (val) {
			if (i+1 >= argc) {
				fprintf(stderr,"Argument missed for parameter %s\n",argv[i]);
				print_usage();
				return -1;
			}
			char *endptr;
			*val =  strtoul(argv[i+1],&endptr,0);
			if (*endptr || *val < minv || *val > maxv) {
				fprintf(stderr,"Invalid argument %s for parameter %s\n",argv[i+1],argv[i]);
				print_usage();
				return -1;
			}
			i++;
		}
	}

	if (((odt_ctx.align & TEXT_ALIGN_CENTER) &&
			(odt_ctx.align & TEXT_ALIGN_RIGHT)) ||
			((odt_ctx.align & TEXT_ALIGN_MIDDLE) &&
			(odt_ctx.align & TEXT_ALIGN_BOTTOM))) {
		fprintf(stderr,"Invalid text alignment\n");
		print_usage();
		return -1;
	}
	if (odt_ctx.x + odt_ctx.width > OSD_WIDTH) {
		fprintf(stderr,"Width and x-position exceeds OSD width\n");
		print_usage();
		return -1;
	}
	if (odt_ctx.y + odt_ctx.height > OSD_HEIGHT) {
		fprintf(stderr,"Height and y-position exceeds OSD height\n");
		print_usage();
		return -1;
	}
	if (i == argc) {
		return 0;
	}
	int text_len = 0;
	int j;
	for (j = i; j < argc; j++) {
		text_len += strlen(argv[j]) + 1;
	}

	char *p = odt_ctx.text = malloc(text_len);
	if (!p) {
		fprintf(stderr,"Cannot alloc memory\n");
		return -1;
	}
	for (j = i; j < argc; j++) {
		int l = strlen(argv[j]);
		memcpy(p,argv[j],l);
		p += l;
		*p++ = ' ';
	}
	p[-1] = 0;
	return 0;
}

int main(int argc, char **argv)
{
	if (parse_cmdline(argc,argv)) {
		return 1;
	}
	if (do_init()) {
		return 1;
	}
	int r = drawtext();
	do_deinit();
	return (r ? 1 : 0);
}
