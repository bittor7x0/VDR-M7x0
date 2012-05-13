#ifndef __OSDSERVER_OSDOBJECTSBASE_H
#define __OSDSERVER_OSDOBJECTSBASE_H

#include <map>
#include <set>
#include <queue>

#include <vdr/tools.h>
#include <vdr/thread.h>
#include <vdr/osdbase.h>
#include <vdr/menuitems.h>

#include "tools.h"

class cServerContext;


class cOsdServerObject {
    // Generic base class for named and unnamed objects of the network interface.
    // These objects live in the network thread
    // These objects may 'own' a VDR main thread object.
    // The OsdServerObject holds a copy of the state of the VDR object, and
    // can push its state to the VDR object, or create it if it doesnt exist yet.
    // The VDR object can detach if its lifetime ends.
    // Before deleting this object, always call Detach().

private:
    int id;
    // Unique ID identifying any object
    
    static int nextId;
    // Common ID counter
    
    cServerContext *context;
    // The 'owning' context. Deleting this context deletes the object.
    // The object can be present in more contexts.

public:
    cOsdServerObject();
    virtual ~cOsdServerObject();

    // IDs to identify certain object instances from base class
    enum enumClassId {
        clsidMenu,
        clsidOsdItem,
        clsidMenuEditStrItem,
        clsidMenuEditIntItem,
        clsidMenuEditListItem
    };
    virtual enumClassId ClassId()=0;

    virtual bool IsMenuItem() { return false; }

    // Generic cast function that checks the id.
    // Use as baseobject->Cast<type>()
    template<class T> T* Cast() {
        if (ClassId() != T::classid) return NULL;
        return dynamic_cast<T*>(this);
    }

    // Context object name or NULL if not named
    inline cString Name();


    class cEvent {
    public:
        enum eventType { NullEvent, KeyEvent, CloseEvent, FocusEvent, BlurEvent, EditEvent } type;
        eKeys key;

        cEvent() { type = NullEvent; key = kNone; }
        cEvent(eventType t) { type = t; key = kNone; }
        cEvent(eKeys k) { type = KeyEvent; key = k; }
        
        bool operator<(const cEvent &ev) const {
            if (type == KeyEvent && ev.type == KeyEvent)
                return key < ev.key;
            return type < ev.type;
        }
    };
    class cTriggeredEvent : public cEvent {
    public:
        cOsdServerObject *source;
        
        cTriggeredEvent(cOsdServerObject *src, cEvent ev) : cEvent(ev) { source=src; }
        cTriggeredEvent(cOsdServerObject *src, eKeys k) : cEvent(k) { source=src; }
        cTriggeredEvent(cOsdServerObject *src, eventType t) : cEvent(t) { source=src; }
        cTriggeredEvent() { source=NULL; }
    };

protected:
    std::set<cEvent> enabledEvents;
    std::queue<cTriggeredEvent> eventQueue;
        
    // These are protected by LockShared mutex,
    // unless both threads are synchronized anyway.

    bool IsEventEnabled(cEvent event) {
        std::set<cEvent>::iterator i = enabledEvents.find(event);
        return i != enabledEvents.end();
    }
    
public:    
    void TriggerEvent(cOsdServerObject *src, cEvent ev) { 
        if (src->IsEventEnabled(ev)) eventQueue.push(cTriggeredEvent(src,ev)); 
    }
    void TriggerEvent(cEvent ev) { 
        TriggerEvent(this,ev); 
    }

    virtual bool EnableEvent(cEvent event) {
        enabledEvents.insert(event);
        return true;
    }
        
    virtual bool PollEvent(cTriggeredEvent &Event) { return false; }
    // Primitive event handling


    inline void SetFocusObject();
    // Set object as focused

    inline void UnsetFocusObject();
    // Set object as focused

    inline bool IsFocusObject();
    // Check whether object is focused

    friend class cServerContext;
};


struct ltcstrcase {
  bool operator()(const cString &s1, const cString &s2) const {
    return strcasecmp(s1, s2) < 0;
  }
};


class cServerContext {
    // Manage a collection of named OsdServerObjects.
    
    typedef std::map<cString, cOsdServerObject*, ltcstrcase> ObjectList;
    ObjectList objects; // Maps name to object
    
    typedef std::map<int, cString> IdList;
    IdList idList; // Maps id to name

    cServerContext *localContext;
    cServerContext *parentContext;
    
    cOsdServerObject *focusObject;
    
public:
    cOsdServerObject* GetName(const char *Name);
    // Get object by name
    
    cString GetNameById(int id) {
        // Get object name by id
        IdList::iterator i = idList.find(id);
        return (i != idList.end()) ? i->second : cString::sprintf("_%i", id);
    }

    void Add(cOsdServerObject *Object, cString Name);
    // Also sets the name of the object
    
    void Remove(cOsdServerObject *Object);
    // Removes an object from the context
    
    cServerContext* MakeLocalContext();
    // Create local context as alias of this one

    cServerContext* GetParentContext() { return parentContext; }
    // Get parent of this context, or NULL for topmost context

