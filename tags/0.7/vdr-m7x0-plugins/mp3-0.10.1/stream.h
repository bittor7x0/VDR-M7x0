/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2006 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#ifndef ___STREAM_H
#define ___STREAM_H

#include "decoder.h"

class cNet;

// ----------------------------------------------------------------

class cStream : public cFileInfo {
private:
  int fd;
  bool ismmap;
protected:
  uint8_t *buffer;
  uint64_t readpos, buffpos;
  uint32_t fill;
public:
  cStream(const char *Filename);
  virtual ~cStream();
  virtual bool Open(bool log=true);
  virtual void Close(void);
  virtual bool Stream(const uint8_t *&data, uint32_t &len, const uint8_t *rest=NULL);
  virtual bool StreamSafer(const uint8_t *&data, uint32_t &len, const uint8_t *rest, uint32_t need);
  virtual bool Seek(uint64_t pos=0);
  virtual bool Skip(const uint8_t *&data, uint32_t &len, const uint8_t *rest, int64_t skip);
  virtual uint64_t BufferPos(void) { return buffpos; }
  virtual int64_t Tell(const uint8_t *rest);

  };

// ----------------------------------------------------------------

class cNetStream : public cStream {
private:
  cNet *net;
  char *host, *path, *auth;
  int port, cc;
  //
  char *icyName, *icyUrl, *icyTitle;
  int metaInt, metaCnt;
  bool icyChanged;
  //
  bool ParseURL(const char *line, bool log);
  bool ParseURLFile(const char *name, bool log);
  bool SendRequest(void);
  bool GetHTTPResponse(void);
  bool ParseHeader(const char *buff, const char *name, char **value);
  bool ParseMetaData(void);
  char *ParseMetaString(const char *buff, const char *name, char **value);
public:
  cNetStream(const char *Filename);
  virtual ~cNetStream();
  virtual bool Open(bool log=true);
  virtual void Close(void);
  virtual bool Stream(const uint8_t *&data, uint32_t &len, const uint8_t *rest=NULL);
  virtual bool Seek(uint64_t pos=0);
  virtual bool Skip(const uint8_t *&data, uint32_t &len, const uint8_t *rest, int64_t skip);
  virtual int64_t Tell(const uint8_t *rest) { return 0; }
  bool Valid(void) { return ParseURLFile(Filename,false); }
  const char *IcyName(void) const { return icyName; }
  const char *IcyUrl(void) const { return icyUrl; }
  const char *IcyTitle(void) const { return icyTitle; }
  bool IcyChanged(void);
  };

#endif //___STREAM_H
