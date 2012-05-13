

#include <stdlib.h>
#include "osdobjects.h"
#include "osdserver.h"


// ---------------------
//   cOsdServerOsdMenu
// ---------------------

cOsdServerMenu::cOsdServerMenu(const char *Title) {
    title=strdup(Title);
    ColorKeyRed = NULL;
    ColorKeyGreen = NULL;
    ColorKeyYellow = NULL;
    ColorKeyBlue = NULL; 
    for (int i=0; i<MaxColumns; i++) Columns[i] = 0;
    current = 0;
    SendState = osUnknown;
    isFocusedMenu = false;
    SubMenu = NULL;
    SuperMenu = NULL;
}

cOsdServerMenu::~cOsdServerMenu() {
    Detach();
    if (SuperMenu) SuperMenu->SubMenu = NULL;
    if (SubMenu) SubMenu->SuperMenu = NULL;
    for (cOsdServerMenuItem *item = menuitems.First(); item; item=menuitems.Next(item)) {
        item->menu = NULL;
    }    
    if (title) free(title);
    if (ColorKeyRed) free(ColorKeyRed);
    if (ColorKeyGreen) free(ColorKeyGreen);
    if (ColorKeyYellow) free(ColorKeyYellow);
    if (ColorKeyBlue) free(ColorKeyBlue);
}

cOsdServerMenu::cPrivateMenu::cPrivateMenu(cOsdServerMenu *parent)
    : cPrivate(parent) {
    SetCols(parent->Columns[0], parent->Columns[1], parent->Columns[2], parent->Columns[3], parent->Columns[4]);
    
    ColorKeyRed = NULL;
    ColorKeyGreen = NULL;
    ColorKeyYellow = NULL;
    ColorKeyBlue = NULL;    
    SubMenu_Dup = NULL;
    SetColorKeyText(parent->ColorKeyRed, parent->ColorKeyGreen, parent->ColorKeyYellow, parent->ColorKeyBlue);
    if (parent->SubMenu) {
        parent->SubMenu->Update();
        AddSubMenu(parent->SubMenu->PrivateMenu());
    }
}

cOsdServerMenu::cPrivateMenu::~cPrivateMenu() {
    cMutexLock Lock(&LockShared);

    if (ParentMenu()) ParentMenu()->DetachClose();  
    if (ColorKeyRed) free(ColorKeyRed);
    if (ColorKeyGreen) free(ColorKeyGreen);
    if (ColorKeyYellow) free(ColorKeyYellow);
    if (ColorKeyBlue) free(ColorKeyBlue);
}


eOSState cOsdServerMenu::cPrivateMenu::ProcessKey(eKeys Key) {
    if (!Parent) 
        return osEnd;
    // We're detached, close this menu

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (HasSubMenu())
        return state;

    cMutexLock Lock(&LockShared);

    if (!ParentMenu() || ParentMenu()->Dirty)
        return state;
        
    if (ParentMenu()->SubMenu) {
        // We're on !HasSubMenu() - the submenu must have been closed
        ParentMenu()->SetMenuFocus(true);
        ParentMenu()->SubMenu->SuperMenu = NULL;
        ParentMenu()->SubMenu = NULL;
    }
    
    if (ParentMenu()->current != Current()) {
        ParentMenu()->TriggerItemEvent(cEvent::BlurEvent);
        ParentMenu()->current = Current();
        ParentMenu()->TriggerItemEvent(cEvent::FocusEvent);
    }

    if (ParentMenu()->SendState != osUnknown) {
        state=ParentMenu()->SendState;
        ParentMenu()->SendState=osUnknown;
        return state;
    }

    
    if (state==osUnknown) {
        ParentMenu()->TriggerEvent(Key);
    }

    return state;
}

void cOsdServerMenu::cPrivateMenu::SetColorKeyText(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    if (ColorKeyRed) free(ColorKeyRed);
    if (ColorKeyGreen) free(ColorKeyGreen);
    if (ColorKeyYellow) free(ColorKeyYellow);
    if (ColorKeyBlue) free(ColorKeyBlue);

    ColorKeyRed = Red ? strdup(Red) : NULL;
    ColorKeyGreen = Green ? strdup(Green) : NULL;
    ColorKeyYellow = Yellow ? strdup(Yellow) : NULL;
    ColorKeyBlue = Blue ? strdup(Blue) : NULL;    
    RestoreButtonBar();
}

