#ifndef __EPGSEARCHCATS_H
#define __EPGSEARCHCATS_H

#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>

// --- cSearchExtCat --------------------------------------------------------
class cSearchExtCat : public cListObject {
public:
    int      id;
    char*    name;
    char*    menuname;
    int      searchmode; // 0 - substring, 1 - substring-and, 2 - substring or, 3 - equal 
    char**   values;
    int      nvalues;
public:
  cSearchExtCat(void);
  virtual ~cSearchExtCat(void);

  bool Parse(const char *s);
  const char* ToText(void);
};

class cSearchExtCats : public cConfig<cSearchExtCat> {
private:
public:
  cSearchExtCats(void) {}
  int GetIndexFromID(int id);
  };

extern cSearchExtCats SearchExtCats;

#endif
