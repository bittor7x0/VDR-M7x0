/*
 * autotimeredit: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menueditkeys.c 0.4 2005/11/16 18:39:18 hflor Exp $
 */

#include "menueditkeys.h"
#include "undelete.h"

// --- cMenuEditKeysItem -----------------------------------------------------

cMenuEditKeysItem::cMenuEditKeysItem(const char *Name, int *Value)
:cMenuEditStraItem(Name, Value, MaxKeysName, KeysName)
{
}

eOSState cMenuEditKeysItem::ProcessKey(eKeys Key)
{
  eOSState state = osUnknown;

  int newkey = -1;
  switch (Key)
  {
    case kBlue:           newkey += 2;  // no break!
    case kYellow:         newkey += 2;  // no break!
    case kGreen:          newkey += 2;  // no break!
    case kRed:            newkey += 2;  // newkey red=1 green=3 yellow=5 blue=7
                          if (*value == newkey)
                            newkey++;
                          if (*value == (newkey + 1))
                            newkey = 0;
                          break;
    case k0...k9:         newkey = Key - k0 + 9;
                          break;
    case kUser1...kUser9: newkey = Key - kUser1 + 19;
                          break;
    default:              state = cMenuEditStraItem::ProcessKey(Key);
                          break;
  }
  if (*value == newkey)
    newkey = 0;
  if (newkey >= 0)
  {
    state = osContinue;
    *value = newkey;
    Set();
  }
  return state;
}
