/*
 * texteffects.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __TEXTEFFECTS_H
#define __TEXTEFFECTS_H

#include <semaphore.h>

#include "common.h"
#include "enigma.h"

#include <vector>

#include <vdr/skins.h>
#include <vdr/thread.h>

#define TE_START(osd) EnigmaTextEffects.Start(osd);
#define TE_STOP EnigmaTextEffects.Stop();
#define TE_LOCK EnigmaTextEffects.UpdateLock();
#define TE_UNLOCK EnigmaTextEffects.UpdateUnlock();
#define TE_WAKEUP EnigmaTextEffects.RefreshEffects();
#define TE_MARQUEE(osd, id, x...) EnigmaTextEffects.DrawAnimatedText(id, 0, x)
#define TE_BLINK(osd, id, x...) EnigmaTextEffects.DrawAnimatedText(id, 1, x)

class cEffect
{
protected:
  bool Active;
  cOsd *Osd;
  int xOff;
  int yOff;
  int Width;
  int Height;
  int ColorDepth;
  char *Text;

  cTimeMs Time;
  cBitmap *bmp;

public:
  cEffect(cOsd *osd, int x, int y, int width, int height = 0, int depth = 1, bool active = true);
  virtual ~cEffect();
  virtual bool SetText(const char *text, const cFont *font, int offset = 0) = 0;
  virtual int DrawIfNeeded(int yMax) = 0;
  bool changed;
};

class cEffectScroll : public cEffect
{
private:
  tColor Fg;
  tColor Bg;
  int Alignment;
  int Delay;

  int pos;
  int dir;
  int delay;
  int xOffset;
  int DestWidth;

  bool IsDrawNeeded(void);

public:
  cEffectScroll(cOsd *osd, int x, int y, int width, int height = 0, tColor fg = 0xFFFFFFFF, tColor bg = 0xFF000000, int alignment = taDefault, int depth = 1, bool active = true);
  virtual ~cEffectScroll()
  {};
  virtual bool SetText(const char *text, const cFont *font, int offset = 0);
  virtual int DrawIfNeeded(int yMax);
};

class cEffectBlink : public cEffect
{
private:
  tColor Fg;
  tColor Bg;
  int Alignment;
  int Delay;

  bool shown;
  int delay;
  int xOffset;
  int DestWidth;

public:
  cEffectBlink(cOsd *osd, int x, int y, int width, int height = 0, tColor fg = 0xFFFFFFFF, tColor bg = 0xFF000000, int alignment = taDefault, int depth = 1, bool active = true);
  virtual ~cEffectBlink()
  {};
  virtual bool SetText(const char *text, const cFont *font, int offset = 0);
  virtual int DrawIfNeeded(int yMax);
};

class cEnigmaTextEffects : public cThread {
private:
  cOsd *osd;

  int yMessageTop;

  typedef std::vector<cEffect*> Effects_t;
  Effects_t vecEffects;
  cCondVar condSleep;
  cMutex mutexSleep;
  cMutex mutexRunning;
  sem_t sem_update;

  void Wakeup(void)
  {
//    printf("WAKE1: %lu\n", pthread_self());
    mutexSleep.Lock();
    condSleep.Broadcast();
    mutexSleep.Unlock();
//    printf("WAKE2: %lu\n", pthread_self());
  }

public:
  cEnigmaTextEffects(const char *Description = NULL);
  ~cEnigmaTextEffects(void);

  virtual void Action(void);

  bool Start(cOsd *o);
  void Stop(void);
  void Clear(void);

  void ResetText(int i, tColor ColorFg = 0, tColor ColorBg = 0, bool fDraw = true);
  void PauseEffects(int y = 0);
  int DrawAnimatedText(int o_id, int action, bool active, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int ColorDepth = 1, int Width = 0, int Height = 0, int Alignment = taDefault);

  void UpdateLock(void)
  { 
//    printf("LOCK1: %lu\n", pthread_self());
    sem_wait(&sem_update);
//    printf("LOCK2: %lu\n", pthread_self());
  }

  void UpdateUnlock(void)
  { 
//    printf("UNLOCK1: %lu\n", pthread_self());
    sem_post(&sem_update);
//    printf("UNLOCK2: %lu\n", pthread_self());
  }

  void RefreshEffects(void)
  { Wakeup(); }
};

extern cEnigmaTextEffects EnigmaTextEffects;

#endif //__TEXTEFFECTS_H
// vim:et:sw=2:ts=2:
