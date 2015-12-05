/*
 * receiver.c: The basic receiver interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "receiver.h"
#include <stdarg.h>
#include <stdio.h>
#include "tools.h"

cReceiver::cReceiver(const cChannel *Channel, int Priority)
{
  device = NULL;
  priority = constrain(Priority, MINPRIORITY, MAXPRIORITY);
  numPids = 0;
  SetPids(Channel);
}

cReceiver::cReceiver(int Ca, int Priority, int Pid, const int *Pids1, const int *Pids2, const int *Pids3, int Pid2, int Pid3)
{
  device = NULL;
  ca = Ca;
  priority = constrain(Priority, MINPRIORITY, MAXPRIORITY);
  numPids = 0;
  activated = false;
  lastPid = 0;
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
  if ((Pid2) && (numPids < MAXRECEIVEPIDS))
     pids[numPids++] = Pid2;
  if ((Pid3) && (numPids < MAXRECEIVEPIDS))
     pids[numPids++] = Pid3;
  if (numPids >= MAXRECEIVEPIDS)
     dsyslog("too many PIDs in cReceiver");
}

cReceiver::~cReceiver()
{
  if (device) {
     const char *msg = "ERROR: cReceiver has not been detached yet! This is a design fault and VDR will abort now!";
     esyslog("%s", msg);
     fprintf(stderr, "%s\n", msg);
     abort();
     }
}

bool cReceiver::AddPid(int Pid)
{
  if (Pid) {
     if (numPids < MAXRECEIVEPIDS)
        pids[numPids++] = Pid;
     else {
        dsyslog("too many PIDs in cReceiver (Pid = %d)", Pid);
        return false;
        }
     }
  return true;
}

bool cReceiver::AddPids(const int *Pids)
{
  if (Pids) {
     while (*Pids) {
           if (!AddPid(*Pids++))
              return false;
           }
     }
  return true;
}

bool cReceiver::AddPids(int Pid1, int Pid2, int Pid3, int Pid4, int Pid5, int Pid6, int Pid7, int Pid8, int Pid9)
{
  return AddPid(Pid1) && AddPid(Pid2) && AddPid(Pid3) && AddPid(Pid4) && AddPid(Pid5) && AddPid(Pid6) && AddPid(Pid7) && AddPid(Pid8) && AddPid(Pid9);
}

bool cReceiver::SetPids(const cChannel *Channel)
{
  numPids = 0;
  if (Channel) {
     channelID = Channel->GetChannelID();
     return AddPid(Channel->Vpid()) &&
            (Channel->Ppid() == Channel->Vpid() || AddPid(Channel->Ppid())) &&
            AddPids(Channel->Apids()) &&
            AddPids(Channel->Dpids()) &&
            AddPids(Channel->Spids());
     }
  return true;
}

bool cReceiver::WantsPid(int Pid)
{
  if (Pid) {
     for (int i = 0; i < numPids; i++) {
         if (pids[i] == Pid) {
            lastPid = i;
            return true;
            }
         }
     }
  return false;
}

void cReceiver::Detach(void)
{
  if (device)
     device->Detach(this);
}
