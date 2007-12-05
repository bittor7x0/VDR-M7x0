#ifndef __AFFICHAGEAIDE_H
#define __AFFICHAGEAIDE_H
#include <vdr/plugin.h>

class cAffichageAide : public cOsdMenu
{
	public : 
	cAffichageAide(const char *selpath,const char *Title);
	~cAffichageAide();
	virtual eOSState ProcessKey (eKeys Key);
};

#endif
