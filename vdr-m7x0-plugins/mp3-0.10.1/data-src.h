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

#ifndef ___DATA_SRC_H
#define ___DATA_SRC_H

#include <vdr/config.h>
#include "data.h"

// ----------------------------------------------------------------

class cFileSources : public cConfig<cFileSource> {
private:
  cFileSource *current;
public:
  virtual bool Load(const char *filename, bool dummy=false);
  void SetSource(cFileSource *source) { current=source; }
  cFileSource *GetSource(void) { return current; }
  cFileSource *FindSource(const char *filename);
  };

#endif //___DATA_SRC_H
