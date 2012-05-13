

#include <vdr/config.h>
#include <vdr/skins.h>

#include "interpreter.h"


// ---------------
//   cConnection
// ---------------


cConnection::cConnection() {
}


char* cConnection::KeyString(char *buffer, int bufferlen, eKeys key) {
    const char *k;

    if ((key&~k_Flags)==kNone) {
        k="None";
    } else {
        k=cKey::ToString((eKeys)(key & ~k_Flags));
        if (!k) k="Unknown";
    }
    switch (key&k_Flags) {
    case 0:
        snprintf(buffer,bufferlen,"key%s",k);
        break;
    case k_Repeat:
        snprintf(buffer,bufferlen,"key%s|Repeat",k);
        break;
    case k_Release:
        snprintf(buffer,bufferlen,"key%s|Release",k);
        break;
    case k_Repeat|k_Release:
        snprintf(buffer,bufferlen,"key%s|Repeat|Release",k);
        break;
    }
    return buffer;
}

int cConnection::ParseKey(const char *str) {
    eKeys Key;
    char keyName[20];
    unsigned int i;
    
    if (strncmp(str, "key",3) != 0)
        return -1;

    str+=3;
    
    i=0;
    while (i < sizeof(keyName) && *str != 0 && *str != '|')
        keyName[i++] = *str++;
    if (i>=sizeof(keyName))  
        return -1;
    keyName[i] = 0;    
    
    Key = cKey::FromString(keyName);
    if (Key == kNone && strcmp(keyName, "None") != 0)
        return -1;
        
    if (*str == '|') {
        str++;
        if (strcmp(str, "Repeat") == 0)
            Key = eKeys(Key|k_Repeat);
        else if (strcmp(str, "Release") == 0)
            Key = eKeys(Key|k_Release);
        else if (strcmp(str, "Repeat|Release") == 0)
            Key = eKeys(Key|k_Repeat|k_Release);
        else
            return -1;
    }                      
    return Key;
}      

// -------------------------------
//   cCommandProcessor::cCommand
// -------------------------------

cCommandProcessor::cCommand::cCommand() {
    Command=NULL;
    Context=NULL;
    AssignVar=NULL;
}

bool cCommandProcessor::cCommand::ParseLine(cConnection &Connection, char *line) {
    char *p,*ps;

    // Reset parameters
    Command=NULL;
    Context=NULL;
    AssignVar=NULL;
    args.clear();

    // Parse one identifier
    line=skipspace(line);
    if (*line==0 || *line=='#') return false;

    p=skipspace(ps=skipword(line));
    if (!p) {
        Connection.ReplyErrorIdentifier(line);
        return false;
    }

    if (*p=='=') {
        // Assignment found
        *ps=0;
        AssignVar=line;
        if (*line=='_') {
            Connection.ReplyErrorIdentifierReserved(line);
            return false;
        }
        // parse next word
        line=skipspace(p+1);
        p=skipspace(ps=skipword(line));
        if (!p) {
            Connection.ReplyErrorIdentifier(line);
            return false;
        }
    }

    if (*p=='.') {
        // Found member operator, parse member sequence and command
        char *lastdot;

        while (*p=='.') {
            // Loop a sequence of identifer.identifier.identifier.command
            lastdot=p;
            p=skipspace(ps=skipword(skipspace(p+1)));
            if (!p) {
                Connection.ReplyErrorIdentifier(line);
                return false;
            }
        }
        // Remember whole sequence up to before last dot
        *lastdot=0;
        Context=line;
        despace(Context);
        // Parse next word
        line=skipspace(lastdot+1);
        p=skipspace(ps=skipword(line));
        if (!p) {
            Connection.ReplyErrorIdentifier(line);
            return false;
        }
    }

    // Now parsed word is a command

    if (!isspace(*ps) && *ps!=0) {
        Connection.ReplyErrorIdentifier(line);
        return false;
    }
    *ps=0;

    Command=line;
    bool lastflag=false;

    while (*p!=0) {
        char *q=p;
        bool singlequote=false;
        bool doublequote=false;

        line=p;

        while (*p!=0 && (!isspace(*p) || singlequote || doublequote)) {
            switch (*p) {
            case '\\':
                p++;
                if (*p=='r') *q++='\r';
                if (*p=='n') *q++='\n';
                if (*p=='t') *q++='\t';
                if (*p=='"') *q++='"';
                if (*p=='\'') *q++='\'';
                if (*p=='\\') *q++='\\';
                break;
            case '"':
                if (singlequote)
                    *q++='"';
                else
                    doublequote=!doublequote;
                break;
            case '\'':
                if (doublequote)
                    *q++='\'';
                else
                    singlequote=!singlequote;
                break;
            default:
                *q++=*p;
            }
            p++;
        }
        p=skipspace(p);
        *q=0;
        if (!lastflag && 0==strcmp(line,"--")) {
            // Marker for end-of-flags reached
            lastflag=true;
            // ignore this parameter and dont increase argflagc from now on
            continue;
        }
        
        bool IsFlag = !lastflag && line[0]=='-';

        args.push_back(cArg(line,IsFlag));
    }

    return true;
}

