#ifndef __EPGSEARCHDIRSELECT_H
#define __EPGSEARCHDIRSELECT_H

#include <string>
#include <set>
#include <vdr/menuitems.h>

using std::string;
using std::set;


class cDirExt : public cListObject {
private:
    char name[MaxFileName];
public:
  cDirExt(void){}
  bool Parse(const char *s) {strcpy(name,s); return true;}
  char* Name() {return name;}
};

class cDirExts : public cConfig<cDirExt> {};

extern cDirExts DirExts;

// --- cMenuDirSelect ---------------------------------------------------------

class cMenuDirSelect : public cOsdMenu {
private:
    int CurLevel;
    int MaxLevel;
    char* Directory;
    char* yellow;
 public:

    static set<string> directorySet;

    cMenuDirSelect(char*);
    ~cMenuDirSelect();
    void Load();
    void AddDistinct(const char* szText);
    virtual eOSState ProcessKey(eKeys Key);
    int Level(const char* szDir);
    void ReplaceDirVars();

    static void CreateDirSet();
  };

#endif