void cOsdServerMenu::cPrivateMenu::RestoreButtonBar() {
    SetHelp(ColorKeyRed, ColorKeyGreen, ColorKeyYellow, ColorKeyBlue);    
}

void cOsdServerMenu::Update() {
    if (!Private) {
        Private=new cPrivateMenu(this);
    } else if (Dirty) {
        PrivateMenu()->SetTitle(title);
        PrivateMenu()->SetColorKeyText(ColorKeyRed, ColorKeyGreen, ColorKeyYellow, ColorKeyBlue);
        PrivateMenu()->SetCols(Columns[0], Columns[1], Columns[2], Columns[3], Columns[4]);
        if (SubMenu) {
            SubMenu->Update();
            if (SubMenu->PrivateMenu() != PrivateMenu()->GetSubMenu())
                PrivateMenu()->AddSubMenu(SubMenu->PrivateMenu());
        } else {
            if (PrivateMenu()->HasSubMenu())
                PrivateMenu()->CloseSubMenu();
        }
    } else {
        if (SubMenu) 
            SubMenu->Update();
    }

    // Now call Update for all menu items:
    cOsdItem *last=NULL;
    for (cOsdServerMenuItem *item = menuitems.First(); item; item=menuitems.Next(item)) {
        item->ItemUpdate();

        // Check ordering of VDR menu items
        cOsdItem *osditem=item->GetItem();
        if (osditem->Prev() == NULL && osditem != PrivateMenu()->First()) {
            // Not yet listed, probably new
            PrivateMenu()->Add(osditem,false,last);
        } else if (osditem->Prev() != last) {
            // Not listed correctly, move it
            cOsdMenu *menu=PrivateMenu();
            menu->cList<cOsdItem>::Del(osditem,false);
            PrivateMenu()->Add(osditem,false,last);
        }
        last=osditem;
    }

    if (last) last=PrivateMenu()->Next(last);
    while (last) {
        // All remaining objects now are stray
        cOsdItem *next = PrivateMenu()->Next(last);
        PrivateMenu()->Del(last->Index());
        last = next;
    }

    if (Dirty) {
        cOsdServerMenuItem *cur=menuitems.Get(current);
        if (cur) PrivateMenu()->SetCurrent(cur->GetItem());
        current=PrivateMenu()->Current(); // should be the same, just to make sure
    }
    
    SetMenuFocus(SubMenu == NULL);

    Dirty=false;
}

void cOsdServerMenu::Detach() {
    // Call Detach for all menu items too
    cMutexLock Lock(&LockShared);
    if (SubMenu) SubMenu->Detach();
    for (cOsdServerMenuItem *item = menuitems.First(); item; item=menuitems.Next(item)) {
        item->ItemDetach();
    }
    cShadowTemplate<cOsdMenu>::Detach();
    SetMenuFocus(false);
}

void cOsdServerMenu::DetachClose() {
    cMutexLock Lock(&LockShared);

    if (SubMenu)
        SubMenu->DetachClose();
    
    if (!Dirty) {
        SetMenuFocus(false);
        TriggerEvent(cEvent::CloseEvent);
    }
    Detach();
}

void cOsdServerMenu::Add(cOsdServerMenuItem *Item, bool Current, cOsdServerMenuItem *After) {
    cMutexLock Lock(&LockShared);
    Item->menu=this;
    menuitems.Add(Item,After);
    if (Current) current=Item->Index();
    Dirty=true;
}

void cOsdServerMenu::Ins(cOsdServerMenuItem *Item, bool Current, cOsdServerMenuItem *Before) {
    cMutexLock Lock(&LockShared);
    Item->menu=this;
    menuitems.Ins(Item,Before);
    if (Current) current=Item->Index();
    Dirty=true;
}

void cOsdServerMenu::Del(cOsdServerMenuItem *Item, bool Delete) {
    cMutexLock Lock(&LockShared);
    menuitems.Del(Item,Delete);
    Dirty=true;
}

cOsdObject* cOsdServerMenu::OpenOsd() {
    Update();
    return PrivateMenu();
}

void cOsdServerMenu::Show() {
    if (SuperMenu) {
        // Redirect to parent
        SuperMenu->Show();
        return;
    }

    cMainThreadLock Lock;
    Lock.LockMainThread();

    if (PrivateMenu()) {
        // There's already a VDR side object, just call show
        Update();
        PrivateMenu()->Show();

        Lock.UnlockMainThread();
        return;
    }

    Lock.UnlockMainThread();

    // Need to open the menu first. Call callback to do it.
    cPluginOsdServer::Plugin()->CallMainMenuAction(this,5000);
}


