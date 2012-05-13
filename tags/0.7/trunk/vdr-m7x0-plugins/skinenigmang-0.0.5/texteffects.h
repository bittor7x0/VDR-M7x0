/*
 * status.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __TEXTEFFECTS_H
#define __TEXTEFFECTS_H

#include "common.h"
#include "enigma.h"

#ifndef DISABLE_ANIMATED_TEXT
#include <vector>
#endif

#include <vdr/skins.h>

#ifdef DISABLE_ANIMATED_TEXT
#define TE_START(osd) ;
#define TE_STOP ;
#define TE_LOCK ;
#define TE_UNLOCK ;
#define TE_WAKEUP ;
#define TE_MARQUEE(osd, id, x...) osd->DrawText(x)
#define TE_BLINK(osd, id, x...) osd->DrawText(x)
#define TE_TITLE(osd, id, s, Width, skin) osd->DrawTitle(s)

#else // !DISABLE_ANIMATED_TEXT
#include <vdr/thread.h>

#define TE_START(osd) EnigmaTextEffects.Start(osd);
#define TE_STOP EnigmaTextEffects.Stop();
#define TE_LOCK EnigmaTextEffects.UpdateLock();
#define TE_UNLOCK EnigmaTextEffects.UpdateUnlock();
#define TE_WAKEUP EnigmaTextEffects.RefreshEffects();
#define TE_MARQUEE(osd, id, x...) EnigmaTextEffects.DrawAnimatedText(id, 0, x)
#define TE_BLINK(osd, id, x...) EnigmaTextEffects.DrawAnimatedText(id, 1, x)
#define TE_TITLE(osd, id, x...) EnigmaTextEffects.DrawAnimatedTitle(id, 0, x)

#endif //DISABLE_ANIMATED_TEXT

#ifdef DISABLE_ANIMATED_TEXT
class cEnigmaTextEffects {
#else
class cEnigmaTextEffects : public cThread {
#endif
private:
  cOsd *osd;

#ifdef HAVE_FREETYPE
  char **availTTFs;
  int nMaxTTFs;
#endif

#ifndef DISABLE_ANIMATED_TEXT
  int yMessageTop;

  struct tEffect {
    int nAction;
    uint nOffset;
    int nDirection;
    uint64_t nNextUpdate;
    std::string strText;
    int x, y, Width, Height;
    tColor ColorFg, ColorBg;
    int FontId;
    const cFont *Font;
    int Alignment;
    cSkinEnigmaOsd *Skin;

  public:
    tEffect(void) : nAction(0), nOffset(0), nDirection(0),
                    nNextUpdate(0), x(0), y(0), Width(0), Height(0),
                    ColorFg(0), ColorBg(0), FontId(fontOsd + 1), Font(NULL),
                    Alignment(taDefault), Skin(NULL)
    {};
  };

  typedef std::vector<tEffect*> tEffects;
  tEffects vecEffects;
  cCondVar condSleep;
  cMutex mutexSleep;
  cMutex mutexRunning;

  void DoEffect(tEffect *e, uint64_t nNow = 0);
  void DoScroll(tEffect *e, uint64_t nNow, bool fDrawItem);
  void DoBlink(tEffect *e, uint64_t nNow, bool fDrawItem);

  void Wakeup(void)
  {
//    printf("WAKE1: %lu\n", pthread_self());
    mutexSleep.Lock();
    condSleep.Broadcast();
    mutexSleep.Unlock();
//    printf("WAKE2: %lu\n", pthread_self());
  }
#endif //DISABLE_ANIMATED_TEXT

public:
#ifdef DISABLE_ANIMATED_TEXT
  cEnigmaTextEffects(void);
  ~cEnigmaTextEffects(void);

#else
  cEnigmaTextEffects(const char *Description = NULL);
  ~cEnigmaTextEffects(void);

  virtual void Action(void);

  bool Start(cOsd *o);
  void Stop(void);
  void Clear(void);

  void ResetText(int i, tColor ColorFg = 0, tColor ColorBg = 0, bool fDraw = true);
  void PauseEffects(int y = 0);
  void UpdateTextWidth(int i, int Width);
  int DrawAnimatedTitle(int o_id, int action, const char *s, int Width, cSkinEnigmaOsd *skin);
  int DrawAnimatedText(int o_id, int action, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, int FontId, int Width = 0, int Height = 0, int Alignment = taDefault);

  void UpdateLock(void)
  { 
//    printf("LOCK1: %lu\n", pthread_self());
    Lock();
//    printf("LOCK2: %lu\n", pthread_self());
  }

  void UpdateUnlock(void)
  { 
//    printf("UNLOCK1: %lu\n", pthread_self());
    Unlock();
//    printf("UNLOCK2: %lu\n", pthread_self());
  }

  void RefreshEffects(void)
  { Wakeup(); }
#endif //DISABLE_ANIMATED_TEXT

#ifdef HAVE_FREETYPE
  int GetNumAvailTTFs(void) { return nMaxTTFs; }
  const char **GetAvailTTFs(void);
#endif
};

extern cEnigmaTextEffects EnigmaTextEffects;

#endif //__TEXTEFFECTS_H
// vim:et:sw=2:ts=2:
