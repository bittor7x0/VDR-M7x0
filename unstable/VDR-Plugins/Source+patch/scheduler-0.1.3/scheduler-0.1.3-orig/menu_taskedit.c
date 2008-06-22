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
#include <vector>
#include "menu_taskedit.h"
#include "i18n.h"

using namespace std;

const char TimeSpecChars[] = "0123456789, -*/";

// --- cMenuTaskEdit -------------------------------------------------------
cMenuTaskEdit::cMenuTaskEdit(cTask* Task, bool New)
:cOsdMenu(tr("Task settings"), 25), task(Task)
{
  AnnounceModes[0] = strdup(trVDR("no"));
  AnnounceModes[1] = strdup(tr("Start"));
  AnnounceModes[2] = strdup(tr("End"));
  AnnounceModes[3] = strdup(tr("Start and end"));

  addIfConfirmed = New;
  data = *Task;
  Set();
}

cMenuTaskEdit::~cMenuTaskEdit()
{
  for(int i=0; i<4; i++) free(AnnounceModes[i]);
}

void cMenuTaskEdit::Set()
{
   int current = Current();
   Clear();
  
   Add(new cMenuEditBoolItem(trVDR("Active"), &data.enabled));
   Add(new cMenuEditStrItem(tr("Description"), data.name, sizeof(data.name), trVDR(FileNameChars)));
   Add(new cMenuEditStrItem(tr("Command"), data.cmd, sizeof(data.cmd), trVDR(FileNameChars)));
   Add(new cMenuEditStrItem(tr("Minute"), data.minute, sizeof(data.minute), TimeSpecChars));
   Add(new cMenuEditStrItem(tr("Hour"), data.hour, sizeof(data.hour), TimeSpecChars));
   Add(new cMenuEditStrItem(tr("Day of week"), data.dayofweek, sizeof(data.dayofweek), TimeSpecChars));
   Add(new cMenuEditStrItem(tr("Day"), data.dayofmonth, sizeof(data.dayofmonth), TimeSpecChars));
   Add(new cMenuEditStrItem(tr("Month"), data.month, sizeof(data.month), TimeSpecChars));
   Add(new cMenuEditBoolItem(tr("Wakeup VDR"), &data.wakeup));
   Add(new cMenuEditStraItem(tr("OSD announce"), &data.announceMode, 4, AnnounceModes));
   Add(new cMenuEditIntItem(tr("Save ... logs"), &data.logging, 0, 9999));

   // Info block
   cOsdItem* pInfoItem = new cOsdItem(tr("Next executions:"));
   pInfoItem->SetSelectable(false);
   Add(pInfoItem);
   
   time_t nextExecution = time(NULL);
   for(int i=0; i<3; i++)
     {
       char* info = NULL;
       nextExecution = data.NextExecution(nextExecution + 60);
       if (nextExecution)
	 {
	   asprintf(&info, "%s %s", *DateString(nextExecution), *TimeString(nextExecution));
	   pInfoItem = new cOsdItem(info);
	   free(info);
	   pInfoItem->SetSelectable(false);
	   Add(pInfoItem);
	 }
     }

   SetCurrent(Get(current));
   Display();
}

eOSState cMenuTaskEdit::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);

   if (Key == kOk)
     Set();

   if (state == osUnknown) {
     switch (Key) {
     case kOk:
       {
	 string oldName = task->Name();
	 *task = data;
	 if (addIfConfirmed)
	   {
	     cMutexLock TasksLock(&Tasks);
	     task->SetId(Tasks.GetNewId());
	     Tasks.Add(task);
	   }
	 else if (oldName != task->Name())
	   task->taskLog.Rename(oldName);
	 Tasks.Save();
	 addIfConfirmed = false;
       }	 
       return osBack;
     default:      
       break;
     }
   }

   return state;
}
