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

#ifndef VDR_WORKERTHREAD_H
#define VDR_WORKERTHREAD_H

#include <string>
#include <vdr/thread.h>

class cTask;

class cWorkerThread: public cThread {
 private:
  long taskid;
 protected:
  virtual void Action(void);
 public:
  cWorkerThread(long TaskId);
  ~cWorkerThread();
  std::string Execute(cTask*);
  void Stop();
  long TaskId() { return taskid; }
};

#endif 
