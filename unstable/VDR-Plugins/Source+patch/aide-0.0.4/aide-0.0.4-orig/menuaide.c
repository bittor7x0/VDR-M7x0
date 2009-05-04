#include "aide.h"
#include "menuaide.h"
#include "affichageaide.h"
#include "aideplayer.h"
#include <vdr/player.h>
#include <dirent.h>

char *AideFilesDir;

cMenuAide::cMenuAide(void) : cOsdMenu (tr("VDR Help"),20)
{
	Draw();
}

void cMenuAide::Draw(void)
{
	cOsdItem *item = new cOsdItem(tr("Available help topics:"));
	item->SetSelectable(false);
	Add(item);
	asprintf(&AideFilesDir,"%s/help/%s",cPlugin::ConfigDirectory(),tr("eng"));
	FindAide(AideFilesDir);
	SetCurrent(Get(1));
}

void cMenuAide::FindAide(const char *dir)
{
DIR *d = opendir(dir);
if(d)
{
	struct dirent *e;
	char *name;
	char *ext;
	char *aidefile;
	while ((e = readdir(d)) != NULL)
	{
		if (strcmp(e->d_name,".") && strcmp(e->d_name,"..") && strpbrk(e->d_name,"."))
		{
    			name=strtok(e->d_name,".");
			ext=strtok(NULL,".");				
			if (!strcmp(ext,"aide")) //find the index files
			{
			    asprintf (&aidefile,"%s/%s.%s",dir,name,ext);
			    FILE *f;
			    f=fopen(aidefile,"r");
			    if (f)
			    {
			        char buff[256];
				char *title;
				char *file;
				char *path;
				char *AideLine;
				while (fgets(buff,sizeof(buff),f))
			    	{
				    if (strpbrk(buff,"=")) //ignore lines without = 
				    {
    					title=strtok(buff,"=");
					file=strtok(NULL,"=");
					file=strtok(file,"\n"); //strip parasitic symbols
					asprintf (&path,"%s/%s.%s",dir,file,"txt"); //fullpath of the link

					FILE *ftest = fopen(path,"r"); //check if it is a real file
					if(ftest)
					{
    					    asprintf(&AideLine,"   %s",title); 
    					    Add(new cOsdItem(AideLine,osUser1));
					    fclose(ftest);
					}
				    } else { // Group entries in the Help menu
					if ((strlen(buff)>1) && (buff[0]==':'))
					{
					    title=strtok(buff,":");
					    cOsdItem *item = new cOsdItem(title);
					    item->SetSelectable(false);
					    Add(item);
					}
				    } 
			    	}
			    fclose(f);
			    }
			}
		}
	}
}
}

cMenuAide::~cMenuAide(void)
{
}

char *cMenuAide::FindPath(const char *seltitle)
{
    char *result="";
    if (!strncmp(seltitle,"+",1))
    {
//    	CloseSubMenu();
//	return osEnd;
    } else {
        // We have a title. Now find index files, read them and 
        // find path to the requested file
        DIR *d = opendir(AideFilesDir); 
        if(d)
	{
	    struct dirent *e;
	    char *name;
	    char *ext;
	    char *aidefile;
	    while ((e = readdir(d)) != NULL)
	    {
	        if (strcmp(e->d_name,".") && strcmp(e->d_name,"..") && strpbrk(e->d_name,".")) //only real names with extensions, please
	        {
    		    name=strtok(e->d_name,".");
		    ext=strtok(NULL,".");				
		    if (!strcmp(ext,"aide")) //find the index files
		    {
		        asprintf (&aidefile,"%s/%s.%s",AideFilesDir,name,ext);

    			FILE *f;
			f=fopen(aidefile,"r");
			if (f)
			{
			    char buff[256];
			    char *title;
			    char *file;
			    while (fgets(buff,sizeof(buff),f))
			    {
			        if (strpbrk(buff,"=")) //ignore lines without = 
			        {
    			    	    title=strtok(buff,"=");
				    file=strtok(NULL,"=");
				    file=strtok(file,"\n"); //strip parasitic symbols
				    if (!strcmp(Trim(seltitle),Trim(title))) //Check if current title matches the requested one
				    {
					asprintf (&result,"%s/%s",AideFilesDir,file);
				    }
				}
			    }
			    fclose(f);
			}
		    }
		}
	    }
	}
    }
    return result;
}


eOSState cMenuAide::ProcessKey(eKeys Key)
{
char *seltitle;

	eOSState state = cOsdMenu::ProcessKey(Key);
	switch (state)
	{
		case osUser1:
			asprintf (&seltitle,"%s",Get(Current())->Text()); //get selected title
			return (AddSubMenu(new cAffichageAide(FindPath(seltitle),seltitle)));
			break;
		default : break;
	}
	return state;
}


char *cMenuAide::Trim(const char *totrim)
{
	char *result="";
	for (int i=0;i<(int) strlen(totrim);i++)
	{
		if (totrim[i]!=' ')
		{
			asprintf(&result,"%s%c",result,totrim[i]);
		}
	}
	return result;
}
