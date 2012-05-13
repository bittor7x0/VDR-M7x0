/*
 * skinsoppalusikka.h: The 'soppalusikka' VDR skin
 *
 * $Id: $
 */

#ifndef __SOPPALUSIKKA_H
#define __SOPPALUSIKKA_H

#include <vdr/skins.h>

class cSkinSoppalusikka : public cSkin {
public:
  cSkinSoppalusikka();
  virtual const char *Description(void);
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual cSkinDisplayMessage *DisplayMessage(void);
  };

#endif //__SOPPALUSIKKA_H
