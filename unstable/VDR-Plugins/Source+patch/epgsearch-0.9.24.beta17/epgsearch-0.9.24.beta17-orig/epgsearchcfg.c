/*
Copyright (C) 2004-2007 Christian Wieninger

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

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#include "epgsearchcfg.h"
#include "epgsearchtools.h"

cEPGSearchConfig EPGSearchConfig;

cEPGSearchConfig::cEPGSearchConfig(void)
{
  hidemenu          = 0;
  redkeymode	    = 0;
  bluekeymode       = 1;
  showProgress      = 1;
  showChannelNr     = 0;
  useSearchTimers   = 0;
  UpdateIntervall   = 30;
  SVDRPPort         = 2001;
  timeShiftValue    = 30;
  toggleGreenYellow = 1;
  StartMenu         = 0;
  DefPriority       = Setup.DefaultPriority;
  DefLifetime       = Setup.DefaultLifetime;
  DefMarginStart    = Setup.MarginStart;
  DefMarginStop     = Setup.MarginStop;
  checkTimerConflictsAfterUpdate = 0;
  checkMinPriority = 0;
  checkMinDuration = 0;
  checkMaxDays = 99;
  ignorePayTV     = 0;
  useExternalSVDRP = 0;
  ignorePrimary = 0;
  strcpy(defrecdir,"");
  useVDRTimerEditMenu = 0;
  showChannelGroups = 0;
  showDaySeparators = 0;
  showEmptyChannels = 0;
  DefSearchTemplateID = -1;
  addSubtitleToTimer = addSubtitleSmart;
  strcpy(mainmenuentry, "");
  WarEagle = 0;
  showRadioChannels = 1;
  onePressTimerCreation = 1;
  conflictCheckIntervall = 30;
  conflictCheckWithinLimit = 60;
  conflictCheckIntervall2 = 15;
  checkTimerConflAfterTimerProg = 0;
  checkTimerConflOnRecording = 0;
  showFavoritesMenu = 0;
  FavoritesMenuTimespan = 24;
  useOkForSwitch = 0;
  strcpy(MailAddress, "");
  strcpy(MailAddressTo, "");
  strcpy(MailServer, "");
  MailUseAuth = 0;
  strcpy(MailAuthUser, "");
  strcpy(MailAuthPass, "");
  strcpy(LastMailConflicts, "");
  mailViaScript = 1;
  manualTimerCheckDefault = 0;
  noAnnounceWhileReplay = 0;
  TimerProgRepeat = 0;
  maxChannelMenuNow = 0;
  noConflMsgWhileReplay = 0;
}

int cShowMode::Compare(const cListObject &ListObject) const
{
    cShowMode *p = (cShowMode *)&ListObject;
    if (EPGSearchConfig.showFavoritesMenu == 1)
    {
	if (mode == showFavorites)
	{
	    if (p->mode <= showNext) return 1;
	    else return -1;
	}
	if (p->mode == showFavorites)
	{
	    if (mode <= showNext) return -1;
	    else return 1;
	}
    }
    if (EPGSearchConfig.showFavoritesMenu == 2)
    {
	if (mode == showFavorites && p->mode != showFavorites) return 1;
	if (p->mode == showFavorites && mode != showFavorites) return -1;
    }
    if (EPGSearchConfig.showFavoritesMenu == 3)
    {
	if (mode == showFavorites)
	{
	    if (p->mode < showNext) return 1;
	    else return -1;
	}
	if (p->mode == showFavorites)
	{
	    if (mode < showNext) return -1;
	    else return 1;
	}
    }
    if (mode > showNext && p->mode > showNext)
	return seekTime - p->seekTime;
    else
	return mode - p->mode;
} 
