#include "common.h"
#include "config.h"
#include "texteffects.h"
#include "tools.h"

#include <algorithm>

#ifndef DISABLE_ANIMATED_TEXT
//Redefine macros
#undef  TE_LOCK
#define TE_LOCK   UpdateLock()
#undef  TE_UNLOCK
#define TE_UNLOCK UpdateUnlock()
#endif

#ifdef HAVE_FREETYPE
// needed for case-insensitive sort of vector (for fonts)
struct NoCase {
  bool operator()(const std::string& x, const std::string& y) {
    std::string lv(x);
    std::string rv(y);
    lcase(lv);
    lcase(rv);
    return lv < rv;
  }

  void lcase(std::string& s) {
    int n = s.size();
    for(int i = 0; i < n; i++)
      s[i] = tolower(s[i]);
  }
};
#endif


#ifdef DISABLE_ANIMATED_TEXT
cEnigmaTextEffects EnigmaTextEffects;
#else
cEnigmaTextEffects EnigmaTextEffects("EnigmaNG effects");
#endif

#ifdef DISABLE_ANIMATED_TEXT
cEnigmaTextEffects::cEnigmaTextEffects(void) : osd(NULL)
#else
cEnigmaTextEffects::cEnigmaTextEffects(const char *Description) : cThread(Description), osd(NULL), condSleep(), mutexSleep(), mutexRunning()
#endif
{
//  SetPriority(19);

#ifdef HAVE_FREETYPE
  availTTFs = NULL;
  nMaxTTFs = 0;
#endif
}

cEnigmaTextEffects::~cEnigmaTextEffects(void)
{
#ifndef DISABLE_ANIMATED_TEXT
//TODO?  Stop();
#endif

#ifdef HAVE_FREETYPE
  if (availTTFs) {
    char **ptr = availTTFs;
    while (*ptr) {
      delete(*ptr);
      ptr++;
    }
    free(availTTFs);
    availTTFs = NULL;
  }
#endif
}

