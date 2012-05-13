/*
 * enigma.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 */

#ifndef __ENIGMA_H
#define __ENIGMA_H

#include "common.h"

#include <vdr/skins.h>
#include <vdr/skinclassic.h>

class cSkinEnigma : public cSkin {
private:
  cSkin *skinFallback;

public:
  cSkinEnigma();
  virtual const char *Description(void);
  virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
  virtual cSkinDisplayMenu *DisplayMenu(void);
  virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
  virtual cSkinDisplayVolume *DisplayVolume(void);
  virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual cSkinDisplayMessage *DisplayMessage(void);
};

// interface for common functions
class cSkinEnigmaBaseOsd {

protected:
  cOsd *osd;
  bool HasChannelTimerRecording(const cChannel *Channel);
  int DrawStatusSymbols(int x0, int xs, int top, int bottom, const cChannel *Channel = NULL);
  int FixWidth(int w, int bpp, bool enlarge = true);
};

#endif //__ENIGMA_H

// vim:et:sw=2:ts=2:
