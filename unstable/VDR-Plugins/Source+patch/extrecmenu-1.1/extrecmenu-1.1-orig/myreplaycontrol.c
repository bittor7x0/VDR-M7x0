/*
 * See the README file for copyright information and how to reach the author.
 */

#include <string>
#include <vdr/interface.h>
#include <vdr/status.h>
#include <vdr/menu.h>
#include <vdr/cutter.h>
#include "myreplaycontrol.h"
#include "mymenusetup.h"
#include "tools.h"

using namespace std;

myReplayControl::myReplayControl()
{
  timesearchactive=false;
}

myReplayControl::~myReplayControl()
{
  if(mysetup.ReturnToPlugin)
    cRemote::CallPlugin("extrecmenu");
}

eOSState myReplayControl::ProcessKey(eKeys Key)
{
  if(Key!=kNone)
  {
    if(timesearchactive && mysetup.UseCutterQueue)
    {
      if(Key<k0 || Key>k9)
        timesearchactive=false;
    }
    else
    {
      if(mysetup.UseCutterQueue)
      {
        if(Key==kEditCut)
        {
          const char *filename=cReplayControl::NowReplaying();

          if(filename)
          {
            if(MoveCutterThread->IsCutting(filename))
              Skins.Message(mtError,tr("Recording already in cutter queue!"));
            else
            {
              cMarks marks;
              marks.Load(filename);
  
              if(!marks.Count())
                Skins.Message(mtError,tr("No editing marks defined!"));
              else
              {
                MoveCutterThread->AddToCutterQueue(filename);
                Skins.Message(mtInfo,tr("Added recording to cutter queue"));
              }
            }
          }
          return osContinue;
        }

        if(Key==kRed)
          timesearchactive=true;
      }      
      if(Key==kBack)
        return osEnd;
    }
  }

  return cReplayControl::ProcessKey(Key);
}
