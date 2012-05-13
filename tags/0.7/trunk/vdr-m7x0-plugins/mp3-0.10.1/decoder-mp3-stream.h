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

#ifndef ___DECODER_MP3_STREAM_H
#define ___DECODER_MP3_STREAM_H

#define DEC_MP3S     DEC_ID('M','P','3','S')
#define DEC_MP3S_STR "MP3S"

#include "decoder-mp3.h"

class cNetStream;
class cNetScanID3;

// ----------------------------------------------------------------

class cMP3StreamDecoder : public cMP3Decoder {
private:
  cNetStream *nstr;
  cNetScanID3 *nscan;
public:
  cMP3StreamDecoder(const char *Filename);
  virtual bool Valid(void);
  virtual bool IsStream(void) { return true; }
  };

#endif //___DECODER_MP3_STREAM_H
