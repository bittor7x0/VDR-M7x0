#include "common.h"
#include "config.h"
#include "texteffects.h"
#include "tools.h"

#include <algorithm>

//Redefine macros
#undef  TE_LOCK
#define TE_LOCK   UpdateLock()
#undef  TE_UNLOCK
#define TE_UNLOCK UpdateUnlock()


// --- cEffect -----------------------------------------------

cEffect::cEffect(cOsd * osd, int x, int y, int width, int height, int depth /* = 1 */, bool active /* = true */)
{
  debug("cEffect::cEffect(x=%d,y=%d,w=%d,h=%d)", x, y, width, height)
  Active = active;
  xOff = x;
  yOff = y;
  Osd = osd;
  bmp = NULL;
  Width = width;
  Height = height;
  ColorDepth = depth;
  Text = NULL;
  changed = false;
}

cEffect::~cEffect()
{
  free(Text);
  Text = NULL;
  delete(bmp);
  bmp = NULL;
}


// --- cEffectScroll -----------------------------------------------

cEffectScroll::cEffectScroll(cOsd *osd, int x, int y, int width, int height /* = 0 */, tColor fg /* = 0xFFFFFFFF */, tColor bg /* = 0xFF000000 */, int alignment /* = taDefault */, int depth /* = 1 */, bool active /* = true */) : cEffect(osd, x, y, width, height, depth, active)
{
  Fg = fg;
  Bg = bg;
  Alignment = alignment;
  DestWidth = width;
  pos = 0;
  dir = 0;
  Delay = EnigmaConfig.scrollPause;
  delay = Delay;
  xOffset = 0;
}

bool cEffectScroll::SetText(const char *text, const cFont *font, int offset /* = 0 */)
{
  debug("cEffectScroll::SetText(%s, %d)", text ? text : "NULL", offset)

  if (!font)
    return false;

  changed = true;
  if (!text) { // || isempty(text)) { // clear area if no text is given
    Osd->DrawRectangle(xOff + xOffset, yOff, xOff + DestWidth, yOff - 1 + Height, Bg);
    return false;
  }

  if (Text && strcmp(Text, text) == 0)
    return true;

  if (Text)
    free(Text);
  Text = strdup(text);

  int textwidth = font->Width(text);
  int textheight = font->Height(text);
  xOffset = offset;
  dir = 0;
  DestWidth = Width - xOffset;

  // text is given, draw it
  Osd->DrawText(xOff + xOffset, yOff, text, Fg, Bg, font, DestWidth, textheight, Alignment);

  if (!DestWidth || !Active)
    return false;

  if (textwidth > DestWidth) {
    pos = 0;
    dir = -1;
    Time.Set();

    if (bmp)
      bmp->SetSize(textwidth, textheight);
    else
      bmp = new cBitmap(textwidth, textheight, ColorDepth);
    bmp->DrawText(0, 0, text, Fg, Bg, font);

    delay = Delay;
    return true;
  }

  return false;
}

bool cEffectScroll::IsDrawNeeded(void)
{
  if (bmp) {
    int elapsed = ((int)Time.Elapsed());
    if (elapsed >= Delay) {
      Delay = EnigmaConfig.scrollDelay;

      if (dir == -2) {
        pos = 0;
        dir = -1;
        Delay = EnigmaConfig.scrollPause;

      } else {
        pos -= dir;

        if (pos < 0) {
          pos = 0;
          dir = -1;
          Delay = EnigmaConfig.scrollPause;

        } else {
          int x = bmp->Width() - DestWidth;
          if (pos > x) {
            if (EnigmaConfig.scrollMode) { // Restart scrolling from the left
              dir = -2;

            } else { // ping-pong scroll
              pos = x;
              dir = 1;
            }

            Delay = EnigmaConfig.scrollPause;
          }
        }
      }

      Time.Set();
      delay = Delay;
      return true;
    } else {
      delay = Delay - elapsed;
    }

  } else {
    delay = 0;
  }

  return false;
}

