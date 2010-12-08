#ifndef __PREFEROSD_H
#define __PREFEROSD_H

#include <vdr/plugin.h>

static const int BORDERSIZE = 2;
static const cFont *font = cFont::GetFont(fontOsd);
static const int LINEHEIGHT = font->Height();


class cPreferOsd : public cOsdObject {
private:
  cOsd *osd;
  tChannelID lastChannel;
  
public:
  cPreferOsd(void);
  ~cPreferOsd();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  
  void ReadPreferedChannels(void);
  
  void CursorUp();
  void CursorDown();
  void CursorLeft();
  void CursorRight();
  void CursorOK();
  void LastChannel();
  void AddChannel();
  void RemoveChannel();
  void MoveChannel(int places);
  
  void DisplayPreferMenu(void); 

private:
  void DrawPreferMenu(int delta, int highlight);
  void DrawChannelsNames(int delta);
  void DrawSymbol(int x, int y, char *SymbolName[], tColor color);
  void DrawHLine(int xstart, int ystart, int width, tColor color);
  void DrawVLine(int xstart, int ystart, int height, tColor color);
};

#endif //__PREFEROSD_H