void cCommandProcessor::cCommand::DropArgs(int start, int count) {
    args.erase(args.begin()+start,args.begin()+start+count);
}

bool cCommandProcessor::cCommand::ArgScanFlag(const char *flag) {
    int pos=0;
    args_iterator i;
    for (i=args.begin(); i!=args.end(); i++) {
        if (i->IsFlag() && 0==strcasecmp(flag,i->Text()+1)) break;
        pos++;
    }
    if (i==args.end()) return false;

    args.erase(i);
    return true;
}

const char* cCommandProcessor::cCommand::ArgScanFlagParam(const char *flag) {
    int pos=0;
    args_iterator i;
    for (i=args.begin(); i!=args.end(); i++) {
        if (i->IsFlag() && 0==strcasecmp(flag,i->Text()+1)) break;
        pos++;
    }
    if (i==args.end()) return NULL;

    const char *result=(i+1 != args.end()) ? ((i+1)->Text()) : (NULL);

    args.erase(i,i+2);
    return result;
}




// ---------------------
//   cCommandProcessor
// ---------------------


cCommandProcessor::cCommandProcessor(cConnection *connection) {
    Connection=connection;
    State=stateNew;
    SleepEventObj=NULL;
    SleepEventTimeout.tv_sec=0;
    SleepEventTimeout.tv_usec=0;
    LocalContext = &BaseContext;
}

cCommandProcessor::~cCommandProcessor() {
    if (Connection) delete Connection;
}


cOsdServerMenu* cCommandProcessor::NewMenu(cCommand &cmd) {
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return NULL;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return NULL;
    }

    cOsdServerMenu *menu = new cOsdServerMenu(cmd.GetArg(0));
    LocalContext->Add(menu, cmd.GetAssignVar());
    return menu;
}

cOsdServerOsdItem* cCommandProcessor::NewOsdItem(cCommand &cmd) {
    bool Unselectable = cmd.ArgScanFlag("unselectable");

    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return NULL;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return NULL;
    }

    cOsdServerOsdItem *item=new cOsdServerOsdItem(cmd.GetArg(0));
    if (Unselectable) item->SetSelectable(false);
    LocalContext->Add(item, cmd.GetAssignVar());

    return item;
}

cOsdServerMenuEditStrItem* cCommandProcessor::NewEditStrItem(cCommand &cmd) {
    if (!cmd.HasMaxArgument(2)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(2));
        return NULL;
    }
    if (!cmd.HasMinArgument(2)) {
        Connection->ReplyErrorMissingParameter();
        return NULL;
    }

    cOsdServerMenuEditStrItem *item=new cOsdServerMenuEditStrItem(cmd.GetArg(0), cmd.GetArg(1), 255);
    LocalContext->Add(item, cmd.GetAssignVar());
    return item;
}

cOsdServerMenuEditIntItem* cCommandProcessor::NewEditIntItem(cCommand &cmd) {
    const char *MinString = cmd.ArgScanFlagParam("MinString");
    const char *MaxString = cmd.ArgScanFlagParam("MaxString");
    int max;
    int min;
    const char *Max = cmd.ArgScanFlagParam("Max");
    const char *Min = cmd.ArgScanFlagParam("Min");
    
    if (!Max) {
        max = INT_MAX;
    } else if (strcasecmp(Max,"none")==0) {
        max = INT_MAX;
    } else {
        char *p;
        max = strtol(Max, &p, 10);
        if (*p != 0) {
            Connection->ReplyErrorParameterInt(Max);
            return NULL;
        }
    }
        
    if (!Min) {
        min = 0;
    } else if (strcasecmp(Min,"none")==0) {
        min = INT_MIN;
    } else {
        char *p;
        min = strtol(Min, &p, 10);
        if (*p != 0) {
            Connection->ReplyErrorParameterInt(Min);
            return NULL;
        }
    }
        
    if (!cmd.HasMaxArgument(2)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(2));
        return NULL;
    }
    if (!cmd.HasMinArgument(2)) {
        Connection->ReplyErrorMissingParameter();
        return NULL;
    }

    char *p;
    int value = strtol(cmd.GetArg(1), &p, 10);
    if (*p != 0) {
        Connection->ReplyErrorParameterInt(cmd.GetArg(1));
        return NULL;
    }

    cOsdServerMenuEditIntItem *item=new cOsdServerMenuEditIntItem(cmd.GetArg(0), value, min, max, MinString, MaxString);
    LocalContext->Add(item, cmd.GetAssignVar());
    return item;
}

