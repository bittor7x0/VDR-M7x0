#ifndef __MAINMENUSHORTCUT_H
#define __MAINMENUSHORTCUT_H

#include <vdr/plugin.h>

class cMainMenuShortcutSetupPage : public cMenuSetupPage 
{
    private:
        const char* _setupEntry;
        int* const _setupValue;

    public:
        cMainMenuShortcutSetupPage(const char* setupText, const char* setupEntry, int* const setupValue);

    protected:
        virtual void Store(void);
};

class cMainMenuShortcut : public cPlugin
{
    private:
        int _mainMenuEntryEnabled;

    public:
        cMainMenuShortcut();
        virtual ~cMainMenuShortcut();
        virtual bool Initialize();
        virtual bool SetupParse(const char* Name, const char* Value);
        virtual cMenuSetupPage* SetupMenu();
        virtual const char* MainMenuEntry();

    protected:
        cOsdMenu* GetEpgSearchMenu(const char* serviceName);
        virtual const char* SetupText() = 0;
        virtual const char* MainMenuText() = 0;
};

#endif
