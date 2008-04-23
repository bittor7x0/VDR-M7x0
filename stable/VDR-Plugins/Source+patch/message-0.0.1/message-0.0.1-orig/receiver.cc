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

#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

char buffer[1000];

const char* dummy = "Hallo VDR!";
int msgID = 0;
const char* message = 0;

//***************************************************************************
// Wait
//***************************************************************************

int wait()
{
   int result;
   struct msgbuf* theMsg  = (struct msgbuf*)buffer;

   if ((result = msgrcv(msgID, theMsg, 1000, 0, 0)) < 0)
   {
      printf("Receiving message failed, '%s'\n", strerror(errno));
      return -1;
   }

   else
   {
      printf("Message '%s' received\n", message);
   }

   return 0;
}

//***************************************************************************
// Main
//***************************************************************************

int main()
{
   struct msgbuf* theMsg  = (struct msgbuf*)buffer;
   message = theMsg->mtext;
   
   if ((msgID = msgget(0xd7000000, IPC_CREAT)) < 0)
   {
      printf("creation of message queue failed, '%s'\n", strerror(errno));
      return -1;
   }

   printf("message queue (%d) created\n", msgID);

   wait();

   if (msgctl(msgID, IPC_RMID, 0) < 0)
      printf("Can't close message queue. '%s'\n", strerror(errno));

   return 0;
}
