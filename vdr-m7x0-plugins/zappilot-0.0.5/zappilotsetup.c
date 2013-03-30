/*
 * zappilotsetup.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "zappilotsetup.h"
#include "config.h"

cZappilotSetup::cZappilotSetup(void)
{

   CloseOnSwitch = config.closeonswitch;
   Hidemenu      = config.hidemenu;
   FastBrowse    = config.fastbrowse;
   FastBrowseAlt = config.fastbrowsealt;
   SwitchTimer   = config.switchtimer;
   SwitchMinsBefore = config.switchminsbefore;

   Add(new cMenuEditBoolItem( tr("Close on OK"), &CloseOnSwitch));
   Add(new cMenuEditBoolItem( tr("Hide main menu entry"), &Hidemenu, trVDR("no"), trVDR("yes")));
   Add(new cMenuEditBoolItem( tr("Fast browse"), &FastBrowse, trVDR("no"), trVDR("yes")));
   Add(new cMenuEditBoolItem( tr("Fast browse (alt. key layout)"), &FastBrowseAlt, trVDR("no"), trVDR("yes")));
   if (config.pEPGSearch)
   {
      Add(new cMenuEditBoolItem( tr("Switch timer"), &SwitchTimer, trVDR("no"), trVDR("yes")));
      Add(new cMenuEditIntItem( tr("Switch ... minutes before start"), &SwitchMinsBefore));
   }
}

void cZappilotSetup::Store(void)
{
   SetupStore("CloseOnSwitch", config.closeonswitch = CloseOnSwitch);
   SetupStore("Hidemenu",      config.hidemenu = Hidemenu);
   SetupStore("FastBrowse",    config.fastbrowse = FastBrowse);
   SetupStore("FastBrowseAlt",    config.fastbrowsealt = FastBrowseAlt);
   SetupStore("SwitchTimer",   config.switchtimer = SwitchTimer);
   SetupStore("SwitchMinsBefore",   config.switchminsbefore = SwitchMinsBefore);
}
