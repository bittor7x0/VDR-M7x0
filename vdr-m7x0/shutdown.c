/*
 * shutdown.c: Handling of shutdown and inactivity
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * Original version written by Udo Richter <udo_richter@gmx.de>.
 *
 */

#include "shutdown.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <errno.h>
#include "channels.h"
#include "config.h"
#include "cutter.h"
#include "i18n.h"
#include "interface.h"
#include "menu.h"
#include "plugin.h"
#include "timers.h"
#include "tools.h"
#include "dummyplayer.h"

cShutdownHandler ShutdownHandler;

bool cUsbAutomounter::Active(void)
{
	int file=open("/var/usbautomounter", O_CREAT | O_WRONLY, DEFFILEMODE);
	if(file!=-1)
	{
		if(flock(file,LOCK_EX|LOCK_NB))
		{
			if(errno==EWOULDBLOCK)
			{
				close(file);
				return true;
			}
		}
		else
			flock(file,LOCK_UN);
		close(file);
	}
	return false;
}

cCountdown::cCountdown(void)
{
  timeout = 0;
  counter = 0;
  timedOut = false;
  message = NULL;
}

void cCountdown::Start(const char *Message, int Seconds)
{
  timeout = time(NULL) + Seconds;
  counter = -1;
  timedOut = false;
  message = Message;
  Update();
}

void cCountdown::Cancel(void)
{
  if (timeout) {
     timeout = 0;
     timedOut = false;
     Skins.Message(mtStatus, NULL);
     }
}

bool cCountdown::Done(void)
{
  if (timedOut) {
     Cancel();
     return true;
     }
  return false;
}

bool cCountdown::Update(void)
{
  if (timeout) {
     int NewCounter = (timeout - time(NULL) + 9) / 10;
     if (NewCounter <= 0)
        timedOut = true;
     if (counter != NewCounter) {
        counter = NewCounter;
        Skins.Message(mtStatus, cString::sprintf(message, *cString::sprintf("%d:%d0", counter > 0 ? counter / 6 : 0, counter > 0 ? counter % 6 : 0)));
        return true;
        }
     }
  return false;
}

cShutdownHandler::cShutdownHandler(void)
{
  activeTimeout = 0;
  retry = 0;
  shutdownCommand = NULL;
  exitCode = -1;
  emergencyExitRequested = false;
}

cShutdownHandler::~cShutdownHandler()
{
  free(shutdownCommand);
}

void cShutdownHandler::RequestEmergencyExit(void)
{
  if (Setup.EmergencyExit) {
     esyslog("initiating emergency exit");
     emergencyExitRequested = true;
     Exit(1);
     }
  else
     dsyslog("emergency exit request ignored according to setup");
}

void cShutdownHandler::CheckManualStart()
{
  if (getIaMode()) {
     // Apparently the user started VDR manually
     cDevice::PrimaryDevice()->SetTvSettings(1);
     dsyslog("assuming manual start of VDR");
     // Set inactive after MinUserInactivity
     SetUserInactiveTimeout();
     }
  else {
     // Set inactive from now on
     cDevice::PrimaryDevice()->SetTvSettings(0);
     dsyslog("assuming automatic start of VDR");
     SetUserInactive();
     // launch the dummy player
     cControl::Shutdown();
     cControl::Launch(new cDummyPlayerControl);
     }
}

void cShutdownHandler::SetShutdownCommand(const char *ShutdownCommand)
{
  free(shutdownCommand);
  shutdownCommand = ShutdownCommand ? strdup(ShutdownCommand) : NULL;
}

void cShutdownHandler::CallShutdownCommand(time_t WakeupTime, int Channel, const char *File, bool UserShutdown)
{
  time_t Delta = WakeupTime ? WakeupTime - time(NULL) : 0;
  cString cmd = cString::sprintf("%s %jd %jd %d \"%s\" %d", shutdownCommand, intmax_t(WakeupTime), intmax_t(Delta), Channel, *strescape(File, "\\\"$"), UserShutdown);
  isyslog("executing '%s'", *cmd);
  int Status = SystemExec(cmd, true);
  if (!WIFEXITED(Status) || WEXITSTATUS(Status))
     esyslog("SystemExec() failed with status %d", Status);
  else
     Setup.Save();
}

void cShutdownHandler::SetUserInactiveTimeout(int Seconds, bool Force)
{
  if (!Setup.MinUserInactivity && !Force) {
     activeTimeout = 0;
     return;
     }
  if (Seconds < 0)
     Seconds = Setup.MinUserInactivity * 60;
  activeTimeout = time(NULL) + Seconds;
}