cOsdServerMenuEditListItem* cCommandProcessor::NewEditListItem(cCommand &cmd) {
    const char *Name = cmd.ArgScanFlagParam("SelectName");
    const char *Number = cmd.ArgScanFlagParam("Select");
    int value = 0;
    
    if (Name) {
        if (Number) {
            Connection->ReplyErrorConflictingParameters("SelectName","Select");
            return NULL;
        }
        value = -1;
    } else if (Number) {
        char *p;
        value = strtol(Number, &p, 10) - 1;
        if (*p != 0) {
            Connection->ReplyErrorParameterInt(Number);
            return NULL;
        }
    }

    if (!cmd.HasMinArgument(2)) {
        Connection->ReplyErrorMissingParameter();
        return NULL;
    }
    int max = cmd.GetArgCount()-1;
    
    const char *(strings[max]);
    
    for (int i=0; i<max; i++) {
        strings[i] = cmd.GetArg(i+1);
        if (Name && value<0 && strcasecmp(strings[i], Name)==0)
            value = i;            
    }
    
    if (Name && value < 0) {
        Connection->ReplyErrorParameterInvalid(Name);
        return NULL;
    }
    if (Number && (value>=max ||value<0)) {
        Connection->ReplyErrorParameterRange(Number);
        return NULL;
    }        
        
    cOsdServerMenuEditListItem *item=new cOsdServerMenuEditListItem(cmd.GetArg(0), value, max, strings);
    LocalContext->Add(item, cmd.GetAssignVar());
    return item;
}

cOsdServerObject* cCommandProcessor::NewObject(cCommand &cmd)
{
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return NULL;
    }

    if (0 == strcasecmp(cmd.GetArg(0), "MENU")) {
        cmd.DropArgs(0,1);
        return NewMenu(cmd);
    } else if (0 == strcasecmp(cmd.GetArg(0), "OSDITEM")) {
        cmd.DropArgs(0,1);
        return NewOsdItem(cmd);
    } else if (0 == strcasecmp(cmd.GetArg(0), "EditStrItem")) {
        cmd.DropArgs(0,1);
        return NewEditStrItem(cmd);
    } else if (0 == strcasecmp(cmd.GetArg(0), "EditIntItem")) {
        cmd.DropArgs(0,1);
        return NewEditIntItem(cmd);
    } else if (0 == strcasecmp(cmd.GetArg(0), "EditListItem")) {
        cmd.DropArgs(0,1);
        return NewEditListItem(cmd);
    }
    Connection->ReplyErrorUnknownType(cmd.GetArg(0));
    return NULL;
}

bool cCommandProcessor::CmdNew(cCommand &cmd) {
    cOsdServerObject *obj = NewObject(cmd);       
    
    return (obj != NULL);
}

bool cCommandProcessor::CmdDelete(cCommand &cmd) {
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    cOsdServerObject *obj = LocalContext->GetName(cmd.GetArg(0));
    if (!obj) {
        Connection->ReplyErrorUnknownObject(cmd.GetArg(0));
        return false;
    }
    
    delete obj;
    return true;
}

bool cCommandProcessor::CmdObjectDelete(cCommand &cmd, cOsdServerObject *osdobj) {
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }

    delete osdobj;
    return true;
}
    
bool cCommandProcessor::CmdVersion(cCommand &cmd) {
    if (cmd.GetContext()) {
        Connection->ReplyErrorUnknownCommand(cmd.GetContext());
        return false;
    }
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    const char *verstr = cmd.GetArg(0);

    unsigned int major,minor;
    int len=0;
    if (sscanf(verstr, "%u.%u%n", &major, &minor, &len) < 2) {
        // Not matched both numbers
        Connection->ReplyErrorUnknownVersion(verstr);
        return false;        
    }
    if (verstr[len] != 0) {
        // Not parsed to end of string
        Connection->ReplyErrorUnknownVersion(verstr);
        return false;        
    }
    
    if (major == 0 && minor <= 1) {
        return true;
    }
        
    Connection->ReplyErrorUnknownVersion(verstr);
    return false;
}


bool cCommandProcessor::CmdQuit(cCommand &cmd) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }
    Connection->ReplyGoodBye();
    return true;
}

