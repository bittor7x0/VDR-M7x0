#include "childlockservice.h"
#include "pin.h"

ChildLockService::ChildLockService(cPinStatusMonitor* statusMonitor)
{
    _statusMonitor = statusMonitor;
}

bool ChildLockService::IsUnlocked()
{
    return PinService::pinValid;
}

bool ChildLockService::IsMenuProtected(const char* MenuName)
{
    return _statusMonitor->MenuItemProtected(MenuName);
}

bool ChildLockService::IsChannelProtected(const cChannel* Channel)
{
    return _statusMonitor->ChannelProtected(Channel);
}

bool ChildLockService::IsRecordingProtected(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory)
{
    return _statusMonitor->ReplayProtected(Recording, Name, Base, isDirectory);;
}

bool ChildLockService::IsPluginProtected(cPlugin* Plugin)
{
    return _statusMonitor->PluginProtected(Plugin);
}

bool ChildLockService::IsMenuHidden(const char* MenuName)
{
    return _statusMonitor->MenuItemProtected(MenuName, true);
}

bool ChildLockService::IsPluginHidden(cPlugin* Plugin)
{
    return _statusMonitor->PluginProtected(Plugin, true);;
}

bool ChildLockService::IsRecordingHidden(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory)
{
    return _statusMonitor->ReplayProtected(Recording, Name, Base, isDirectory, true);
}

void ChildLockService::NotifyTimerCreation(cTimer* Timer, const cEvent *Event)
{
    return _statusMonitor->TimerCreation(Timer, Event);
}

void ChildLockService::NotifyRecordingStart(const char* FileName)
{
    return _statusMonitor->RecordingFile(FileName);
}

void ChildLockService::NotifyUserAction(const eKeys key, const cOsdObject* Interact)
{
    return _statusMonitor->UserAction(key, Interact);
}

