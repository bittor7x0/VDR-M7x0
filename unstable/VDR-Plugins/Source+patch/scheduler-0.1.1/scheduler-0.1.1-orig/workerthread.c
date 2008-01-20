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
#include <vdr/plugin.h>

#include "workerthread.h"
#include "taskmanager_thread.h"
#include "task.h"
#include "log.h"
#include "menu_tasks.h"

using namespace std;


cWorkerThread::cWorkerThread(long TaskId)
: taskid(TaskId)
{
  cTask* task = Tasks.TaskFromId(taskid);
  if (task)
    {
      SetDescription("'%s' worker", task->Name());
      LogFile.Log(3,"creating worker thread for task '%s'",  task->Name());
    }
}

cWorkerThread::~cWorkerThread()
{
  cTask* task = Tasks.TaskFromId(taskid);
  if (task)
    LogFile.Log(3,"deleting worker thread for task '%s'",  task->Name());
  cMenuTasks::update = true; 
}

string cWorkerThread::Execute(cTask* task)
{
  string command = task->Cmd();
  string result = "";
  dsyslog("taskman: executing command '%s'", command.c_str());
  cPipe p;
  if (p.Open(command.c_str(), "r")) {
     int c;
     while ((c = fgetc(p)) != EOF) 
       {
	 result += char(c);
	 task->SetCurrentOutput(result);
       }
     p.Close();
  }
  else
    esyslog("ERROR: can't open pipe for command '%s'", command.c_str());
  return result;
}

void cWorkerThread::Stop()
{
  Cancel();
}

void cWorkerThread::Action(void) 
{
  cTask* task = Tasks.TaskFromId(taskid);
  if (!task) 
    {
      LogFile.Log(2,"ooops. No task with id: %ld", taskid);
      return;
    }

  LogFile.Log(1,"starting '%s' (cmd: %s)", task->Name(), task->Cmd());

  if (task->announceMode & 1) 
    Skins.QueueMessage(mtInfo, cString::sprintf(tr("%s started"), task->Name()), 5, 0);
  task->lastStart = time(NULL); 

  string result = Execute(task);
  if (task->logging) 
    {
      task->SetCurrentOutput("");
      task->taskLog.Insert(result);
    }

  cMenuTasks::update = true;

  LogFile.Log(1,"finished '%s' (cmd: %s)", task->Name(), task->Cmd());
  if (!result.empty())
      LogFile.Log(3,"result from '%s':\n%s", task->Name(), result.c_str());
  
  if (task->announceMode & 2) 
    Skins.QueueMessage(mtInfo, cString::sprintf(tr("%s finished"), task->Name()), 5, 0);

  if (cTasks::IsTemp(task))
    {
      cMutexLock TempTasksLock(&TempTasks);
      TempTasks.Del(task);
    }
  cTaskManagerThread::SetFinished(taskid);
}

