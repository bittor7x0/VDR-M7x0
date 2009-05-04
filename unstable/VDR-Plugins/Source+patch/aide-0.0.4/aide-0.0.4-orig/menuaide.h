#ifndef __MENUAIDE_H
#define __MENUAIDE_H
#include <vdr/plugin.h>


class cMenuAide : public cOsdMenu
{
	public : 
		cMenuAide(void);
		~cMenuAide(void);
		virtual eOSState ProcessKey(eKeys Key);
		void FindAide(const char *);
		char *FindPath(const char *);
		void Draw(void);
		char *Trim(const char *);
};
#endif
