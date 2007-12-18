/*
 * rules.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/epg.h>
#include <vdr/timers.h>

#include "rules.h"

extern bool g_autotimer_Silent; // autotimer.c
#define ISYSLOG(a...) void( (!g_autotimer_Silent) ? isyslog(a) : void() )


static char *Ident(int ident)
{
  static char str[1024];
  str[ident] = 0;
  while(ident) 
    str[--ident] = ' ';
  return str;
}

//--- cRule -----------------------------------------------------

int     cRule::g_Priority = -1;
cString cRule::g_Folder(NULL);
cString cRule::g_Name(NULL);

bool cRule::Match(bool Result) const
{
  if(Result) {
    if(m_Priority>=0 && g_Priority < 0) {
      g_Priority = m_Priority;
    }
    if(*m_Folder) {
      if(!*g_Folder) {
	g_Folder   = m_Folder;
      } else {
	char file[2048];
	snprintf(file, sizeof(file), "%s~%s", *m_Folder, *g_Folder);
	g_Folder = cString(file);
      }
    }
    if(*m_Name) {
      if(!*g_Name) {
	g_Name = m_Name;
      }
    }
  }
  return Result;
}

bool cRule::Matches(cEvent *Event, cString& Name, int& Priority) const
{
  g_Priority = -1;
  g_Folder = cString(NULL);
  g_Name = cString(NULL);

  if(Matches(Event)) {
    if(*g_Name) {
      if(strstr(g_Name, "%SUBTITLE%")) {
	g_Name = cString(strreplace(strdup(*g_Name), 
				    "%SUBTITLE%", 
				    Event->ShortText()), 
			 true);
      }
      if(strstr(g_Name, "%DESCRIPTION%")) {
	g_Name = cString(strreplace(strdup(*g_Name), 
				    "%DESCRIPTION%", 
				    Event->Description()), 
			 true);
      }
    } else {
      g_Name = cString(Event->Title());
    }
    Priority = (g_Priority >= 0) ? g_Priority : 99;
    if(*g_Folder)
      Name = cString::sprintf("%s~%s", *g_Folder, *g_Name);
    else
      Name = g_Name;
    return true;
  }
  return false;
}

cTimer *cRule::XXXMatches(cEvent *Event) const
{
  cTimer *timer;
  cString Name, Folder;
  int Priority;

  if(!Matches(Event, Folder, Priority))
    return NULL;

  timer = new cTimer(Event);

  if(Priority >= 0)
    timer->SetPriority(Priority);

  if(*Folder && (*Folder)[0]) {
    char file[2048];
    if(*Name && (*Name)[0])
      snprintf(file, sizeof(file), "%s~%s", *Folder, *Name);
    else
      snprintf(file, sizeof(file), "%s~%s", *Folder, Event->Title());
    timer->SetFile(file);
  }

  return timer;
}

cString cRule::ToText(void) const
{
//#warning  DODO: include folder, name, priority, ... ! and call this from derived rules.
  return ToText(0);
}

typedef cList<cRule> cRules;


//--- cRuleTitleSubstring ---------------------------------------

class cRuleTitleSubstring : public cRule 
{
  protected:
    cString m_Str;
  public:
    cRuleTitleSubstring(const char *String) : m_Str(String) {}
    virtual bool Matches(cEvent *Event) const {
      return Match(strstr(Event->Title(), *m_Str));
    }
    virtual cString ToText(int ident) const { 
      return cString::sprintf("%sTITLE CONTAINS \"%s\"", Ident(ident), *m_Str);
    }
};


//--- cRuleSubstring --------------------------------------------

class cRuleSubstring : public cRule 
{
  protected:
    cString m_Str;
  public:
    cRuleSubstring(const char *String) : m_Str(String) {}
    virtual bool Matches(cEvent *Event) const {
      return Match((Event->Title() &&       strstr(Event->Title(),       *m_Str)) ||
		   (Event->ShortText() &&   strstr(Event->ShortText(),   *m_Str)) ||
		   (Event->Description() && strstr(Event->Description(), *m_Str)));
    }
    virtual cString ToText(int ident) const { 
      return cString::sprintf("%sINFO CONTAINS \"%s\"", Ident(ident), *m_Str);
    }
};


//--- cRuleTitle ------------------------------------------------

class cRuleTitle : public cRule 
{
  protected:
    cString m_Str;
  public:
    cRuleTitle(const char *String) : m_Str(String) {}
    virtual bool Matches(cEvent *Event) const {
      return Match(!strcasecmp(Event->Title(), *m_Str));
    }
    virtual cString ToText(int ident) const { 
      return cString::sprintf("%sTITLE IS \"%s\"", Ident(ident), *m_Str);
    }
};


//--- cRuleChannelRange -----------------------------------------

#include <vdr/channels.h>

class cRuleChannelRange : public cRule 
{
  protected:
    int m_Start;
    int m_End;
  public:
    cRuleChannelRange(int Start, int End) : m_Start(Start), m_End(End) {}
    virtual bool Matches(cEvent *Event) const {
      cChannel *ch = Channels.GetByChannelID(Event->ChannelID());
      if(ch)
	if(ch->Number() < m_Start || ch->Number() > m_End)
	  return Match(false);
      return Match(true);
    }
    virtual cString ToText(int ident) const { 
      if(m_Start==m_End)
	return cString::sprintf("%sCHANNELNUMBER IS %d", Ident(ident), m_Start);      
      return cString::sprintf("%sCHANNELNUMBER IS %d-%d", Ident(ident), m_Start, m_End);
    }
};


//--- cRuleNot ---------------------------------------------------

class cRuleNot : public cRule 
{
  private:
    cRuleNot(cRuleNot&);
  protected:
    cRule *m_Rule;
  public:
    cRuleNot(cRule *Rule) : m_Rule(Rule) {}
    virtual ~cRuleNot() { delete m_Rule; }
    virtual bool Matches(cEvent *Event) const {
      return Match(!m_Rule->Matches(Event));
    }
    virtual cString ToText(int ident) const { 
      return cString::sprintf("%sNOT {%s}", Ident(ident), *m_Rule->ToText());
    }
};


//--- cRuleAnd --------------------------------------------------

#include <stdarg.h>

class cRuleAnd : public cRule 
{
  protected:
    cRules m_Rules;
  public:
    cRuleAnd(cRule *Rule1, cRule *Rule2 = NULL, ...) {
      m_Rules.Add(Rule1);
      cRule *next = Rule2;

      va_list ap;
      va_start(ap, Rule2);

      while(next) {
	m_Rules.Add(next);
	next = va_arg(ap, cRule*);
      }
      
      va_end(ap);
    }
    void Add(cRule *Rule) { m_Rules.Add(Rule); }
    virtual bool Matches(cEvent *Event) const {
      bool result = true;
      for(cRule *i = m_Rules.First(); i && result; i=m_Rules.Next(i))
	result = /*result &&*/ i->Matches(Event);
      return Match(result);
    }
    virtual cString ToText(int ident) const { 
      cString str = cString::sprintf("%s{\n%s", Ident(ident), *m_Rules.First()->ToText(ident+4));
      for(cRule *i = m_Rules.Next(m_Rules.First()); i; i=m_Rules.Next(i))
	str = cString::sprintf("%s AND \n%s", *str, *i->ToText(ident+4));
      str = cString::sprintf("%s\n%s}", *str, Ident(ident));
      return str;
    }
};


