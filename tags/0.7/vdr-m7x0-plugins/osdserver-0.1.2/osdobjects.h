#ifndef __OSDSERVER_OSDOBJECTS_H
#define __OSDSERVER_OSDOBJECTS_H

#include <map>
#include <set>
#include <queue>

#include <vdr/tools.h>
#include <vdr/thread.h>
#include <vdr/osdbase.h>
#include <vdr/menuitems.h>

#include "tools.h"

#include "osdobjectsbase.h"


class cOsdServerMenu : public cShadowObjectTemplate<cOsdMenu>, public cOpenOsd::cCallback {
    // Shadow of cOsdMenu

    // Shadow variables of cOsdMenu
    char *title;
    char *ColorKeyRed, *ColorKeyGreen, *ColorKeyYellow, *ColorKeyBlue; 
    static const int MaxColumns = 5;
    int Columns[MaxColumns];

    cList<cOsdServerMenuItem> menuitems;
    int current;
    
    cOsdServerMenu *SubMenu;
    cOsdServerMenu *SuperMenu;

    eOSState SendState;

    class cPrivateMenu : public cPrivate {
        char *ColorKeyRed, *ColorKeyGreen, *ColorKeyYellow, *ColorKeyBlue; 
        cPrivateMenu *SubMenu_Dup;
        public:
        cPrivateMenu(cOsdServerMenu *parent);
        virtual ~cPrivateMenu();
        cOsdServerMenu* ParentMenu() { return (cOsdServerMenu*)Parent; }

        virtual eOSState ProcessKey(eKeys Key);
        // Closes menu if it was detached

        void SetColorKeyText(const char *Red, const char *Green, const char *Yellow, const char *Blue);
        void RestoreButtonBar();

        eOSState AddSubMenu(cPrivateMenu *SubMenu) { 
            SubMenu_Dup = SubMenu; 
            return cOsdMenu::AddSubMenu(SubMenu); 
        }
        cPrivateMenu* GetSubMenu() { 
            if (!HasSubMenu()) SubMenu_Dup = NULL; 
            return SubMenu_Dup; 
        }        

        // Make some protected stuff public
        cOsdMenu::SetTitle;
        cOsdMenu::SetCurrent;
        cOsdMenu::SetCols;
        cOsdMenu::Del;
        cOsdMenu::HasSubMenu;
        cOsdMenu::CloseSubMenu;
    };
    friend class cPrivateMenu;
    friend class cPrivate;
    cPrivateMenu* PrivateMenu() { return (cPrivateMenu*)Private; }

    void TriggerItemEvent(cEvent ev);
    
    bool isFocusedMenu;
    
    void SetMenuFocus(bool focus) {
        if (!isFocusedMenu && focus) {
            isFocusedMenu = focus;
            TriggerEvent(cEvent::FocusEvent);
            SetFocusObject();
        }
        if (isFocusedMenu && !focus) {
            isFocusedMenu = focus;
            TriggerEvent(cEvent::BlurEvent);
            UnsetFocusObject();
        } 
    }

public:
    cOsdServerMenu(const char *Title);
    virtual ~cOsdServerMenu();

    virtual void Update();
    virtual void Detach();
    
    void DetachClose();
    // Detach and post close events

    void Add(cOsdServerMenuItem *Item, bool Current = false, cOsdServerMenuItem *After = NULL);
    void Ins(cOsdServerMenuItem *Item, bool Current = false, cOsdServerMenuItem *Before = NULL);
    void Del(cOsdServerMenuItem *Item, bool Delete = true);
    
    virtual cOsdObject* OpenOsd();
    // Callback for opening the OSD from main thread

    void Show();
    // Calls cOsdMenu::Show() or creates the cOsdMenu object and displays it. Does the necessary thread syncing.
    // For a sub menu, redirects to the parent menu

    void GetCurrent(int &cur, cString &curs);
    void SetCurrent(int cur);
    
    void SetColorKeyRed(const char *Red);
    void SetColorKeyGreen(const char *Green);
    void SetColorKeyYellow(const char *Yellow);
    void SetColorKeyBlue(const char *Blue);
    void RestoreButtonBar() { if (!IsDetached()) PrivateMenu()->RestoreButtonBar(); }

    void AddSubMenu(cOsdServerMenu *Menu);
    
    void SetColumn(int number, int value);

