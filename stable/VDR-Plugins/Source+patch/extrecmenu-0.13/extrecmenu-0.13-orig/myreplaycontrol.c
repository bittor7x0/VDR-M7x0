/*
 * See the README file for copyright information and how to reach the author.
 */

#include <vdr/interface.h>
#include <vdr/status.h>
#include <vdr/menu.h>
#include "myreplaycontrol.h"
#include "mymenusetup.h"

using namespace std;

char *myReplayControl::Cut=NULL;
char *myReplayControl::Cutted=NULL;

myReplayControl::~myReplayControl()
{
  if(mysetup.ReturnToPlugin)
    cRemote::CallPlugin("extrecmenu");
}

eOSState myReplayControl::ProcessKey(eKeys Key)
{
  if(Key==kBack)
   return osEnd;

  if(Key==kEditCut)
  {
    free(Cut);
    Cut=strdup(LastReplayed());

    cRecording recording(LastReplayed());
    free(Cutted);
    Cutted=strdup(recording.PrefixFileName('%'));
  }
 
  return cReplayControl::ProcessKey(Key);
}
