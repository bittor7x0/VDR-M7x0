

#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>

#include <vdr/tools.h>

#include "def.h"
#include "talk.h"

//***************************************************************************
// Statics
//***************************************************************************

static int fTimeout = no;

//*************************************************************************
// Timeout-Handler
//*************************************************************************

void timeoutHandler(...)
{
   fTimeout = true;
}

//***************************************************************************
// Object
//***************************************************************************

Talk::Talk()
{
   struct msgbuf* theMsg = (struct msgbuf*)buffer;

   header = (Header*)theMsg->mtext;
   message = theMsg->mtext + sizeof(Header);
   receiverID = na;
   msgID = na;
   timeout = 30;
   opened = no;
}

Talk::~Talk()
{
   if (opened)
   {
      if (msgctl(msgID, IPC_RMID, 0) < 0)
         tell(eloAlways, "Error: Closing message queue failed, '%s'", strerror(errno));
   }
}

//***************************************************************************
// Init
//***************************************************************************

int Talk::init()
{
   return success;
}

//***************************************************************************
// Open
//***************************************************************************

int Talk::open(long port)
{
   if ((msgID = msgget(0xd8000000 + port, IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) < 0)
   {
      tell(eloAlways, "Creating of message queue failed, error was '%s'", strerror(errno));

      return fail;
   }

   opened = yes;

   return success;
}

//***************************************************************************
// Wait
//***************************************************************************

int Talk::wait()
{
   int result;
   int status = success;
   struct itimerval oldTimer,  newTimer;
   struct sigaction oldAction, newAction;
   struct msgbuf* theMsg = (struct msgbuf*)buffer;

   // clear message buffer

   memset(buffer, 0, sizeBuffer);

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

   // receive auf

   if ((result = msgrcv(msgID, theMsg, sizeBuffer, 0, 0)) < 0)
   {
      if (!fTimeout)
      {
         if (errno == EINTR)
            return wrnSysInterrupt;
         else if (errno == E2BIG)
            return errMessageToBig;
         else if (errno != ENOMSG)
            tell(eloAlways, "Receiving message failed, errno (%d), '%s'", 
                    errno, strerror(errno));
      }

      status = fail;
   }

   if (timeout)
   {
      sigaction(SIGALRM, &oldAction, 0);
      setitimer(ITIMER_REAL, &oldTimer,0);
   }

   if (status == success)
   {
      event = header->event;
      from = header->sender;
   }

   return status;
}

//***************************************************************************
// Send
//***************************************************************************

int Talk::send(long to, int event, const char* msg)
{
   struct msgbuf* theMsg = (struct msgbuf*)buffer;
   int sizeMsg = sizeof(Header);
   int receiverID;

   if (!msg) msg = "";

   if (strlen(msg) > sizeBuffer - sizeof(Header) - sizeof(theMsg->mtype))
   {
      tell(eloAlways, "Message to big, aborting");
      return fail;
   }

   // try to get the receiver ID
   
   if ((receiverID = msgget(0xd8000000 + to, 0)) < 0)
   {
      tell(eloAlways, "Attach to message queue failed, '%s'", strerror(errno));
      return fail;
   }

   strcpy(message, msg);
   sizeMsg += strlen(message)+1;
   
   theMsg->mtype = 1;
   header->sender = msgID;
   header->receiver = receiverID;
   header->event = event;
   header->type = 0;

   if (msgsnd(header->receiver, theMsg, sizeMsg, 0) < 0)
   {
      tell(eloAlways, "send failed, '%s'", strerror(errno));
      return fail;
   }

   tell(eloAlways, "send to (%ld) succeeded", header->receiver);

   return success;
}
