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

#include <string.h>

#include "common.h"
#include "setup-mp3.h"

cMP3Setup MP3Setup;

// --- cMP3Setup ---------------------------------------------------------------

cMP3Setup::cMP3Setup(void)
{
  InitLoopMode = 0;
  InitShuffleMode = 0;
  BgrScan = 1;
  EditorMode = 0;
  DisplayMode = 3;
  DirectPlaylist = 0;
  BackgrMode = 1;
  MenuMode = 1;
  UseProxy = 0;
  strcpy(ProxyHost,"localhost");
  ProxyPort = 8080;
  AbortAtEOL = 1;
  ReplayDisplay = 0;
  HideMainMenu = 0;
  KeepSelect = 0;
  TitleArtistOrder = 0;
}
