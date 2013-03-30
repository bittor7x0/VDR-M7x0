#ifndef __ZAPPILOTOSD_H
#define __ZAPPILOTOSD_H

#include <vdr/osd.h>
#include <vdr/menuitems.h>
#include <vdr/menu.h>
#include <vdr/skins.h>
#include <ctype.h>

#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <time.h>
#include "config.h"

class cZappilotOsd : public cOsdObject
{
   private:

      cOsd *osd;
      cSkinDisplayChannel *displayChannel;
      class cMenuEvent *displayEvent;
      class cMenuEditTimer *displayTimer;
      int lastTime;
      int number;
      int group;
      char ChanName[255];
      char ChanNumber[255];
      int extraInfo;
      const cEvent *Present;
      const cEvent *Following;
      const cEvent **pArray;
      const char *textPresentInfo;
      const char *textFollowingInfo;
      const char *titlePresentInfo;
      const char *titleFollowingInfo;
      const char *subtitlePresentInfo;
      const char *subtitleFollowingInfo;
      int currentEvent;
      int offset;
      int lines;
      int type;
      cChannel* currentChannel;

   public:
      cZappilotOsd(void);
      ~cZappilotOsd();
      virtual void Show(void);
      virtual eOSState ProcessKey(eKeys Key);

      void CursorUp();
      void CursorDown();
      void CursorOK();

      void AddDelSwitchTimer(const cEvent *event);
      void UpdateEPGInfo(int NowPrevNext);
      void DrawMenu(int delta, int highlight);
      void DisplayBitmap(void);
      void DisplayInfo(int delta);
      void DisplayExtraInfo(void);
      void DisplayTimer(void);
      void DisplayChannel(const cChannel *Channel);
};
#endif                           //__ZAPPILOTOSD_H
