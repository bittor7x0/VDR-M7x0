/***************************************************************************
                          commands.h  -  description
                             -------------------
    begin                : Sa Jan 15 2005
    copyright            : (C) 2005 by Holger Brunn
    email                : holger.brunn@stud.uni-karlsruhe.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILEBROWSER_COMMANDS_H
#define FILEBROWSER_COMMANDS_H

#include <vdr/keys.h>
#include <vdr/osdbase.h>

#include <stdio.h>

#include "tools.h"
#ifdef FILEBROWSER_PLUGIN_BUILD
#include "threads.h"
#endif
#include "statebag.h"
#include "menu-filebrowser.h"

// Forward declaration eMenuFilebrowserTask (from menu.h)
enum eMenuFilebrowserTask;

class cFilebrowserCommand
{
  protected:
    char* Name;
    char* Pattern;
    bool Confirm;
    bool UseDestination;
    bool UseCurrentFile;
    bool UseSelectedFiles;
    bool UseNewDestination;
    bool PatternIsShellscript;
    bool Synchronous;
    bool Menu;
    bool RemoveFinished;
    cFilebrowserStatebag* Statebag;
    cFilebrowserCommand(cFilebrowserStatebag* Statebag);
    bool MatchesSingleFile(const char* Filename);
    eOSState State;
    char* AccessCode;
  public:
    virtual ~cFilebrowserCommand();
    virtual char* GetName();
    virtual eKeys GetHotkey();
    virtual bool UsesDestination() { return UseDestination || UseNewDestination; };
    virtual bool UsesCurrentFile() { return UseCurrentFile; };
    virtual bool UsesSelectedFiles() { return UseSelectedFiles; };
    virtual bool UsesNewDestination() { return UseNewDestination; };
    virtual bool Matches(const char* Filename);
    virtual bool MatchesDestination(const char* Filename) { return true; };
    virtual bool NeedsConfirmation() { return Confirm; };
    virtual bool IsSynchronous() { return Synchronous; };
    virtual bool ShowMenu() { return Menu; };
    virtual bool RemoveWhenFinished() { return RemoveFinished; };
    virtual bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    virtual char* GetAccessCode() { return AccessCode; };
    virtual eMenuFilebrowserTask Task() { return taskBrowse; };
    virtual eOSState GetState() { return State; };
};

class cFilebrowserConfigCommand : public cFilebrowserCommand
{
  protected:
    char* Command;
    static char* FlagHandler(const char* OrgString, const char* CurrentPos, const cCommandParser::cHandlerParameters* Params);
  public:
    cFilebrowserConfigCommand(cFilebrowserStatebag* Statebag);
    ~cFilebrowserConfigCommand();
    static cFilebrowserConfigCommand* Parse(char* Config, cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    char* GetCommand();
};

class cFilebrowserMarkCommand : public cFilebrowserCommand
{
  public:
    cFilebrowserMarkCommand(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

class cFilebrowserMarkAllCommand : public cFilebrowserCommand
{
  public:
    cFilebrowserMarkAllCommand(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

class cFilebrowserUnmarkCommand : public cFilebrowserCommand
{
  public:
    cFilebrowserUnmarkCommand(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

class cFilebrowserUnmarkAllCommand : public cFilebrowserCommand
{
  public:
    cFilebrowserUnmarkAllCommand(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

class cFilebrowserDestinationNewCommand : public cFilebrowserCommand
{
  public:
    cFilebrowserDestinationNewCommand(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
    virtual eMenuFilebrowserTask Task() { return taskSelectDestination; };
};

class cFilebrowserDestinationContainerCommand : public cFilebrowserCommand
{
  protected:
    cFilebrowserCommand* Command;
    char* CurrentFile;
    char* DestinationFile;
  public:
    cFilebrowserDestinationContainerCommand(cFilebrowserCommand* Command, char* CurrentFile, cFilebrowserStatebag* Statebag);
    ~cFilebrowserDestinationContainerCommand();
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
    char* GetAccessCode();
};

class cFilebrowserDestinationAbortCommand : public cFilebrowserCommand
{
  public:
    cFilebrowserDestinationAbortCommand(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

#ifdef FILEBROWSER_PLUGIN_BUILD

class cFilebrowserCommandThreadList : public cFilebrowserCommand
{
  public:
    cFilebrowserCommandThreadList(cFilebrowserStatebag* Statebag);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* SelectedFile);
    bool Matches(const char* Filename);
};

#endif

class cFilebrowserCommandContainer : public cListContainer<cFilebrowserCommand>
{
  public:
    cFilebrowserCommandContainer(cFilebrowserCommand* Object, bool DelOnFree=true) : cListContainer<cFilebrowserCommand>(Object, DelOnFree) {};
    cFilebrowserCommandContainer* Next() { return (cFilebrowserCommandContainer*)cListContainer<cFilebrowserCommand>::Next(); };
    cFilebrowserCommandContainer* Prev() { return (cFilebrowserCommandContainer*)cListContainer<cFilebrowserCommand>::Prev(); };
};

class cFilebrowserCommands : public cList<cFilebrowserCommandContainer> {};

#endif