//--- cRuleOr ---------------------------------------------------

class cRuleOr : public cRule 
{
  protected:
    cRules m_Rules;
  public:
    cRuleOr(cRule *Rule1, cRule *Rule2 = NULL, ...) {
      m_Rules.Add(Rule1);
      cRule *next = Rule2;

      va_list ap;
      va_start(ap, Rule2);

      while(next) {
	m_Rules.Add(next);
	next = va_arg(ap, cRule*);
      }
      
      va_end(ap);
    }
    void Add(cRule *Rule) { m_Rules.Add(Rule); }
    virtual bool Matches(cEvent *Event) const {
      bool result = false;
      for(cRule *i = m_Rules.First(); i && !result; i=m_Rules.Next(i))
	result = /*result ||*/ i->Matches(Event);
      return Match(result);
    }
    virtual cString ToText(int ident) const { 
      cString str = cString::sprintf("%s{\n%s", Ident(ident), *m_Rules.First()->ToText(ident+4));
      for(cRule *i = m_Rules.Next(m_Rules.First()); i; i=m_Rules.Next(i))
	str = cString::sprintf("%s OR\n%s", *str, *i->ToText(ident+4));
      str = cString::sprintf("%s\n%s}\n", *str, Ident(ident));
      return str;
    }
};


//--- cRule -----------------------------------------------------

