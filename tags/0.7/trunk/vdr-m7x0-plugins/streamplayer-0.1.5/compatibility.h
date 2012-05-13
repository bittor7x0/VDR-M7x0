#ifndef __streamplayer_compatibility_h__
#define __streamplayer_compatibility_h__

#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/player.h>


#if VDRVERSNUM >= 10307

#include <vdr/osd.h>
#include <vdr/skins.h>

#define eDvbColor tColor
#define clrBackground clrGray50


class cSetupCompat : public cSetup {
	public:
	class cOSDwidth {
		public:
		operator int() { return Setup.OSDWidth/12; }
	} OSDwidth;
	class cOSDheight {
		public:
		operator int() { return Setup.OSDHeight/27; }
	} OSDheight;
};
static const cSetup &Setup13=Setup;
#define Setup SetupCompat
extern cSetupCompat SetupCompat;



class cBitmapCompat : public cBitmap {
	public:
	class cwidth {
		cBitmapCompat *Parent;
		public:
		cwidth(cBitmapCompat *parent) { Parent=parent; }
		operator int() { return Parent->Width(); }
	} width;
	class cheight {
		cBitmapCompat *Parent;
		public:
		cheight(cBitmapCompat *parent) { Parent=parent; }
		operator int() { return Parent->Height(); }
	} height;

	cBitmapCompat(int Width, int Height, int Bpp, bool ClearWithBackground = true)
	  : cBitmap(Width,Height,Bpp,ClearWithBackground)
	  , width(this)
	  , height(this)
	  { }

	void Fill(int x1, int y1, int x2, int y2, eDvbColor Color) { DrawRectangle(x1,y1,x2,y2,Color); }
};
typedef cBitmap cBitmap13;
#define cBitmap cBitmapCompat



class cOsdItemCompat : public cOsdItem {
public:
  cOsdItemCompat(eOSState State = osUnknown)
  : cOsdItem(State) {};
  cOsdItemCompat(const char *Text, eOSState State = osUnknown)
  : cOsdItem(Text,State) {};
  bool HasUserColor(void) { return Selectable(); }
  void SetColor(eDvbColor FgColor, eDvbColor BgColor = clrBackground) { SetSelectable(false); }
};
typedef cOsdItem cOsdItem13;
#define cOsdItem cOsdItemCompat



class cOsdMenuCompat : public cOsdMenu {
public:
	cOsdMenuCompat(const char *Title, int c0 = 0, int c1 = 0, int c2 = 0, int c3 = 0, int c4 = 0)
	: cOsdMenu(Title,c0,c1,c2,c3,c4) { }

	cOsdItemCompat* Get(int Index) { return (cOsdItemCompat*)cOsdMenu::Get(Index); }
	cOsdItemCompat* First(void) { return (cOsdItemCompat*)cOsdMenu::First(); }
	cOsdItemCompat* Last(void) { return (cOsdItemCompat*)cOsdMenu::Last(); }
	cOsdItemCompat* Prev(const cOsdItemCompat *object) { return (cOsdItemCompat*)cOsdMenu::Prev(object); }
	cOsdItemCompat* Next(const cOsdItemCompat *object) { return (cOsdItemCompat*)cOsdMenu::Next(object); }
};
typedef cOsdMenu cOsdMenu13;
#define cOsdMenu cOsdMenuCompat



class cInterface {
  cOsd *Osd;
public:
  static void Info(const char *s) { Skins.Message(mtInfo,s); }
  static void Error(const char *s) { Skins.Message(mtError,s); }

  void Open(int NumCols = 0, int NumLines = 0);
  void Close(void);
  void Clear(void);
  void Write(int x, int y, const char *s, eDvbColor FgColor = clrWhite, eDvbColor BgColor = clrBackground);
  void Flush(void);

  cInterface() { Osd=NULL; }
};
extern cInterface *Interface;



#endif // VDRVERSNUM >= 10307

#if VDRVERSNUM >= 10318



class cPlayerCompat : public cPlayer {
public:
	cPlayerCompat(ePlayMode PlayMode = pmAudioVideo)
	: cPlayer(PlayMode) { }
  	int PlayVideo(const uchar *Data, int Length) {
		return PlayPes(Data,Length,false);
	}
};
typedef cPlayer cPlayer13;
#define cPlayer cPlayerCompat

extern char *readline(FILE *f);

const char *AddDirectoryCompat(const char *DirName, const char *FileName); ///< \warning returns a statically allocated string!
inline cString AddDirectory13(const char *DirName, const char *FileName) {
	return AddDirectory(DirName,FileName);
}
#define AddDirectory AddDirectoryCompat


#endif // VDRVERSNUM >= 10318

#endif // __streamplayer_compatibility_h__


