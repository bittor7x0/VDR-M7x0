

#include <stdlib.h>
#include "osdobjectsbase.h"
#include "osdserver.h"

// --------------------
//   cOsdServerObject
// --------------------

cMutex cShadowBase::LockShared;

cOsdServerObject::cOsdServerObject() {
    name = NULL;
    context = NULL;
    id=0;
}


cOsdServerObject::~cOsdServerObject() {
    if (context) context->Remove(this);
    if (name) free(name);
}


// ---------------------
//   cOsdServerContext
// ---------------------


cOsdServerObject* cServerContext::GetName(const char *Name) {
    if (!Name) return NULL;
    
    ObjectList::iterator i = objects.find(Name);
    if (i != objects.end())
       return i->second;
    return NULL;
}

void cServerContext::Add(cOsdServerObject *Object, const char *Name) {
    maxid++;
    Object->id = maxid;
    if (Name) {
        ObjectList::iterator i = objects.find(Name);
        if (i != objects.end()) {
            cOsdServerObject *prev = i->second;
            objects.erase(i);
            
            free(prev->name);
            asprintf(&prev->name,"_%i", prev->id);
            objects.insert(std::make_pair(prev->name, prev));
        }
        
        Object->name = strdup(Name);
    } else {
        asprintf(&Object->name, "_%i", Object->id);
    }
    Object->context = this;
    objects.insert(std::make_pair(Object->name, Object));
}

void cServerContext::Remove(cOsdServerObject *Object) {
    ObjectList::iterator i = objects.find(Object->name);
    if (i != objects.end() && i->second == Object) {
        objects.erase(i);           
    }
}

cServerContext::~cServerContext() {
    // Avoid the default clear destructor - objects removes itself from list

    
    while (!objects.empty()) {
        cOsdServerObject *obj = objects.begin()->second;
        objects.erase(objects.begin());
        delete obj;
    }
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

