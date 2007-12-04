#include "rcfile.h"
#include "log.h"

cRCLine::cRCLine(void)
{
  name = value = NULL;
}

cRCLine::cRCLine(const char *Name, const char *Value)
{
  name = strdup(Name);
  value = strdup(Value);
}

cRCLine::~cRCLine()
{
  free(name);
  free(value);
}

bool cRCLine::Parse(char *s)
{
  char *p = strchr(s, '=');
  if (p) 
  {
      *p = 0;
      char *Name  = compactspace(s);
      char *Value = compactspace(p + 1);
      if (*Name) 
      {
	  name = strdup(Name);
	  value = strdup(Value);
	  return true;
      }
  }
  return false;
}

cRCFile::cRCFile()
{
  ChannelNr = -1;
  SearchMode = 1; // default is 'AND'
  UseTitle = 1;
  UseSubtitle = 1;
  UseDescr = 1;
  strcpy(Search, "");
}

bool cRCFile::Load(const char *FileName)
{
  if (cConfig<cRCLine>::Load(FileName, true)) 
  {
      bool result = true;
      for (cRCLine *l = First(); l; l = Next(l)) 
      {
	  bool error = false;
	  if (!Parse(l->Name(), l->Value()))
	      error = true;
	  if (error) 
	  {
	      LogFile.eSysLog("ERROR: unknown parameter: %s = %s", l->Name(), l->Value());
	      result = false;
	  }
      }
      return result;
  }
  return false;
}

bool cRCFile::Parse(const char *Name, const char *Value)
{
  if (!strcasecmp(Name, "Search")) strn0cpy(Search, Value, MAXSTRINGLEN);
  else if (!strcasecmp(Name, "SearchMode")) SearchMode = atoi(Value);
  else if (!strcasecmp(Name, "ChannelNr")) ChannelNr = atoi(Value);
  else if (!strcasecmp(Name, "UseTitle")) UseTitle = atoi(Value);
  else if (!strcasecmp(Name, "UseSubtitle")) UseSubtitle = atoi(Value);
  else if (!strcasecmp(Name, "UseDescr")) UseDescr = atoi(Value);
  else return false;

  return true;
}
