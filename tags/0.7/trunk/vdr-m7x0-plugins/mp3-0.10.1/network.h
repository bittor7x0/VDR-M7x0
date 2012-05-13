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

#ifndef ___NETWORK_H
#define ___NETWORK_H

#include <vdr/thread.h>
#include <vdr/ringbuffer.h>

class cRingBufferLinear;

// ----------------------------------------------------------------

class cNet : public cRingBufferLinear, cThread {
private:
  int fd;
  bool connected, netup;
  int deferedErrno;
  int rwTimeout, conTimeout;
  unsigned char lineBuff[4096];
  int count;
  //
  void Close(void);
  int RingRead(unsigned char *dest, int len);
  void CopyFromBuff(unsigned char *dest, int n);
protected:
  virtual void Action(void);
public:
  cNet(int size, int ConTimeoutMs, int RwTimeoutMs);
  ~cNet();
  bool Connect(const char *hostname, const int port);
  void Disconnect(void);
  bool Connected(void) { return connected; }
  int Gets(char *dest, int len);
  int Puts(char *dest);
  int Read(unsigned char *dest, int len);
  int Write(unsigned char *dest, int len);
  };

#endif //___NETWORK_H