int cEffectScroll::DrawIfNeeded(int yMax)
{
  bool needed=IsDrawNeeded();
//  printf("cEffectScroll::DrawIfNeeded(%d) bmp=%p dir=%d yOff=%d needed=%d, DestWidth=%d, h=%d\n", yMax, bmp, dir, yOff, needed, DestWidth, yOff + Height > yMax ? yMax - yOff : bmp->Height());

  changed = false;
  if (bmp && dir && (yMax == 0 || yOff < yMax) && needed) { //TODO
    changed = true;
    for (int i = 0; i < DestWidth; i++) {
      int nHeight = (yMax > 0 && yOff + Height > yMax) ? yMax - yOff : bmp->Height();
      for (int j = 0; j < nHeight; j++) {
        Osd->DrawPixel(xOff + xOffset + i, yOff + j, bmp->Color(*(bmp->Data(i + pos, j))));
      }
    }
  }

  return delay;
}


// --- cEffectBlink -----------------------------------------------

cEffectBlink::cEffectBlink(cOsd *osd, int x, int y, int width, int height /* = 0 */, tColor fg /* = 0xFFFFFFFF */, tColor bg /* = 0xFF000000 */, int alignment /* = taDefault */, int depth /* = 1 */, bool active /* = true */) : cEffect(osd, x, y, width, height, depth, active)
{
  Fg = fg;
  Bg = bg;
  Alignment = alignment;
  Delay = EnigmaConfig.blinkPause;
  delay = Delay;
  shown = false;
  changed = false;
}

bool cEffectBlink::SetText(const char *text, const cFont *font, int offset /* = 0 */)
{
  debug("cEffectBlink::SetText(%s, %d)", text ? text : "NULL", offset)

  if (!font)
    return false;

  changed = true;
  if (!text || isempty(text)) { // clear area if no text is given
    Osd->DrawRectangle(xOff + xOffset, yOff, xOff + DestWidth, yOff - 1 + Height, Bg);
    return false;
  }

  xOffset = offset;
  shown = true;
  DestWidth = Width - xOffset;

  int textwidth = font->Width(text);
  int textheight = font->Height(text);
  if (textwidth < DestWidth)
    textwidth = DestWidth;
  if (textheight < Height)
    textheight = Height;

  // text is given, draw it
  Osd->DrawText(xOff + xOffset, yOff, text, Fg, Bg, font, DestWidth, textheight, Alignment);

  if (!DestWidth || !Active)
    return false;

  if (bmp)
    bmp->SetSize(textwidth, textheight);
  else
    bmp = new cBitmap(textwidth, textheight, ColorDepth);
  bmp->DrawText(0, 0, text, Fg, Bg, font, textwidth, textheight, Alignment);

  Time.Set();
  delay = Delay;
  return true;
}

int cEffectBlink::DrawIfNeeded(int yMax)
{
  //debug("cEffectBlink::DrawIfNeeded")

  changed = false;
  if (yMax != 0 && yMax < yOff) //TODO
    return delay;

  if (bmp) {
    int elapsed = ((int)Time.Elapsed());
    if (elapsed >= Delay) {
      if (shown) { //hide
        Osd->DrawRectangle(xOff, yOff, xOff + Width - 1, yOff + Height - 1, Bg);
        shown = false;

      } else { //show
        for (int i = 0; i < Width; i++) {
          for (int j = 0; j < bmp->Height(); j++) {
            Osd->DrawPixel(xOff + xOffset + i, yOff + j, bmp->Color(*(bmp->Data(i, j))));
          }
        }
        shown = true;
      }

      Time.Set();
      delay = Delay;
      changed = true;

    } else {
      delay = Delay - elapsed;
    }

  } else {
    delay = 0;
  }

  return delay;
}


// --- cEnigmaTextEffects -----------------------------------------------

cEnigmaTextEffects EnigmaTextEffects("EnigmaNG effects");

cEnigmaTextEffects::cEnigmaTextEffects(const char *Description) : cThread(Description), osd(NULL), condSleep(), mutexSleep(), mutexRunning()
{
//  SetPriority(19);
  sem_init(&sem_update, 0, 1);
}

cEnigmaTextEffects::~cEnigmaTextEffects(void)
{
//TODO?  Stop();
}

