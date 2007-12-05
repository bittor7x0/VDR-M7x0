/*
*  C Implementation: menu-accesscode
*
* Description: 
*
*
* Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2006
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include "menu-accesscode.h"
#include "menu-filebrowser.h"
#include <vdr/menuitems.h>

cOsdMenuAccessCode::cOsdMenuAccessCode(char* Code)
  : cOsdMenu(tr("Access code needed"), 10)
{
  this->Code=Code;
  this->CodeBuffer=(char*)malloc(strlen(Code)+1);
  this->CodeBuffer[0]='\0';
  for(int i=0; i < DisplayMenu()->MaxItems() / 2; i++)
  {
    Add(new cOsdItem("", osUnknown, false));
  }
  Add(new cMenuEditNumItem(tr("Code"), CodeBuffer, strlen(Code), true));
}

cOsdMenuAccessCode::~cOsdMenuAccessCode()
{
  if(CodeBuffer) free(CodeBuffer);
}

bool cOsdMenuAccessCode::CheckCode()
{
  return strcmp(this->Code, this->CodeBuffer) == 0;
}

eOSState cOsdMenuAccessCode::ProcessKey(eKeys Key)
{
  eOSState retval=cOsdMenu::ProcessKey(Key);
  
  switch(Key)
  {
    case kYellow:
    case kGreen:
    case kBlue:
    case kRed:
      return osContinue;
    case k0...k9:
      if(CheckCode())
      {
        return (eOSState)osFilebrowserAccessCodeOk;
      }
      break;
    case kOk:
      if(CheckCode())
      {
        return (eOSState)osFilebrowserAccessCodeOk;
      }
      else
      {
        Skins.Message(mtError, tr("Wrong code!"));
      }
      return osContinue;
    case kBack:
      return (eOSState)osFilebrowserCloseRefresh;
    default:
      return retval;
  }
  
  return retval;
}

