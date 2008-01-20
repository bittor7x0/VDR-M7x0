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

#include <vector>
#include "menu_myedittimer.h"
#include "epgsearchcfg.h"
#include "epgsearchcats.h"
#include "timer_thread.h"
#include "menu_dirselect.h"
#include "menu_searchedit.h"
#include "epgsearchtools.h"
#include "recstatus.h"
#include "uservars.h"
#include "menu_deftimercheckmethod.h"
#include <math.h>

const char *cMenuMyEditTimer::CheckModes[3];

cMenuMyEditTimer::cMenuMyEditTimer(cTimer *Timer, bool New, const cEvent* Event, const cChannel* forcechannel)
:cOsdMenu(trVDR("Edit timer"), 14)
{
    CheckModes[0] = tr("no check");
    CheckModes[UPD_CHDUR] = tr("by channel and time");
    CheckModes[UPD_EVENTID] = tr("by event ID");

    strcpy(file, "");
    strcpy(directory, "");
    UserDefDaysOfWeek = 0;
    checkmode = 0;
    if (Timer) 
    {
	timer = Timer;
	event = Event;
	flags = Timer->Flags();
	day = Timer->Day();
	weekdays = Timer->WeekDays();
	
	start = Timer->Start();
	stop = Timer->Stop();
	priority = Timer->Priority();
	lifetime = Timer->Lifetime();
	strcpy(file, Timer->File());
	channel = Timer->Channel()->Number();
#ifdef USE_PINPLUGIN
    fskProtection = Timer->FskProtection();  
#endif
	if (forcechannel)
	    channel = forcechannel->Number();
	SplitFile();
	
	addIfConfirmed = New;
	Set();
	SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, NULL, NULL);
    }
}

void cMenuMyEditTimer::SplitFile()
{
    char* tmp = strrchr(file, '~');
    if (tmp) // split file in real file and directory
    {
	if (event && !isempty(event->ShortText())) 
	{
	    char* eventFile = NULL;
	    asprintf(&eventFile, "%s~%s", event->Title(), event->ShortText());
	    char* tmp2 = strstr(file, eventFile); 
	    free(eventFile);
	    if (tmp2) // file contains title and subtitle
	    {
		if (tmp2 > file)
		{
		    *(tmp2-1) = 0;
		    strcpy(directory, file);
		    strcpy(file, tmp2);
		}
		else
		    *directory = 0;
	    }
	    else
	    {
		*tmp = 0;
		strcpy(directory, file);
		strcpy(file, tmp+1);
	    }
	}
	else
	{
	    *tmp = 0;
	    strcpy(directory, file);
	    strcpy(file, tmp+1);
	}
    }
    else
	*directory = 0;
}

void cMenuMyEditTimer::Set()
{
    int current = Current();
    Clear();

    cSearchExt* search = TriggeredFromSearchTimer(timer);    

    Add(new cMenuEditBitItem( trVDR("Active"),       &flags, tfActive));
#ifdef USE_PINPLUGIN
    if (cOsd::pinValid) Add(new cMenuEditChanItem(tr("Channel"), &channel));
    else {
       char* buf = 0;
       asprintf(&buf, "%s\t%s", tr("Channel"), Channels.GetByNumber(channel)->Name());
       Add(new cOsdItem(buf));
       free(buf);
    }
#else
    Add(new cMenuEditChanItem(trVDR("Channel"),      &channel));
#endif
    Add(new cMenuEditDateItem(trVDR("Day"),          &day, &weekdays));

    if (!IsSingleEvent())
	Add(new cMenuEditDateItem(trVDR("First day"), &day));

    Add(new cMenuEditTimeItem(trVDR("Start"),        &start));
    Add(new cMenuEditTimeItem(trVDR("Stop"),         &stop));
    Add(new cMenuEditBitItem( trVDR("VPS"),          &flags, tfVps));
    Add(new cMenuEditIntItem( trVDR("Priority"),     &priority, 0, MAXPRIORITY));
    Add(new cMenuEditIntItem( trVDR("Lifetime"),     &lifetime, 0, MAXLIFETIME));
#ifdef USE_PINPLUGIN
    if (cOsd::pinValid || !fskProtection) Add(new cMenuEditBoolItem(tr("Childlock"),&fskProtection));
    else {
       char* buf = 0;
       asprintf(&buf, "%s\t%s", tr("Childlock"), fskProtection ? trVDR("yes") : trVDR("no"));
       Add(new cOsdItem(buf));
       free(buf);
    }
#endif
    Add(new cMenuEditStrItem( trVDR("File"), file, MaxFileName, trVDR(FileNameChars)));
    Add(new cMenuEditStrItem( tr("Directory"), directory, MaxFileName, tr(AllowedChars)));

    if (search)
    {
	cMenuEditStrItem* searchtimerItem = new cMenuEditStrItem( tr("Search timer"), search->search, MaxFileName, tr(AllowedChars));
	searchtimerItem->SetSelectable(false);
	Add(searchtimerItem);
    }
    else if (IsSingleEvent() && event)
    {
	checkmode = DefTimerCheckModes.GetMode(Channels.GetByNumber(channel));
	char* checkmodeAux = GetAuxValue(timer, "update");
	if (checkmodeAux)
	{
	    checkmode = atoi(checkmodeAux);
	    free(checkmodeAux);
	}
	Add(new cMenuEditStraItem(tr("Timer check"), &checkmode, 3, CheckModes));
    }

    int deviceNr = gl_recStatusMonitor->TimerRecDevice(timer);
    if (deviceNr > 0)
    {
	cOsdItem* pInfoItem = new cOsdItem("");
	pInfoItem->SetSelectable(false);
	Add(pInfoItem);
	char* info = NULL;
	asprintf(&info, "%s: %d/%d", tr("recording with device"), deviceNr, cDevice::NumDevices());
	pInfoItem = new cOsdItem(info);
	free(info);
	pInfoItem->SetSelectable(false);
	Add(pInfoItem);
    }

    if (current > -1)
    {
	SetCurrent(Get(current));
        RefreshCurrent();
    }
    Display();
}

