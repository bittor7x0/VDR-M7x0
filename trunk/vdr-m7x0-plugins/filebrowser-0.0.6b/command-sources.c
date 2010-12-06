/*
*  C Implementation: sources
*
* Description: 
*
*
* Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2005
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include "command-sources.h"
#include "menu-setup.h"

/*
  Implementation of cFilebrowserCommandSourcesList
*/

cFilebrowserCommandSourcesList::cFilebrowserCommandSourcesList(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Sources"));
  Sources=new cSourcesList();

  cConfigReader SourcesReader=cConfigReader(Statebag, "sources.conf");
  char* Line=NULL;
  while((Line=SourcesReader.Read()))
  {
    Sources->Add(new cSource(Line));
  }
}

cFilebrowserCommandSourcesList::~cFilebrowserCommandSourcesList()
{
  if(Sources) delete(Sources);
}

bool cFilebrowserCommandSourcesList::Execute(cOsdMenu* Menu, char* DestinationFile, char* SelectedFile)
{
  cOsdMenuFilebrowser* FbMenu=(cOsdMenuFilebrowser*)Menu;
  cOsdMenuSourcesList* List=new cOsdMenuSourcesList(Statebag, Sources);
  for(cSource* Source=Sources->First(); Source; Source=Sources->Next(Source))
  {
    List->Add(new cOsdItemSource(Statebag, Source));
  }
  FbMenu->AddSubMenu(List);
  return true;
}

bool cFilebrowserCommandSourcesList::Matches(const char* Filename)
{
  return true;
}

/*
  Implementation of cFilebrowserCommandSourcesList::cSource
*/

cFilebrowserCommandSourcesList::cSource::cSource(char* Line)
{
  cConfigParser Parser=cConfigParser(Line);
  for(char* Text=Parser.First(); Text; Text=Parser.Next())
  {
    switch(Parser.GetFieldNumber())
    {
      case 0:
        Name=Text;
        break;
      case 1:
        Path=Text;
        break;
      case 2:
        Filter=Text;
        break;
    }

  }
  D(fprintf(stderr, "[filebrowser] New source: \"%s\" in \"%s\" [%s]\n", *Name, *Path, *Filter));

  if(!*Name || !*Path)
  {
    D(fprintf(stderr, "[filebrowser] Misconfigured sources.conf: \"%s\"!\n", Line));
    esyslog("[filebrowser] Misconfigured sources.conf: \"%s\"!\n", Line);
    Name=*Name ? Name : "Error!";
    Path=*Path ? Path : "/";
  }
}

void cFilebrowserCommandSourcesList::cSource::SwitchTo(cFilebrowserStatebag* Statebag)
{
  D(fprintf(stderr, "[filebrowser] Setting new source: %s\n", *Path));
  sprintf(Statebag->BaseDir, Path);
  Statebag->CurrentFiles->Clear();
  Statebag->CurrentDirectory=Path;
  Statebag->Filter=Filter;
  Statebag->FilebrowserPlugin->SetupStore(SETUP_PARAM_BASEDIR, Path);
  Statebag->FilebrowserPlugin->SetupStore(SETUP_PARAM_FILTER, Filter);
}

/*
  Implementation of cOsdMenuSourcesList
*/

cOsdMenuSourcesList::cOsdMenuSourcesList(cFilebrowserStatebag* Statebag, cFilebrowserCommandSourcesList::cSourcesList* Sources) : cOsdMenu(tr("Sources"))
{
  this->Statebag=Statebag;
  this->Sources=Sources;
}


cOsdMenuSourcesList::~cOsdMenuSourcesList()
{}

/*
  Implementation of cOsdItemSource
*/

cOsdItemSource::cOsdItemSource(cFilebrowserStatebag* Statebag, cFilebrowserCommandSourcesList::cSource* Source)
{
  this->Statebag=Statebag;
  this->Source=Source;
  SetText(Source->Name);
}

eOSState cOsdItemSource::ProcessKey(eKeys Key)
{
  if(Key==kOk)
  {
    Source->SwitchTo(Statebag);
    return (eOSState)osFilebrowserCloseRefresh;
  }
  else
  {
    return cOsdItem::ProcessKey(Key);
  }
}

