#include "affichageaide.h"
#include "aideplayer.h"
#include <vdr/status.h>
#include <string>

char *mpgfile;

cAffichageAide::cAffichageAide(const char* selpath,const char* Title) : cOsdMenu (Title)
{
	asprintf(&mpgfile,"%s.%s",selpath,"mpg");
	FILE *fmpg = fopen(mpgfile,"r");
	if (fmpg)
	{
	    fclose(fmpg);
	    SetHelp(tr("StillImage"),NULL,NULL,NULL);
	}
	
	char *txtfile;
	asprintf(&txtfile,"%s.%s",selpath,"txt");

	FILE *f = fopen(txtfile,"r");
	free(txtfile);
	if (f)
	{
	        text = "";
		char buff[8192];
		std::string buff2 = "";
		cOsdItem *item;
		while (fgets(buff,sizeof(buff),f)) {
			if (!strcmp(buff,":HEADER\n"))
			{
				char *Header ="";
				char *Header2 ="";
				char *fgetresult=buff;
				asprintf(&Header2,"%s", "");

				while(fgetresult && strcmp(buff,":BODY\n"))
				{
					char *tmp;
					strtok(buff,"=");
					if (!strcmp(buff,"NAME"))
					{
						tmp=strtok(NULL,"=");
						asprintf(&Header,tr(" %sPlugin : %s"),Header,tmp);
						free(Header2);
						Header2=Header;
					}
					else if (!strcmp(buff,"VERSION"))
					{
						tmp=strtok(NULL,"=");
						asprintf(&Header,tr(" %sVersion : %s"),Header,tmp);
						free(Header2);
						Header2=Header;

					}
					else if (!strcmp(buff,"AUTOR"))
					{
						tmp=strtok(NULL,"=");
						asprintf(&Header,tr(" %sBy : %s"),Header,tmp);
						free(Header2);
						Header2=Header;
					}
					fgetresult=fgets(buff,sizeof(buff),f);
				}
				fgets(buff,sizeof(buff),f);
				item = new cOsdItem(Header);
				Add(item);
				free(Header);

			}

			std::string buff3 = "";
			char *buff3_cstr;
			std::string margin = "";
			bool ismargin=true;
			
			for (int i=0;i<(int) strlen(buff);i++)
			{
			    if (buff[i]!=' ') { ismargin=false; };
			    
			    if (ismargin)
			    {
			      margin += buff[1];
			    } else {
			      buff3 += buff[i];
			    }			
			}
			
	
			buff2 = margin;
			buff3_cstr = new char [buff3.size()+1];
			strcpy (buff3_cstr, buff3.c_str());
			strtok(buff3_cstr," ");
			while (buff3_cstr)
			{
			  if ((buff2.length()+strlen(buff3_cstr)+margin.length())<(Setup.OSDWidth/11))
				{
				  if (buff2.length()>margin.length())
					{
					  buff2 += " ";
					  buff2 += buff3_cstr;
					} else {
					  buff2 += buff3_cstr;
					}
				}
				else
				{
				  text += buff2;
				  text += "\n";
				  buff2 = margin;
				  buff2 += buff3_cstr;
				}
				buff3_cstr=strtok(NULL," ");
			}
			delete[] buff3_cstr;  
			text += buff2;

			buff2 = "";
			buff3 = "";
			margin = "";
		}

		fclose(f);
	}
}

cAffichageAide::~cAffichageAide(void)
{
  free(mpgfile);
}

void cAffichageAide::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetText(text.c_str(), false);
  cStatus::MsgOsdTextItem(text.c_str());
}

eOSState cAffichageAide::ProcessKey(eKeys Key)
{

  switch (Key) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
         DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
         cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp);
         return osContinue;
    default:
         break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
    case kOk:
      return osBack;
      
    case kRed:
      {
	FILE *fmpg = fopen(mpgfile,"r");
	if (fmpg)
	  {
	    fclose(fmpg);
	    CloseSubMenu();
	    cControl::Launch(new cAidePlayerControl(mpgfile));
	    return osEnd;
	  };
      };
      break;
    default:
      state = osContinue;
    }
    state = osContinue;
  }
  return state;
  
}
