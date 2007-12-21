/*
 * OSD Test plugin for the Video Disk Recorder
 *
 * osdtest256.c: main source file
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <vdr/plugin.h>

static const char *VERSION        = "0.1.0";
static const char *DESCRIPTION    = "256 color, full-screen OSD test pattern";
static const char *MAINMENUENTRY  = "256 color OSD test";


class cOsd256 : public cOsdObject {
private:
  cOsd *osd;
public:
  cOsd256(void) { osd = NULL; }
  ~cOsd256(void) { delete osd; }
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};

void cOsd256::Show(void)
{
  osd = cOsdProvider::NewOsd(60, 40);
  if (osd)
  {
      tArea Area = { 0, 0, 599, 499, 8 };
      osd->SetAreas(&Area, 1);
      osd->DrawRectangle(0, 0, 599, 499, clrGray50);

      for (int i = 0; i < 60; i++)
          osd->DrawRectangle(10*i,  0,10*(i+1), 99, 0xff000000+0x040404*(60-i));
      for (int i = 0; i < 60; i++)
          osd->DrawRectangle(10*i,100,10*(i+1),199, 0xff000000+((15+i*4)<<16));
      for (int i = 0; i < 60; i++)
          osd->DrawRectangle(10*i,200,10*(i+1),299, 0xff000000+((15+(60-i)*4)<<8));
      for (int i = 0; i < 60; i++)
          osd->DrawRectangle(10*i,300,10*(i+1),399, 0xff000000+(15+i*4));
      for (int i = 0; i < 60; i++)
          osd->DrawRectangle(10*i,400,10*(i+1),499, 0xff000000+0x040404*(60-i));

      osd->Flush();
  }
}

eOSState cOsd256::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown)
  {
      switch (Key & ~k_Repeat)
      {
          case kOk:
          case kBack:
 	      return osEnd;

          default:
              return state;
       }

       osd->Flush();
       state = osContinue;
   }

  return state;
}


class cPluginOsdTest256 : public cPlugin {
public:
  cPluginOsdTest256(void) {};
  virtual ~cPluginOsdTest256() {};
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void) { return new cOsd256; }
};


VDRPLUGINCREATOR(cPluginOsdTest256); // Don't touch this!
