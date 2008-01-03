/*
 * menu.c: menu handling
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menu.c 632 2006-03-04 18:35:22Z udo $
 */

#include "menu.h"

class cMenuGroupPluginItem : public cOsdItem {
private:
    cPluginProxy *plugin;
public:
    cMenuGroupPluginItem(const char *Name, cPluginProxy *Plugin);
    cPluginProxy* Plugin(void) { return plugin; }
};

cMenuGroupPluginItem::cMenuGroupPluginItem(const char *Name, cPluginProxy *Plugin)
    :cOsdItem(Name) {
    plugin = Plugin;
}

cMenuGroup::cMenuGroup(const char *aGroupName, cPluginProxy *aCaller) 
    : cOsdMenu("") {
    SetHasHotkeys();
    GroupName=strdup(aGroupName);
    Caller=aCaller;
    SetTitle(GroupName);
    

    for (cPluginProxy *p=cPluginProxy::Proxies.First(); p; p=cPluginProxy::Proxies.Next(p)) {
        if (!p->GroupName) continue;
        if (p->GroupLeader) continue;
        if (0!=strcmp(GroupName,p->GroupName)) continue;
        
        const char *name=p->ProxyMainMenuEntry();
        
        if (name) Add(new cMenuGroupPluginItem(hk(name),p));
    }
};

eOSState cMenuGroup::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state != osUnknown) return state;

    switch (Key) {
    case kOk: {
        cMenuGroupPluginItem *item = (cMenuGroupPluginItem *)Get(Current());
        if (item) {
            cPluginProxy *p = item->Plugin();
            if (p) {
                cOsdObject *menu = p->MainMenuAction();
                
                char *text=(char*)item->Text();
                if (text && (text = skipspace(text)) != NULL && text[0]>'0' && text[0]<'9') {
                    asprintf(&text," %c %s",text[0],p->ProxyMainMenuEntry());               
                } else {
                    text=strdup(p->ProxyMainMenuEntry());
                }
                item->SetText(text,false);
                
                if (menu) {
                    if (menu->IsMenu()) {
                        return AddSubMenu((cOsdMenu *)menu);
                    } else {
                        Caller->PassThroughAction=menu;
                        return osPlugin;
                    }
                } else {
                    DisplayCurrent(true);
                }
            }
        }
        return osEnd;
    }
    default:
        break;
    }
    return osUnknown;
}

cMenuGroup::~cMenuGroup() {
    delete GroupName;   
}

class cProxySetupMenuItem : public cOsdItem {
private:
    cPluginProxy *plugin;
    void SetPluginText();
public:
    cProxySetupMenuItem(cPluginProxy *Plugin);
    cPluginProxy* Plugin(void) { return plugin; }
    eOSState ProcessKey(eKeys Key);
};

cProxySetupMenuItem::cProxySetupMenuItem(cPluginProxy *Plugin)
    :cOsdItem() {
    plugin = Plugin;
    SetPluginText();
}

void cProxySetupMenuItem::SetPluginText() {
    char *text;
    if (plugin->plugin) {
        asprintf(&text,"%s: %s",tr("Running"),plugin->Name());
    } else {
        asprintf(&text,"%s: %s",tr("Suspended"),plugin->Name());
    }
    SetText(text,false);
}

eOSState cProxySetupMenuItem::ProcessKey(eKeys Key) {
    eOSState state = cOsdItem::ProcessKey(Key);

    if (state != osUnknown) return state;
    
    switch (Key) {
    case kRed:
        plugin->ResumePlugin(); 
        SetPluginText();
        return osUser1;
    case kYellow: 
        plugin->SuspendPlugin();    
        SetPluginText();
        return osUser1;
    default: break;
    }
    
    return osUnknown;   
}


cProxySetupPage::cProxySetupPage() {
    for (cPluginProxy *p=cPluginProxy::Proxies.First(); p; p=cPluginProxy::Proxies.Next(p)) {
        if (p->GroupLeader) continue;
        if (p->ProxyLeader) continue;
        
        Add(new cProxySetupMenuItem(p));
    }
    SetHelp(tr("Resume"),NULL,tr("Suspend"),NULL); 
}

eOSState cProxySetupPage::ProcessKey(eKeys Key) {
    eOSState state = cMenuSetupPage::ProcessKey(Key);

    if (state == osUser1) {
        DisplayCurrent(true);
        return osUnknown;
    }

    return state;
}

void cProxySetupPage::Store(void) {}
