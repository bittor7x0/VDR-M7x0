/*
 * externalplayer-remotes.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <string.h>

#include "externalplayer-remotes.h"

void cRemotesDisable::deactivateRemotes() {
  if (!deactivated) {
    isyslog("externalplayer-plugin: deactivating remotes");
    for (cRemote * i = Remotes.First(); i != NULL; i = Remotes.Next(i)) {
      if (strcmp(i->Name(), "LIRC") == 0) {
        ((cRemotesDisableHelper *) i)->deactivate();
      }
      else if (strcmp(i->Name(), "RCU") == 0) {
        ((cRemotesDisableHelper *) i)->deactivate();
      }
    }
  }

  deactivated = true;
}

void cRemotesDisable::reactivateRemotes() {
  if (deactivated) {
    isyslog("externalplayer-plugin: reactivating remotes");
    for (cRemote * i = Remotes.First(); i != NULL; i = Remotes.Next(i)) {
      if (strcmp(i->Name(), "LIRC") == 0) {
        ((cRemotesDisableHelper *) i)->reactivate();
      }
      else if (strcmp(i->Name(), "RCU") == 0) {
        ((cRemotesDisableHelper *) i)->reactivate();
      }
    }
  }

  deactivated = false;
}

// --- cRemotesDisableHelper -------------------------------------------------

cRemotesDisableHelper::cRemotesDisableHelper(const char * name) : cRemote(name) {
}

void cRemotesDisableHelper::deactivate() {
  if (Active()) {
    Cancel();
  }
}

void cRemotesDisableHelper::reactivate() {
  if (!Active()) {
    Start();
  }
}
