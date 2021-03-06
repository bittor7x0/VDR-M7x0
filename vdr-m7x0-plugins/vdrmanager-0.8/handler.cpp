/*
 * event und message handler
 */

#include <unistd.h>
#include <vdr/plugin.h>
#include <vdr/timers.h>
#include "sock.h"
#include "select.h"
#include "vdrmanagerthread.h"
#include "helpers.h"

bool cHandler::HandleNewClient(cVdrmanagerClientSocket * sock)
{
  return true;
}

bool cHandler::HandleClientRequest(cVdrmanagerClientSocket * sock)
{
  while(sock->Read())
  {
    // get lines
    while (sock->IsLineComplete())
    {
      string line;
      sock->GetLine(line);

      // parse request
      size_t space = line.find(' ');
      string cmd;
      string args;
      if (space != string::npos) {
        cmd = cHelpers::ToUpper(line.substr(0, space));
        args = cHelpers::Trim(line.substr(space+1));
      } else {
        cmd = cHelpers::ToUpper(line);
        args = "";
      }

      if (!sock->IsLoggedIn() && cmd != "PASSWD") {
        sock->PutLine("!ERROR\r\n");
      }
      else if (cmd == "PASSWD")
      {
        if (args != sock->GetPassword()) {
          sock->PutLine("!ERROR\r\n");
        } else {
          sock->SetLoggedIn();
          sock->PutLine("!OK\r\n");
        }
      }
      else if (cmd == "TIMERS")
      {
        string text = cHelpers::GetTimers(args);
        sock->PutLine(text);
      }
      else if (cmd == "CHANNELS")
      {
        string text = cHelpers::GetChannels(args);
        sock->PutLine(text);
      }
      else if (cmd == "TEVENTS")
      {
        string text = cHelpers::GetTimeEvents(args);
        sock->PutLine(text);
      }
      else if (cmd == "CEVENTS")
      {
        string text = cHelpers::GetChannelEvents(args);
        sock->PutLine(text);
      }
      else if (cmd == "TIMER")
      {
        string text = cHelpers::SetTimer(args);
        sock->PutLine(text);
      }
      else if (cmd == "SEARCH")
      {
        string text = cHelpers::SearchEvents(args);
        sock->PutLine(text);
      }
      else if(cmd == "RECORDINGS")
      {
        string text = cHelpers::GetRecordings(args);
        sock->PutLine(text);
      }
      else if(cmd == "DRECORDING")
      {
        string text = cHelpers::DelRecording(args);
        sock->PutLine(text);
      }
      else if (cmd == "SETCHANNEL")
      {
        string text = cHelpers::SetChannel(args);
        sock->PutLine(text);
      }
      else if (cmd == "QUIT")
      {
        // close socket
        sock->PutLine(string("Good bye! :-)\n"));
        sock->Disconnect();
      }
    }
  }

  return true;
}
