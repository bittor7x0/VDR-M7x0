/*
 * prefersetup.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include "config.h"
#include "i18n.h"

#include "prefersetup.h"

static const char *SortOpt[] = { "As entered", "None", "Name", NULL };

// --- cMenuEditSortByItem --------------------------------------------------

class cMenuEditSortByItem : public cMenuEditIntItem {
protected:
  virtual void Set(void);
public:
  cMenuEditSortByItem(const char *Name, int *Value);
  };

cMenuEditSortByItem::cMenuEditSortByItem(const char *Name, int *Value)
:cMenuEditIntItem(Name, Value, 1, 3)
{
  Set();
}

void cMenuEditSortByItem::Set(void)
{
  char buf[255];
  snprintf(buf, sizeof(buf), "%s", tr(SortOpt[*value-1]));
  SetValue(buf);
}

// --- cMenuEditNumChanItem --------------------------------------------------
//
class cMenuEditNumChanItem : public cMenuEditIntItem {
protected:
  virtual void Set(void);
public:
  cMenuEditNumChanItem(const char *Name, int *Value);
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuEditNumChanItem::cMenuEditNumChanItem(const char *Name, int *Value)
:cMenuEditIntItem(Name, Value, 3, 7)
{
  Set();
}

void cMenuEditNumChanItem::Set(void)
{
  char buf[16];
  snprintf(buf, sizeof(buf), "%d", *value);
  SetValue(buf);
}

eOSState cMenuEditNumChanItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     int newValue = *value;
     Key = NORMALKEY(Key);
     switch (Key) {
       case kNone: break;
       case kLeft: 
            newValue = *value - 2;
            fresh = true;
            break;
       case kRight:
            newValue = *value + 2;
            fresh = true;
            break;
       default:
            if (*value < 3) { *value = 3; Set(); }
            if (*value > 7) { *value = 7; Set(); }
            return state;
       }
     if ((!fresh || 3 <= newValue) && newValue <= 7) {
        *value = newValue;
        Set();
        }
     state = osContinue;
     }
  return state;
}

cPreferSetup::cPreferSetup(void)
{
  //Red         = config.red;
  //Green       = config.green;
  //Blue        = config.blue;
  Originx     = config.originx;
  Originy     = config.originy;
  Width       = config.width;
  Height      = config.height;
  //Alpha1      = config.alpha1;
  //Alpha2      = config.alpha2;
  //AlphaBorder = config.alphaborder;
  CloseOnSwitch = config.closeonswitch;
  SortBy      = config.sortby;
  Lines       = config.lines;
  
//  Add(new cMenuEditIntItem(tr("Red"),                  &Red, 0, 255));
//  Add(new cMenuEditIntItem(tr("Green"),                &Green, 0, 255));
//  Add(new cMenuEditIntItem(tr("Blue"),                 &Blue, 0, 255));
  Add(new cMenuEditIntItem(tr("Origin X"),             &Originx, 0, 768));
  Add(new cMenuEditIntItem(tr("Origin Y"),             &Originy, 0, 576));
  Add(new cMenuEditIntItem(tr("Width"),                &Width, 0, 768));
  Add(new cMenuEditNumChanItem(tr("Number of channels"),&Lines));
//  Add(new cMenuEditIntItem(tr("Background opacity"),   &Alpha1, 0, 255));
//  Add(new cMenuEditIntItem(tr("Selected channel opacity"), &Alpha2, 0, 255));
//  Add(new cMenuEditIntItem(tr("Border opacity"),       &AlphaBorder, 0, 255));
  Add(new cMenuEditBoolItem(tr("Close on switch"),     &CloseOnSwitch));
  Add(new cMenuEditSortByItem(tr("Sort by"),           &SortBy));
}

void cPreferSetup::Store(void)
{
  //SetupStore("Red",         config.red = Red);
  //SetupStore("Green",       config.green = Green);
  //SetupStore("Blue",        config.blue = Blue);
  SetupStore("OriginX",     config.originx = Originx);
  SetupStore("OriginY",     config.originy = Originy);
  SetupStore("Width",       config.width = Width);
  SetupStore("Lines",       config.lines = Lines);
  //SetupStore("Alpha1",      config.alpha1 = Alpha1);
  //SetupStore("Alpha2",      config.alpha2 = Alpha2);
  //SetupStore("AlphaBorder", config.alphaborder = AlphaBorder);
  SetupStore("CloseOnSwitch", config.closeonswitch = CloseOnSwitch);
  SetupStore("SortBy",      config.sortby = SortBy);
}
