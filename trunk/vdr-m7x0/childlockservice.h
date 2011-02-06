#ifndef __CHILDLOCKSERVICE_H
#define __CHILDLOCKSERVICE_H

#include "keys.h"

class cChannel;
class cRecording;
class cPlugin;
class cTimer;
class cEvent;
class cOsdObject;

namespace PinPatch
{

#define CHILDLOCK_SERVICE_ID "ChildLockService-v0.1::ChildLockService"

class IChildLockService
{
    public:
        virtual ~IChildLockService() {};
        virtual bool IsUnlocked() = 0;

        virtual bool IsMenuProtected(const char* MenuName) = 0;
        virtual bool IsChannelProtected(const cChannel* Channel) = 0;
        virtual bool IsRecordingProtected(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory) = 0;
        virtual bool IsPluginProtected(cPlugin* Plugin) = 0;
        
        virtual bool IsMenuHidden(const char* MenuName) = 0;
        virtual bool IsPluginHidden(cPlugin* Plugin) = 0;
        virtual bool IsRecordingHidden(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory) = 0;
};

#define NOTIFICATION_SERVICE_ID "PinPlugin-v0.1::NotificationService"

class INotificationService
{
    public:
        virtual ~INotificationService() {};
        virtual void NotifyTimerCreation(cTimer* Timer, const cEvent *Event) = 0;
        virtual void NotifyRecordingStart(const char* FileName) = 0;
        virtual void NotifyUserAction(const eKeys key, const cOsdObject* Interact) = 0;
};

};
#endif
