#include "rcfile.h"

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
  strcpy(recordingfile, "");
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
	      esyslog("ERROR: unknown parameter: %s = %s", l->Name(), l->Value());
	      result = false;
	  }
      }
      return result;
  }
  return false;
}

bool cRCFile::Parse(const char *Name, const char *Value)
{
  if (!strcasecmp(Name, "recording")) strn0cpy(recordingfile, Value, MAXSTRINGLEN);
  else return false;

  return true;
}