    void DestroyLocalContext();
    // Delete local contexts of this one
    
    cOsdServerObject* GetFocusObject();
    // Get the currently focused object (not menu items, just menus)

    cServerContext();
    virtual ~cServerContext();

    friend class cOsdServerObject;
};



// Forwarded inlines of cOsdServerObject

inline cString cOsdServerObject::Name() {
    return context ? context->GetNameById(id) : cString(); 
}

inline void cOsdServerObject::SetFocusObject() {
    if (context) context->focusObject = this; 
}

inline void cOsdServerObject::UnsetFocusObject() {
    if (context && context->focusObject == this) 
        context->focusObject = NULL;
}

inline bool cOsdServerObject::IsFocusObject() {
    return context ? (this == context->focusObject) : false;
}



class cOsdServerMenu;

class cOsdServerMenuItem : public cListObject, public cOsdServerObject {
    // Base class for menu items
    cOsdServerMenu *menu;
protected:
    virtual cOsdItem* GetItem()=0;

    cOsdServerMenuItem() { menu=NULL; }

public:
    cOsdServerMenu* GetMenu() { return menu; }

    virtual bool IsMenuItem() { return true; }

    virtual void ItemUpdate()=0;
    // Forwarder to cShadowBase if of that class
    
    virtual void ItemDetach()=0;
    // Forwarder to cShadowBase if of that class
    
    friend class cOsdServerMenu; // to set menu
};




class cShadowBase {
    // Abstract base class for shadowed objects. Acts as base class for cShadowTemplate

protected:
    bool Dirty;
    // =true means that this object has changes that are not yet Update()d to
    // the VDR object. Also means that the VDR object will not upload
    // local changes, they will be discarded on Update().
    // Protected by LockShared.

    static cMutex LockShared;
    // Common lock to access certain members from the VDR main thread

    class cPrivateBase {
    public:
        cShadowBase *Parent;
        // Parent is a pointer to the shadow object, or NULL if detached.
        // This member is protected by LockShared mutex.

        cPrivateBase(cShadowBase *parent);
        // Constructors are always called with synced threads

        virtual ~cPrivateBase();
        // Warning: Higher level destructors need to call Parent->Detach().
        // Foreground thread always calls destructor on its own
    };

public:
    virtual void Update()=0;
    // Push object status to VDR object or create VDR object.
    // Both threads MUST be synchronized before calling this

    virtual void Detach()=0;
    // Detach VDR object from this object. Can be called
    // from any context.

    virtual bool IsDetached()=0;
    // Check if object is 'live'

    cShadowBase();
    virtual ~cShadowBase() {};
};

template<class T> class cShadowTemplate : public cShadowBase {
    // VDR object shadow class
    // This template encapsulates a VDR object as cOsdServerShadow<T>::cPrivate
    // and provides the shadow cOsdServerObject for it.
    // cPrivate lives in the VDR main thread, while cOsdServerShadow lives in the network thread.
    // T needs to have a parameter-less constructor. Make one if you need one.

protected:

    class cPrivate : public T, public cShadowBase::cPrivateBase {
        public:
        inline cPrivate(cShadowTemplate<T> *parent) : cShadowBase::cPrivateBase(parent) { };

        friend class cShadowTemplate<T>;
    };
    
    cPrivate *Private;
    // Pointer to the shadowed VDR object, or NULL if none attached (yet).
    // Call Upade() to attach one.

public:
    // Constructor and destructor are called from network thread.
    // Warning: Higher level destructors need to call Detach().
    cShadowTemplate<T>() {
        Private=NULL;
    }

    virtual ~cShadowTemplate<T>() {
        cMutexLock Lock(&LockShared);
        if (Private) {
            esyslog("osdserver: hard detaching of OSD shadow object");
            Detach();
        }
    }

    virtual void Detach() {
        // This Detach() removes the cross-links between cOsdServerShadow and cPrivate.

        // Detach private VDR object, make it independent
        cMutexLock Lock(&LockShared);
    
        if (!Private) return; // already detached
    
        Private->Parent=NULL;
        Private=NULL;        
    }

    virtual bool IsDetached() { return Private==NULL; }
};

template<class T> class cShadowObjectTemplate : public cOsdServerObject, public cShadowTemplate<T> {
    virtual bool EnableEvent(cEvent event) {
        cMutexLock Lock(&cShadowBase::LockShared);
        return cOsdServerObject::EnableEvent(event);
    }
};

template<class T> class cShadowMenuItemTemplate : public cOsdServerMenuItem, public cShadowTemplate<T> {
public:
    using cShadowTemplate<T>::Update;
    using cShadowTemplate<T>::Detach;
    using cShadowTemplate<T>::Private;
    virtual void ItemUpdate() { Update(); }
    virtual void ItemDetach() { Detach(); }
    virtual cOsdItem* GetItem() { return Private; }
    
    virtual bool EnableEvent(cEvent event) {
        cMutexLock Lock(&cShadowBase::LockShared);
        return cOsdServerObject::EnableEvent(event);
    }
};



#endif
