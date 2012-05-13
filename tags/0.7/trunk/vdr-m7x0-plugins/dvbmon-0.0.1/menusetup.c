/***************************************************************************
 *   Copyright (C) 2009 by Hannes Brunner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 *************************************************************************** 
 *   
 *   dvbmon: plugin for VDR 
 *   menusetup.c: setup menu
 *
 ***************************************************************************/
 
#include "i18n.h"
#include "menusetup.h"

// --- cMenuSetupDVBmon  ------------------------------------------------------

cMenuSetupDVBmon::cMenuSetupDVBmon(struct SetupData *pSetupData)
{
   this->pSetupData = pSetupData;
   
   if (pSetupData) {
      newSetupData = *pSetupData;
      Add(new cMenuEditIntItem( tr("Cycle time (1s..60s)"),      &newSetupData.cycleTime));   
   }   
}

void cMenuSetupDVBmon::Store(void)
{
   if (pSetupData != NULL) {
      // check values
      if (newSetupData.cycleTime < 1)
         newSetupData.cycleTime = 1;
      if (newSetupData.cycleTime > 60)
         newSetupData.cycleTime = 60;      
      // pass to caller
      *pSetupData = newSetupData;
      // store new values
      SetupStore("CycleTime", newSetupData.cycleTime);
   }
}
