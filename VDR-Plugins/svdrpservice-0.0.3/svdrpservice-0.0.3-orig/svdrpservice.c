/*
 * svdrpservice.c: generic service providing access to a remote SVDRP server
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/thread.h>
#include <vdr/tools.h>

#include "svdrpservice.h"
#include "connection.h"
#include "setup.h"
#include "i18n.h"

#define MAX_SVDRP_CONNECTIONS 8

static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = "SVDRP client";

class cPluginSvdrpService : public cPlugin {
private:
  cMutex connsMutex;
  cSvdrpConnection* connections[MAX_SVDRP_CONNECTIONS];
  int FindSharedConnection(const char *ServerIp, unsigned int ServerPort) const;
  int AddConnection(const char *ServerIp, unsigned int ServerPort, bool Shared);
public:
  cPluginSvdrpService(void);
  virtual ~cPluginSvdrpService();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginSvdrpService::cPluginSvdrpService(void)
{
	for (int i = 0; i < MAX_SVDRP_CONNECTIONS; i++)
		connections[i] = NULL;
}

cPluginSvdrpService::~cPluginSvdrpService()
{
	for (int i = 0; i < MAX_SVDRP_CONNECTIONS; i++) {
		DELETENULL(connections[i]);
	}
}

const char *cPluginSvdrpService::CommandLineHelp(void)
{
  return NULL;
}

bool cPluginSvdrpService::ProcessArgs(int argc, char *argv[])
{
  return true;
}

bool cPluginSvdrpService::Initialize(void)
{
  return true;
}

bool cPluginSvdrpService::Start(void)
{
  RegisterI18n(Phrases);
  return true;
}

void cPluginSvdrpService::Stop(void)
{
}

void cPluginSvdrpService::Housekeeping(void)
{
}

cOsdObject *cPluginSvdrpService::MainMenuAction(void)
{
  return NULL;
}

cMenuSetupPage *cPluginSvdrpService::SetupMenu(void)
{
  return new cSvdrpServiceMenuSetup();;
}

bool cPluginSvdrpService::SetupParse(const char *Name, const char *Value)
{
  return SvdrpServiceSetup.Parse(Name, Value);
}

bool cPluginSvdrpService::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  if (strcmp(Id, "SvdrpConnection-v1.0") == 0) {
	  if (Data) {
		  SvdrpConnection_v1_0 *conn = (SvdrpConnection_v1_0 *) Data;
		  connsMutex.Lock();
		  if (conn->handle < 0) {
			  if (conn->serverIp[0] == 0 || strcmp(conn->serverIp, "0.0.0.0") == 0)
				  conn->serverIp = SvdrpServiceSetup.serverIp;
			  if (conn->serverPort == 0)
				  conn->serverPort = SvdrpServiceSetup.serverPort;
			  if (conn->shared)
				  conn->handle = FindSharedConnection(conn->serverIp, conn->serverPort);
			  if (conn->handle < 0)
				  conn->handle = AddConnection(conn->serverIp, conn->serverPort, conn->shared);
			  if (conn->handle >= 0) {
				  connections[conn->handle]->Lock();
				  bool IsOpen = connections[conn->handle]->Open();
				  connections[conn->handle]->Unlock();
				  if (IsOpen)
					  connections[conn->handle]->AddRef();
				  else
					  conn->handle = -1;
			  }
		  }
		  else if (conn->handle < MAX_SVDRP_CONNECTIONS && connections[conn->handle]) {
			  if (connections[conn->handle]->DelRef() == 0)
				  DELETENULL(connections[conn->handle]);
			  conn->handle = -1;
		  }
		  else {
			  esyslog("svdrpservice: SvdrpConnection: Invalid handle %d", conn->handle);
		  }
		  connsMutex.Unlock();
	  }
	  return true;
  }
  if (strcmp(Id, "SvdrpCommand-v1.0") == 0) {
	  if (Data) {
		  SvdrpCommand_v1_0 *cmd = (SvdrpCommand_v1_0 *) Data;
		  cmd->reply.Clear();
		  cmd->responseCode = 0;
		  if (cmd->handle >= 0 && cmd->handle < MAX_SVDRP_CONNECTIONS && connections[cmd->handle]) {
			  connections[cmd->handle]->Lock();
			  if (connections[cmd->handle]->Send(cmd->command))
				  cmd->responseCode = connections[cmd->handle]->Receive(&cmd->reply);
			  connections[cmd->handle]->Unlock();
		  }
		  else {
			  esyslog("svdrpservice: SvdrpCommand: Invalid handle %d", cmd->handle);
		  }
	  }
	  return true;
  }
  return false;
}

const char **cPluginSvdrpService::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginSvdrpService::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

int cPluginSvdrpService::FindSharedConnection(const char *ServerIp, unsigned int ServerPort) const {
  for (int i = 0; i < MAX_SVDRP_CONNECTIONS; i++) {
	  if (connections[i] && connections[i]->IsShared() &&
			  connections[i]->HasDestination(ServerIp, ServerPort))
		  return i;
  }
  return -1;
}

int cPluginSvdrpService::AddConnection(const char *ServerIp, unsigned int ServerPort, bool Shared) {
  for (int i = 0; i < MAX_SVDRP_CONNECTIONS; i++) {
	  if (connections[i] == NULL) {
		  connections[i] = new cSvdrpConnection(ServerIp, ServerPort, Shared);
		  return i;
	  }
  }
  esyslog("svdrpservice: Too many open connections");
  return -1;
}

VDRPLUGINCREATOR(cPluginSvdrpService); // Don't touch this!
