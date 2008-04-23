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

#ifndef VDR_TASKMAANGER_THREAD_H
#define VDR_TASKMAANGER_THREAD_H

#include <map>
#include <vector>
#include <vdr/thread.h>
#include "task.h"
#include "workerthread.h"

class cTaskManagerThread: public cThread {
 private:
    bool m_Active;
    static std::map<long, cWorkerThread*> workerThreads;
    static std::vector<long> finishedTasks;
    static bool shutdown;
 protected:
    virtual void Action(void);
    void Stop(void);
 public:
    static cTaskManagerThread *m_Instance;

    cTaskManagerThread(void);
    virtual ~cTaskManagerThread();
    static void Init(void);
    static void Exit(void);
    static bool StartTask(cTask*);
    static bool CancelTask(cTask*);
    static bool TaskActive(cTask*);
    static cWorkerThread* ThreadById(long Id);
    static void SetFinished(long);
    static void RemoveFinished();
};

#endif 
