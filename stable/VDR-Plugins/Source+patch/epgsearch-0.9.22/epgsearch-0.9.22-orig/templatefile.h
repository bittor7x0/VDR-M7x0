#ifndef __EPGSEARCHTEMPLFILE_H
#define __EPGSEARCHTEMPLFILE_H

#include <vdr/config.h>
#include <vdr/skins.h>
#include <set>

using std::set;

#define MAXTEMPLLEN 2000

class cMenuTemplate
{
 private:
    char name[MAXTEMPLLEN];
    char menuTemplate[MAXTEMPLLEN];
    int  menuTabs[cSkinDisplayMenu::MaxTabs];    
 public:
    cMenuTemplate(const char* Name) 
      {
	strcpy(name, Name); 
	menuTemplate[0] = 0;
	for(int i=0; i<cSkinDisplayMenu::MaxTabs; i++) menuTabs[i] = 0;
      }
    const char* Name(void) {return name;}
    const char* MenuTemplate(void) {return menuTemplate;}
    int Tab(int i) {return menuTabs[i];}
    bool PrepareTemplate(const char* templateLine);
};

class cTemplLine : public cListObject {
private:
  char *name;
  char *value;
public:
  cTemplLine(void);
  cTemplLine(const char *Name, const char *Value);
  virtual ~cTemplLine();
  const char *Name(void) { return name; }
  const char *Value(void) { return value; }
  bool Parse(char *s);
};


class cTemplFile : public cConfig<cTemplLine> 
{
    bool Parse(const char *Name, const char *Value);
public:
    static set<cMenuTemplate*> menuTemplates; // the set of all templates
    static char** SearchTemplates; // an array thats stores the name of all search results templates
    cTemplFile();
    bool Load(const char *FileName);
    static void PrepareDefaultTemplates();
    static void Reset();
    static cMenuTemplate* GetTemplateByName(const char* Name);
    static int CountSearchResultsTemplates();
    static cMenuTemplate* GetSearchTemplateByPos(int iPos);
};


#endif
