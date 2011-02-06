#ifndef ___CHILDLOCKSERVICE_H
#define ___CHILDLOCKSERVICE_H

#include <vdr/childlockservice.h>

class cChannel;
class cRecording;
class cPlugin;
class cPinStatusMonitor;

class ChildLockService: public PinPatch::IChildLockService, public PinPatch::INotificationService
{
    private:
        cPinStatusMonitor* _statusMonitor;

    public:
        ChildLockService(cPinStatusMonitor* statusMonitor);
        
        bool IsUnlocked();

        bool IsMenuProtected(const char* MenuName);
        bool IsChannelProtected(const cChannel* Channel);
        bool IsRecordingProtected(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory);
        bool IsPluginProtected(cPlugin* Plugin);
        
        bool IsMenuHidden(const char* MenuName);
        bool IsPluginHidden(cPlugin* Plugin);
        bool IsRecordingHidden(const cRecording* Recording, const char* Name, const char* Base, bool isDirectory);

        void NotifyTimerCreation(cTimer* Timer, const cEvent *Event);
        void NotifyRecordingStart(const char* FileName);
        void NotifyUserAction(const eKeys key, const cOsdObject* Interact);
};

#endif