void cOsdServerMenu::GetCurrent(int &cur, cString &curs) {
    cur = current;
    cOsdServerMenuItem *o = menuitems.Get(cur);
    curs = o ? o->Name() : cString();
}

void cOsdServerMenu::SetCurrent(int cur) {
    cMutexLock Lock(&LockShared);
    current=cur;
    Dirty=true;
}

void cOsdServerMenu::TriggerItemEvent(cEvent ev) {
    cOsdServerMenuItem *item = menuitems.Get(current);
    if (item) TriggerEvent(item, ev);   
}

void cOsdServerMenu::SetColorKeyRed(const char *Red) {
    cMutexLock Lock(&LockShared);
    if (ColorKeyRed) free(ColorKeyRed);
    ColorKeyRed = Red ? strdup(Red) : NULL;
    Dirty=true;
}
void cOsdServerMenu::SetColorKeyGreen(const char *Green) {
    cMutexLock Lock(&LockShared);
    if (ColorKeyGreen) free(ColorKeyGreen);
    ColorKeyGreen = Green ? strdup(Green) : NULL;
    Dirty=true;
}
void cOsdServerMenu::SetColorKeyYellow(const char *Yellow) {
    cMutexLock Lock(&LockShared);
    if (ColorKeyYellow) free(ColorKeyYellow);
    ColorKeyYellow = Yellow ? strdup(Yellow) : NULL;
    Dirty=true;
}
void cOsdServerMenu::SetColorKeyBlue(const char *Blue) {
    cMutexLock Lock(&LockShared);
    if (ColorKeyBlue) free(ColorKeyBlue);
    ColorKeyBlue = Blue ? strdup(Blue) : NULL;
    Dirty=true;
}

void cOsdServerMenu::AddSubMenu(cOsdServerMenu *Menu) {
    cMutexLock Lock(&LockShared);
    SubMenu = Menu;
    SubMenu->SuperMenu = this;
    SetMenuFocus(false);
    Dirty = true;
}

void cOsdServerMenu::SetColumn(int number, int value) {
    cMutexLock Lock(&LockShared);
    if (number>=0 && number<MaxColumns) Columns[number]=value;
    Dirty=true;
}


void cOsdServerMenu::SendOSState(eOSState State) {
    cMutexLock Lock(&LockShared);

    SendState=State;
    cRemote::Put(eKeys(kNone | k_Release));
}

bool cOsdServerMenu::PollEvent(cTriggeredEvent &Event) {
    cMutexLock Lock(&LockShared);
    for (cOsdServerMenuItem *item = menuitems.First(); item; item=menuitems.Next(item)) {
        if (item->PollEvent(Event)) return true;
    }
    
    if (!eventQueue.empty()) {
        Event = eventQueue.front();
        eventQueue.pop();
        return true;
    }
    
    return false;
}

// ---------------------
//   cOsdServerOsdItem
// ---------------------


cOsdServerOsdItem::cOsdServerOsdItem(const char *Text, bool Selectable) {
    text=strdup(Text);
    selectable=Selectable;
}

cOsdServerOsdItem::~cOsdServerOsdItem() {
    cMutexLock Lock(&LockShared);
    Detach();
    if (GetMenu()) GetMenu()->Del(this,false);
    if (text) free(text);
}

cOsdServerOsdItem::cPrivateOsdItem::cPrivateOsdItem(cOsdServerOsdItem *parent)
    : cPrivate(parent) {
    cOsdItem::SetText(parent->text);
    cOsdItem::SetSelectable(parent->selectable);
}

eOSState cOsdServerOsdItem::cPrivateOsdItem::ProcessKey(eKeys Key) {
    eOSState state = cOsdItem::ProcessKey(Key);

    cMutexLock Lock(&LockShared);
    if (state==osUnknown && ParentOsdItem() && !ParentOsdItem()->Dirty && ParentOsdItem()->GetMenu()) {
        ParentOsdItem()->GetMenu()->TriggerEvent(ParentOsdItem(),Key);
    }

    return state;
}

void cOsdServerOsdItem::Update() {
    if (!Private) {
        Private=new cPrivateOsdItem(this);
    } else if (Dirty) {
        PrivateOsdItem()->SetText(text);
        PrivateOsdItem()->SetSelectable(selectable);
    }
    Dirty=false;
}

