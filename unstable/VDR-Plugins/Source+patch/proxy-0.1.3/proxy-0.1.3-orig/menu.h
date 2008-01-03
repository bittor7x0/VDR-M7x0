/*
 * menu.h: menu handling
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menu.h 632 2006-03-04 18:35:22Z udo $
 */
 
#ifndef __PROXY_MENU_H
#define __PROXY_MENU_H

#include "proxy.h"

class cMenuGroup : public cOsdMenu {
    const char *GroupName;
    cPluginProxy *Caller;
    
public:
    cMenuGroup(const char *GroupName, cPluginProxy *Caller);
    virtual ~cMenuGroup();
    virtual eOSState ProcessKey(eKeys Key);
};

class cProxySetupPage : public cMenuSetupPage {
public:
    cProxySetupPage();
    virtual void Store(void);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif
