#ifndef __MENUMAIN_H
#define __MENUMAIN_H

#include <vdr/menuitems.h>
#include <vdr/recording.h>
#include <vdr/font.h>

// --- cMenuShowInfoMain --------------------------------------------------------

class cMenuShowInfoMain : public cOsdMenu {
private:
  int col1;
  int iWidth;
  const cFont *font;
  bool AddItem(const char *Titel, const char* Text=NULL);
  void SetHelpKeys(void);
public:
  cMenuShowInfoMain(cRecording *pRecording=NULL);
  virtual ~cMenuShowInfoMain();
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUMAIN_H
