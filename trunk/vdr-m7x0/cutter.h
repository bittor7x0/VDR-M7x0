/*
 * cutter.h: The video cutting facilities
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __CUTTER_H
#define __CUTTER_H

class cCuttingThread;
class cMutex;

class cCutter {
private:
  static char *editedVersionName;
  static cCuttingThread *cuttingThread;
  static bool error;
  static bool ended;
  static cMutex *cutterLock;
public:
  static bool Start(const char *FileName);
  static void Stop(void);
  static bool Active(void);
  static bool Error(void);
  static bool Ended(void);
  };

bool CutRecording(const char *FileName);

#endif //__CUTTER_H
