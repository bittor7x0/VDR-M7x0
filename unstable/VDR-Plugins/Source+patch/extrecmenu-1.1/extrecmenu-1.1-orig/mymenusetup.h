#include <vdr/menu.h>

class mySetup
{
 public:
  mySetup();
  int ShowRecDate;
  int ShowRecTime;
  int ShowRecLength;
  int HideMainMenuEntry;
  int ReplaceOrgRecMenu;
  int PatchNew;
  int ShowNewRecs;
  int DescendSorting;
  int GoLastReplayed;
  int ReturnToPlugin;
  int LimitBandwidth;
  int UseVDRsRecInfoMenu;
  int PatchFont;
  int FileSystemFreeMB;
  int UseCutterQueue;
};

extern mySetup mysetup;

class myMenuSetup:public cMenuSetupPage
{
 private:
  const char *sortingtypetexts[2];
  int showrecdate;
  int showrectime;
  int showreclength;
  int hidemainmenuentry;
  int replaceorgrecmenu;
  int patchnew;
  int shownewrecs;
  int descendsorting;
  int golastreplayed;
  int returntoplugin;
  int limitbandwidth;
  int usevdrsrecinfomenu;
  int patchfont;
  int filesystemfreemb;
  int usecutterqueue;
 protected:
  virtual void Store();
 public:
  myMenuSetup();
};
