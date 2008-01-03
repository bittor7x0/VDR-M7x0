/*
 * See the README file for copyright information and how to reach the author.
 */

#include <vdr/menu.h>
#include "mymenusetup.h"

mySetup::mySetup()
{
  mysetup.HideMainMenuEntry=0;
  mysetup.PatchNew=1;
  mysetup.ReplaceOrgRecMenu=0;
  mysetup.ShowRecDate=1;
  mysetup.ShowRecTime=1;
  mysetup.ShowRecLength=0;
  mysetup.ShowNewRecs=1;
  mysetup.DescendSorting=0;
  mysetup.GoLastReplayed=0;
  mysetup.ReturnToPlugin=1;
  mysetup.LimitBandwidth=0;
  mysetup.UseVDRsRecInfoMenu=0;
  mysetup.PatchFont=1;
  mysetup.FileSystemFreeMB=1;
  mysetup.UseCutterQueue=1;
}

mySetup mysetup;

myMenuSetup::myMenuSetup()
{
  SetCols(36);
  
  hidemainmenuentry=mysetup.HideMainMenuEntry;
  patchnew=mysetup.PatchNew;
  replaceorgrecmenu=mysetup.ReplaceOrgRecMenu;
  showrecdate=mysetup.ShowRecDate;
  showrectime=mysetup.ShowRecTime;
  showreclength=mysetup.ShowRecLength;
  shownewrecs=mysetup.ShowNewRecs;
  descendsorting=mysetup.DescendSorting;
  golastreplayed=mysetup.GoLastReplayed;
  returntoplugin=mysetup.ReturnToPlugin;
  limitbandwidth=mysetup.LimitBandwidth;
  usevdrsrecinfomenu=mysetup.UseVDRsRecInfoMenu;
  patchfont=mysetup.PatchFont;
  filesystemfreemb=mysetup.FileSystemFreeMB;
  usecutterqueue=mysetup.UseCutterQueue;
 
  sortingtypetexts[0]=tr("ascending");
  sortingtypetexts[1]=tr("descending");
 
  Add(new cMenuEditBoolItem(tr("Show recording date"),&showrecdate));
  Add(new cMenuEditBoolItem(tr("Show recording time"),&showrectime));
  Add(new cMenuEditBoolItem(tr("Show recording length"),&showreclength));
  Add(new cMenuEditBoolItem(tr("Show nr. of new recordings of a directory"),&shownewrecs));
  Add(new cMenuEditBoolItem(tr("Show alternative to new marker"),&patchnew));
  Add(new cMenuEditBoolItem(tr("Show free disk space for each file system"),&filesystemfreemb));
  Add(new cMenuEditStraItem(tr("Sorting"),&descendsorting,2,sortingtypetexts));
  Add(new cMenuEditBoolItem(tr("Hide main menu entry"),&hidemainmenuentry));
#ifdef MAINMENUHOOKSVERSNUM
  Add(new cMenuEditBoolItem(tr("Replace original recordings menu"),&replaceorgrecmenu));
#endif
  Add(new cMenuEditBoolItem(tr("Jump to last replayed recording"),&golastreplayed));
  Add(new cMenuEditBoolItem(tr("Call plugin after playback"),&returntoplugin));
#if VDRVERSNUM < 10503
  Add(new cMenuEditBoolItem(tr("Patch font"),&patchfont));
#endif
  Add(new cMenuEditBoolItem(tr("Limit bandwidth for move recordings"),&limitbandwidth));
  Add(new cMenuEditBoolItem(tr("Use VDR's recording info menu"),&usevdrsrecinfomenu));
  Add(new cMenuEditBoolItem(tr("Use cutter queue"),&usecutterqueue));
}

void myMenuSetup::Store()
{
  SetupStore("HideMainMenuEntry",mysetup.HideMainMenuEntry=hidemainmenuentry);
  SetupStore("PatchNew",mysetup.PatchNew=patchnew);
  SetupStore("ReplaceOrgRecMenu",mysetup.ReplaceOrgRecMenu=replaceorgrecmenu);
  SetupStore("ShowRecDate",mysetup.ShowRecDate=showrecdate);
  SetupStore("ShowRecTime",mysetup.ShowRecTime=showrectime);
  SetupStore("ShowRecLength",mysetup.ShowRecLength=showreclength);
  SetupStore("ShowNewRecs",mysetup.ShowNewRecs=shownewrecs);
  SetupStore("DescendSorting",mysetup.DescendSorting=descendsorting);
  SetupStore("GoLastReplayed",mysetup.GoLastReplayed=golastreplayed);
  SetupStore("ReturnToPlugin",mysetup.ReturnToPlugin=returntoplugin);
  SetupStore("LimitBandwidth",mysetup.LimitBandwidth=limitbandwidth);
  SetupStore("UseVDRsRecInfoMenu",mysetup.UseVDRsRecInfoMenu=usevdrsrecinfomenu);
  SetupStore("PatchFont",mysetup.PatchFont=patchfont);
  SetupStore("FileSystemFreeMB",mysetup.FileSystemFreeMB=filesystemfreemb);
  SetupStore("UseCutterQueue",mysetup.UseCutterQueue=usecutterqueue);
}

