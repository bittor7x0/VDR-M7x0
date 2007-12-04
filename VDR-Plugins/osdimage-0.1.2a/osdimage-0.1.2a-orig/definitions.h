// definitions.h, brougs78

#ifndef _OSDIMAGE_DEFINITIONS_H_
#define _OSDIMAGE_DEFINITIONS_H_


#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <math.h>
// To get the OSDMAXWIDTH and the OSDMAXHEIGHT
#include <vdr/config.h>
// To get the osd-objects (cBitmap ...)
#include <vdr/osd.h>


// commands do read the directory-content
//-iname '*.bmp' -o -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.tiff' -o -iname '*.png' -o -iname '*.xpm'
#define FIND_DIRS "find '%s' -follow -type d -maxdepth 1 | sort -f"
#define FIND_FILES "find '%s' -follow -type f -maxdepth 1 %s | sort -f"
#define FIND_PICS "find '%s' -follow  -type f %s | sort -f"
#define DIR_UP ".."

// Netpbm-stuff
#define NETPBM_CMD "bmptopnm '%s' | ppmquant %s %d | ppmtobmp > '%s'"
#define TMPIMAGERESIZED "/tmp/OSDimage_resized.bmp"
#define TMPIMAGEQUANTIZED "/tmp/OSDimage_quantized.bmp"


const int MAXOSDMEM = 92000;
const int OSDMAXWIDTH = 720; // not MAXOSDWIDTH of vdr/config.h
const int OSDMAXHEIGHT = 576; // not MAXOSDHEIGHT of vdr/config.h
const int OSDXOFFSET = (720 - OSDMAXWIDTH) / 2; // useless now
const int OSDYOFFSET = (576 - OSDMAXHEIGHT) / 2;
const unsigned int BIGJUMP = 10; // jump BIGJUMP pictures up or down
const int SHADOWOFFSET = 3;


using namespace std;


struct sColorSet
{
	tColor txtFG; // color of the text-foreground (font)
	tColor txtBG; // color of the text-background (text-area)
	tColor txtShadow; // color of the text-shadow
	tColor picBG; // color of the picture-background
};

struct sFiles
{
	stack<string> dirHistory;
	stack<string> pathHistory;
	vector<string> files;
	unsigned int curPic;
	unsigned int nDirs;
	unsigned int Size(void) { return files.size(); }
	const char *CurPicName(void) { return files[curPic].c_str(); }
	const char *CurPicPath(void) { return (pathHistory.top() + "/" + files[curPic]).c_str(); }
	
	void PushDir(string &dir)
	{
		if (pathHistory.size() == 0)
		{
			pathHistory.push(dir);
		}
		else
		{
			dirHistory.push(dir);
			pathHistory.push(pathHistory.top() + "/" + dir);
		}
	}
	
	void PopDir(void) { dirHistory.pop(); pathHistory.pop(); }
	void ClearHistory(void) { while (dirHistory.size() > 0) dirHistory.pop(); while (pathHistory.size() > 1) pathHistory.pop(); }
	void Reset(void) { curPic = 0; nDirs = 0; files.clear(); }
	const char *CurPath(void) { return pathHistory.top().c_str(); }
	const char *CurDir(void) { return dirHistory.top().c_str(); }
	bool IsRoot(void) { return (dirHistory.size() == 0); }
};


/*
struct sDirectory {
	char dir[256];
	bool includesSubDirs;
	vector<char[256]> files;
};

class cOSDImageDummy
{
public:
	static char picture[256];
	
};
*/

#endif