#define CONFERR(msg...) \
  do { \
    esyslog("autotimer: config error: " msg ); \
    return NULL; \
  } while(0)

static void SkipSpace(char **pText)
{
  while(**pText == ' ' || **pText == '\r' || **pText == '\n' || **pText == '\t')
    (*pText)++;
}

static void Compact(char *str)
{
  char *p = str, *e = str;
  int q = 0;
  while(*e) {
    if(*e == '\"') {
      q = !q;
      *p++ = *e++;
      continue;
    }
    
    if(!q) {
      if(*e == '\r' || *e == '\n' || *e == '\t' || *e == ' ') {
	*e = ' ';
	*p++ = *e++;
	while(*e == '\r' || *e == '\n' || *e == '\t' || *e == ' ')
	  e++;
	continue;
      }
      if(*e == '#') {
	e++;
	while(*e != '\r' && *e != '\n' && *e != '\0')
	  e++;
      }
    }
      
    *p++ = *e++;
  }
  *p = 0;
}

static cString TakeString(char **pText)
{
  char str[1024]="", *s = str;
  char *Text = *pText;

  while(*Text && *Text != '\"')
    *Text++ = ' ';
  *Text++ = ' ';

  while(*Text && *Text != '\"') {
    *s++ = *Text;
    *Text++ = ' ';
  }
  *Text = 0;
  *s = 0;

  *pText = Text+1;

  return cString(str);
}

#define MAX_FILE_SIZE 0x10000

cRule *cRule::RuleFromFile(const char *File)
{
  char  str[MAX_FILE_SIZE + 1], *pt = str, *tmp;
  int   n;
  FILE *fp;

  /* open */
  if(NULL == (fp = fopen(File, "r"))) {
    esyslog("autotimer: Error opening  %s: %m", File);
    return NULL;
  }

  /* read */
  if((n = fread(str, 1, MAX_FILE_SIZE, fp)) < 0) {
    esyslog("autotimer: Error reading from %s: %m", File);
    fclose(fp);
    return NULL;
  }
  fclose(fp);

  /* check size */
  if(n>0x10000) {
    esyslog("autotimer: file is too large, rule truncated");
    n = 0x10000;
  }

  ISYSLOG("autotimer: Rules loaded from %s (%d bytes)", File, n);

  /* parse */
  str[n] = 0;
  Compact(str);
  cRule *Rule = cRule::Parse(&pt);
  if(!Rule) {
    esyslog("autotimer: Error parsing %s (%d:\"%20s\"", File, (pt-str), pt);
    return NULL;
  }

  /* dump */
  asprintf(&tmp, "%s.parsed", File);
  fp = fopen(tmp, "w");
  fprintf(fp, "#\n# automatically generated file\n#\n\n%s\n", *Rule->ToText());
  fclose(fp);
  free(tmp);
  
  return Rule;
}

cRule *cRule::Parse(const char *Text) 
{
  char *tmp = strdup(Text);
  Compact(tmp);
  cRule *result = cRule::Parse(&tmp);
  free(tmp);
  return result;
}