cMenuMyEditTimer::~cMenuMyEditTimer()
{
  if (timer && addIfConfirmed)
     delete timer; // apparently it wasn't confirmed
}

void cMenuMyEditTimer::HandleSubtitle()
{
    const char* ItemText = Get(Current())->Text();
    if (strstr(ItemText, tr("File")) != ItemText)
	return;
    if (InEditMode(ItemText, tr("File"), file))
	return;

    if (!event || (event && !event->ShortText()))
	return;
    char* tmp = strchr(file, '~');
    if (tmp)
    {
	*tmp = 0;
	SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, trVDR("Button$Reset"), tr("Button$With subtitle"));
    }
    else
    {
	strcat(file, "~");
	strcat(file, event->ShortText());
	SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, trVDR("Button$Reset"), tr("Button$Without subtitle"));
    }
    RefreshCurrent();
    Display();
}

bool cMenuMyEditTimer::IsSingleEvent(void) const
{
  return !weekdays;
}

eOSState cMenuMyEditTimer::DeleteTimer()
{
    // Check if this timer is active:
    if (timer) {
	if (Interface->Confirm(trVDR("Delete timer?"))) {
	    if (timer->Recording()) {
		if (Interface->Confirm(tr("Timer still recording - really delete?"))) {
		    timer->Skip();
		    cRecordControls::Process(time(NULL));
		}
		else
		    return osContinue;
	    }
	    LogFile.iSysLog("deleting timer %s", *timer->ToDescr());
	    Timers.Del(timer);
	    cOsdMenu::Del(Current());
	    Timers.SetModified();
	    Display();
	    return osBack;
        }
    }
    return osContinue;
}

