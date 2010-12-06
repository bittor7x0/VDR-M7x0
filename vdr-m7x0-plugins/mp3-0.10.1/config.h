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

// This files contains some option switches/values for compile time
// configuration of the MP3/MPlayer plugin. You should only alter
// this file, if you have understand the source code parts which deal
// with the changed value.

// After changing this file you should do a "make plugins-clean ; make plugins"
// to recompile vdr.

#ifndef ___CONFIG_H
#define ___CONFIG_H

// Uncomment to enable generic debugging messages to the console. This may slow
// down operation in some cases.
#define DEBUG
//#define NO_DEBUG

// Defines the filename extention to use for playlist files.
#define PLAYLISTEXT ".m3u"

// Defines the text to identify WinAmp-Style playlists.
#define WINAMPEXT "#EXTM3U"

// Defines the timeout in seconds for functions which use a single key
// (e.g. openning the playlist window). If the key is repressed during
// the timeout, the secondary function is activated.
#define MULTI_TIMEOUT 3

// Defines the timeout in ms for entering the single digits in direct song
// selection.
#define SELECT_TIMEOUT 1000

// If the progress display is closed on direct song selection, the display
// is opend temporarily. This defines the time in seconds after the display
// is closed again.
#define SELECTHIDE_TIMEOUT 3

// Defines the time in seconds to jump inside a song with left/right.
#define JUMPSIZE 30

#endif //___CONFIG_H
