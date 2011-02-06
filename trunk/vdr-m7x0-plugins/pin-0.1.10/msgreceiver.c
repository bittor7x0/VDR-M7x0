/*
 * msgreceiver.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 11.04.05 - 10.09.06
 */

//***************************************************************************
// Includes
//***************************************************************************

#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>

#include <vdr/interface.h>
#include "pin.h"

//***************************************************************************
// Object
//***************************************************************************

MessageReceiver::MessageReceiver()
{ 
   active = false; 
   talk = new Talk;
   talk->setTimeout(0);   // wichtig !!
}

MessageReceiver::~MessageReceiver()
{
   delete talk;
}

//***************************************************************************
// Start Receiver
//***************************************************************************

int MessageReceiver::StartReceiver()
{
   if (!talk->isOpen())
      talk->open(10);

   if (!active)
   {
      Start();
   }
   else
   {
      tell(eloDebug, "Info: Receiver still running, stopping first!");

      StopReceiver();
      usleep(250);
      Start();
   }

   return success;
}

//***************************************************************************
// Stop Receiver
//***************************************************************************

int MessageReceiver::StopReceiver()
{
   tell(eloAlways, "Try to stop receiver thread (%d)", pid);
   active = false;
   talk->send(10, Talk::evtExit);
   Cancel(3);

   return success;
}

//***************************************************************************
// Action
//***************************************************************************
   
void MessageReceiver::Action()
{
   active = true;
   pid = getpid();

   tell(eloAlways, "PIN plugin receiver thread started (pid=%d)", pid);

   while (active)
   {
      wait();
      tell(eloDebug, "wait ...");
   }  
    
   tell(eloAlways, "PIN plugin receiver thread ended (pid=%d)", pid);
}

//***************************************************************************
// Wait
//***************************************************************************
   
int MessageReceiver::wait()
{
   int status;

   if ((status = talk->wait()) != success)
   {
      sleep(1);

      return status;
   }

   // info

   tell(eloDebug, "Got event (%d) from (%ld) with message [%s]", 
           talk->getEvent(), talk->getFrom(), 
           talk->getMessage() ? talk->getMessage() : "");

   // perform request

   switch (talk->getEvent())
   {
      case Talk::evtShow:
      {
         if (talk->getMessage() && *talk->getMessage())
         {
            Skins.QueueMessage(mtInfo, tr(talk->getMessage()));
            tell(eloDebug, "Got '%s'", talk->getMessage());
         }

         break;
      }

      case Talk::evtCheck:
      {
         int evt = cOsd::pinValid ? Talk::evtConfirm : Talk::evtAbort;

         if (talk->send(11, evt) != success)
            tell(eloDebug, "Sending event (%d) to (%d) failed", evt, 11);
      }
   }
   
   return success;
}
