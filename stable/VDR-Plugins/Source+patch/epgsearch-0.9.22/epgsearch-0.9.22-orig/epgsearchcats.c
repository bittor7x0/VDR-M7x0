#include <string>
#include "epgsearchcats.h"
#include "log.h"
#include <vdr/tools.h>

using std::string;

// -- cSearchExtCat -----------------------------------------------------------------
cSearchExtCat::cSearchExtCat(void)
{
    id = 0;
    name = NULL;
    menuname = NULL;
    searchmode = 1; // default: all substrings must exist
    values = NULL;
    nvalues = 0;
}

cSearchExtCat::~cSearchExtCat(void)
{
    free(name);
    free(menuname);
    for(int i=0; i<nvalues; i++)
	free(values[i]);
    free(values);
}

bool cSearchExtCat::Parse(const char *s)
{
  char *line;
  char *pos;
  char *pos_next;
  int parameter = 1;
  int valuelen;

#define MAXVALUELEN (10 * MaxFileName)

  char value[MAXVALUELEN];

  pos = line = strdup(s);
  pos_next = pos + strlen(pos);
  if (*pos_next == '\n') *pos_next = 0;
  while (*pos) {
    while (*pos == ' ') pos++;
    if (*pos) {
      if (*pos != '|') {
        pos_next = strchr(pos, '|');
        if (!pos_next)
          pos_next = pos + strlen(pos);
        valuelen = pos_next - pos + 1;
        if (valuelen > MAXVALUELEN) 
	{
	    LogFile.eSysLog("entry '%s' is too long. Will be truncated!", pos);  
	    valuelen = MAXVALUELEN;
	}
        strn0cpy(value, pos, valuelen);
        pos = pos_next;
        switch (parameter) {
	    case 1:  id = atoi(value);
	      break;
	    case 2:  name = strdup(value);
	      break;
	    case 3:  menuname = strdup(value);
	      break;
	    case 4:  
	    {
		char* szBuffer = strdup(value);
		char* pptr;
		char* pstrToken=strtok_r(szBuffer, ",", &pptr);
		while(pstrToken) 
		{
		    nvalues++;
		    values = (char**) realloc(values, nvalues * sizeof(char*));
		    values[nvalues-1] = strdup(pstrToken);
		    pstrToken=strtok_r(NULL, ",", &pptr);
		}
		free(szBuffer);
		break;
	    }
	    case 5:  searchmode = atoi(value);
	      break;
	    break;
        } //switch
      }
      parameter++;
    }
    if (*pos) pos++;
  } //while

  free(line);
  return (parameter >= 3) ? true : false;
}

const char *cSearchExtCat::ToText(void)
{
    char* buffer = NULL;
    string sValues = "";
    for(int i=0; i<nvalues; i++)
	sValues += string(values[i]) + ((i<nvalues-1)?", ":"");
    
    asprintf(&buffer, "%d|%s|%s|%s|%d", 
	     id, name, menuname, sValues.c_str(), searchmode);
    return buffer;
}

// -- cSearchExtCats ----------------------------------------------------------------
int cSearchExtCats::GetIndexFromID(int id)
{
  cSearchExtCat *SearchExtCat = SearchExtCats.First();
  int index = 0;
  while (SearchExtCat) 
  {
      if (SearchExtCat->id == id)
	  break;
      index++;
      SearchExtCat = SearchExtCats.Next(SearchExtCat);
  }
  if (!SearchExtCat && index == 0)
      return -1;
  else
      return index;
}
