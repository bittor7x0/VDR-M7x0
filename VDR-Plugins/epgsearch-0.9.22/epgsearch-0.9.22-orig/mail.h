#ifndef __EPGSEARCH_MAIL_H
#define __EPGSEARCH_MAIL_H

#include <string>
#include <set>

#include <vdr/thread.h>
#include <vdr/plugin.h>
#include "conflictcheck.h"

using std::string;
using std::set;

// --- cMailNotifier --------------------------------------------------------
class cMailNotifier 
{
 protected:
    string subject;
    string body;

    bool SendMailViaSendmail();
    bool SendMailViaScript();
    bool SendMail();
    bool ExecuteMailScript(string ScriptArgs);
 public:
    string scriptReply;

    cMailNotifier() {}
    cMailNotifier(string Subject, string Body);
    bool TestMailAccount(string MailAddressTo, string MailAddress, string MailServer, string AuthUser, string AuthPass);
    static string LoadTemplate(const string& templtype);
    static string GetTemplValue(const string& templ, const string& entry);

    static string MailCmd;
};

class cMailTimerNotification
{
    tEventID eventID;
    tChannelID channelID;

    const cEvent* GetEvent() const;

 public:
    cMailTimerNotification(tEventID EventID, tChannelID ChannelID) 
	: eventID(EventID), channelID(ChannelID) {}
    bool operator< (const cMailTimerNotification &N) const;
    string Format(const string& templ) const;
};

class cMailDelTimerNotification
{
    tChannelID channelID;
    time_t start;
 public:
    string formatted;

    cMailDelTimerNotification(cTimer* t, const cEvent* pEvent, const string& templ); 
    bool operator< (const cMailDelTimerNotification &N) const;
};

class cMailUpdateNotifier : public cMailNotifier
{
    set<cMailTimerNotification> newTimers;
    set<cMailTimerNotification> modTimers;
    set<cMailDelTimerNotification> delTimers;

    string mailTemplate;
 public:
    cMailUpdateNotifier();
    void AddNewTimerNotification(tEventID EventID, tChannelID ChannelID);
    void AddModTimerNotification(tEventID EventID, tChannelID ChannelID);
    void AddRemoveTimerNotification(cTimer* t, const cEvent* e = NULL);
    void SendUpdateNotifications();
};

class cMailConflictNotifier : public cMailNotifier
{
 public:
    void SendConflictNotifications(cConflictCheck& conflictcheck);
};

#endif