bool cCommandProcessor::CmdIdle(cCommand &cmd) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }
    return true;
}

bool cCommandProcessor::CmdMessage(cCommand &cmd) {
    /*
    MESSAGE [-info|-warn|-error] -queue [-timeout #] [-seconds #] message
    MESSAGE [-info|-warn|-error] [-seconds #] message
    MESSAGE -status message
    MESSAGE -statusclear
    */

    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }

    enum eMode { status, info, warn, error, statusclear }
        mode=info;

    int seconds=0;
    int timeout=0;

    bool queue=cmd.ArgScanFlag("queue");

    const char *cseconds=cmd.ArgScanFlagParam("seconds");
    if (cseconds) seconds=atoi(cseconds);

    const char *ctimeout=cmd.ArgScanFlagParam("timeout");
    if (ctimeout) timeout=atoi(ctimeout);

    if (ctimeout && !queue) {
        Connection->ReplyErrorParameter("-timeout requires -queue");
        return false;
    }


    int ArgCount=1;

    if (cmd.ArgScanFlag("status")) {
        mode=status;
        if (cseconds || queue) {
            Connection->ReplyErrorParameter("-status doesnt accept -queue or -seconds");
            return false;
        }
    } else if (cmd.ArgScanFlag("info")) {
        mode=info;
    } else if (cmd.ArgScanFlag("warn")) {
        mode=warn;
    } else if (cmd.ArgScanFlag("error")) {
        mode=error;
    } else if (cmd.ArgScanFlag("statusclear")) {
        mode=statusclear;
        if (cseconds || queue) {
            Connection->ReplyErrorParameter("-statusclear doesnt accept -queue or -seconds");
            return false;
        }
        ArgCount=0;
    }

    if (!cmd.HasMaxArgument(ArgCount)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(ArgCount));
        return false;
    }
    if (!cmd.HasMinArgument(ArgCount)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    if (queue) {
        int k=kNone;
        switch (mode) {
        case info:
            k=Skins.QueueMessage(mtInfo,cmd.GetArg(0),seconds,timeout);
            break;
        case warn:
            k=Skins.QueueMessage(mtWarning,cmd.GetArg(0),seconds,timeout);
            break;
        case error:
            k=Skins.QueueMessage(mtError,cmd.GetArg(0),seconds,timeout);
            break;
        default:;
        }

        if (timeout!=0) {
            Connection->ReplyMessageKeyEvent("Message", (eKeys)k);
        } else {
            Connection->ReplyMessageEventTimeout();
        }
    } else {
        int k=kNone;
        cMainThreadLock Lock;

        Lock.LockMainThread();
        switch (mode) {
        case status:
            k=Skins.Message(mtStatus,cmd.GetArg(0),0);
            break;
        case statusclear:
            k=Skins.Message(mtStatus,NULL,0);
            break;
        case info:
            k=Skins.Message(mtInfo,cmd.GetArg(0),seconds);
            break;
        case warn:
            k=Skins.Message(mtWarning,cmd.GetArg(0),seconds);
            break;
        case error:
            k=Skins.Message(mtError,cmd.GetArg(0),seconds);
            break;
        }
        Lock.UnlockMainThread();

        if (mode!=status && mode!=statusclear) {
            if (k != -1)
                Connection->ReplyMessageKeyEvent("Message",(eKeys)k);
            else
                Connection->ReplyMessageEventTimeout();
        }
    }
    return true;
}

bool cCommandProcessor::CmdEnterLocal(cCommand &cmd) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }
    LocalContext = LocalContext->MakeLocalContext();
    return true;
}

bool cCommandProcessor::CmdLeaveLocal(cCommand &cmd) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }
    if (!LocalContext->GetParentContext()) {
        Connection->ReplyErrorNoParentContext();
        return false;
    }
    LocalContext = LocalContext->GetParentContext();
    LocalContext->DestroyLocalContext();
    return true;
}

bool cCommandProcessor::CmdMenuAdd(cCommand &cmd, cOsdServerMenu *menu) {
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    cOsdServerObject *obj = LocalContext->GetName(cmd.GetArg(0));
    if (!obj) {
        Connection->ReplyErrorUnknownObject(cmd.GetArg(0));
        return false;
    }
    
    if (!obj->IsMenuItem()) {
        Connection->ReplyErrorIncompatibleType(cmd.GetArg(0));
        return false;
    }
    menu->Add((cOsdServerMenuItem*)obj);
    return true;
}