#ifndef DISABLE_ANIMATED_TEXT
void cEnigmaTextEffects::Action(void)
{
  mutexRunning.Lock();
  mutexSleep.Lock();

  debug("cEnigmaTextEffects::Action() %lu", pthread_self());

  while (EnigmaConfig.useTextEffects && osd) {
    uint64_t nNow = cTimeMs::Now();
    int nSleepMs = 0;

    TE_LOCK; //This causes an initial wait until thet first Flush() is called (which TE_UNKOCKs)
    for (tEffects::iterator effect = vecEffects.begin(); (effect != vecEffects.end()) && osd; effect++) {
      tEffect *e = (*effect);
      if (e == NULL)
        continue;

      if (e->nNextUpdate == 0) {
        e->nNextUpdate = nNow + (e->nAction == 0 ? EnigmaConfig.scrollPause : EnigmaConfig.blinkPause);
      } else if(nNow >= e->nNextUpdate) {
        DoEffect(e, nNow);
      }

//      printf("NOW=%llu NEXT=%llu DIFF=%d SLEEP=%d\n", nNow, e->nNextUpdate, (int)(e->nNextUpdate - nNow), nSleepMs);
      int nDiff = std::max(3, (int)(e->nNextUpdate - nNow));
      if (nSleepMs == 0 || nDiff < nSleepMs)
        nSleepMs = nDiff;
    }

    if (osd)
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

void cEnigmaTextEffects::DoEffect(tEffect *e, uint64_t nNow)
{
  bool fDrawItem = ((yMessageTop == 0) || (e->y + e->Height < yMessageTop));

  switch (e->nAction) {
    case 0:  // Scroll
      DoScroll(e, nNow, fDrawItem);
      break;

    case 1: // Blink
      DoBlink(e, nNow, fDrawItem);
      break;
  }
}

void cEnigmaTextEffects::DoScroll(tEffect *e, uint64_t nNow, bool fDrawItem)
{
//  debug("cEnigmaTextEffects::DoScroll()");
  const cFont *Font = EnigmaConfig.GetFont(e->FontId, e->Font);
  if (Font->Width(e->strText.c_str()) <= e->Width) {
    if (fDrawItem) {
      if (e->Skin)
        e->Skin->DrawTitle(e->strText.c_str());
      else
        osd->DrawText(e->x, e->y, e->strText.c_str(), e->ColorFg, e->ColorBg, Font, e->Width, e->Height, e->Alignment);
    }

    if (nNow)
      e->nNextUpdate = nNow + EnigmaConfig.scrollPause;
    return;
  }

  if (nNow) {
    int nDelay = EnigmaConfig.scrollDelay;
    if (fDrawItem) {
      switch (e->nDirection) {
        case 0: // Scroll from left to right
          if (Font->Width(e->strText.c_str() + e->nOffset) <= e->Width) {
            if (EnigmaConfig.scrollMode)
              e->nDirection = 2;
            else
              e->nDirection = 1;
            nDelay = EnigmaConfig.scrollPause;
          } else if (e->nOffset < e->strText.length())
            e->nOffset++;
          break;

        case 1: // Scroll from right to left
          if (e->nOffset > 0)
            e->nOffset--;
          if (e->nOffset <= 0) {
            e->nDirection = false;
            nDelay = EnigmaConfig.scrollPause;
          }
          break;
  
        case 2: // Restart scrolling from the left
          nDelay = EnigmaConfig.scrollPause;
          e->nOffset = 0;
          e->nDirection = 0;
          break;
      }
    }
    e->nNextUpdate = nNow + nDelay;
  }

  if (fDrawItem) {
//    printf("SCROLL: %d %d %d/%d (%s) %d %lu %lu\n", e->nOffset, e->nDirection, Font->Width(e->strText.c_str() + e->nOffset), e->Width, e->strText.c_str() + e->nOffset, e->strText.length(), nNow, e->nNextUpdate);
    if (e->Skin)
      e->Skin->DrawTitle(e->strText.c_str() + e->nOffset);
    else
      osd->DrawText(e->x, e->y, e->strText.c_str() + e->nOffset, e->ColorFg, e->ColorBg, Font, e->Width, e->Height);
  }
}

void cEnigmaTextEffects::DoBlink(tEffect *e, uint64_t nNow, bool fDrawItem)
{
//  debug("cEnigmaTextEffects::DoBlink()");
  const cFont *Font = EnigmaConfig.GetFont(e->FontId, e->Font);
  if (fDrawItem) {
    if (nNow) {
      e->nDirection = (e->nDirection == 0 ? 1 : 0);
      e->nNextUpdate = nNow + EnigmaConfig.blinkPause;
    }
    if (e->nDirection == 1)
      osd->DrawText(e->x, e->y, e->strText.c_str() + e->nOffset, e->ColorFg, e->ColorBg, Font, e->Width, e->Height, e->Alignment);
    else
      osd->DrawText(e->x, e->y, e->strText.c_str() + e->nOffset, e->ColorBg, e->ColorBg, Font, e->Width, e->Height, e->Alignment);
  } else {
    e->nNextUpdate = nNow + EnigmaConfig.blinkPause;
  }
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

  for (tEffects::iterator effect = vecEffects.begin(); effect != vecEffects.end(); effect++) {
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

void cEnigmaTextEffects::ResetText(int i, tColor ColorFg, tColor ColorBg, bool fDraw)
{
  debug("cEnigmaTextEffects::ResetText(%d)", i);

  //Must be TE_LOCKed by caller

  if (i < 0 || i >= (int)vecEffects.size())
    return;

  tEffect *e = vecEffects[i];
  if (e) {
    if (fDraw && osd) {
      const cFont *Font = EnigmaConfig.GetFont(e->FontId, e->Font);
      osd->DrawText(e->x, e->y, e->strText.c_str(),
                    ColorFg ? ColorFg : e->ColorFg,
                    ColorBg ? ColorBg : e->ColorBg,
                    Font, e->Width, e->Height);
    }
    delete(e);
    vecEffects[i] = NULL;
  }
  if (i == (int)vecEffects.size() - 1)
    vecEffects.resize(vecEffects.size() - 1);
}

void cEnigmaTextEffects::UpdateTextWidth(int i, int Width)
{
  debug("cEnigmaTextEffects::UpdateTextWidth(%d)", i);

  //Must be TE_LOCKed by caller

  if (i < 0 || i >= (int)vecEffects.size())
    return;

  tEffect *e = vecEffects[i];
  if (e) {
    e->Width = Width;
  }
}

int cEnigmaTextEffects::DrawAnimatedTitle(int o_id, int action, const char *s, int Width, cSkinEnigmaOsd *skin)
{
  //Must be TE_LOCKed by caller

  if (osd == NULL || skin == NULL)
    return -1;

  debug("cEnigmaTextEffects::DrawAnimatedTitle(%d, %d, %s)", o_id, EnigmaConfig.useTextEffects, s);

  if (o_id >= 0) {
    // Update animated text
    tEffect *effect = vecEffects[o_id];
    if (effect) {
      if (s == NULL)
        effect->strText = "";
      else if (strcmp(effect->strText.c_str(), s) != 0) {
        effect->strText = s;
        effect->nOffset = 0;
        effect->nDirection = 0;
      }
      DoEffect(effect);
      return o_id;
    } else {
      return -1;
    }
  } else {
    skin->DrawTitle(s);
    const cFont *Font = EnigmaConfig.GetFont(FONT_OSDTITLE);
    if (EnigmaConfig.useTextEffects && ((Font->Width(s ? s : "") > Width) || (action > 0))) {
      // New scrolling text
      tEffect *effect = new tEffect;
      if (effect == NULL) {
        return -1;
      }

      effect->nAction = action;
      effect->strText = std::string(s ? s : "");
      effect->Width = Width;
      effect->FontId = FONT_OSDTITLE;;
      effect->Font = Font;
      effect->Skin = skin;
      vecEffects.push_back(effect);
      return vecEffects.size() - 1;
    } else {
      return -1;
    }
  }
}

int cEnigmaTextEffects::DrawAnimatedText(int o_id, int action, int x, int y, const char *s, tColor ColorFg, tColor ColorBg, int FontId, int Width, int Height, int Alignment)
{
  //Must be TE_LOCKed by caller

  if (osd == NULL)
    return -1;

  debug("cEnigmaTextEffects::DrawAnimatedText(%d, %d, %s)", o_id, EnigmaConfig.useTextEffects, s);

  if (o_id >= 0) {
    // Update animated text
    tEffect *effect = vecEffects[o_id];
    if (effect) {
      if (s == NULL)
        effect->strText = "";
      else if (strcmp(effect->strText.c_str(), s) != 0) {
        effect->strText = s;
        effect->nOffset = 0;
        effect->nDirection = 0;
      }
      DoEffect(effect);
      return o_id;
    } else {
      return -1;
    }
  } else {
    const cFont *Font = EnigmaConfig.GetFont(FontId);
    if (Height == 0)
      Height = Font->Height(s);
    osd->DrawText(x, y, s ? s : "", ColorFg, ColorBg, Font, Width, Height, Alignment);
    // New animated text
    tEffect *effect = new tEffect;
    if (effect == NULL) {
      return -1;
    }

    effect->nAction = action;
    effect->strText = std::string(s ? s : "");
    effect->x = x;
    effect->y = y;
    effect->Width = Width;
    effect->Height = Height;
    effect->ColorFg = ColorFg;
    effect->ColorBg = ColorBg;
    effect->FontId = FontId;
    effect->Font = Font;
    effect->Alignment = Alignment;
    vecEffects.push_back(effect);
    return vecEffects.size() - 1;
  }
}
#endif //DISABLE_ANIMATED_TEXT

#ifdef HAVE_FREETYPE
const char **cEnigmaTextEffects::GetAvailTTFs(void)
{
  if (availTTFs == NULL) {
    std::vector<std::string> vecFonts;
    cReadDir d(EnigmaConfig.GetFontsDir());
    struct dirent *e;
    while ((e = d.Next()) != NULL) {
      if ((strcmp(e->d_name, ".") != 0) && (strcmp(e->d_name, "..") != 0)) {
        if (strcmp(e->d_name + strlen(e->d_name) - 4, ".ttf") == 0) {
          debug("Loading %s", e->d_name);
          vecFonts.push_back(std::string(e->d_name));
        } else {
          error("Ignoring non-font file: %s", e->d_name);
        }
      }
    }

    if (vecFonts.size() > 0) {
      sort(vecFonts.begin(), vecFonts.end(), NoCase());
      availTTFs = (char **)calloc(vecFonts.size() + 1, sizeof(char*));
      if (availTTFs) {
        char **ptr = availTTFs;
        for (std::vector<std::string>::iterator i = vecFonts.begin(); i != vecFonts.end(); i++) {
          if (!(*i).empty()) {
            *ptr = strdup((*i).c_str());
            ptr++;
            nMaxTTFs++;
          }
        }
      }
    }

    vecFonts.clear();
  }

  return (const char**)availTTFs;
}
#endif
// vim:et:sw=2:ts=2:
