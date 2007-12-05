/*
 * rules.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _RULES_H_
#define _RULES_H_

#include <vdr/tools.h>

class cEvent;
class cTimer;

class cMatch {

  private:
    cString m_Folder;
    cString m_Name;
    //cString m_Result;
    int     m_Priority;

    cMatch();
    cMatch(cMatch&);

  protected:

    cMatch(const cString& Folder, const cString& Name, int Priority) :
        m_Folder(Folder), m_Name(Name), m_Priority(Priority) {};

    friend class cRule;

    cEvent *m_Event;

  public:
    int            Priority(void) const { return m_Priority; }
    const cString& Name(void)     const { return m_Name; }
    const cString& Folder(void)   const { return m_Folder; }

    cTimer *CreateTimer(void) const; 
};

class cRule : public cListObject {

  private:
    static cRule *Parse(char **pText);

    static int     g_Priority;
    static cString g_Folder;
    static cString g_Name;

  private:
    cRule(cRule&);

  protected:
    int     m_Priority;
    cString m_Folder;
    cString m_Name;

  protected:
    cRule() : m_Priority(-1) {}

    bool   Match(bool Result) const;
 public:
    virtual bool Matches(cEvent *Event) const = 0;
    virtual cString ToText(int ident) const = 0;

  public:
    virtual ~cRule() {}

    cTimer *XXXMatches(cEvent *Event) const;
    bool    Matches(cEvent *Event, cString& Name, int& Priority) const;

    cString ToText(void) const;

    static cRule *Parse(const char *Text);
    static cRule *RuleFromFile(const char *File);
};

#endif // _RULES_H_