bool cCommandProcessor::CmdMenuAddNew(cCommand &cmd, cOsdServerMenu *menu) {
    cOsdServerObject *obj = NewObject(cmd);
    if (!obj) return false;
    
    if (!obj->IsMenuItem()) {
        Connection->ReplyErrorIncompatibleType(cmd.GetArg(0));
        return false;
    }
    menu->Add((cOsdServerMenuItem*)obj);
    return true;
}

bool cCommandProcessor::CmdMenuAddSubMenu(cCommand &cmd, cOsdServerMenu *menu) {
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }
    cOsdServerObject *obj = LocalContext->GetName(cmd.GetArg(0));
    if (!obj) {
        Connection->ReplyErrorUnknownObject(cmd.GetArg(0));
        return false;
    }
    
    if (obj->ClassId() != cOsdServerObject::clsidMenu) {
        Connection->ReplyErrorIncompatibleType(cmd.GetArg(0));
        return false;
    }
    menu->AddSubMenu(obj->Cast<cOsdServerMenu>());
    return true;
}

bool cCommandProcessor::CmdMenuShow(cCommand &cmd, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    menu->Show();
    return true;
}

bool cCommandProcessor::CmdMenuSendState(cCommand &cmd, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    if (strcmp(cmd.GetArg(0),"osEnd") == 0) {
        menu->SendOSState(osEnd);
    } else if (strcmp(cmd.GetArg(0),"osBack") == 0) {
        menu->SendOSState(osBack);
    } else if (strcmp(cmd.GetArg(0),"osContinue") == 0) {
        menu->SendOSState(osContinue);
    } else {
        Connection->ReplyErrorUnknownState(cmd.GetArg(0));
        return false;
    }
    return true;
}

bool cCommandProcessor::CmdMenuSleepEvent(cCommand &cmd, int &timeout, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }

    SleepEventTimeout.tv_sec=0; // means infinite
    const char *par=cmd.ArgScanFlagParam("timeout");
    if (par) {
        gettimeofday(&SleepEventTimeout,NULL);
        SleepEventTimeout.tv_sec+=atoi(par);
    } else {
        const char *par=cmd.ArgScanFlagParam("timeoutms");
        if (par) {
            gettimeofday(&SleepEventTimeout,NULL);
            int t=atoi(par);
            SleepEventTimeout.tv_sec+=t/1000;
            SleepEventTimeout.tv_usec+=1000*(t%1000);
            if (SleepEventTimeout.tv_usec>=1000000) {
                SleepEventTimeout.tv_usec-=1000000;
                SleepEventTimeout.tv_sec++;
            }
        }
    }

    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    SleepEventObj=menu;

    if (HandleSleepEvent(timeout)) {
        return false;
    }
    State=stateSleepEvent;
    return true;
}

bool cCommandProcessor::CmdMenuGetCurrent(cCommand &cmd, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    int current;
    cString name;
    menu->GetCurrent(current,name);
    Connection->ReplyMessageMenuCurrent(current,name);
    return true;
}

bool cCommandProcessor::CmdMenuSetCurrent(cCommand &cmd, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    int NewCurrent=atoi(cmd.GetArg(0));

    menu->SetCurrent(NewCurrent);
    return true;
}

bool cCommandProcessor::CmdMenuSetColorKeyText(cCommand &cmd, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }

    const char *Red   =cmd.ArgScanFlagParam("red");
    const char *Green =cmd.ArgScanFlagParam("green");
    const char *Yellow=cmd.ArgScanFlagParam("yellow");
    const char *Blue  =cmd.ArgScanFlagParam("blue");

    for (int i=0;cmd.GetArg(i);i++) {
        if (!Red) {
            Red = cmd.GetArg(i);
        } else if (!Green) {
            Green = cmd.GetArg(i);
        } else if (!Yellow) {
            Yellow = cmd.GetArg(i);
        } else if (!Blue) {
            Blue = cmd.GetArg(i);
        } else {
            Connection->ReplyErrorColorKeyAlreadySet();
            return false;
        }
    }
    if (Red   ) menu->SetColorKeyRed   (*Red   !=0 ? Red    : NULL);
    if (Green ) menu->SetColorKeyGreen (*Green !=0 ? Green  : NULL);
    if (Yellow) menu->SetColorKeyYellow(*Yellow!=0 ? Yellow : NULL);
    if (Blue  ) menu->SetColorKeyBlue  (*Blue  !=0 ? Blue   : NULL);
    return true;
}