    void SendOSState(eOSState State);

    virtual bool PollEvent(cTriggeredEvent &Event);

    static const enumClassId classid=clsidMenu;
    virtual enumClassId ClassId() { return classid; }
};


// Specialize construction of cOsdMenu, because the default cPrivate constructor would expect
// a cOsdMenu() default constructor. (see explicit template specialization)
template<> inline cShadowTemplate<cOsdMenu>::cPrivate::cPrivate(cShadowTemplate<cOsdMenu> *parent)
    : cOsdMenu(((cOsdServerMenu*)parent)->title), 
      cShadowBase::cPrivateBase(parent) { 
    // nothing
}




class cOsdServerOsdItem : public cShadowMenuItemTemplate<cOsdItem> {
    // Shadow of cOsdItem

    // Shadow variables of cOsdItem
    char *text;
    bool selectable;
    // These are protected by LockShared mutex,
    // unless both threads are synchronized anyway.

    class cPrivateOsdItem : public cPrivate {
        public:
        cPrivateOsdItem(cOsdServerOsdItem *parent);
        virtual ~cPrivateOsdItem() { if (Parent) Parent->Detach(); }
        virtual eOSState ProcessKey(eKeys Key);
        cOsdServerOsdItem* ParentOsdItem() { return (cOsdServerOsdItem*)Parent; }
    };
    friend class cPrivateOsdItem;
    cPrivateOsdItem* PrivateOsdItem() { return (cPrivateOsdItem*)Private; }

public:
    cOsdServerOsdItem(const char *Text, bool Selectable = true);
    virtual ~cOsdServerOsdItem();

    virtual void Update();

    void SetSelectable(bool Selectable);
    void SetText(const char *text);

    static const enumClassId classid=clsidOsdItem;
    virtual enumClassId ClassId() { return classid; }
};

class cOsdServerMenuEditStrItem : public cShadowMenuItemTemplate<cMenuEditStrItem> {
    // Shadow of cMenuEditStrItem

    // Shadow variables of cMenuEditStrItem
    char *name;
    char *value;
    int length;

    // These are protected by LockShared mutex,
    // unless both threads are synchronized anyway.

    class cPrivateMenuEditStrItem : public cPrivate {
        char *value;
        bool InEdit;
        public:
        cPrivateMenuEditStrItem(cOsdServerMenuEditStrItem *parent);
        virtual ~cPrivateMenuEditStrItem();
        virtual eOSState ProcessKey(eKeys Key);
        cOsdServerMenuEditStrItem* ParentMenuEditStrItem() { return (cOsdServerMenuEditStrItem*)Parent; }
    };
    friend class cPrivateMenuEditStrItem;
    cPrivateMenuEditStrItem* PrivateMenuEditStrItem() { return (cPrivateMenuEditStrItem*)Private; }

    void RestoreButtonBar() { if (GetMenu()) GetMenu()->RestoreButtonBar(); }
public:
    cOsdServerMenuEditStrItem(const char *Name, const char *Value, int length);
    virtual ~cOsdServerMenuEditStrItem();

    virtual void Update();

    char* GetValue();
    // allocated copy, free()!

    static const enumClassId classid=clsidMenuEditStrItem;
    virtual enumClassId ClassId() { return classid; }
    
    friend class cPrivate;
};

// Specialize construction of cMenuEditStrItem, because the default cPrivate constructor would expect
// a cMenuEditStrItem() default constructor. (see explicit template specialization)
template<> inline cShadowTemplate<cMenuEditStrItem>::cPrivate::cPrivate(cShadowTemplate<cMenuEditStrItem> *parent)
    : cMenuEditStrItem(
        ((cOsdServerMenuEditStrItem*)parent)->name, 
        ((cOsdServerMenuEditStrItem*)parent)->value, 
        ((cOsdServerMenuEditStrItem*)parent)->length, 
#if VDRVERSNUM < 10511
        FileNameChars
#else
        NULL
#endif
    ), 
      cShadowBase::cPrivateBase(parent) { 
    // nothing
}
    
class cOsdServerMenuEditIntItem : public cShadowMenuItemTemplate<cMenuEditIntItem> {
    // Shadow of cMenuEditIntItem

    // Shadow variables of cMenuEditIntItem
    char *name;
    int value;
    int min,max;
    char *minString, *maxString;

