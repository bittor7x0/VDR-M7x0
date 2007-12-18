/*
 * Radiolist plugin for VDR
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef __SETUP_RADIOLIST_H
#define __SETUP_RADIOLIST_H

#include <vdr/menuitems.h>

class cRadiolistSetup {
 public:
    int HideMainMenu;
    int DisplaySeperators;
    
 public:
    cRadiolistSetup(void);

    bool SetupParse(const char *Name, const char *Value);
};

class cMenuSetupRadiolist : public cMenuSetupPage {
 private:
    cRadiolistSetup data;
 protected:
    virtual void Store(void);
 public:
    cMenuSetupRadiolist(void);
};

extern cRadiolistSetup RadiolistSetup;

#endif // __SETUP_RADIOLIST_H
