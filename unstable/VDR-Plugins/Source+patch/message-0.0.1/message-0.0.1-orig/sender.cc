//***************************************************************************
/*
 * sender.cc: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
//***************************************************************************

//***************************************************************************
// Includes
//***************************************************************************

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

#include "message.hpp"
#include <vdr/skins.h>

static int fTimeout = no;

//*************************************************************************
// Timeout-Handler
//*************************************************************************

void timeoutHandler(...)
{
   fTimeout = true;
}

//*************************************************************************
// Global
//*************************************************************************

int choice(char** argv, int& cmd, const char*& msg, int& silent, long& timeout);

const char* commands[] =
{
   "Status",
   "Info",
   "Warning",
   "Error",
   "Ask",

   0
};

enum Command
{
   cmdUnknown = na,

   cmdStatus,
   cmdInfo,
   cmdWarning,
   cmdError,
   cmdAsk,

   cmdCount
};

//***************************************************************************
// Class Sender
//***************************************************************************

class Sender : public MessageService
{
   public:

      Sender();
      ~Sender();

      int wait(int& event);
      int init();
      int send(int event, int type, const char* msg);

      long getTimeout()          { return timeout; }
      void setTimeout(long t)    { timeout = t; } 
      void setSilent(int s)      { silent = s; } 

   protected:

      // data

      int receiverID;
      int msgID;
      char buffer[sizeBuffer];
      char* message;
      Header* header;
      long timeout;
      int open;
      int silent;
};

//***************************************************************************
// Object
//***************************************************************************

Sender::Sender()
{
   struct msgbuf* theMsg  = (struct msgbuf*)buffer;

   message = theMsg->mtext + sizeof(Header);
   header = (Header*)theMsg->mtext;
   receiverID = na;
   msgID = na;
   timeout = 30;
   open = no;
   silent = no;
}

Sender::~Sender()
{
   if (open)
   {
      if (msgctl(msgID, IPC_RMID, 0) < 0)
         if (!silent) printf("Error: Closing message queue failed, '%s'\n", strerror(errno));
   }
}

//***************************************************************************
// Init
//***************************************************************************

int Sender::init()
{
   // attach to VDR's message queue

   if ((receiverID = msgget(0xd7000000, 0)) < 0)
   {
      if (!silent) printf("Attach to message queue failed, '%s'\n", strerror(errno));
      return fail;
   }

   // create my own message queue

   if ((msgID = msgget(0xd7000001, IPC_CREAT)) < 0)
   {
      if (!silent) printf("creation of message queue failed\n");
      return fail;
   }

   open = yes;

   return success;
}

//***************************************************************************
// Wait
//***************************************************************************

int Sender::wait(int& event)
{
   int result;
   int status = success;
   struct itimerval oldTimer,  newTimer;
   struct sigaction oldAction, newAction;

   struct msgbuf* theMsg = (struct msgbuf*)buffer;

   // set timeout-handler

   if (timeout)
   {
      // init register

      memset(&newAction, 0, sizeof(newAction));
      memset(&newTimer, 0, sizeof(newTimer));

      // set timer

      newTimer.it_value.tv_sec = timeout;
      setitimer(ITIMER_REAL, &newTimer, &oldTimer);

      // set handler

      newAction.sa_handler = (void(*)(int))timeoutHandler;
      sigaction(SIGALRM, &newAction, &oldAction);

      // clear timeout-flag

      fTimeout = no;
   }

   // receive

   if ((result = msgrcv(msgID, theMsg, sizeBuffer, 0, 0)) < 0)
   {
      if (fTimeout)
      {
         if (!silent) 
            printf("Timeout on message receive\n");
      }
      else
      {
         if (!silent) 
            printf("Receiving message failed, '%s'\n", strerror(errno));
      }

      status = fail;
   }

   if (timeout)
   {
      sigaction(SIGALRM, &oldAction, 0);
      setitimer(ITIMER_REAL, &oldTimer,0);
   }

   if (status == success)
      event = header->event;

   return status;
}

//***************************************************************************
// Send
//***************************************************************************

int Sender::send(int event, int type, const char* msg)
{
   int sizeMsg = sizeof(Header);
   struct msgbuf* theMsg  = (struct msgbuf*)buffer;

   if (strlen(msg) > sizeBuffer - sizeof(Header) - sizeof(theMsg->mtype))
   {
      if (!silent) printf("Message to big, aborting\n");
      return fail;
   }

   strcpy(message, msg);
   sizeMsg += strlen(message)+1;
   
   theMsg->mtype = 1;
   header->sender = msgID;
   header->receiver = receiverID;
   header->event = event;
   header->type = type;

   if (msgsnd(header->receiver, theMsg, sizeMsg, 0) < 0)
   {
      if (!silent) printf("send failed, '%s'\n", strerror(errno));
      return fail;
   }

   if (!silent) printf("success\n");

   return success;
}

//***************************************************************************
// Main
//***************************************************************************

int main(int argc, char** argv)
{
   Sender sender;
   int event;
   const char* msg;
   int cmd;
   long t;
   int type = mtInfo;
   int silent = no;

   t = sender.getTimeout();

   if (choice(argv, cmd, msg, silent, t) != success)
   {
      printf("Usage: send <command> [-t timeout] <text>\n");
      printf("         command:  status\n");
      printf("                   info\n");
      printf("                   warning\n");
      printf("                   error\n");
      printf("                   ask\n");

      return fail;
   }

   // init

   sender.init(); 
   sender.setTimeout(t);
   sender.setSilent(silent);

   // 

   switch (cmd)
   {
      case cmdStatus:  event = Ms::evtShow, type = mtStatus;  break;
      case cmdInfo:    event = Ms::evtShow, type = mtInfo;    break;
      case cmdWarning: event = Ms::evtShow, type = mtWarning; break;
      case cmdError:   event = Ms::evtShow, type = mtError;   break;
      case cmdAsk:     event = Ms::evtAsk,  type = mtInfo;    break;
   }

   // send 

   if (sender.send(Ms::evtAsk, type, msg) != success)
      return fail;

   // wait

   if (cmd == cmdAsk)
   {
      if (sender.wait(event) == success)
      {
         if (!silent) printf("Got (%d) \n", event);

         if (event == Ms::evtConfirm) return 2;
         if (event == Ms::evtAbort)   return 3;

         return 0;
      }

      else
         return 0;
   }

   return 1;
}

//***************************************************************************
// Choise
//***************************************************************************

int choice(char** argv, int& cmd, const char*& msg, int& silent, long& timeout)
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
         if (strcmp(*argv, "-s") == 0)
         {
            // silent

            silent = yes;
         }
         else if (strcmp(*argv, "-t") == 0)
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
