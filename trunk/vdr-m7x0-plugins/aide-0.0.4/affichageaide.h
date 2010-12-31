#ifndef __AFFICHAGEAIDE_H
#define __AFFICHAGEAIDE_H
#include <vdr/plugin.h>
#include <string>

class cAffichageAide : public cOsdMenu
{
 private:
  std::string text;

	public : 
	cAffichageAide(const char *selpath,const char *Title);
	~cAffichageAide();
	virtual void Display(void);
	virtual eOSState ProcessKey (eKeys Key);
};

#endif
