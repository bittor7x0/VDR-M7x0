/********************************************************************************
 * csmenu.h channelscan menu part
 * writen by markus Hahn @ reel-multimedia
 *
 * See REDME for copyright information and
 * how to reach the authors.
 *
 ********************************************************************************/

#ifndef __SERVICE_H
#define __SERVICE_H


#include <vdr/osdbase.h>
#include <string>
#include <vector>

// --- sSelectChannelsMenu  ----------------------------------------------------

struct sSelectChannelsMenu
{
  // in 
  // out
  cOsdMenu* pSelectMenu;   // pointer to the select channels menu
};


/*
class cAutoScan 
{
public:

  void LoadSources();
 
private:
  std::vector<int> v_sources;

};
*/


#endif
