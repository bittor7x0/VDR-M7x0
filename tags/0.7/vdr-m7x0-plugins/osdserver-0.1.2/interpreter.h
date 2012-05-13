#ifndef __OSDSERVER_INTERPRETER_H
#define __OSDSERVER_INTERPRETER_H

#include <vdr/tools.h>
#include <sys/time.h>
#include <time.h>
#include <vector>

#include "tools.h"
#include "connection.h"
#include "osdobjects.h"




class cConnection : public cConnectionBase {
public:
    cConnection();

public:
    void ReplyReady() { Reply(200,"Ready."); }
    void ReplyWelcome() { Reply(201,"Welcome to OSDServer version %s, VDR version %s.",PLUGINVERSION,VDRVERSION); }
    void ReplyGoodBye() { Reply(202,"Good Bye."); }

    // error 400: Access Denied. See tools.c.
    void ReplyErrorIdentifier(const char *id) { Reply(401,"Error: Expected an identifier: %s",id); }
    void ReplyErrorIdentifierReserved(const char *id) { Reply(402,"Error: Reserved identifier: %s",id); }
    void ReplyErrorUnknownObject(const char *c) { Reply(403,"Error: Unknown object %s.",c); }
    void ReplyErrorUnknownCommand(const char *s) { Reply(404,"Error: Unknown command %s",s); }
    void ReplyErrorMissingVersion() { Reply(405,"Error: Need to specify VERSION first"); }
    void ReplyErrorUnknownVersion(const char *s) { Reply(406,"Error: Protocol version %s not supported.",s); }
    void ReplyErrorMissingParameter() { Reply(407,"Error: Missing Parameter."); }
    void ReplyErrorUnknownParameter(const char *par) { Reply(408,"Error: Unknown Parameter: %s",par); }
    void ReplyErrorAssign() { Reply(409,"Error: Command does not return an object to assign."); }
    void ReplyErrorParameter(const char *s) { Reply(410,"Error: Parameter Error: %s",s); }
    void ReplyErrorUnknownEvent(const char *s) { Reply(411,"Error: Unknown Event %s.",s); }
    void ReplyErrorObjectNotActive() { Reply(412,"Error: Object is not active."); }
    void ReplyErrorUnknownState(const char *s) { Reply(413,"Error: State is not known: %s.",s); }
    void ReplyErrorColorKeyAlreadySet() { Reply(414,"Error: All color keys already set."); }
    void ReplyErrorParameterInt(const char *s) { Reply(414,"Error: Expected numeric parameter: %s",s); }
    void ReplyErrorParameterRange(const char *s) { Reply(415,"Error: Numeric parameter out of range: %s",s); }
    void ReplyErrorConflictingParameters(const char *s1, const char *s2) { Reply(416,"Error: Conflicting parameters: %s %s",s1,s2); }
    void ReplyErrorParameterInvalid(const char *s) { Reply(417,"Error: Invalid parameter: %s",s); }
    void ReplyErrorUnknownType(const char *s) { Reply(418,"Error: Unknown object type: %s",s); }
    void ReplyErrorIncompatibleType(const char *s) { Reply(419,"Error: Incompatible object type: %s",s); }
    void ReplyErrorNoParentContext() { Reply(420, "Error: No parent context"); }
    
    void ReplyMessageEvent(const char *who, const char *event) {
        Reply(300,"%s %s",who,event);
    }
    void ReplyMessageKeyEvent(const char *who, eKeys k) {
        char buffer[32];
        ReplyMessageEvent(who,KeyString(buffer,sizeof buffer,k));
    }
    void ReplyMessageEventTimeout() {
        Reply(301,"timeout");
    }
    void ReplyMessageMenuCurrent(int i, const char *s) {
        if (s)
            Reply(302,"%i %s",i,s);
        else
            Reply(302,"%i",i);
    }

    void ReplyTextGetValue(bool quoted, char *value) {
        ReplyPlain(quoted?500:600, value);
    }

    static char* KeyString(char *buffer, int bufferlen, eKeys key);
    static int ParseKey(const char *str);
};



class cCommandProcessor : public cListObject {
public:
    cCommandProcessor(cConnection *Connection);
    ~cCommandProcessor();

    bool Action(cSelect &Selector);


    class cCommand {
    private:
        char *Command;
        char *Context;
        char *AssignVar;
        
        class cArg {
            const char *text;
            bool isFlag;
        public:
            const char* Text() { return text; }
            bool IsFlag() { return isFlag; }
            bool GetInt(int &res) {
                char *p; 
                res = strtol(text, &p, 10); 
                return (*p==0); 
            }
            cArg(const char *Text, bool IsFlag) { text = Text; isFlag = IsFlag; }
        };

