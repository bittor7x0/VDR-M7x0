/*
 * $Id: setup.h,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#ifndef VDR_TASTE_SETUP_H
#define VDR_TASTE_SETUP_H

#include <vdr/menuitems.h>

#include "event.h"
#include "config.h"

class cMenuSetupTaste : public cMenuSetupPage {
private:
  cEventsTaste mEventsData;
  cSetupTaste  mSetupData;

protected:
  virtual void Store(void);
  virtual eOSState ProcessKey(eKeys Key);

  void Set(void);
  void SetHelpKeys(void);

  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);

public:
  cMenuSetupTaste();
};

class cMenuSetupEditTaste : public cOsdMenu {
private:
  cEventTaste *mEvent;
  cEventTaste  mData;

protected:
  eOSState ProcessKey(eKeys Key);

public:
  cMenuSetupEditTaste(cEventTaste *Event);
};

#endif // VDR_TASTE_SETUP_H