bool cCommandProcessor::CmdMenuSetColumns(cCommand &cmd, cOsdServerMenu *menu) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(5)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(5));
        return false;
    }
    int i;
    for (i=0;cmd.GetArg(i);i++) {
        int res;
        if (!cmd.GetArgInt(i,res)) {
            Connection->ReplyErrorParameterInt(cmd.GetArg(i)); 
            return false;
        }
        menu->SetColumn(i,res);
    }
    for (;i<5;i++) {
        menu->SetColumn(i,0);
    }
    return true;
}

bool cCommandProcessor::CmdItemSetCurrent(cCommand &cmd, cOsdServerMenuItem *menuitem) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    cOsdServerMenu *menu=menuitem->GetMenu();
    int NewCurrent=menuitem->Index();

    menu->SetCurrent(NewCurrent);
    return true;
}

bool cCommandProcessor::CmdItemSetSelectable(cCommand &cmd, cOsdServerOsdItem *osditem, bool Selectable) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    osditem->SetSelectable(Selectable);
    return true;
}

bool cCommandProcessor::CmdItemSetText(cCommand &cmd, cOsdServerOsdItem *osditem) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    if (!cmd.HasMinArgument(1)) {
        Connection->ReplyErrorMissingParameter();
        return false;
    }
    if (!cmd.HasMaxArgument(1)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(1));
        return false;
    }

    osditem->SetText(cmd.GetArg(0));
    return true;
}

bool cCommandProcessor::CmdEditStrItemGetValue(cCommand &cmd, cOsdServerMenuEditStrItem *editobj) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }
    bool quoted=cmd.ArgScanFlag("Quoted");

    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    char *value = editobj->GetValue();
    
    Connection->ReplyTextGetValue(quoted, value);
    
    free(value);
    return true;
}

bool cCommandProcessor::CmdEditIntItemGetValue(cCommand &cmd, cOsdServerMenuEditIntItem *editobj) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }

    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    int value = editobj->GetValue();

    char str[20];
    sprintf(str, "%i", value);
    Connection->ReplyTextGetValue(false, str);
    
    return true;
}

bool cCommandProcessor::CmdEditListItemGetValue(cCommand &cmd, cOsdServerMenuEditListItem *editobj) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }

    bool Name = cmd.ArgScanFlag("Name");
    bool quoted=cmd.ArgScanFlag("Quoted");

    if (!cmd.HasMaxArgument(0)) {
        Connection->ReplyErrorUnknownParameter(cmd.GetArg(0));
        return false;
    }

    if (Name) {
        char *value = editobj->GetValueName();

        Connection->ReplyTextGetValue(quoted, value);
        free(value);
    } else {
        int value = editobj->GetValue();

        char str[20];
        sprintf(str, "%i", value+1);
        Connection->ReplyTextGetValue(quoted, str);
    }
    
    return true;
}

bool cCommandProcessor::CmdObjectEnableEvent(cCommand &cmd, cOsdServerObject *obj) {
    if (cmd.IsAssignment()) {
        Connection->ReplyErrorAssign();
        return false;
    }

    for (int i=0;cmd.GetArg(i);i++) {
        int key = cConnection::ParseKey(cmd.GetArg(i));
        if (key >= 0) {
            obj->EnableEvent((eKeys)key);
        } else if (0==strcmp(cmd.GetArg(i),"close")) {
            obj->EnableEvent(cOsdServerObject::cEvent::CloseEvent);
        } else if (0==strcmp(cmd.GetArg(i),"focus")) {
            obj->EnableEvent(cOsdServerObject::cEvent::FocusEvent);
        } else if (0==strcmp(cmd.GetArg(i),"blur")) {
            obj->EnableEvent(cOsdServerObject::cEvent::BlurEvent);
        } else if (0==strcmp(cmd.GetArg(i),"edit")) {
            obj->EnableEvent(cOsdServerObject::cEvent::EditEvent);
        } else {
            Connection->ReplyErrorUnknownEvent(cmd.GetArg(i));
        }
    }
    return true;
}



// Context based command dispatchers



