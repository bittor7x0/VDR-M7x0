/*
 * pilotskinsetup.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 

#include "pilotskinsetup.h"
#include "config.h"

cPilotskinSetup::cPilotskinSetup(void)
{

  CloseOnSwitch = config.closeonswitch;
  Hidemenu     = config.hidemenu;
  
  Add(new cMenuEditBoolItem(tr("Close on OK"),         &CloseOnSwitch));
  Add(new cMenuEditBoolItem(  tr("Hide main menu entry"),        &Hidemenu,       tr("no"),            tr("yes")));
}

void cPilotskinSetup::Store(void)
{
  SetupStore("CloseOnSwitch", config.closeonswitch = CloseOnSwitch);
  SetupStore("Hidemenu",     config.hidemenu = Hidemenu);
}


