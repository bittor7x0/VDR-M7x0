/*
Copyright (C) 2007 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/taskman
*/

#include <string>
#include <sstream>
#include "menu_logs.h"
#include <vdr/interface.h>

using namespace std;

// --- cMenuTaskLog ----------------------------------------------------------
cMenuTaskLog::cMenuTaskLog(cTask* Task, time_t Start)
: cMenuText(Task?Task->Name():"", ""), task(Task), start(Start)
{
  ostringstream info;
  if (task)
    {
      time_t lastStart = task->LastStart();
      if (task->Active() && lastStart == start)
	{

	  info << tr("Start") << ": " << *DateString(lastStart) << " " << *TimeString(lastStart) << endl;
	  info << tr("Current output") << ":" << endl << task->CurrentOutput();
	}
      else	
	info << task->taskLog.Read(start, true);
    }

  SetText(info.str().c_str());
}

// --- cMenuTaskLogItem ----------------------------------------------------------
cMenuTaskLogItem::cMenuTaskLogItem(cTask* Task, time_t Start)
: task(Task), start(Start)
{
  Set();
}

void cMenuTaskLogItem::Set()
{
  if (!task) return;
  ostringstream line;  
  line << string(task->Active() && start == task->LastStart()?"#":"") << "\t" << *DateString(start) << "\t" << *TimeString(start);
  SetText(strdup(line.str().c_str()), false);
}

// --- cMenuTaskLogs ----------------------------------------------------------
cMenuTaskLogs::cMenuTaskLogs(cTask* Task)
:cOsdMenu(tr("Log entries"), 2, 20), task(Task)
{
  Set();
}

void cMenuTaskLogs::Set()
{
  int current = Current();
  Clear();
  
  if (task->Active())
    Add(new cMenuTaskLogItem(task, task->LastStart())); // current log

  task->taskLog.Read();
  std::vector<time_t>::iterator i;
  for(uint i = 0; i<task->taskLog.logTimes.size(); i++)
    Add(new cMenuTaskLogItem(task, task->taskLog.logTimes[i]));
  
  SetCurrent(Get(current));
}

eOSState cMenuTaskLogs::Info()
{
  cMenuTaskLogItem *item = (cMenuTaskLogItem *)Get(Current());
  if (!item) return osContinue;
  return AddSubMenu(new cMenuTaskLog(item->task, item->start)); 
}

eOSState cMenuTaskLogs::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (!HasSubMenu() && state == osUnknown) {
    switch (Key) {
    case kOk:
      state = Info();
      break;      
    default: break;
    }
  }
  
  return state;
}