bool cCommandProcessor::ProcessGlobalCommand(cCommand &cmd, int &timeout) {
    if (cmd.IsCommand("QUIT")) {
        if (CmdQuit(cmd)) {
            State=stateTerminated;
            return true;
        }
    } else if (cmd.IsCommand("IDLE")) {
        CmdIdle(cmd);
    } else if (cmd.IsCommand("MESSAGE")) {
        CmdMessage(cmd);
    } else if (cmd.IsCommand("NEW")) {
        CmdNew(cmd);
    } else if (cmd.IsCommand("DELETE")) {
        CmdDelete(cmd);
    } else if (cmd.IsCommand("ENTERLOCAL")) {
        CmdEnterLocal(cmd);
    } else if (cmd.IsCommand("LEAVELOCAL")) {
        CmdLeaveLocal(cmd);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessMenuCommand(cCommand &cmd, int &timeout, cOsdServerMenu *menu) {
    if (cmd.IsCommand("GETCURRENT")) {
        CmdMenuGetCurrent(cmd,menu);
    } else if (cmd.IsCommand("SETCURRENT")) {
        CmdMenuSetCurrent(cmd,menu);
    } else if (cmd.IsCommand("ADD")) {
        CmdMenuAdd(cmd,menu);
    } else if (cmd.IsCommand("ADDNEW")) {
        CmdMenuAddNew(cmd,menu);
    } else if (cmd.IsCommand("ADDSUBMENU")) {
        CmdMenuAddSubMenu(cmd,menu);
    } else if (cmd.IsCommand("SHOW")) {
        CmdMenuShow(cmd,menu);
    } else if (cmd.IsCommand("SENDSTATE")) {
        CmdMenuSendState(cmd,menu);
    } else if (cmd.IsCommand("SETCOLORKEYTEXT")) {
        CmdMenuSetColorKeyText(cmd,menu);
    } else if (cmd.IsCommand("SETCOLUMNS")) {
        CmdMenuSetColumns(cmd,menu);
    } else if (cmd.IsCommand("SLEEPEVENT")) {
        if (CmdMenuSleepEvent(cmd,timeout,menu)) {
            return true;
        }
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessMenuItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuItem *menuitem) {
    if (cmd.IsCommand("SETCURRENT")) {
        CmdItemSetCurrent(cmd,menuitem);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessOsdItemCommand(cCommand &cmd, int &timeout, cOsdServerOsdItem *osditem) {
    if (cmd.IsCommand("SETSELECTABLE")) {
        CmdItemSetSelectable(cmd,osditem,true);
    } else if (cmd.IsCommand("SETUNSELECTABLE")) {
        CmdItemSetSelectable(cmd,osditem,false);
    } else if (cmd.IsCommand("SETTEXT")) {
        CmdItemSetText(cmd,osditem);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessEditStrItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuEditStrItem *edititem) {
    if (cmd.IsCommand("GETVALUE")) {
        CmdEditStrItemGetValue(cmd,edititem);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessEditIntItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuEditIntItem *edititem) {
    if (cmd.IsCommand("GETVALUE")) {
        CmdEditIntItemGetValue(cmd,edititem);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessEditListItemCommand(cCommand &cmd, int &timeout, cOsdServerMenuEditListItem *edititem) {
    if (cmd.IsCommand("GETVALUE")) {
        CmdEditListItemGetValue(cmd,edititem);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessOsdObjectCommand(cCommand &cmd, int &timeout, cOsdServerObject *osdobj) {
    if (cmd.IsCommand("ENABLEEVENT")) {
        CmdObjectEnableEvent(cmd,osdobj);
    } else if (cmd.IsCommand("DELETE")) {
        CmdObjectDelete(cmd,osdobj);
    } else {
        return false;
    }

    Connection->ReplyReady();
    return true;
}

bool cCommandProcessor::ProcessCommand(cCommand &cmd, int &timeout) {
    if (!cmd.GetContext()) {
        return ProcessGlobalCommand(cmd,timeout);
    }

    cOsdServerObject *ContextObj=LocalContext->GetName(cmd.GetContext());
    if (!ContextObj) {
        Connection->ReplyErrorUnknownObject(cmd.GetContext());
        Connection->ReplyReady();
        return true;
    }

    if (ProcessOsdObjectCommand(cmd,timeout,ContextObj)) return true;
        
    if (ContextObj->IsMenuItem()) {
        if (ProcessMenuItemCommand(cmd,timeout,(cOsdServerMenuItem*)ContextObj)) return true;
    }
        
    switch (ContextObj->ClassId()) {
    case cOsdServerObject::clsidMenu:
        if (ProcessMenuCommand(cmd,timeout,ContextObj->Cast<cOsdServerMenu>())) return true;
        break;
    case cOsdServerObject::clsidOsdItem:
        if (ProcessOsdItemCommand(cmd,timeout,ContextObj->Cast<cOsdServerOsdItem>())) return true;
        break;
    case cOsdServerObject::clsidMenuEditStrItem:
        if (ProcessEditStrItemCommand(cmd,timeout,ContextObj->Cast<cOsdServerMenuEditStrItem>())) return true;
        break;
    case cOsdServerObject::clsidMenuEditIntItem:
        if (ProcessEditIntItemCommand(cmd,timeout,ContextObj->Cast<cOsdServerMenuEditIntItem>())) return true;
        break;
    case cOsdServerObject::clsidMenuEditListItem:
        if (ProcessEditListItemCommand(cmd,timeout,ContextObj->Cast<cOsdServerMenuEditListItem>())) return true;
        break;
    }

    return false;
}


void cCommandProcessor::ProcessWelcome(cCommand &cmd, int &timeout) {
    if (cmd.IsCommand("QUIT")) {
        if (CmdQuit(cmd)) {
            State=stateTerminated;
            return;
        }
    } else if (cmd.IsCommand("VERSION")) {
        if (CmdVersion(cmd)) {
            State=stateReady;
            Connection->ReplyReady();
            return;
        }
    } else {
        Connection->ReplyErrorMissingVersion();
    }

    Connection->ReplyWelcome();
}

void cCommandProcessor::ProcessUnknown(cCommand &cmd) {
    if (cmd.GetCommand()) Connection->ReplyDebug(100, "Unknown Command: \"%s\"",cmd.GetCommand());
    if (cmd.GetContext()) Connection->ReplyDebug(100, "Context: \"%s\"",cmd.GetContext());
    if (cmd.GetAssignVar()) Connection->ReplyDebug(100, "Assignment to: \"%s\"",cmd.GetAssignVar());

    for (int i=0;cmd.GetArg(i);i++) {
        Connection->ReplyDebug(100, "%s %i: \"%s\"",cmd.IsArgFlag(i)?"Arg/Flag":"Argument",i,cmd.GetArg(i));
    }
    Connection->ReplyErrorUnknownCommand(cmd.GetCommand()?cmd.GetCommand():"");
    Connection->ReplyReady();
}



bool cCommandProcessor::HandleSleepEvent(int &timeout) {
    cOsdServerObject::cTriggeredEvent event;

    if (SleepEventObj->PollEvent(event)) {
        switch (event.type) {
        case cOsdServerObject::cEvent::KeyEvent:
            Connection->ReplyMessageKeyEvent(event.source->Name(), event.key);
            return true;
        case cOsdServerObject::cEvent::CloseEvent:
            Connection->ReplyMessageEvent(event.source->Name(), "close");
            return true;
        case cOsdServerObject::cEvent::EditEvent:
            Connection->ReplyMessageEvent(event.source->Name(), "edit");
            return true;
        case cOsdServerObject::cEvent::FocusEvent:
            Connection->ReplyMessageEvent(event.source->Name(), "focus");
            return true;
        case cOsdServerObject::cEvent::BlurEvent:
            Connection->ReplyMessageEvent(event.source->Name(), "blur");
            return true;
        case cOsdServerObject::cEvent::NullEvent: ;
        }
    }

    if (SleepEventObj->IsDetached()) {
        Connection->ReplyErrorObjectNotActive();
        return true;
    }

    if (SleepEventTimeout.tv_sec==0) {
        timeout=100;
        return false;
    }

    timeval t;
    gettimeofday(&t,NULL);
    int ms=1000*(SleepEventTimeout.tv_sec-t.tv_sec)
           +(SleepEventTimeout.tv_usec-t.tv_usec)/1000;

    if (ms<=0) {
        Connection->ReplyMessageEventTimeout();
        return true;
    }

    timeout=min(ms,100);
    return false;
}


// Main command handler


bool cCommandProcessor::Action(cSelect &Selector) {

    for (;;) {
        int timeout=0;
        char *line;
        cCommand cmd;

        if (!Connection->IsConnected()) {
            State=stateTerminated;
        }

        switch (State) {
        case stateNew:
            Connection->ReplyWelcome();
            State=stateWelcome;
            break;

        case stateTerminated:
            return false;

        case stateSleepEvent:
            if (HandleSleepEvent(timeout)) {
                State=stateReady;
                Connection->ReplyReady();
            }
            break;

        case stateReady:
            line=Connection->ReadLine();
            if (!line) {
                Connection->AddSelect(Selector);
                return true;
            }
            if (cmd.ParseLine(*Connection,line)) {
                if (!ProcessCommand(cmd,timeout)) ProcessUnknown(cmd);
            }
            Connection->DelLine();
            break;

        case stateWelcome:
            line=Connection->ReadLine();
            if (!line) {
                Connection->AddSelect(Selector);
                return true;
            }
            if (cmd.ParseLine(*Connection,line)) {
                ProcessWelcome(cmd,timeout);
            }
            Connection->DelLine();
            break;
        } // end switch (State)

        if (timeout) {
            Selector.SetMinTimeoutMs(timeout);
            return true;
        }
    } // end for (;;)
    return false; // never reached
}