void cOsdServerOsdItem::SetSelectable(bool Selectable) { 
    cMutexLock Lock(&LockShared);
    selectable = Selectable; 
    Dirty = true; 
}

void cOsdServerOsdItem::SetText(const char *NewText) { 
    cMutexLock Lock(&LockShared);
    
    free(text);
    text = strdup(NewText);
    
    Dirty = true; 
}

// ---------------------------
//   cOsdServerMenuEditStrItem
// ---------------------------

cOsdServerMenuEditStrItem::cOsdServerMenuEditStrItem(const char *Name, const char *Value, int Length) {
    name = strdup(Name);
    value = (char*)malloc(Length+1);
    length = Length;
    strn0cpy(value, Value, length);
}

cOsdServerMenuEditStrItem::~cOsdServerMenuEditStrItem() {
    cMutexLock Lock(&LockShared);
    Detach();
    if (GetMenu()) GetMenu()->Del(this,false);
    if (name) free(name);
    if (value) free(value);    
}

cOsdServerMenuEditStrItem::cPrivateMenuEditStrItem::cPrivateMenuEditStrItem(cOsdServerMenuEditStrItem *parent)
    : cPrivate(parent) {

    // ParentMenuEditStrItem()->value now belongs to this object, see explicit template specialization

    value = ParentMenuEditStrItem()->value;
    ParentMenuEditStrItem()->value = (char*)malloc(ParentMenuEditStrItem()->length+1);
    strn0cpy(ParentMenuEditStrItem()->value, value, ParentMenuEditStrItem()->length);
    
    InEdit = false;
}

cOsdServerMenuEditStrItem::cPrivateMenuEditStrItem::~cPrivateMenuEditStrItem() { 
    if (Parent) Parent->Detach();
    if (value) free(value);
}

eOSState cOsdServerMenuEditStrItem::cPrivateMenuEditStrItem::ProcessKey(eKeys Key) {
    eOSState state = cMenuEditStrItem::ProcessKey(Key);

    cMutexLock Lock(&LockShared);

    if (InEdit && !InEditMode() && ParentMenuEditStrItem() && !ParentMenuEditStrItem()->Dirty) {
        // Left edit mode. Copy value
        strn0cpy(ParentMenuEditStrItem()->value, value, ParentMenuEditStrItem()->length);
        // Re-set button bar
        ParentMenuEditStrItem()->RestoreButtonBar();
        // Notify
        ParentMenuEditStrItem()->TriggerEvent(cEvent::EditEvent);
            
        InEdit = false;
    }
    if (InEditMode()) InEdit = true;

    if (state==osUnknown && ParentMenuEditStrItem() && !ParentMenuEditStrItem()->Dirty && ParentMenuEditStrItem()->GetMenu()) {
        ParentMenuEditStrItem()->GetMenu()->TriggerEvent(ParentMenuEditStrItem(),Key);
    }

    return state;
}

void cOsdServerMenuEditStrItem::Update() {
    if (!Private) {
        Private=new cPrivateMenuEditStrItem(this);
    } else if (Dirty) {
    }
    Dirty=false;
}

char* cOsdServerMenuEditStrItem::GetValue() {
    cMutexLock Lock(&LockShared);
    return strdup(value);    
}


// ---------------------------
//   cOsdServerMenuEditIntItem
// ---------------------------

cOsdServerMenuEditIntItem::cOsdServerMenuEditIntItem(const char *Name, int Value, int Min, int Max, const char *MinString, const char *MaxString) {
    name = strdup(Name);
    value = Value;
    min = Min;
    max = Max;
    minString = MinString ? strdup(MinString) : NULL;
    maxString = MaxString ? strdup(MaxString) : NULL;
}

cOsdServerMenuEditIntItem::~cOsdServerMenuEditIntItem() {
    cMutexLock Lock(&LockShared);
    Detach();
    if (GetMenu()) GetMenu()->Del(this,false);
    if (name) free(name);
    if (minString) free(minString);
    if (maxString) free(maxString);
}

cOsdServerMenuEditIntItem::cPrivateMenuEditIntItem::cPrivateMenuEditIntItem(cOsdServerMenuEditIntItem *parent)
    : cPrivate(parent) {

    // Replace temporary values of explicit template specialization constructor
    
    value = ParentMenuEditIntItem()->value;
    cMenuEditIntItem::value = &value;
    minString = ParentMenuEditIntItem()->minString ? strdup(ParentMenuEditIntItem()->minString) : NULL;
    maxString = ParentMenuEditIntItem()->maxString ? strdup(ParentMenuEditIntItem()->maxString) : NULL;
}

