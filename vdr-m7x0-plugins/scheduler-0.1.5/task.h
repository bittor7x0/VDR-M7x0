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

#ifndef __TASK_H
#define __TASK_H

#include <string>
#include <set>
#include <vector>
#include <vdr/plugin.h>

// --- cValueSpec --------------------------------------------------------
class cValueSpec
{
  std::set<int> values;
  int rangeFrom;
  int rangeTo;
  void Insert(int value);
  bool valid;
 public:
  
  cValueSpec(std::string value, int range_from, int range_to);
  bool Matches(int value);
  bool IsValid();
};

class cTask;

// --- cTaskLog --------------------------------------------------------
class cTaskLog : public cMutex
{
  friend class cTask;
  friend class cMenuTaskLogs;
 protected:
  cTask* task;
  std::string currentOutput;
  std::vector<time_t> logTimes;
 public:
  static std::string logDirectory;
  std::string Filename(std::string Name = "");
  cTaskLog(cTask* Task);
  std::string Read(time_t logTime = 0, bool skipFirstLine = false);
  bool Insert(std::string);
  bool Rename(std::string);
};

// --- cTask --------------------------------------------------------
class cTask : public cListObject, cMutex {
  friend class cMenuTaskEdit;
  friend class cWorkerThread;
  friend class cTaskLog;
 protected:
  long id;
  char name[MaxFileName];
  char cmd[MaxFileName];
  // execution at
  char minute[MaxFileName];
  char hour[MaxFileName];
  char dayofmonth[MaxFileName];
  char month[MaxFileName];
  char dayofweek[MaxFileName];
  int enabled;
  int wakeup;
  int announceMode;
  time_t lastStart;
  int logging;
  int killAfterMins;
  int shutdown;

 public:
  cTaskLog taskLog;

 private:


 public:
  cTask(void);
  cTask(const char* Name, const char* Cmd, int AnnounceMode = 0);
  cTask& operator= (const cTask &Task);

  bool Parse(const char *s);
  const char *ToText(void);
  bool Save(FILE *f);

  const char* Name() const { return name; }
  const char* Cmd() const { return cmd; }
  bool MatchesTime(time_t t);
  time_t NextExecution(time_t start = 0);
  bool Run();
  bool Cancel();
  bool Active();
  bool Enabled() const { return enabled; }
  bool Wakeup() const { return wakeup; }
  bool Shutdown() const { return shutdown; }
  time_t LastStart() const { return lastStart; }
  long Id() const { return id; }
  void SetId(long Id) { id = Id; }
  void SetCurrentOutput(std::string);
  int KillAfterMins() { return killAfterMins; }
  std::string CurrentOutput();
};

// --- cTasks --------------------------------------------------------
class cTasks : public cConfig<cTask>, public cMutex {
private:
public:
    cTasks(void) {}
    ~cTasks(void) {}
    static cString Active();
    static int CountActive();
    static time_t NextWakeupTime();
    static cTask* TaskFromId(long id);
    static cTask* TaskFromName(const char* Name);
    static long GetNewId();
    static cTask* AddTemp(const char* Cmd);
    static bool IsTemp(cTask* Task);
};

extern cTasks Tasks;
extern cTasks TempTasks;

#endif
