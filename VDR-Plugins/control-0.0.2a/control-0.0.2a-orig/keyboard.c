/*
 * gateway.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE


#include "global.h"
#include "keyboard.h"



struct tKbdMap {
  eKbdFunc func;
  uint64 code;
};



static const tKbdMap keyboardKbdMap[] = {
  { kfF1,     0x0000001B5B31317EULL },
  { kfF1,     0x000000001B5B5B41ULL },
  { kfF1,     0x00000000009B5B41ULL },
  { kfF1,     0x0000000000001B31ULL },

  { kfF2,     0x0000001B5B31327EULL },
  { kfF2,     0x000000001B5B5B42ULL },
  { kfF2,     0x00000000009B5B42ULL },
  { kfF2,     0x0000000000001B32ULL },

  { kfF3,     0x0000001B5B31337EULL },
  { kfF3,     0x000000001B5B5B43ULL },
  { kfF3,     0x00000000009B5B43ULL },
  { kfF3,     0x0000000000001B33ULL },

  { kfF4,     0x0000001B5B31347EULL },
  { kfF4,     0x000000001B5B5B44ULL },
  { kfF4,     0x00000000009B5B44ULL },
  { kfF4,     0x0000000000001B34ULL },

  { kfF5,     0x0000001B5B31357EULL },
  { kfF5,     0x000000001B5B5B45ULL },
  { kfF5,     0x00000000009B5B45ULL },
  { kfF5,     0x0000000000001B35ULL },

  { kfF6,     0x0000001B5B31377EULL },
  { kfF6,     0x000000001B5B5B47ULL },
  { kfF6,     0x00000000009B5B47ULL },
  { kfF6,     0x0000000000001B36ULL },

  { kfF7,     0x0000001B5B31387EULL },
  { kfF7,     0x000000001B5B5B48ULL },
  { kfF7,     0x00000000009B5B48ULL },
  { kfF7,     0x0000000000001B37ULL },

  { kfF8,     0x0000001B5B31397EULL },
  { kfF8,     0x000000001B5B5B49ULL },
  { kfF8,     0x00000000009B5B49ULL },
  { kfF8,     0x0000000000001B38ULL },

  { kfF9,     0x0000001B5B32307EULL },
  { kfF9,     0x000000001B5B5B3AULL },
  { kfF9,     0x00000000009B5B3AULL },
  { kfF9,     0x0000000000001B39ULL },

  { kfF10,    0x0000001B5B32317EULL },
  { kfF10,    0x000000001B5B5B3BULL },
  { kfF10,    0x00000000009B5B3BULL },
  { kfF10,    0x0000000000001B3AULL },

  { kfF11,    0x0000001B5B32327EULL },
  { kfF11,    0x000000001B5B5B3DULL },
  { kfF11,    0x00000000009B5B3DULL },

  { kfF12,    0x0000001B5B32337EULL },
  { kfF12,    0x000000001B5B5B3EULL },
  { kfF12,    0x00000000009B5B3EULL },

  { kfUp,     0x00000000001B5B41ULL },
  { kfDown,   0x00000000001B5B42ULL },
  { kfLeft,   0x00000000001B5B44ULL },
  { kfRight,  0x00000000001B5B43ULL },
  { kfHome,   0x00000000001B5B48ULL },
  { kfEnd,    0x00000000001B5B46ULL },
  { kfPgUp,   0x000000001B5B357EULL },
  { kfPgDown, 0x000000001B5B367EULL },
  { kfIns,    0x000000001B5B327EULL },
  { kfDel,    0x000000001B5B337EULL },
  { kfNone,   0x0000000000000000ULL }
  };





cCtrlKeyboard* cCtrlKeyboard::_pRemote = NULL;



cCtrlKeyboard::cCtrlKeyboard()
//: cRemote("CONTROL-KBD")
{
conLOG_KEYB("cCtrlKeyboard constructed %s", Name());
}



cCtrlKeyboard::~cCtrlKeyboard()
{
}



bool cCtrlKeyboard::intPut(uint64 Command, bool Repeat, bool Release) {

  bool ret = cRemote::Put(Command, Repeat, Release);
conLOG_KEYB("%s cKbdRemoteForControl::intPut(0x%016LX)", ret ? "true" : "false", Command);
  return ret;
}



bool cCtrlKeyboard::Put(uint64 Command, bool Repeat, bool Release) {

  if (! _pRemote)
    _pRemote = new cCtrlKeyboard();

  return _pRemote->intPut(Command, Repeat, Release);
}



int cCtrlKeyboard::MapCodeToFunc(uint64 Code) {

  for (const tKbdMap *p = keyboardKbdMap; p->func != kfNone; p++) {
    if (p->code == Code)
       return p->func;
  }
  return (Code <= 0xFF) ? Code : kfNone;
}




/*
bool cInterface::QueryKeys(cRemote *Remote, cSkinDisplayMenu *DisplayMenu) {
  DisplayMenu->SetItem(tr("Phase 1: Detecting RC code type"), 1, false, false);
  DisplayMenu->SetItem(tr("Press any key on the RC unit"), 3, false, false);
  DisplayMenu->Flush();
  if (Remote->Initialize()) {
     DisplayMenu->SetItem(tr("RC code detected!"), 3, false, false);
     DisplayMenu->SetItem(tr("Do not press any key..."), 4, false, false);
     DisplayMenu->Flush();
     sleep(3);
     DisplayMenu->SetItem("", 3, false, false);
     DisplayMenu->SetItem("", 4, false, false);
     DisplayMenu->SetItem(tr("Phase 2: Learning specific key codes"), 1, false, false);
     eKeys NewKey = kUp;
     while (NewKey != kNone) {
           char *Prompt;
           asprintf(&Prompt, tr("Press key for '%s'"), tr(cKey::ToString(NewKey)));
           DisplayMenu->SetItem(Prompt, 3, false, false);
           free(Prompt);
           cRemote::Clear();
           DisplayMenu->Flush();
           for (eKeys k = NewKey; k == NewKey; ) {
               char *NewCode = NULL;
               eKeys Key = cRemote::Get(100, &NewCode);
               switch (Key) {
                 case kUp: if (NewKey > kUp) {
                                NewKey = eKeys(NewKey - 1);
                                cKey *last = Keys.Last();
                                if (last && last->Key() == NewKey)
                                   Keys.Del(last);
                                }
                             break;
                 case kDown: DisplayMenu->SetItem(tr("Press 'Up' to confirm"), 3, false, false);
                             DisplayMenu->SetItem(tr("Press 'Down' to continue"), 4, false, false);
                             DisplayMenu->SetItem("", 5, false, false);
                             DisplayMenu->SetItem("", 6, false, false);
                             DisplayMenu->SetItem("", 7, false, false);
                             DisplayMenu->Flush();
                             for (;;) {
                                 Key = cRemote::Get(100);
                                 if (Key == kUp) {
                                    DisplayMenu->Clear();
                                    return true;
                                    }
                                 else if (Key == kDown) {
                                    DisplayMenu->SetItem("", 4, false, false);
                                    k = kNone; // breaks the outer for() loop
                                    break;
                                    }
                                 }
                             break;
                 case kMenu: NewKey = eKeys(NewKey + 1);
                             break;
                 case kNone: if (NewCode) {
                                dsyslog("new %s code: %s = %s", Remote->Name(), NewCode, cKey::ToString(NewKey));
                                Keys.Add(new cKey(Remote->Name(), NewCode, NewKey));
                                NewKey = eKeys(NewKey + 1);
                                free(NewCode);
                                }
                             break;
                 default: break;
                 }
               }
           if (NewKey > kUp)
              DisplayMenu->SetItem(tr("(press 'Up' to go back)"), 5, false, false);
           else
              DisplayMenu->SetItem("", 5, false, false);
           if (NewKey > kDown)
              DisplayMenu->SetItem(tr("(press 'Down' to end key definition)"), 6, false, false);
           else
              DisplayMenu->SetItem("", 6, false, false);
           if (NewKey > kMenu)
              DisplayMenu->SetItem(tr("(press 'Menu' to skip this key)"), 7, false, false);
           else
              DisplayMenu->SetItem("", 7, false, false);
           }
     return true;
     }
  return false;
}



void cInterface::LearnKeys(void) {
  for (cRemote *Remote = Remotes.First(); Remote; Remote = Remotes.Next(Remote)) {
      if (!Remote->Ready()) {
         esyslog("ERROR: remote control %s not ready!", Remote->Name());
         continue;
         }
      bool known = Keys.KnowsRemote(Remote->Name());
      dsyslog("remote control %s - %s", Remote->Name(), known ? "keys known" : "learning keys");
      if (!known) {
         cSkinDisplayMenu *DisplayMenu = Skins.Current()->DisplayMenu();
         char Headline[256];
         snprintf(Headline, sizeof(Headline), tr("Learning Remote Control Keys (%s)"), Remote->Name());
         cRemote::Clear();
         DisplayMenu->SetTitle(Headline);
         cRemote::SetLearning(Remote);
         bool rc = QueryKeys(Remote, DisplayMenu);
         cRemote::SetLearning(NULL);
         DisplayMenu->Clear();
         if (!rc) {
            delete DisplayMenu;
            continue;
            }
         DisplayMenu->SetItem(tr("Phase 3: Saving key codes"), 1, false, false);
         DisplayMenu->SetItem(tr("Press 'Up' to save, 'Down' to cancel"), 3, false, false);
         for (;;) {
             eKeys key = GetKey();
             if (key == kUp) {
                Keys.Save();
                delete DisplayMenu;
                break;
                }
             else if (key == kDown) {
                Keys.Load();
                delete DisplayMenu;
                break;
                }
             }
         }
      }
}
*/

