#include "childlock.h"
#include "childlockservice.h"
#include "plugin.h"

using namespace PinPatch;

IChildLockService* ChildLock::_cachedChildLockService = NULL;
INotificationService* ChildLock::_cachedNotificationService = NULL;

IChildLockService* ChildLock::ChildLockService()
{
    if (!_cachedChildLockService)
    {
        IChildLockService* childLockService;

        if (cPluginManager::CallFirstService(CHILDLOCK_SERVICE_ID, &childLockService))
        {
            _cachedChildLockService = childLockService;
        }
        else
        {
            _cachedChildLockService = NULL;
        }
    }

    return _cachedChildLockService;
}

bool ChildLock::IsUnlocked()
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsUnlocked();
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsMenuProtected(const char* MenuName)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsMenuProtected(MenuName);
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsChannelProtected(const cChannel* Channel)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsChannelProtected(Channel);
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsRecordingProtected(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsRecordingProtected(Recording, Name, Base, isDirectory);
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsPluginProtected(cPlugin* Plugin)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsPluginProtected(Plugin);
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsMenuHidden(const char* MenuName)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsMenuHidden(MenuName);
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsPluginHidden(cPlugin* Plugin)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsPluginHidden(Plugin);
    }
    else
    {
        return false;
    }
}

bool ChildLock::IsRecordingHidden(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory)
{
    if (IChildLockService* childLockService = ChildLockService())
    {
        return childLockService->IsRecordingHidden(Recording, Name, Base, isDirectory);
    }
    else
    {
        return false;
    }
}

INotificationService* ChildLock::NotificationService()
{
    if (!_cachedNotificationService)
    {
        INotificationService* notificationService;

        if (cPluginManager::CallFirstService(NOTIFICATION_SERVICE_ID, &notificationService))
        {
            _cachedNotificationService = notificationService;
        }
        else
        {
            _cachedNotificationService = NULL;
        }
    }

    return _cachedNotificationService;
}

void ChildLock::NotifyTimerCreation(cTimer* Timer, const cEvent *Event)
{
    if (INotificationService* notificationService = NotificationService())
    {
        notificationService->NotifyTimerCreation(Timer, Event);
    }
}

void ChildLock::NotifyRecordingStart(const char* FileName)
{
    if (INotificationService* notificationService = NotificationService())
    {
        notificationService->NotifyRecordingStart(FileName);
    }
}

void ChildLock::NotifyUserAction(const eKeys key, const cOsdObject* Interact)
{
    if (INotificationService* notificationService = NotificationService())
    {
        notificationService->NotifyUserAction(key, Interact);
    }
}