    // These are protected by LockShared mutex,
    // unless both threads are synchronized anyway.

    class cPrivateMenuEditIntItem : public cPrivate {
        int value;
        public:
        cPrivateMenuEditIntItem(cOsdServerMenuEditIntItem *parent);
        virtual ~cPrivateMenuEditIntItem() { if (Parent) Parent->Detach(); }
        virtual eOSState ProcessKey(eKeys Key);
        cOsdServerMenuEditIntItem* ParentMenuEditIntItem() { return (cOsdServerMenuEditIntItem*)Parent; }
    };
    friend class cPrivateMenuEditIntItem;
    cPrivateMenuEditIntItem* PrivateMenuEditIntItem() { return (cPrivateMenuEditIntItem*)Private; }

public:
    cOsdServerMenuEditIntItem(const char *Name, int Value, int Min = 0, int Max = INT_MAX, const char *MinString = NULL, const char *MaxString = NULL);
    virtual ~cOsdServerMenuEditIntItem();

    virtual void Update();

    int GetValue();

    static const enumClassId classid=clsidMenuEditIntItem;
    virtual enumClassId ClassId() { return classid; }
    
    friend class cPrivate;
};

// Specialize construction of cMenuEditIntItem, because the default cPrivate constructor would expect
// a cMenuEditIntItem() default constructor. (see explicit template specialization)
template<> inline cShadowTemplate<cMenuEditIntItem>::cPrivate::cPrivate(cShadowTemplate<cMenuEditIntItem> *parent)
    : cMenuEditIntItem(
        ((cOsdServerMenuEditIntItem*)parent)->name,
        &((cOsdServerMenuEditIntItem*)parent)->value, // temporary, will be replaced by higher constructor
        ((cOsdServerMenuEditIntItem*)parent)->min,
        ((cOsdServerMenuEditIntItem*)parent)->max,
        ((cOsdServerMenuEditIntItem*)parent)->minString, // temporary, will be replaced by higher constructor
        ((cOsdServerMenuEditIntItem*)parent)->maxString // temporary, will be replaced by higher constructor
    ), 
    cShadowBase::cPrivateBase(parent)
{ 
    // nothing
}


class cOsdServerMenuEditListItem : public cShadowMenuItemTemplate<cMenuEditStraItem> {
    // Shadow of cMenuEditListItem

    // Shadow variables of cMenuEditListItem
    char *name;
    int value;
    int max;
    char **strings;
    char **tmp_strings; // temporary for object construction only

    // These are protected by LockShared mutex,
    // unless both threads are synchronized anyway.

    class cPrivateMenuEditListItem : public cPrivate {
        int value;
        char **strings;
        public:
        cPrivateMenuEditListItem(cOsdServerMenuEditListItem *parent);
        virtual ~cPrivateMenuEditListItem();
        virtual eOSState ProcessKey(eKeys Key);
        cOsdServerMenuEditListItem* ParentMenuEditListItem() { return (cOsdServerMenuEditListItem*)Parent; }
    };
    friend class cPrivateMenuEditListItem;
    cPrivateMenuEditListItem* PrivateMenuEditListItem() { return (cPrivateMenuEditListItem*)Private; }


public:
    cOsdServerMenuEditListItem(const char *Name, int Value, int Max, const char * const *Strings);
    virtual ~cOsdServerMenuEditListItem();

    virtual void Update();

    int GetValue();
    char* GetValueName();
    // allocated copy, free()!

    static const enumClassId classid=clsidMenuEditListItem;
    virtual enumClassId ClassId() { return classid; }
    
    friend class cPrivate;
};

// Specialize construction of cMenuEditStraItem, because the default cPrivate constructor would expect
// a cMenuEditStraItem() default constructor. (see explicit template specialization)
template<> inline cShadowTemplate<cMenuEditStraItem>::cPrivate::cPrivate(cShadowTemplate<cMenuEditStraItem> *parent)
    : cMenuEditStraItem(
        ((cOsdServerMenuEditListItem*)parent)->name, 
        &((cOsdServerMenuEditListItem*)parent)->value, // temporary, will be replaced by higher constructor
        ((cOsdServerMenuEditListItem*)parent)->max, 
        ((cOsdServerMenuEditListItem*)parent)->tmp_strings
    ), 
    cShadowBase::cPrivateBase(parent)
{ 
    // nothing
}
    


#endif
