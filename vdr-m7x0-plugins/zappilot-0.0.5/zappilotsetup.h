#ifndef __ZAPPILOTSETUP_H
#define __ZAPPILOTSETUP_H

#include <vdr/plugin.h>

class cZappilotSetup : public cMenuSetupPage
{
   private:
      int CloseOnSwitch;
      int Hidemenu;
      int FastBrowse;
      int FastBrowseAlt;
      int SwitchTimer;
      int SwitchMinsBefore;
   protected:
      virtual void Store(void);
   public:
      cZappilotSetup(void);
};
#endif                           //__ZAPPILOTSETUP_H
