//***************************************************************************
/*
 * fskcheck.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: talk.h,v 1.1 2007/01/13 07:52:27 root Exp $
 */
//***************************************************************************

//***************************************************************************
// Includes
//***************************************************************************

#include "def.h"

//***************************************************************************
// Class Talk
//***************************************************************************

class Talk
{
   public:

      // declarations

      enum Error
      {
         errMessageServiceFirst = -1000,

         errMessageToBig,

         wrnSysInterrupt,
         wrnNoMessage
      };

      enum Size
      {
         sizeBuffer = 1000
      };

      enum Event
      {
         evtUnknown = na,

         evtCheck,         // = 0
         evtShow,          // = 1
         evtAsk,           // = 2
         evtConfirm,       // = 3
         evtAbort,         // = 4
         evtAnswer,        // = 5
         evtExit           // = 6
      };
      
      struct Header
      {
         long sender;
         long receiver;
         int event;
         int type;           // eMessageType
      };

      // object

      Talk();
      ~Talk();

      // functions

      int wait();
      int init() const;
      int send(long to, int event, const char* msg = 0);
      int open(long port);

      int isOpen() const               { return opened; }
      long getTimeout() const          { return timeout; }
      int getEvent() const             { return event; } 
      long getFrom() const             { return from; } 
      const char* getMessage() const   { return message; }

      void setTimeout(long t)    { timeout = t; } 

   protected:

      // data

      int receiverID;
      int msgID;
      char buffer[sizeBuffer];
      char* message;
      Header* header;
      long timeout;
      int opened;

      int event;
      long from;
};
