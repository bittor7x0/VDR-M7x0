/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ___SETUP_MP3_H
#define ___SETUP_MP3_H

extern const char *cddbpath;
extern const char *netscript;
#ifdef WITH_OSS
extern const char *dspdevice;
#endif

// ----------------------------------------------------------------


#define MAX_HOSTNAME 128

#define AUDIOOUTMODES    2
#define AUDIOOUTMODE_DVB 0


class cMP3Setup {
public:
  int InitLoopMode;
  int InitShuffleMode;
  int BgrScan;
  int EditorMode;
  int DisplayMode;
  int DirectPlaylist;
  int BackgrMode;
  int MenuMode;
  int TargetLevel;
  int LimiterLevel;
  int UseProxy;
  char ProxyHost[MAX_HOSTNAME];
  int ProxyPort;
  int AbortAtEOL;
  int ReplayDisplay;
  int HideMainMenu;
  int KeepSelect;
  int TitleArtistOrder;
public:
  cMP3Setup(void);
  };

extern cMP3Setup MP3Setup;

#endif //___SETUP_MP3_H
