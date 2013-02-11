/*
 * VdrmonThread
 */
#include <string.h>
#include <vdr/plugin.h>
#include <vdr/thread.h>
#include "vdrmanagerthread.h"
#include "select.h"
#include "helpers.h"

cAndroVdrThread::cAndroVdrThread(int port, const char * password, bool forceCheckSvdrp)
{
  select = NULL;
  this->port = port;
  this->password = password;
  this->forceCheckSvdrp = forceCheckSvdrp;
}

cAndroVdrThread::~cAndroVdrThread()
{
  Cleanup();
}

void cAndroVdrThread::Action(void)
{
  // create listener socket
  if (!Init())
    return;

  // do processing
  select->Action();

  // cleanup
  Cleanup();
}

bool cAndroVdrThread::Init()
{
  // create select
  select = new cSelect();
  if (select == NULL)
    return false;

  // create server socket
  cVdrmanagerServerSocket * sock = new cVdrmanagerServerSocket();
  if (sock == NULL || !sock->Create(port, password, forceCheckSvdrp))
    return false;

  // register server socket
  select->SetServerSocket(sock);

  return true;
}

void cAndroVdrThread::Cleanup()
{
  if (select)
    delete select;
}

void cAndroVdrThread::Shutdown()
{
}