        std::vector<cArg> args;
        typedef std::vector<cArg>::iterator args_iterator;

    public:
        cCommand();

        bool ParseLine(cConnection &Connection, char *line);

        const char* GetCommand() { return Command; }
        const char* GetContext() { return Context; }
        const char* GetAssignVar() { return AssignVar; }

        bool IsCommand(const char *cmd) { return strcasecmp(Command,cmd)==0; }
        bool IsAssignment() { return AssignVar!=NULL; }
        bool HasMaxArgument(unsigned int max) { return args.size()<=max; }
        bool HasMinArgument(unsigned int min) { return args.size()>=min; }
        int GetArgCount() { return args.size(); }

        const char* GetArg(unsigned int i) { if (i>=0 && i<args.size()) return args[i].Text(); else return NULL; }
        bool GetArgInt(unsigned int i, int &res) { if (i>=0 && i<args.size()) return args[i].GetInt(res); else return false; }
        bool IsArgFlag(unsigned int i) { if (i>=0 && i<args.size()) return args[i].IsFlag(); else return false; }
        void DropArgs(int start, int count);
        
        bool ArgScanFlag(const char *flag);
        const char* ArgScanFlagParam(const char *flag);
    };

protected:
    cServerContext BaseContext;
    cServerContext *LocalContext;

    cOsdServerMenu *SleepEventObj;
    timeval SleepEventTimeout;

    cConnection *Connection;

    enum enumState {
        stateNew,
        stateWelcome,
        stateReady,
        stateTerminated,
        stateSleepEvent
    };
    enumState State;


    
    cOsdServerMenu* NewMenu(cCommand &cmd);
    cOsdServerOsdItem* NewOsdItem(cCommand &cmd);
    cOsdServerMenuEditStrItem* NewEditStrItem(cCommand &cmd);
    cOsdServerMenuEditIntItem* NewEditIntItem(cCommand &cmd);
    cOsdServerMenuEditListItem* NewEditListItem(cCommand &cmd);
    
    cOsdServerObject* NewObject(cCommand &cmd);

    bool CmdVersion(cCommand &cmd);
    bool CmdQuit(cCommand &cmd);
    bool CmdIdle(cCommand &cmd);
    bool CmdEnterLocal(cCommand &cmd);
    bool CmdLeaveLocal(cCommand &cmd);

    bool CmdMessage(cCommand &cmd);
    bool CmdNew(cCommand &cmd);
    bool CmdDelete(cCommand &cmd);
    bool CmdObjectDelete(cCommand &cmd, cOsdServerObject *osdobj);

    bool CmdMenuAdd(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuAddNew(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuAddSubMenu(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuShow(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuSendState(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuSleepEvent(cCommand &cmd, int &timeout, cOsdServerMenu *menu);
    bool CmdMenuGetCurrent(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuSetCurrent(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuSetColorKeyText(cCommand &cmd, cOsdServerMenu *menu);
    bool CmdMenuSetColumns(cCommand &cmd, cOsdServerMenu *menu);
    
    bool CmdItemSetCurrent(cCommand &cmd, cOsdServerMenuItem *menuitem);
    
    bool CmdItemSetSelectable(cCommand &cmd, cOsdServerOsdItem *osditem, bool Selectable);
    bool CmdItemSetText(cCommand &cmd, cOsdServerOsdItem *osditem);
    
    bool CmdObjectEnableEvent(cCommand &cmd, cOsdServerObject *osdobj);

    bool CmdEditStrItemGetValue(cCommand &cmd, cOsdServerMenuEditStrItem *editobj);
    bool CmdEditIntItemGetValue(cCommand &cmd, cOsdServerMenuEditIntItem *editobj);
    bool CmdEditListItemGetValue(cCommand &cmd, cOsdServerMenuEditListItem *editobj);

    bool ProcessGlobalCommand(cCommand &cmd, int &timeout);
    bool ProcessMenuCommand(cCommand &cmd, int &timeout, cOsdServerMenu *menu);
    bool ProcessMenuItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuItem *menuitem);
    bool ProcessOsdItemCommand(cCommand &cmd, int &timeout, cOsdServerOsdItem *osditem);
    bool ProcessEditStrItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuEditStrItem *edititem);
    bool ProcessEditIntItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuEditIntItem *edititem);
    bool ProcessEditListItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuEditListItem *edititem);
    bool ProcessOsdObjectCommand(cCommand &cmd, int &timeout, cOsdServerObject *osdobj);
    void ProcessUnknown(cCommand &cmd);

    bool ProcessCommand(cCommand &cmd, int &timeout);
    void ProcessWelcome(cCommand &cmd, int &timeout);

    bool HandleSleepEvent(int &timeout);
};



#endif
