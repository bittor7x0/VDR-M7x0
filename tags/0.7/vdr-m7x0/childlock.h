#ifndef __CHILDLOCK_H
#define __CHILDLOCK_H

#include "keys.h"

class cChannel;
class cRecording;
class cPlugin;
class cTimer;
class cEvent;
class cOsdObject;

namespace PinPatch
{

class IChildLockService;
class INotificationService;

class ChildLock
{
    private:
        static IChildLockService* _cachedChildLockService;
        static INotificationService* _cachedNotificationService;
        
    private:
        static IChildLockService* ChildLockService();
        static INotificationService* NotificationService();

    public:
        static bool IsUnlocked();

        static bool IsMenuProtected(const char* MenuName);
        static bool IsChannelProtected(const cChannel* Channel);
        static bool IsRecordingProtected(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory);
        static bool IsPluginProtected(cPlugin* Plugin);
        
        static bool IsMenuHidden(const char* MenuName);
        static bool IsPluginHidden(cPlugin* Plugin);
        static bool IsRecordingHidden(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory);
        
        static void NotifyTimerCreation(cTimer* Timer, const cEvent *Event);
        static void NotifyRecordingStart(const char* FileName);
        static void NotifyUserAction(const eKeys key, const cOsdObject* Interact);
};

};
#endif
