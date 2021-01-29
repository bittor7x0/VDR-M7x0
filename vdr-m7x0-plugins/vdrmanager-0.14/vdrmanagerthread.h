/*
 * VdrmonThread
 */

#ifndef _VDRMON_THREAD
#define _VDRMON_THREAD

#include <sys/poll.h>
#include <vdr/plugin.h>
#include <vdr/thread.h>
#include <vdr/device.h>
#include <vdr/player.h>
#include <string>

using namespace std;

class cSelect;

class cVdrManagerThread : public cThread {
private:
  cSelect * select;
  int port;
  const char * password;
  bool forceCheckSvdrp;
  int compressionMode;
#if VDRMANAGER_USE_SSL
  int sslPort;
  const char * certFile;
  const char * keyFile;
#endif
public:
  cVdrManagerThread(int port,
#if VDRMANAGER_USE_SSL
      int sslPort,
#endif
      const char * password, bool forceCheckSvdrp, int compressionMode
#if VDRMANAGER_USE_SSL
      , const char * certFile, const char * keyFile
#endif
      );
  virtual void Action(void);
  void Shutdown();
private:
  virtual ~cVdrManagerThread();
  void Cleanup();
  bool Init();
};

#endif