eOSState cOsdServerMenuEditIntItem::cPrivateMenuEditIntItem::ProcessKey(eKeys Key) {
    eOSState state = cMenuEditIntItem::ProcessKey(Key);

    cMutexLock Lock(&LockShared);

    if (ParentMenuEditIntItem() && value != ParentMenuEditIntItem()->value && !ParentMenuEditIntItem()->Dirty) {
        // Change detected.
        ParentMenuEditIntItem()->value = value;
        // Notify
        ParentMenuEditIntItem()->TriggerEvent(cEvent::EditEvent);
    }

    if (state==osUnknown && ParentMenuEditIntItem() && !ParentMenuEditIntItem()->Dirty && ParentMenuEditIntItem()->GetMenu()) {
        ParentMenuEditIntItem()->GetMenu()->TriggerEvent(ParentMenuEditIntItem(),Key);
    }

    return state;
}

void cOsdServerMenuEditIntItem::Update() {
    if (!Private) {
        Private=new cPrivateMenuEditIntItem(this);
    } else if (Dirty) {
    }
    Dirty=false;
}

int cOsdServerMenuEditIntItem::GetValue() {
    return value;
}

// ----------------------------
//   cOsdServerMenuEditListItem
// ----------------------------

cOsdServerMenuEditListItem::cOsdServerMenuEditListItem(const char *Name, int Value, int Max, const char * const *Strings) {
    name = strdup(Name);
    value = Value;
    max = Max;
    strings = new char*[max];
    for (int i=0; i<max; i++) strings[i] = strdup(Strings[i]);
}

cOsdServerMenuEditListItem::~cOsdServerMenuEditListItem() {
    cMutexLock Lock(&LockShared);
    Detach();
    if (GetMenu()) GetMenu()->Del(this,false);
    if (name) free(name);
    for (int i=0; i<max; i++) free(strings[i]);
    delete[] strings;
}

cOsdServerMenuEditListItem::cPrivateMenuEditListItem::cPrivateMenuEditListItem(cOsdServerMenuEditListItem *parent)
    : cPrivate(parent) {
    // Hint: cMenuEditStraItem::max is ParentMenuEditListItem()->max - 1

    // Replace temporary values of explicit template specialization constructor
    value = ParentMenuEditListItem()->value;
    cMenuEditStraItem::value = &value;

    strings = ParentMenuEditListItem()->tmp_strings;
    ParentMenuEditListItem()->tmp_strings = NULL;
}

cOsdServerMenuEditListItem::cPrivateMenuEditListItem::~cPrivateMenuEditListItem() { 
    if (Parent) Parent->Detach(); 
    for (int i=0; i<max+1; i++) free(strings[i]);
    delete[] strings;
}

eOSState cOsdServerMenuEditListItem::cPrivateMenuEditListItem::ProcessKey(eKeys Key) {
    eOSState state = cMenuEditStraItem::ProcessKey(Key);

    cMutexLock Lock(&LockShared);

    if (ParentMenuEditListItem() && value != ParentMenuEditListItem()->value && !ParentMenuEditListItem()->Dirty) {
        // Change detected.
        ParentMenuEditListItem()->value = value;
        // Notify
        ParentMenuEditListItem()->TriggerEvent(cEvent::EditEvent);
    }

    if (state==osUnknown && ParentMenuEditListItem() && !ParentMenuEditListItem()->Dirty && ParentMenuEditListItem()->GetMenu()) {
        ParentMenuEditListItem()->GetMenu()->TriggerEvent(ParentMenuEditListItem(),Key);
    }

    return state;
}

void cOsdServerMenuEditListItem::Update() {
    if (!Private) {
        tmp_strings = new char*[max];
        for (int i=0; i<max; i++) tmp_strings[i] = strdup(strings[i]);
        
        Private=new cPrivateMenuEditListItem(this);
    } else if (Dirty) {
    }
    Dirty=false;
}

int cOsdServerMenuEditListItem::GetValue() {
    return value;
}

char* cOsdServerMenuEditListItem::GetValueName() {
    cMutexLock Lock(&LockShared);
    return strdup(strings[value]);
}

