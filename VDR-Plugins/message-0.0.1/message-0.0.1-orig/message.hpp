//***************************************************************************
/*
 * message.hpp: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
//***************************************************************************

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

//***************************************************************************
// Includes
//***************************************************************************

#include <sys/msg.h>

//***************************************************************************
// Common
//***************************************************************************

enum Return
{
   success = 0,
   na      = -1,
   done    = success,
   fail    = na,
   yes = 1,
   no = 0
};


//***************************************************************************
// Class 
//***************************************************************************

class MessageService
{
   public:
 
      enum Misc
      {
         sizeBuffer = 1000
      };

      enum Event
      {
         evtShow  = 0,
         evtAsk,
         evtConfirm,
         evtAbort,
         evtAnser
      };

      struct Header
      {
         long sender;
         long receiver;
         int event;
         int type;           // eMessageType
      };
};

typedef MessageService Ms;

//***************************************************************************
#endif // __MESSAGE_H__
