#include "affichageaide.h"
#include "aideplayer.h"

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
	if (f)
	{

		char buff[8192];
		char *buff2;
		cOsdItem *item;
		while (fgets(buff,sizeof(buff),f)) {
			if (!strcmp(buff,":HEADER\n"))
			{
				char *Header;
				asprintf(&Header,"%s","");
				char *fgetresult=buff;
				while(fgetresult && strcmp(buff,":BODY\n"))
				{
					char *tmp;
					strtok(buff,"=");
					if (!strcmp(buff,"NAME"))
					{
						tmp=strtok(NULL,"=");
						asprintf(&Header,tr(" %sPlugin : %s"),Header,tmp);
					}
					else if (!strcmp(buff,"VERSION"))
					{
						tmp=strtok(NULL,"=");
						asprintf(&Header,tr(" %sVersion : %s"),Header,tmp);
					}
					else if (!strcmp(buff,"AUTOR"))
					{
						tmp=strtok(NULL,"=");
						asprintf(&Header,tr(" %sBy : %s"),Header,tmp);
					}
					fgetresult=fgets(buff,sizeof(buff),f);
				}
				fgets(buff,sizeof(buff),f);
				item = new cOsdItem(Header);
				Add(item);

			}
			/*int loop = ((int)strlen(buff)/60);
			int nbr=0;
			for (int i=0; i < (loop + 1);i++)
			{
				i<loop ? nbr=60 : nbr=strlen(buff)%60 ; 
				strncpy(buff2,buff+(60*i),nbr );
				buff2[nbr]='\0';
				item = new cOsdItem(buff2);
				Add(item);	
			}*/
			
			char *buff3="";
			char *margin="";
			bool ismargin=true;
			
			for (int i=0;i<(int) strlen(buff);i++)
			{
			    if (buff[i]!=' ') { ismargin=false; };
			    
			    if (ismargin)
			    {
				asprintf(&margin,"%s%c",margin,buff[i]);
			    } else {
				asprintf(&buff3,"%s%c",buff3,buff[i]);
			    }			
			}
			
	
			asprintf(&buff2,"%s",margin);
			strtok(buff3," ");
			while (buff3)
			{
				if ((strlen(buff2)+strlen(buff3)+strlen(margin))<(Setup.OSDWidth/10))
				{
					if (strlen(buff2)>strlen(margin))
					{
					    asprintf(&buff2,"%s %s",buff2,buff3);
					} else {
					    asprintf(&buff2,"%s%s",buff2,buff3);
					}
				}
				else
				{
					item=new cOsdItem(buff2);
					Add(item);
					asprintf(&buff2,"%s%s",margin,buff3);
				}
				buff3=strtok(NULL," ");
			}	
			item=new cOsdItem(buff2);
			Add(item);
			asprintf(&buff2,"%s","");
		}
		fclose(f);
	}
}

cAffichageAide::~cAffichageAide(void)
{
}

eOSState cAffichageAide::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
		switch(Key)
		{
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
			default : break;
		}
	return state;
}