eOSState cMenuMyEditTimer::ProcessKey(eKeys Key)
{
    bool bWasSingleEvent = IsSingleEvent();

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (bWasSingleEvent != IsSingleEvent())
    {
	Set();
	Display();
    }

    int iOnDirectoryItem = 0;
    int iOnFileItem = 0;
    int iOnDayItem = 0;
    const char* ItemText = Get(Current())->Text();
    if (!HasSubMenu() && ItemText && strlen(ItemText)>0)
    {
	if (strstr(ItemText, tr("Day")) == ItemText)
	{
	    if (!IsSingleEvent())
	    {
		SetHelp(trVDR("Button$Edit"));
		iOnDayItem = 1;
	    }
	    else
		SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, NULL, NULL);
	}
	else if (strstr(ItemText, tr("Directory")) == ItemText)
	{
	    if (!InEditMode(ItemText, tr("Directory"), directory))
		SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, trVDR("Button$Reset"), tr("Button$Select"));
	    iOnDirectoryItem = 1;
	}
	else if (strstr(ItemText, tr("File")) == ItemText)
	{
	    if (!InEditMode(ItemText, tr("File"), file))
	    {
		if (event && event->ShortText())
		{
		    if (strchr(file, '~'))
			SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, trVDR("Button$Reset"), tr("Button$Without subtitle"));
		    else
			SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, trVDR("Button$Reset"), tr("Button$With subtitle"));
		}
		else
		    SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, trVDR("Button$Reset"), NULL);
	    }
	    iOnFileItem = 1;
	}
	else
	    SetHelp(addIfConfirmed?NULL:trVDR("Button$Delete"), NULL, NULL, NULL);
    }
    
    if ((Key == kYellow) && ((iOnDirectoryItem && !InEditMode(ItemText, tr("Directory"), directory)) ||
			     (iOnFileItem && !InEditMode(ItemText, tr("File"), file))))
    {
	if (iOnDirectoryItem)
	    strcpy(directory, "");
	if (iOnFileItem && event)
	    strn0cpy(file, event->Title(),sizeof(file));
	RefreshCurrent();
	Display();
    }
    
    if (state == osUnknown) 
    {
	switch (Key) 
	{
	    case kOk:     
	    {
		cChannel *ch = Channels.GetByNumber(channel);
		if (!ch)
		{
		    Skins.Message(mtError, tr("*** Invalid Channel ***"));
		    break;
		}
		
		string fullaux = "";
        string aux = "";
		if (timer && timer->Aux())
		    fullaux = timer->Aux();

		int bstart = 0, bstop = 0; // calculate margins
		if (event && IsSingleEvent())
		{
		    time_t startTime = 0, stopTime = 0;;
		    int begin  = cTimer::TimeToInt(start); // seconds from midnight
		    int length = cTimer::TimeToInt(stop) - begin;		  
		    if (length < 0)
			length += SECSINDAY;	      
		    startTime = cTimer::SetTime(day, begin);
		    stopTime = startTime + length;
		    bstart = event->StartTime() - startTime;
		    bstop = stopTime - event->EndTime();

		    char* epgsearchaux = GetAuxValue(timer, "epgsearch");
		    if (epgsearchaux)
		    {
			aux = epgsearchaux;
			free(epgsearchaux);
		    }	      
		    aux = UpdateAuxValue(aux, "channel", NumToString(ch->Number()) + " - " + CHANNELNAME(ch));
		    aux = UpdateAuxValue(aux, "update", checkmode);
		    aux = UpdateAuxValue(aux, "eventid", event->EventID());
		    aux = UpdateAuxValue(aux, "bstart", bstart);
		    aux = UpdateAuxValue(aux, "bstop", bstop);
		    fullaux = UpdateAuxValue(fullaux, "epgsearch", aux);
		}
		
#ifdef USE_PINPLUGIN
        aux = "";
        aux = UpdateAuxValue(aux, "protected", fskProtection ? "yes" : "no");
        fullaux = UpdateAuxValue(fullaux, "pin-plugin", aux);
#endif
		
		char* tmpFile = strdup(file);
		tmpFile = strreplace(tmpFile, ':', '|');
		char* tmpDir = strdup(directory);
		tmpDir = strreplace(tmpDir, ':', '|');
		if (strlen(tmpFile) == 0)
		    tmpFile = strdup(CHANNELNAME(ch));
		
		if (timer) 
		{
		    char* cmdbuf = NULL;
		    if (addIfConfirmed)
			asprintf(&cmdbuf, "NEWT %d:%d:%s:%04d:%04d:%d:%d:%s%s%s:%s", 
				 flags,
				 ch->Number(),
#if VDRVERSNUM < 10503
                 PRINTDAY(day, weekdays),
#else
                 PRINTDAY(day, weekdays, true),
#endif
				 start,
				 stop,
				 priority, 
				 lifetime, 
				 strlen(tmpDir)>0?tmpDir:"",
				 (strlen(tmpDir)>0 && strlen(tmpFile)>0)?"~":"",
				 tmpFile,
				 fullaux.c_str());
		    else
			asprintf(&cmdbuf, "MODT %d %d:%d:%s:%04d:%04d:%d:%d:%s%s%s:%s", 
				 timer->Index()+1,
				 flags,
				 ch->Number(),
#if VDRVERSNUM < 10503
                 PRINTDAY(day, weekdays),
#else
                 PRINTDAY(day, weekdays, true),
#endif
				 start,
				 stop,
				 priority, 
				 lifetime, 
				 strlen(tmpDir)>0?tmpDir:"",
				 (strlen(tmpDir)>0 && strlen(tmpFile)>0)?"~":"",
				 tmpFile,
				 fullaux.c_str());
		    
		    cTimerThread timerThread;
		    timerThread.Init(cmdbuf);
		    
		    free(cmdbuf);
		    free(tmpFile);
		    free(tmpDir);
		    
		    addIfConfirmed = false;
		}
	    }
	    return osBack;
	    case kRed:
		if (HasSubMenu())
		    return osContinue;
		else
		    if (iOnDayItem)
			return AddSubMenu(new cMenuEditDaysOfWeek(&weekdays,1,false));
		    else
			return DeleteTimer();
		break;
	    case kGreen:
	    case kYellow:
		return osContinue;
	    case kBlue:   
		if (HasSubMenu())
		    return osContinue;
		if (iOnDirectoryItem && !InEditMode(ItemText, tr("Directory"), directory))
		    state = AddSubMenu(new cMenuDirSelect(directory));
		if (iOnFileItem)
		{
		    HandleSubtitle();
		    return osContinue;
		}
		break;
	    default: break;
	}
    }
    
    if (Key != kNone && !HasSubMenu())
    {
	if (iOnDirectoryItem && !InEditMode(ItemText, tr("Directory"), directory))		
	    ReplaceDirVars();
    }
    return state;
}

void cMenuMyEditTimer::ReplaceDirVars()
{
    if (!strchr(directory,'%') || !event)
	return;

    cVarExpr varExpr(directory);
    strcpy(directory, varExpr.Evaluate(event).c_str());
    if (strchr(directory, '%') != NULL) // only set directory to new value if all categories could have been replaced
	*directory = 0;
    if (varExpr.DependsOnVar("%title%", event) || varExpr.DependsOnVar("%subtitle%", event))
    {
	strcpy(file, directory);
	*directory = 0;
	SplitFile();
    }

    RefreshCurrent();
    // update title too
    if (Current() > 0)
	Get(Current()-1)->Set();
    Display();
    return;
}
 
