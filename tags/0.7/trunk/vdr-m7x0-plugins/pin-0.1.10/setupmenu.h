
#ifndef __PINSETUTMENU_H_
#define __PINSETUTMENU_H_

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/plugin.h>
#include "pin.h"

//***************************************************************************
// Pin Setup Menu
//***************************************************************************

class PinSetupMenu : public cMenuSetupPage
{

   public:
      
      PinSetupMenu();
      ~PinSetupMenu();
      
   protected:
      
      void Store(void);

      // data

      char pinCode[cPinPlugin::sizePinCode+TB];
      int skipChannelSilent;
      int pinResetTime;
      int autoMenuOpen;
      int autoProtectionMode;
      int hidePinCode;
      int hideProtectedMenus;
      int hideProtectedPlugins;
      int hideProtectedRecordings;
};

//***************************************************************************
#endif // __PINSETUTMENU_H_