void cEnigmaTextEffects::Action(void)
{
  mutexRunning.Lock();
  mutexSleep.Lock();

  debug("cEnigmaTextEffects::Action() %lu", pthread_self());

  while (EnigmaConfig.useTextEffects && osd) {
    int nSleepMs = 5000;
    int tempSleep = 0;

    TE_LOCK; //This causes an initial wait until the first Flush() is called (which TE_UNKOCKs)
    bool changed = false;
    for (Effects_t::iterator effect = vecEffects.begin(); (effect != vecEffects.end()) && osd; ++effect) {
      cEffect *e = (*effect);
      if (e == NULL)
        continue;

      tempSleep = e->DrawIfNeeded(yMessageTop);
      changed = (changed || e->changed);
      //printf("TEMPSLEEP %d\n", tempSleep);
      nSleepMs = nSleepMs < tempSleep ? nSleepMs : (tempSleep <= 0 ? nSleepMs : tempSleep);
    }

    if (osd && changed)
      osd->Flush();
    TE_UNLOCK;

    if (osd) {
//      printf("SLEEP1: %d, %lu\n", nSleepMs, pthread_self());
      if (nSleepMs)
        condSleep.TimedWait(mutexSleep, nSleepMs);
      else
        condSleep.TimedWait(mutexSleep, EnigmaConfig.scrollPause); //TODO
//      printf("SLEEP2: %d, %lu\n", nSleepMs, pthread_self());
    }
  }

  mutexSleep.Unlock();
  mutexRunning.Unlock();
}

bool cEnigmaTextEffects::Start(cOsd *o)
{
  osd = o;

  if (!EnigmaConfig.useTextEffects)
    return false;

  debug("cEnigmaTextEffects::Start(%p) %lu", osd, pthread_self());

  if (osd == NULL)
    return false;

  if (Running()) {
    error("cEnigmaTextEffects::Start - already running");
    return false; //TODO? maybe Cancel()
  }

  yMessageTop = 0;

  TE_LOCK;
  return cThread::Start();
}

void cEnigmaTextEffects::Stop(void)
{
  //Must be TE_LOCKed by caller (calls TE_UNLOCK)

  debug("cEnigmaTextEffects::Stop()");
  osd = NULL;
  Clear();
  TE_UNLOCK;
  Wakeup(); // break sleeping Action() thread
  mutexRunning.Lock(); // Wait for Action() to finish
  mutexRunning.Unlock();
}

void cEnigmaTextEffects::Clear(void)
{
  debug("cEnigmaTextEffects::Clear()");

  //Must be TE_LOCKed by caller

  for (Effects_t::iterator effect = vecEffects.begin(); effect != vecEffects.end(); ++effect) {
    delete(*effect);
  }

  vecEffects.clear();
}

void cEnigmaTextEffects::PauseEffects(int y)
{
  debug("cEnigmaTextEffects::PauseEffects(%d)", y);

  //Must be TE_LOCKed by caller

  yMessageTop = y;
}

void cEnigmaTextEffects::ResetText(int i, tColor /* ColorFg */, tColor /* ColorBg */, bool /* fDraw */)
{
  debug("cEnigmaTextEffects::ResetText(%d)", i);

  //Must be TE_LOCKed by caller

  if (i < 0 || i >= (int)vecEffects.size())
    return;

  cEffect *e = vecEffects[i];
  if (e) {
    delete(e);
    vecEffects[i] = NULL;
  }
  if (i == (int)vecEffects.size() - 1)
    vecEffects.resize(vecEffects.size() - 1);
}

int cEnigmaTextEffects::DrawAnimatedText(int o_id, int action, bool _active, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int ColorDepth /* = 1 */, int Width /* = 0 */, int Height /* = 0 */, int Alignment /* = taDefault */)
{
  //Must be TE_LOCKed by caller

  if (osd == NULL)
    return -1;

  debug("cEnigmaTextEffects::DrawAnimatedText(%d, %d, %d, %d, %d, %s, %d, %d, %p, %d, %d, %d, %d)", o_id, action, _active, x, y, s, ColorFg, ColorBg, Font, ColorDepth, Width, Height, Alignment);

  if (o_id >= 0) {
    // Update animated text
    cEffect *effect = vecEffects[o_id];
    if (effect) {
      effect->SetText(s, Font);
      return o_id;
    } else {
      return -1;
    }
  } else {
    // New animated text
    cEffect *effect = NULL;
    if (action == 0)
      effect = new cEffectScroll(osd, x, y, Width, Height, ColorFg, ColorBg, Alignment, ColorDepth, _active);
    else
      effect = new cEffectBlink(osd, x, y, Width, Height, ColorFg, ColorBg, Alignment, ColorDepth, _active);
    if (effect == NULL) {
      return -1;
    }

    if (effect->SetText(s, Font)) {
      vecEffects.push_back(effect);
      return vecEffects.size() - 1;
    } else {
      delete effect;
      return -1;
    }
  }
}
// vim:et:sw=2:ts=2:
