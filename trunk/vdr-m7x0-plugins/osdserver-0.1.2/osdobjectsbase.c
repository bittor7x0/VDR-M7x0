

#include <stdlib.h>
#include "osdobjectsbase.h"
#include "osdserver.h"

// --------------------
//   cOsdServerObject
// --------------------

cMutex cShadowBase::LockShared;


int cOsdServerObject::nextId = 0;

cOsdServerObject::cOsdServerObject() {
    context = NULL;
    id = nextId++;
}


cOsdServerObject::~cOsdServerObject() {
    if (context) context->Remove(this);
}


// ---------------------
//   cOsdServerContext
// ---------------------

cOsdServerObject* cServerContext::GetName(const char *Name) {
    if (!Name) return NULL;
        
    if (Name[0] == '_' && (Name[1] < '0' || Name[1] > '9')) {
        // Reserved name, but not _[0-9]*   
        if (0 == strcasecmp(Name, "_FOCUS")) {
            return GetFocusObject();
        }
    }
    
    ObjectList::iterator i = objects.find(Name);
    if (i != objects.end())
       return i->second;
    return NULL;
}

void cServerContext::Add(cOsdServerObject *Object, cString Name) {
    if (localContext)
        localContext->Add(Object, Name);

    if (!(const char*)Name)
        Name = cString::sprintf("_%i", Object->id);

    ObjectList::iterator i = objects.find(Name);
    if (i != objects.end()) {
        // Already other object of same name. Rename the other.
        cOsdServerObject *prev = i->second;
        cString pname = cString::sprintf("_%i", prev->id);
        
        objects.erase(i);
        objects.insert(std::make_pair(pname, prev));
        
        IdList::iterator id = idList.find(prev->id);
        if (id != idList.end())
            idList.erase(id);
        idList.insert(std::make_pair(prev->id, pname));
    }
    objects.insert(std::make_pair(Name, Object));
    idList.insert(std::make_pair(Object->id, Name));

    Object->context = this;
}

void cServerContext::Remove(cOsdServerObject *Object) {
    if (Object == focusObject)
        focusObject = NULL;

    if (localContext)
        localContext->Remove(Object);
    
    IdList::iterator id = idList.find(Object->id);
    if (id == idList.end())
        return;
        
    ObjectList::iterator i = objects.find(id->second); // search by name
    if (i != objects.end() && i->second == Object) {        
        objects.erase(i);
    }
    
    idList.erase(id);
}


cServerContext* cServerContext::MakeLocalContext() {
    // Create local context as alias of this one
    if (localContext)
        return localContext;
    
    localContext = new cServerContext;
    
    localContext->objects = objects;
    localContext->idList = idList;
    localContext->parentContext = this;
    
    return localContext;
}

void cServerContext::DestroyLocalContext() {
    if (localContext) {
        delete localContext;
        localContext = NULL;
    }
}

cOsdServerObject* cServerContext::GetFocusObject() {
    if (focusObject)
        return focusObject;
    if (parentContext)
        return parentContext->GetFocusObject();
    return NULL;
}

cServerContext::cServerContext() {
    localContext = NULL;
    parentContext = NULL;
    
    focusObject = NULL;
}

cServerContext::~cServerContext() {
    // Avoid the default clear destructor - objects removes itself from list
    DestroyLocalContext();
    
    while (!objects.empty()) {
        cOsdServerObject *obj = objects.begin()->second;
        objects.erase(objects.begin());
        if (obj->context == this)
            delete obj;
    }
    
    if (parentContext)
        parentContext->localContext = NULL;    
}




// ---------------
//   cShadowBase
// ---------------


cShadowBase::cShadowBase() {
    Dirty=true;
}

cShadowBase::cPrivateBase::cPrivateBase(cShadowBase *parent) {
    Parent=parent;
}

cShadowBase::cPrivateBase::~cPrivateBase() {
    if (!Parent) return;
    esyslog("osdserver: hard detaching of OSD VDR object");
    Parent->Detach();
}

