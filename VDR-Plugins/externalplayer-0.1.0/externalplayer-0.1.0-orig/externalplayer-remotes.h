/*
 * externalplayer-remotes.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _EXTERNALPLAYER_REMOTES_H_
#define _EXTERNALPLAYER_REMOTES_H_

#include <vdr/thread.h>
#include <vdr/remote.h>

using namespace std;

class cRemotesDisable {
private:
  bool deactivated;
public:
  void deactivateRemotes();
  void reactivateRemotes();
};

class cRemotesDisableHelper : public cRemote, public cThread {
public:
  cRemotesDisableHelper(const char * name);
  void deactivate();
  void reactivate();
};

#endif /*_EXTERNALPLAYER_REMOTES_H_*/
