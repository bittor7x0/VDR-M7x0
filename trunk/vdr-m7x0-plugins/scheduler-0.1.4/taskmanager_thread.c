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

#include <vdr/plugin.h>
#include <vdr/remote.h>
#include "taskmanager_thread.h"
#include "task.h"
#include "log.h"

using namespace std;

#define SLEEPSECS(x) cCondWait::SleepMs(1000*x)

cTaskManagerThread *cTaskManagerThread::m_Instance = NULL;
map<long, cWorkerThread*> cTaskManagerThread::workerThreads;
vector<long> cTaskManagerThread::finishedTasks;
bool cTaskManagerThread::shutdown = false;

cTaskManagerThread::cTaskManagerThread(void)
   : cThread("Tasker: task manager")
{
   m_Active = false;
}

cTaskManagerThread::~cTaskManagerThread() 
{
  if (m_Active) 
    Stop();
  for(map<long, cWorkerThread*>::iterator i = workerThreads.begin(); i != workerThreads.end(); ++i)
    DELETENULL((*i).second);    
}

void cTaskManagerThread::Init(void) {
   if (m_Instance == NULL) {
      m_Instance = new cTaskManagerThread;
      m_Instance->Start();
   }
}

void cTaskManagerThread::Exit(void) {
   if (m_Instance != NULL) {
      m_Instance->Stop();
      DELETENULL(m_Instance);
   }
}

void cTaskManagerThread::Stop(void) {
   m_Active = false;
   for(map<long, cWorkerThread*>::iterator i = workerThreads.begin(); i != workerThreads.end(); ++i)
     (*i).second->Stop();    
   Cancel(3);
}

bool cTaskManagerThread::StartTask(cTask* Task) 
{
  cThreadLock TaskManagerLock(cTaskManagerThread::m_Instance);
  cWorkerThread* workerThread = ThreadById(Task->Id());
  if (!workerThread)
    {
      workerThread = new cWorkerThread(Task->Id());
      pair<map<long, cWorkerThread*>::iterator,bool> ret;
      ret = workerThreads.insert(map<long, cWorkerThread*>::value_type(Task->Id(), workerThread));
      if (ret.second == false) return false;
    }
  shutdown = false;
  return workerThread->Start();
}

bool cTaskManagerThread::CancelTask(cTask* Task) 
{
  if (cTasks::IsTemp(Task)) return false;
  cThreadLock TaskManagerLock(cTaskManagerThread::m_Instance);
  cWorkerThread* workerThread = ThreadById(Task->Id());
  if (!workerThread || !workerThread->Active()) return false;
  workerThreads.erase(Task->Id());
  delete workerThread;
  return true;
}

cWorkerThread* cTaskManagerThread::ThreadById(long Id)
{
  RemoveFinished();
  map<long, cWorkerThread*>::iterator i = workerThreads.find(Id);
  if (i != workerThreads.end() && (*i).second)
    return (*i).second;
  else
    return NULL;
}

bool cTaskManagerThread::TaskActive(cTask* Task)
{
  cWorkerThread* workerThread = ThreadById(Task->Id());
  return (workerThread && workerThread->Active());
}

void cTaskManagerThread::SetFinished(long TaskId)
{
  cThreadLock TaskManagerLock(cTaskManagerThread::m_Instance);
  finishedTasks.push_back(TaskId);
}

void cTaskManagerThread::RemoveFinished()
{
  cThreadLock TaskManagerLock(cTaskManagerThread::m_Instance);
  if (finishedTasks.size() == 0) return;

  // remove finished worker threads
  vector<long>::iterator i;
  for(i = finishedTasks.begin(); i != finishedTasks.end(); ++i)
    {
      cTask* task = Tasks.TaskFromId(*i);
      if (!task) continue;
      
      map<long, cWorkerThread*>::iterator iW = workerThreads.find(*i);
      if (iW != workerThreads.end())
	{
	  while ((*iW).second->Active()) SLEEPSECS(1);
	  DELETENULL((*iW).second);
	  workerThreads.erase(*i);
	}
      if (task->Wakeup())
	{
	  LogFile.Log(3,"trigger shutdown after task '%s'", task->Name());
	  shutdown = true;
	}
    }
  finishedTasks.clear();
}

void cTaskManagerThread::Action(void) 
{
   m_Active = true;
   LogFile.iSysLog("starting task manager thread");
   time_t nextUpdate = time(NULL);
   while (m_Active) 
   {
     RemoveFinished();
     cString active = Tasks.Active();
     if (shutdown && isempty(active))
       {
	 shutdown = false;
	 cRemote::Put(kPower);
       }

      time_t now = time(NULL);
      if (now >= nextUpdate)
      {
         LogFile.Log(2,"look for tasks to execute or cancel");
         cMutexLock TasksLock(&Tasks);
         cTask* task = Tasks.First();
         while (task) 
         {
	   if (task->Active() && task->KillAfterMins() && now - task->LastStart() > task->KillAfterMins()*60)
	     if (!CancelTask(task))
	       LogFile.eSysLog("failed to cancel '%s' (cmd: '%s')", task->Name(), task->Cmd());
	     else
	       LogFile.iSysLog("job '%s' (cmd: '%s') killed (took too long)", task->Name(), task->Cmd());

	   if (task->Enabled() && task->MatchesTime(now))
	     if (!StartTask(task))
	       LogFile.eSysLog("failed to start '%s' (cmd: '%s')", task->Name(), task->Cmd());

	   task = Tasks.Next(task);
         }
         SLEEPSECS(1);
         nextUpdate = long(time(NULL)/60)*60+ 60; // check at each full minute
      }
      while (m_Active && time(NULL)%60 != 0 && !shutdown) // sync heart beat to minutes
	{
         SLEEPSECS(1);
	 RemoveFinished();
	}
   };
   LogFile.iSysLog("leaving task manager thread");
   m_Instance = NULL;
}

