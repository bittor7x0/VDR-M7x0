#ifndef VDR_EPGSEARCH_H
#define VDR_EPGSEARCH_H

#include "epgsearchext.h"

class cPluginEpgsearch : public cPlugin {
public:
    bool showConflicts;
    bool showAnnounces;

    cPluginEpgsearch(void);
    virtual ~cPluginEpgsearch();
    virtual const char *Version(void);
    virtual const char *Description(void);
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual const char *MainMenuEntry(void);
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    cOsdObject *DoInitialSearch(char* rcFilename);
    void LoadMenuTemplates();
    bool Service(const char *Id, void *Data);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Cmd, const char *Option, int &ReplyCode);
    void LoadUserVars();
};

#endif
