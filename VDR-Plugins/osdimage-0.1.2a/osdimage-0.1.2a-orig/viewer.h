// viewer.h, brougs78

#ifndef _OSDIMAGE_VIEWER_H_
#define _OSDIMAGE_VIEWER_H_

#include "definitions.h"
#include "bitmap.h"
#include <vdr/plugin.h>
//#include <vdr/osd.h>
#include <stdio.h>
#include <string.h>
//#include <vector>


static const cFont *FONT = cFont::GetFont(fontSml);
static const int FONTSIZE = FONT->Height();


enum eViewMode 
{
	pics,
	picsInfo,
	picsInfoHelp,
};


class cOSDImageViewer : public cOsdObject //, public cOSDImageDummy
{
public:
	cOSDImageViewer(sFiles &pics);
	~cOSDImageViewer();
	virtual void Show();
	virtual eOSState ProcessKey(eKeys Key);
	void cOSDImageViewer::Load(void);
	void cOSDImageViewer::ViewPicture(void);
	void cOSDImageViewer::ViewBackground(void);
	void cOSDImageViewer::ViewInfo(const char *info = "");
	
private:
	cOsd *osd;
	sFiles *myPics;
	sColorSet myColors;
	int OSDLeft, OSDTop, OSDWidth, OSDHeight;
	int PicLeft, PicTop;
	int wPicArea, hPicArea, wTxtArea;
	bool slideShow;
	int quantizeColors;
};

#endif
