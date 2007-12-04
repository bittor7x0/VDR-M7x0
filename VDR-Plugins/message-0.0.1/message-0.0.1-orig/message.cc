//***************************************************************************
/*
 * message.cc: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
//***************************************************************************

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include "message.hpp"

//***************************************************************************
// Statics
//***************************************************************************

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Message Plugin";
//static const char* MAINMENUENTRY  = "Message";

//***************************************************************************
// Class 
//***************************************************************************

class MessageReceiver : public cThread, public MessageService
{
   public:
 
      MessageReceiver();
      ~MessageReceiver();

      int StartReceiver();
      const char* getMessage() { return message; }
      Header* getHeader()      { return header; }

   protected:

      void Action();
      int wait();

      // data

      bool active;
      int msgID;
      char buffer[sizeBuffer];
      char* message;
      Header* header;

};

//***************************************************************************
// Object
//***************************************************************************

MessageReceiver::MessageReceiver()
{ 
   struct msgbuf* theMsg  = (struct msgbuf*)buffer;

   active = false; 
   msgID = na;
   message = theMsg->mtext + sizeof(Header);
   header = (Header*)theMsg->mtext;
}

MessageReceiver::~MessageReceiver()
{
   if (active)
   {
      active = false;
      Cancel(3);
   }

   if (msgctl(msgID, IPC_RMID, 0) < 0)
      dsyslog("Error: Closing message queue failed, '%s'", strerror(errno));
}

//***************************************************************************
// Start Receiver
//***************************************************************************

int MessageReceiver::StartReceiver()
{
   if (!active)
   {
      if ((msgID = msgget(0xd7000000, IPC_CREAT)) < 0)
      {
         dsyslog("creation of message queue failed");
         return fail;
      }

      dsyslog("message queue (%d) created", msgID);
      
      Start();
   }
   else
   {
      active = false;
      Cancel(3);
      usleep(250);
      Start();
   }

   return success;
}

//***************************************************************************
// Action
//***************************************************************************
   
void MessageReceiver::Action()
{
   active = true;
   dsyslog("Message Receiver Thread started (pid=%d)", getpid());

   while (active)
   {
      wait();
   }  
    
   active = false;
   dsyslog("Message Receiver Thread ended (pid=%d)", getpid());
    
   Cancel(0);
}

//***************************************************************************
// Wait
//***************************************************************************
   
int MessageReceiver::wait()
{
   int result;
   long from;
   struct msgbuf* theMsg  = (struct msgbuf*)buffer;

   memset(buffer, 0, sizeBuffer);

   if ((result = msgrcv(msgID, theMsg, sizeBuffer, 0, 0)) < 0)
   {
      dsyslog("Receiving message failed, errno %d", errno);
      return fail;
   }

   from = getHeader()->sender;

   // info

   dsyslog("Message (%d) from (%ld) received [%s]", 
           getHeader()->event, getHeader()->sender, 
           getMessage() ? getMessage() : "");

   // check message buffer

   if (!getMessage() || !(*getMessage()))
      return fail;

   // perform request

   switch (getHeader()->event)
   {
      case evtShow:
      {
         Skins.Message((eMessageType)getHeader()->type, getMessage());
         break;
      }

      case evtAsk:
      {
         int evt;   
         int sizeMsg = sizeof(Header);

         evt = Interface->Confirm(getMessage()) ? evtConfirm : evtAbort;
         theMsg->mtype = 1;
         header->sender = msgID;
         header->receiver = from;
         header->event = evt;
         *message = 0;

         if (msgsnd(header->receiver, theMsg, sizeMsg, 0) < 0)
            dsyslog("Sending message failed, errno %d", errno);
      }
   }
   
   return success;
}

//***************************************************************************
// Class Pugin Message
//***************************************************************************
     
class cPluginMessage : public cPlugin
{
   public:

      cPluginMessage(void);
      ~cPluginMessage();

      virtual const char* Version(void)            { return VERSION; }
      virtual const char* Description(void)        { return DESCRIPTION; }
      virtual const char* MainMenuEntry(void)      { return 0; }
      virtual const char* CommandLineHelp(void);
      virtual bool ProcessArgs(int argc, char *argv[]);
      virtual bool Initialize(void);
      virtual bool Start(void);
      virtual void Housekeeping(void);

      virtual cOsdObject *MainMenuAction(void);
      virtual cMenuSetupPage *SetupMenu(void);
      virtual bool SetupParse(const char *Name, const char *Value);

   protected:

      MessageReceiver* receiver;
      
};

cPluginMessage::cPluginMessage(void)
{
   receiver = 0;
}

cPluginMessage::~cPluginMessage()
{
   if (receiver) delete receiver;
}

const char *cPluginMessage::CommandLineHelp(void)
{
   return 0;
}

bool cPluginMessage::ProcessArgs(int argc, char *argv[])
{
   return true;
}

bool cPluginMessage::Initialize(void)
{
   return true;
}

bool cPluginMessage::Start(void)
{
   receiver = new MessageReceiver;
   receiver->StartReceiver();

   return true;
}

void cPluginMessage::Housekeeping(void)
{
}

cOsdObject* cPluginMessage::MainMenuAction(void)
{
   return 0;
}

cMenuSetupPage* cPluginMessage::SetupMenu(void)
{
   return 0;
}

bool cPluginMessage::SetupParse(const char *Name, const char *Value)
{
   return false;
}

//***************************************************************************
// VDR Internals
//***************************************************************************

VDRPLUGINCREATOR(cPluginMessage);