cRule *cRule::Parse(char **pText)
{
  cRule *Prev = NULL;
  cRule *Result = NULL;
  char *Text = *pText;
  cRuleAnd *rand = NULL;
  cRuleOr *ror = NULL;
  //cRuleNot *rnot = NULL;
  bool bNot = false;
  int Priority = -1;
  cString Folder = cString(NULL);

  while(*Text) {

    /* skip whitespace */
    SkipSpace(pText);
    Text = *pText;
    if(!*Text)
      continue;

    if(!strncmp(Text, "AND", 3)) {
      if(!rand) {
	if(!Result)
	  CONFERR("AND without first rule (%s)", Text);
	rand = new cRuleAnd(Result);
	Result = Prev = NULL;
      }
      (*pText) += 3;
    } else if(!strncmp(Text, "OR", 2)) {
      if(!ror) {
	if(!Result)
	  CONFERR("OR without first rule (%s)", Text);
	ror = new cRuleOr(Result);
	Result = Prev = NULL;
      }
      (*pText) += 2;
    } else if(!strncmp(Text, "AVOID DUPLICATES", 16)) {
//#warning todo
      (*pText) += 16;      
    } else if(!strncmp(Text, "NAME", 4)) {
      (*pText) += 4;
      if(Prev) {
	Prev->m_Name = TakeString(pText);
      } else {
	cString tmp = TakeString(pText);
	CONFERR("No rule before NAME (%s)", *tmp);
      }
    } else if(!strncmp(Text, "FOLDER", 6)) {
      (*pText) += 6;
      if(Prev) {
	Prev->m_Folder = TakeString(pText);
      } else {
	Folder = TakeString(pText);
//#warning TODO: FOLDER xxx ... AND ... AND ... -> Folder will be duplicated n times !
      }
//#warning TODO: must store prev result, might be second in AND etc.
    } else if(!strncmp(Text, "PRIORITY", 8)) {
      int p;
      if(Prev && 1==sscanf(Text, "PRIORITY %d", &p)) {
	Prev->m_Priority = p; 
      } else {
	Priority = p;
      }
      (*pText) += 8;
      SkipSpace(pText);
      while(**pText && **pText != ' ' && **pText != '\r' && **pText != '\n')
	(*pText)++;
//#warning TODO: must store prev result, might be second in AND etc.
    } else if(!strncmp(Text, "NOT ", 4)) {
      bNot = true;
      (*pText) += 3;
    } else {
      if(Result) 
	CONFERR("multiple rules without operator");

      if(*Text == '{') {
	*(Text++) = 0;
	*pText = Text;
	
	int n = 1, pos = 0, skipping = 0;
	while(Text[pos] && n) {
	  if(skipping && Text[pos] == '\"')
	    skipping = 0;
	  else if(Text[pos] == '\"')
	    skipping = 1;
	  else if(!skipping && Text[pos] == '{')
	    n++;
	  else if(!skipping && Text[pos] == '}')
	    n--;
	  if(n)
	    pos++;
	}
	
	if(!Text[pos] && n)
	  CONFERR("missing closing \'}\'");
	
	Text[pos++] = 0;
	
	Result = Parse(pText);
	*pText = &Text[pos];
      } else if(!strncmp(Text, "TITLE IS", 8)) {
	(*pText) += 8;
	Result = new cRuleTitle(TakeString(pText));
      } else if(!strncmp(Text, "TITLE CONTAINS", 14)) {
	(*pText) += 14;
	Result = new cRuleTitleSubstring(TakeString(pText));
      } else if(!strncmp(Text, "INFO CONTAINS", 13)) {
	(*pText) += 13;
	Result = new cRuleSubstring(TakeString(pText));
      } else if(!strncmp(Text, "CHANNELNUMBER IS", 16)) {
	int s=0, e=0;
	if(2==sscanf(Text+16, "%d-%d", &s, &e))
	  Result = new cRuleChannelRange(s, e);
	else if(1==sscanf(Text+16, "%d", &s))
	  Result = new cRuleChannelRange(s, s);
	else
	  CONFERR("missing channel number");

	(*pText) += 16;
	SkipSpace(pText);
	while(**pText && **pText != ' ' && **pText != '\r' && **pText != '\n')
	  (*pText)++;
      } else if(*Text) {
	CONFERR("unknown rule: %40s", Text);
      }
    }

    if(Result) {
      if(bNot)
	Result = new cRuleNot(Result);
      bNot = false;

      Prev = Result;
      Result->m_Priority = Priority;
      Result->m_Folder = Folder;
    }

    if(rand && Result) {
      rand->Add(Result);
      Result = NULL;
    } else if(ror && Result) {
      ror->Add(Result);
      Result = NULL;
    }
    if(rand && ror)
      CONFERR("AND and OR mixed");

    Text = *pText;
  }

  if(!Result && rand)
    return rand;
  if(!Result && ror)
    return ror;

  return Result;
}


