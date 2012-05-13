#ifndef __MENUAIDE_H
#define __MENUAIDE_H
#include <vdr/plugin.h>
#include <string>

class cMenuAide : public cOsdMenu
{
 private:
  char *AideFilesDir;
	public : 
		cMenuAide(void);
		~cMenuAide(void);
		virtual eOSState ProcessKey(eKeys Key);
		void FindAide(const char *);
		char *FindPath(const char *);
		void Draw(void);
		std::string Trim(const char *);
};
#endif
