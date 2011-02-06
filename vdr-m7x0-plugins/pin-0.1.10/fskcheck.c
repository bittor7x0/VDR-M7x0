//***************************************************************************
/*
 * fskcheck.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: fskcheck.c,v 1.3 2007/01/13 07:52:27 root Exp $
 */
//***************************************************************************

//***************************************************************************
// Includes
//***************************************************************************

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/msg.h>

#include "def.h"
#include "talk.h"

//*************************************************************************
// Global
//*************************************************************************

int choice(char** argv, int& cmd, const char*& msg, long& timeout);

const char* commands[] =
{
   "Check",
   "Info",

   0
};

enum Command
{
   cmdUnknown = na,

   cmdCheck,
   cmdInfo,

   cmdCount
};

//***************************************************************************
// Main
//***************************************************************************

int main(int argc, char** argv)
{
   Talk sender;
   int status;
   int event = Talk::evtUnknown;
   const char* msg;
   int cmd;
   long t;

   if ((status = sender.open(11)) != success)
   {
      printf("Falal: Can't init message queue, '%s'!\n", strerror(errno));
      return fail;
   }

   t = sender.getTimeout();
   msg = 0;

   if (choice(argv, cmd, msg, t) != success)
   {
      printf("Usage: fskcheck <command> [-t timeout][-v]\n");
      printf("         command:  check          - check if vdr currently protected\n");
      printf("         command:  info <message> - show message on osd\n");

      return fail;
   }

   // init

   sender.init(); 
   sender.setTimeout(t);

   // select command

   switch (cmd)
   {
      case cmdCheck: event = Talk::evtCheck;  break;
      case cmdInfo:  event = Talk::evtShow;   break;
   }

   // send 

   if (sender.send(10, event, msg) != success)
   {
      printf("Fatal: Can't send event (%d) with message '%s' to vdr (port %d), '%s'\n", 
             event, msg ? msg : "", 10, strerror(errno));
      return fail;
   }

   // wait

   if (cmd == cmdCheck)
   {
      if (sender.wait() == success)
      {
         // printf("Got (%d) \n", sender.getEvent());

         if (sender.getEvent() == Talk::evtConfirm) return 1;
         if (sender.getEvent()  == Talk::evtAbort)   return 2;

         return 0;
      }

      else
         return 0;
   }

   return 0;
}

//***************************************************************************
// Choise
//***************************************************************************

int choice(char** argv, int& cmd, const char*& msg, long& timeout)
{
   // skip program path

   argv++;

   // get command

   if (!(*argv)) 
      return fail;

   for (cmd = 0; cmd < cmdCount; cmd++)
      if (strcasecmp(commands[cmd], *argv) == 0)
         break;

   if (cmd >= cmdCount)
      return fail;

   argv++;

   while (*argv)
   {
      if (**argv == '-')
      {
         if (strcmp(*argv, "-t") == 0)
         {
            // timeout 

            if (!(*(++argv))) break;

            timeout = atoi(*argv);
         }
      }
      else
      {
         // the message

         msg = *argv;
      }

      argv++;
   }

   return success;
}
