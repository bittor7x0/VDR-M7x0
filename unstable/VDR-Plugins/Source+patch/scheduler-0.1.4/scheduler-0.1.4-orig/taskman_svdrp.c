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

/*
 * taskman_svdrp.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <string>
#include <vdr/plugin.h>
#include "scheduler.h"
#include "task.h"
#include "workerthread.h"
#include "taskmanager_thread.h"
#include "log.h"

using namespace std;

const char **cPluginScheduler::SVDRPHelpPages(void)
{
   static const char *HelpPages[] = {
      "LSTT [ Id ]\n"
      "    List all tasks or the one with the given Id",
      "NEWT <tasksettings>\n"
      "    Adds a new task with <tasksettings> as in tasks.conf",
      "DELT <taskId>\n"
      "    Delete the task with given Id",
      "EDIT <tasksettings>\n"
      "    Modify a task with the given new settings",
      "RUNC <cmd>\n"
      "    Starts the given command and returns\n"
      "    'task started with id: <taskId>' on successful startup",
      "RUNT <taskname>\n"
      "    Starts an existing task with the given name and returns\n"
      "    'task started with id: <taskId>' on successful startup",
      "STAT <taskId>\n"
      "    Returns the execution state of a task given by <taskId>",
      NULL
   };
   return HelpPages;
}

cString cPluginScheduler::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if (strcasecmp(Command, "LSTT") == 0) 
    {
      if (*Option) 
	{
	  if (isnumber(Option))
	    {
	      cTask *Task = Tasks.TaskFromId(atol(Option));
	      if (Task)
		return cString::sprintf("%s", Task->ToText());
	      else
		{
		  ReplyCode = 901;
		  return cString::sprintf("task id %s not defined", Option);
		}
	    }
	  else
	    {
	      ReplyCode = 901;
	      return cString::sprintf("Error in task ID \"%s\"", Option);	
	    }
	}
      else if (Tasks.Count()>0) 
	{
	  string sBuffer;
	  cMutexLock TasksLock(&Tasks);
	  for (int i = 0; i < Tasks.Count(); i++) 
	    {
	      cTask* Task = Tasks.Get(i);
	      if (Task)
		sBuffer += string(Task->ToText()) + string((i<Tasks.Count()-1)?"\n":"");
	    }
	  return sBuffer.c_str();
	}
      else
	{
	  ReplyCode = 901;
	  return cString("no tasks defined");	    	
	}
    }
  else if (strcasecmp(Command, "NEWT") == 0) 
    {
      if (*Option) 
	{
	  cTask* Task = new cTask;;
	  if (Task->Parse(Option))
	    {
	      Task->SetId(Tasks.GetNewId());
	      LogFile.Log(1,"added task '%s' (%ld) via SVDRP", Task->Name(), Task->Id());
	      cMutexLock TasksLock(&Tasks);
	      Tasks.Add(Task);
	      Tasks.Save();
	      return cString::sprintf("task '%s' (with new ID %ld) added", Task->Name(), Task->Id());
         }
         else
         {
	   ReplyCode = 901;
	   delete Task;
	   return cString("Error in task settings");	
         }
      }
      else
      {
         ReplyCode = 901;
         return cString("missing task settings");	    	
      }
    }
  else if (strcasecmp(Command, "EDIT") == 0) 
    {
      if (*Option) 
	{
	  cTask Task;
	  if (Task.Parse(Option))
	    {
	      cTask *taskTemp = Tasks.TaskFromId(Task.Id());
	      if (taskTemp)
		{
		  taskTemp->Parse(Option);
		  LogFile.Log(1,"modified task '%s' (%ld) via SVDRP", taskTemp->Name(), taskTemp->Id());
		  Tasks.Save();
		  return cString::sprintf("task '%s' with %ld modified",  taskTemp->Name(), taskTemp->Id());
		}
	      else
		{
		  ReplyCode = 901;
		  return cString::sprintf("task id %ld does not exists", Task.Id());
		}
	    }
	  else
	    {
	      ReplyCode = 901;
	      return cString("Error in task settings");	
	    }
	}
      else
	{
	  ReplyCode = 901;
	  return cString("missing task settings");	    	
	}
    }
  else if (strcasecmp(Command, "RUNC") == 0) 
    {
      if (*Option) 
	{
	  cTask* tempTask = TempTasks.AddTemp(Option);
	  if (tempTask && tempTask->Run())
	    return cString::sprintf("task started with id: %ld", tempTask->Id());
	  else
	    {
	      ReplyCode = 901;
	      return cString::sprintf("failed to start task '%s'", Option);
	    }
	}
      else
	{
	  ReplyCode = 901;
	  return cString("no task name given");	    	
	}
    }
  else if (strcasecmp(Command, "DELT") == 0) 
    {
      if (*Option) 
	{
	  if (isnumber(Option))
	    {
	      long taskId = atol(Option);
	      cTask *Task = Tasks.TaskFromId(atol(Option));
	      if (Task)
		{
		  if (Task->Active())
		    {
		      ReplyCode = 901;
		      return cString("task is currently executed");
		    }
		  LogFile.Log(1,"task '%s' deleted via SVDRP", Task->Name());
		  cMutexLock TasksLock(&Tasks);
		  Tasks.Del(Task);
		  Tasks.Save();
		  return cString::sprintf("task id %ld deleted", taskId);
		}
	      else
		{
		  ReplyCode = 901;
		  return cString::sprintf("task id %ld not defined", taskId);
		}
	    }
	  else
	    {
	      ReplyCode = 901;
	      return cString::sprintf("Error in task ID \"%s\"", Option);	
	    }
	}
      else
	{
	  ReplyCode = 901;
	  return cString("missing task ID");	    	
	}
    }
  else if (strcasecmp(Command, "RUNT") == 0) 
    {
      if (*Option) 
	{
	  cTask* Task = Tasks.TaskFromName(Option);
	  if (!Task)
	    {
	      ReplyCode = 901;
	      return cString("task not found");	    	
	    }
	  if (Task->Run())
	    return cString::sprintf("task started with id: %ld", Task->Id());
	  else
	    {
	      ReplyCode = 901;
	      return cString::sprintf("failed to start task '%s'", Option);
	    }
	}
      else
	{
	  ReplyCode = 901;
	  return cString("no task name given");	    	
	}
    }
  else if (strcasecmp(Command, "STAT") == 0) 
    {
      if (*Option) 
	{
	  long Id = atol(Option);
	  cTask* Task = cTasks::TaskFromId(Id);
	  cWorkerThread* workerThread = cTaskManagerThread::ThreadById(Id);
	  if (!Task)
	    {
	      ReplyCode = 901;
	      return cString("task not found");	    	
	    }
	  if (workerThread && workerThread->Active())
	    return cString("task running");
	  else
	    return cString("task not running");
	}
      else
	{
	  ReplyCode = 901;
	  return cString("no task name given");	    	
	}
    }
   return NULL;
}
