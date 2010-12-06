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

#include <sstream>
#include "menu_tasks.h"
#include "menu_taskedit.h"
#include "menu_logs.h"
#include <vdr/interface.h>
#include <vdr/menu.h>
#include "i18n.h"

using namespace std;

bool cMenuTasks::update = false;

cMenuTasksItem::cMenuTasksItem(cTask* Task)
: task(Task)
{
  Set();
}

void cMenuTasksItem::Set()
{
  if (!task) return;

  string status;
  if (task->Enabled()) status = ">";
  if (task->Active()) status = "#";

  ostringstream line;  


  line << status << "\t" << task->Name();
  SetText(strdup(line.str().c_str()), false);
}

// --- cMenuTasks ----------------------------------------------------------
cMenuTasks::cMenuTasks()
:cOsdMenu(tr("Tasks"), 2)
{
  keysToggled = false;
  Set();
  update = true;
}

void cMenuTasks::Set()
{
  int current = Current();
  Clear();
  
  cMutexLock TasksLock(&Tasks);
  cTask *Task = Tasks.First();
  while (Task) {	
    Add(new cMenuTasksItem(Task));
    Task = Tasks.Next(Task);
  }
  
  SetCurrent(Get(current));

  Task = CurrentTask();
  SetHelp(trVDR("Button$Edit"), tr("Button$New"), trVDR("Button$Delete"), 
	  Task && Task->Active() ? tr("Button$Stop"):tr("Button$Start"));
  Display();
  update = false;
}

cTask *cMenuTasks::CurrentTask(void)
{
    cMenuTasksItem *item = (cMenuTasksItem *)Get(Current());
    if (!item) return NULL;
    return item->Task();
}

eOSState cMenuTasks::New()
{
  return AddSubMenu(new cMenuTaskEdit(new cTask, true));
}

eOSState cMenuTasks::Edit()
{
  if (CurrentTask())
    return AddSubMenu(new cMenuTaskEdit(CurrentTask(), false));
  else
    return osContinue;
}

eOSState cMenuTasks::Delete()
{
  cTask* Task = CurrentTask();
  if (Task)
    {
      if (Task->Active())
	Skins.Message(mtInfo, cString::sprintf(tr("%s currently running"), Task->Name()));	    
      else if (Interface->Confirm(tr("Delete task?"))) 
	{
	  cMutexLock TasksLock(&Tasks);
	  Tasks.Del(Task);
	  Tasks.Save();
	  cOsdMenu::Del(Current());
	  Display();
	}
    }
  return osContinue;
}

eOSState cMenuTasks::StartStop()
{
  cTask* Task = CurrentTask();
  if (Task) 
    {
      if (Task->Active())
	{
	  if (Interface->Confirm(tr("Abort this task?")))
	    Task->Cancel();
	}
      else if (Interface->Confirm(tr("Execute this task?")))
	Task->Run();
    }
  Set();
  return osContinue;
}

eOSState cMenuTasks::Info()
{
  cTask* curTask = CurrentTask();
  if (curTask)
    return AddSubMenu(new cMenuTaskLogs(curTask)); 
  return osContinue;
}

eOSState cMenuTasks::ProcessKey(eKeys Key)
{
  int TaskNumber = HasSubMenu() ? Count() : -1;

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (!HasSubMenu() && (update || Key != kNone))
    Set();

  if (!HasSubMenu() && state == osUnknown) {
    switch (Key) {
    case kRed:
      state = Edit();
      break;
    case kGreen:
      state = New();
      break;      
    case kYellow:
      state = Delete();
      break;      
    case kBlue:
      state = StartStop();  
      break;
    case kOk:
      state = Info();
      break;      
    default: break;
    }
  }
  if (TaskNumber >= 0 && !HasSubMenu()) 
  {      
      cTask* Task = Tasks.Get(TaskNumber);
      if (Task)       // a newly created search was confirmed with Ok
	  Add(new cMenuTasksItem(Task));
      Display();
  }
  
  return state;
}