bool cShutdownHandler::ConfirmShutdown(bool Interactive)
{
  if (!shutdownCommand) {
     if (Interactive)
        Skins.Message(mtError, tr("Can't shutdown - option '-s' not given!"));
     return false;
     }
  bool IaMode = getIaMode();
  if (cCutter::Active()) {
     if (IaMode) {
        setIaMode(0);
        cDevice::PrimaryDevice()->SetTvSettings(0);
        }
     //if (!Interactive || !Interface->Confirm(tr("Editing - shut down anyway?")))
        return false;
     }
  if (cUsbAutomounter::Active()) {
     if (IaMode) {
        setIaMode(0);
        cDevice::PrimaryDevice()->SetTvSettings(0);
        }
        return false;
     }

  cTimer *timer = Timers.GetNextActiveTimer();
  time_t Next = timer ? timer->StartTime() : 0;
  time_t Delta = timer ? Next - time(NULL) : 0;

  if (cRecordControls::Active() || (Next && Delta <= 0)) {
     if (IaMode) {
        setIaMode(0);
        cDevice::PrimaryDevice()->SetTvSettings(0);
        }
     return false;
/*
     // VPS recordings in timer end margin may cause Delta <= 0
     if (!Interactive || !Interface->Confirm(tr("Recording - shut down anyway?")))
        return false;
*/
     }
  else if (Next && Delta <= Setup.MinEventTimeout * 60) {
     // Timer within Min Event Timeout
     if (!Interactive)
        return false;
/*
     cString buf = cString::sprintf(tr("Recording in %jd minutes, shut down anyway?"), intmax_t(Delta / 60));
     if (!Interface->Confirm(buf))
        return false;
*/
     }

  if (cPluginManager::Active(Interactive ? tr("shut down anyway?") : NULL))
     return false;

  cPlugin *Plugin = cPluginManager::GetNextWakeupPlugin();
  Next = Plugin ? Plugin->WakeupTime() : 0;
  Delta = Next ? Next - time(NULL) : 0;
  if (Next && Delta <= Setup.MinEventTimeout * 60) {
     // Plugin wakeup within Min Event Timeout
     if (!Interactive) {
        if (IaMode) {
           setIaMode(0);
           cDevice::PrimaryDevice()->SetTvSettings(0);
           }
        return false;
        }
     //cString buf = cString::sprintf(tr("Plugin %s wakes up in %jd min, continue?"), Plugin->Name(), intmax_t(Delta / 60));
     //if (!Interface->Confirm(buf))
     //   return false;
     }

  return true;
}

bool cShutdownHandler::ConfirmRestart(bool Interactive)
{
  if (cCutter::Active()) {
     if (!Interactive || !Interface->Confirm(tr("Editing - restart anyway?")))
        return false;
     }

  cTimer *timer = Timers.GetNextActiveTimer();
  time_t Next  = timer ? timer->StartTime() : 0;
  time_t Delta = timer ? Next - time(NULL) : 0;

  if (cRecordControls::Active() || (Next && Delta <= 0)) {
     // VPS recordings in timer end margin may cause Delta <= 0
     if (!Interactive || !Interface->Confirm(tr("Recording - restart anyway?")))
        return false;
     }

  if (cPluginManager::Active(Interactive ? tr("restart anyway?") : NULL))
     return false;

  return true;
}

bool cShutdownHandler::DoShutdown(bool Force)
{
  dsyslog("DEBUG: DoShutdown called");
  time_t Now = time(NULL);
  cTimer *timer = Timers.GetNextActiveTimer();
  cPlugin *Plugin = cPluginManager::GetNextWakeupPlugin();

  time_t Next = timer ? timer->StartTime() : 0;
  time_t NextPlugin = Plugin ? Plugin->WakeupTime() : 0;
  if (NextPlugin && (!Next || Next > NextPlugin)) {
     Next = NextPlugin;
     timer = NULL;
     }
  time_t Delta = Next ? Next - Now : 0;

  if (Next && Delta < Setup.MinEventTimeout * 60) {
     if (!Force)
        return false;
     Delta = Setup.MinEventTimeout * 60;
     Next = Now + Delta;
     timer = NULL;
     dsyslog("reboot at %s", *TimeToString(Next));
     }

  if (Next && timer) {
     dsyslog("next timer event at %s", *TimeToString(Next));
     CallShutdownCommand(Next, timer->Channel()->Number(), timer->File(), Force);
     }
  else if (Next && Plugin) {
     CallShutdownCommand(Next, 0, Plugin->Name(), Force);
     dsyslog("next plugin wakeup at %s", *TimeToString(Next));
     }
  else
     CallShutdownCommand(Next, 0, "", Force); // Next should always be 0 here. Just for safety, pass it.

  return true;
}
