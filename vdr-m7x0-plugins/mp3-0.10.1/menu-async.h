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

#ifndef ___MENU_ASYNC_H
#define ___MENU_ASYNC_H

#include <vdr/thread.h>

// ----------------------------------------------------------------

class cAsyncStatus : private cMutex {
private:
  const char *text;
  bool changed;
public:
  cAsyncStatus(void);
  ~cAsyncStatus();
  void Set(const char *Text);
  bool Changed(void) { return changed; }
  const char *Begin(void);
  void Finish(void);
  };

extern cAsyncStatus asyncStatus;

#endif //___MENU_ASYNC_H
