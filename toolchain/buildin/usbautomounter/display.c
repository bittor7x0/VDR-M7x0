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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "display.h"
#include "tools.h"

#define TEXT_COLOR 0xfffcfcfc
#define LINE_COLOR 0xfff07800
#define INIT_COLOR 0xffb2b2b2
#define CHECKED_COLOR 0xff268035
#define BAR_WIDTH (OSD_WIDTH - (2 * (OSD_HKEEPOUT + 20)))
#define BAR_HEIGHT 30
#define BAR_XPOS ((OSD_WIDTH - BAR_WIDTH) / 2)
#define SVDRADDRESS "localhost"
#define SVDRPORT 2001
#define SVDRPBarStep 10
#define SVDRPBarMinTime 30
#define SVDRPBarMaxTime 60

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
extern int useSVDRP;
int SendSVDRPMessage(const char* Message);
void CloseSVDRP();

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
	if(useSVDRP>0)
	{
		get_vdr_config();
		i18n_init(-1);
		if(i18n_register(mounter_texts))
			SYSLOG_ERR("Cannot init translation");
		SendSVDRPMessage(tr("Filesystem Check"));
		return 0;
	}
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
	if(useSVDRP>0)
	{
		CloseSVDRP();		
		i18n_deinit();
		return;
	}
	if (display_ctx.init)
		libosd_deinit(display_ctx.autoboot,1);
}

#define MAXBUF 512
int LastBar=0;
int LastBarTime=0;
int display_update_bar(int value)
{
	if(useSVDRP>0)
	{
		time_t Now=time(NULL);
		if(Now-LastBarTime<SVDRPBarMinTime)
			return 0;
		if((value>LastBar)&&(value-LastBar<SVDRPBarStep)&&(Now-LastBarTime<SVDRPBarMaxTime))
			return 0;
		LastBar=(value/SVDRPBarStep)*SVDRPBarStep;
		LastBarTime=Now;
		char Buffer[MAXBUF];
		snprintf(Buffer,MAXBUF,"%s (%d%%)",tr("Filesystem Check"),LastBar);
		Buffer[MAXBUF-1]=0;
		SendSVDRPMessage(Buffer);
		return 0;
	}
	int r;
	r = osd_progress_update(&display_ctx.bar, 1, value);
	osd_flush();
	return r;
}

void display_msg(const char *txt)
{
	if(useSVDRP>0)
	{
		LastBarTime=0;
		SendSVDRPMessage(txt);
		return;
	}
	int text_height;
	text_height = font_get_line_height();
	osd_draw_text_line(0, display_ctx.y_msg, OSD_WIDTH , text_height, txt,
		COLOR_INDEX_BACKGROUND, display_ctx.text_color,
		TEXT_ALIGN_CENTER | TEXT_ALIGN_TOP);
	osd_flush();
}

int sd=-1;
int RecvSVDRP()
{
	if(sd==-1)
		return 0;
	char buf1[MAXBUF];
	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sd,&rfds);
	while(1)
	{
		tv.tv_sec=1;
		tv.tv_usec=0;
		if(select(sd+1,&rfds,NULL,NULL,&tv)<1)
		{
			close(sd);
			sd=-1;
			return 0;
		}
		int Received=recv(sd,buf1,MAXBUF,0);
		if(Received<1)
		{
			close(sd);
			sd=-1;
			return 0;
		}
		int f;		
		for(f=0;f<Received;f++)
			if(buf1[f]=='\n')
				return 1;
	}     
	return 0;
}
int OpenSVDRP()
{
	if(sd!=-1)
		return 1;
	struct hostent *hostaddr=gethostbyname(SVDRADDRESS);
	if(!hostaddr)
		return 0;

	sd=socket(PF_INET,SOCK_STREAM,6);
	if(sd==-1)
		return 0;

	struct sockaddr_in socketaddr;
	memset(&socketaddr,0,sizeof(socketaddr));
	socketaddr.sin_family=AF_INET;
	socketaddr.sin_port=htons(SVDRPORT);

	memcpy(&socketaddr.sin_addr,hostaddr->h_addr,hostaddr->h_length);

	if(connect(sd,(struct sockaddr *)&socketaddr,sizeof(socketaddr))==-1)
	{
		close(sd);
		sd=-1;
		return 0;
	}
	return 1;
}
int SendSVDRP(const char *msg)
{
	if(!OpenSVDRP())
		return 0;
	if(send(sd,msg,strlen(msg),0)!=strlen(msg))
	{
		close(sd);
		sd=-1;
		return 0;	
	}
	return 1;
}
void CloseSVDRP()
{
	if(sd==-1)
		return;
	if(!SendSVDRP("QUIT\n\r"))
		return;
	if(!RecvSVDRP())
		return;
	close(sd);
	sd=-1;
}
int SendSVDRPMessage(const char* Message)
{
	char Buffer[MAXBUF];
	snprintf(Buffer,MAXBUF,"MESG %s\n\r",Message);
	Buffer[MAXBUF-1]=0;
	if(!SendSVDRP(Buffer))
		return 0;
	if(RecvSVDRP())
		CloseSVDRP();
	return 1;
}
