/*
 * receiver.c: The basic receiver interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "receiver.h"
#include <stdarg.h>
#include <stdio.h>
#include "tools.h"

cReceiver::cReceiver(int Ca, int Priority, int Pid, const int *Pids1, const int *Pids2, const int *Pids3)
{
  device = NULL;
  ca = Ca;
  priority = Priority;
  numPids = 0;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  frameEventsWanted = false;
#endif
  if (Pid)
     pids[numPids++] = Pid;
  if (Pids1) {
     while (*Pids1 && numPids < MAXRECEIVEPIDS)
           pids[numPids++] = *Pids1++;
     }
  if (Pids2) {
     while (*Pids2 && numPids < MAXRECEIVEPIDS)
           pids[numPids++] = *Pids2++;
     }
  if (Pids3) {
     while (*Pids3 && numPids < MAXRECEIVEPIDS)
           pids[numPids++] = *Pids3++;
     }
  if (numPids >= MAXRECEIVEPIDS)
     dsyslog("too many PIDs in cReceiver");
}

cReceiver::~cReceiver()
{
  Detach();
}

bool cReceiver::WantsPid(int Pid)
{
  if (Pid) {
     for (int i = 0; i < numPids; i++) {
         if (pids[i] == Pid)
            return true;
         }
     }
  return false;
}

void cReceiver::Detach(void)
{
  if (device)
     device->Detach(this);
}
