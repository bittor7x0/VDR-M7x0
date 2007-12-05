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

  Red         = config.red;
  Green       = config.green;
  Blue        = config.blue;
  CloseOnSwitch = config.closeonswitch;
  Hidemenu     = config.hidemenu;
  
  Add(new cMenuEditIntItem(tr("Red"),                  &Red, 0, 255));
  Add(new cMenuEditIntItem(tr("Green"),                &Green, 0, 255));
  Add(new cMenuEditIntItem(tr("Blue"),                 &Blue, 0, 255));
  Add(new cMenuEditBoolItem(tr("Close on OK"),         &CloseOnSwitch));
  Add(new cMenuEditBoolItem(  tr("Hide main menu entry"),        &Hidemenu,       tr("no"),            tr("yes")));
}

void cPilotskinSetup::Store(void)
{
  SetupStore("Red",         config.red = Red);
  SetupStore("Green",       config.green = Green);
  SetupStore("Blue",        config.blue = Blue);
  SetupStore("CloseOnSwitch", config.closeonswitch = CloseOnSwitch);
  SetupStore("Hidemenu",     config.hidemenu = Hidemenu);
}


