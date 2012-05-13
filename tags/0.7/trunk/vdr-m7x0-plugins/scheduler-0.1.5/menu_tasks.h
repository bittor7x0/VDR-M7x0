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
#include "task.h"

class cMenuTasksItem : public cOsdItem {
 private:
  cTask* task;
 public:
  cMenuTasksItem(cTask* Task);
  cTask* Task() { return task; }
  void Set();
};


class cMenuTasks : public cOsdMenu {
  bool keysToggled;
  eOSState Edit();
  eOSState New(void);
  eOSState Delete(void);
  eOSState StartStop(void);
  eOSState Info(void);
  void Set();
 protected:
  virtual eOSState ProcessKey(eKeys Key);
 public:
  static bool update;
  cMenuTasks();
  cTask* CurrentTask(void);
};

