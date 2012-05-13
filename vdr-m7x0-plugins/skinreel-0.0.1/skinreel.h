#ifndef P__SKINREEL_H
#define P__SKINREEL_H

#include <vdr/skins.h>

class cSkinReel : public cSkin {
public:
  cSkinReel(void);
  virtual const char *Description();
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu();
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume();
  virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual cSkinDisplayMessage *DisplayMessage();
  };

#endif //__SKP__SKINREEL_H
