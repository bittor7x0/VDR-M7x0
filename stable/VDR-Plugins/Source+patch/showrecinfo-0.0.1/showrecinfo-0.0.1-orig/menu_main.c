/*
 * menu_main.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <vdr/tools.h>
#include "menu_main.h"


const char* lineHeaders[]={"Name:",
                           "Path:",
                           "Date:",
                           "Length:",
                           "Priority:",
                           "Lifetime:",
                           "overall:",
                           ""};

char *FormatThousands(char *pstrText, long long llSize, int Unit)
{
   char strTmp[32];
   int i1,i2,m, y=0;
   int base=(Unit==1)? 1000:(Unit==2)? 1024:1;
   long long llSizeTmp=llSize;
static const char* strUnit[4];
   strUnit[0]=tr("Bytes");
   strUnit[1]="KB"; // should be the same in all languages
   strUnit[2]="MB";
   strUnit[3]="GB";

   if((Unit==1)||(Unit==2))
      for(y=0, llSizeTmp=llSize; (llSizeTmp>base)&&(y<4); y++)
         llSizeTmp=llSizeTmp/base;

   sprintf(strTmp, "%lld", llSizeTmp);
   m=strlen(strTmp)%3;

   for(i1=0,i2=0;i1<m;i1++,i2++) pstrText[i1]=strTmp[i2];

   while(strTmp[i2]) {
      if(i1)
         pstrText[i1++]='.';
      for(m=0;m<3;m++) pstrText[i1++]=strTmp[i2++];
   }
   pstrText[i1]='\0';
   if(Unit>-1) {
      strcat(pstrText, " ");
      strcat(pstrText, strUnit[y]);
   }
   return pstrText;
}

// --- cMenuShowInfoMain ---------------------------------------------------------

cMenuShowInfoMain::cMenuShowInfoMain(cRecording *pRecording)
:cOsdMenu(tr("additional info about recording"), 10)
{
  col1=2;
  font = cFont::GetFont(fontOsd);

  for(int i=0;*(lineHeaders[i]);i++)
     col1=(col1<strlen(lineHeaders[i]))? strlen(lineHeaders[i]): col1;
//  DisplayMenu()->SetCols(col1);

  iWidth=DisplayMenu()->EditableWidth()-font->Width("B")*5-font->Width(" ")*8;
  esyslog("Width: %d, %d, m=%d, col1: %d", DisplayMenu()->EditableWidth(), iWidth, font->Width("m"), 10);

  if(pRecording) {
     struct stat fileinfo;             /* Holds file information structure */
     char *cmd = NULL;
     char strTmp[256];
     asprintf(&cmd, "find '%s' -follow -type f -name '*.*'|sort ", pRecording->FileName());

     FILE *p = popen(cmd, "r");
     int ret=0;
     long long lSize=0;
     int FileCount=0;
     if (p) {
        char *s;
        cReadLine ReadLine;
        while ((s = ReadLine.Read(p)) != NULL) {
           if ((ret=stat(s, &fileinfo)) != -1) {
              lSize+=(long long)fileinfo.st_size;
              FileCount++;
              }
        }
     }
     pclose(p);
     delete cmd;

     cIndexFile *index = new cIndexFile(pRecording->FileName(), false);
     asprintf(&cmd, "%s, %s %s\n", *IndexToHMSF(index->Last()), FormatThousands(strTmp, index->Last(), -1), tr("Frames"));
     delete index;

     AddItem(tr(lineHeaders[0]), pRecording->Name());
     AddItem(tr(lineHeaders[1]), pRecording->FileName());
     AddItem(""); // a new line
     AddItem(tr(lineHeaders[2]), DayDateTime(pRecording->start));
     AddItem(tr(lineHeaders[3]), cmd);
     delete cmd;
     AddItem(tr(lineHeaders[4]), itoa(pRecording->priority));
     AddItem(tr(lineHeaders[5]), itoa(pRecording->lifetime));
     AddItem(""); // a new line
     AddItem(tr(lineHeaders[6]), FormatThousands(strTmp, lSize, 0));
  }
  else
     AddItem(tr("Sorry, no data found or no parameter"), "");

  SetHelpKeys();
}

cMenuShowInfoMain::~cMenuShowInfoMain()
{
}

#define SETCHAR(x, c) {*(x)=*(c);*(x+1)='\0';}

bool cMenuShowInfoMain::AddItem(const char *Titel, const char* Text)
{cMenuEditItem *pItem=new cMenuEditItem(Titel);

  esyslog("Titel: %s", Titel);
  esyslog("Text : %s", Text);
  Add(pItem);
  pItem->SetSelectable(false);

  if(Text && font->Width(Text)>iWidth) {
     char* pTemp=(char*)malloc((strlen(Text)+2)*sizeof(char));
     int i=1;

     SETCHAR(pTemp, Text);
     while(font->Width(pTemp)<iWidth) {
        SETCHAR(pTemp+i, Text+i);
        i++;
     }
     pItem->SetValue(pTemp);
esyslog("SetValue: %s", pTemp);
     if(strlen(Text+i)) // there are still some chars that does not fit into this line
        AddItem("", Text+i);
     free(pTemp);
  }
  else
     if(Text)
        pItem->SetValue(Text);

esyslog("leaving AddItem");
  return true;
}

eOSState cMenuShowInfoMain::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
	 case kGreen:  
	 case kYellow: 
	     break;
	 case kBlue:
	     break;
         case kOk: 
	     break;
	 default:
	     break;
     }
  }
  else if (!HasSubMenu()) {
     SetHelpKeys();
  }
  return state;
}

void cMenuShowInfoMain::SetHelpKeys(void)
{
  SetHelp(tr("More"), tr("More"), tr("Next"), tr("Switch"));
}

