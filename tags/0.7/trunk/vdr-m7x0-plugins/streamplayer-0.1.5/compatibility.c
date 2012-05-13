#include <vdr/config.h>

#if VDRVERSNUM >= 10313 && VDRVERSNUM <= 10327
  // trick load remux.h
  #define private protected
  #include <vdr/remux.h>
  #undef private
#endif

#include "compatibility.h"

#if VDRVERSNUM >= 10307

cInterface myInterface;
cInterface *Interface=&myInterface;
cSetupCompat SetupCompat;

inline void SetArea(tArea &Area,int x,int y,int w,int h,int bpp) {
	Area.x1=x;
	Area.y1=y;
	Area.x2=x+(w+8/bpp-1)&~(8/bpp-1)-1;
	Area.y2=y+h-1;
	Area.bpp=bpp;
}


void cInterface::Open(int NumCols, int NumLines) {
	const int charWidth=12;
	const int lineHeight=27;

   	tArea Areas[3];
   	int nAreas;

	if (NumCols == 0)
		NumCols = Setup.OSDwidth;
	if (NumLines == 0)
		NumLines = Setup.OSDheight;

	int w=NumCols;
	int h=NumLines;
	int d = (h < 0) ? Setup.OSDheight + h : 0;
	h = abs(h);
	w *= charWidth;
	h *= lineHeight;
	d *= lineHeight;
	int x = (720 - w + charWidth) / 2; //TODO PAL vs. NTSC???
	int y = (576 - Setup.OSDheight * lineHeight) / 2 + d;

	Osd=cOsdProvider::NewOsd(x,y);

	if (!Osd) {
		printf("NewOsd(%i,%i) failed\n",x,y);
		return;
	}


	if (h / lineHeight == 5) { //XXX channel display
		nAreas=1;
		SetArea(Areas[0],0,0, w, h, 4);
		if (!Osd->CanHandleAreas(Areas,nAreas)==oeOk) {
			printf("CanHandleAreas(1) failed.\n");
			printf("Area: %i %i %i %i %i\n",Areas[0].x1,Areas[0].y1,Areas[0].x2,Areas[0].y2,Areas[0].bpp);
		}
		Osd->SetAreas(Areas,nAreas);
    } else if (h / lineHeight == 1) { //XXX info display
		nAreas=1;
		SetArea(Areas[0],0,0, w,h,4);
		if (!Osd->CanHandleAreas(Areas,nAreas)==oeOk) {
			printf("CanHandleAreas(2) failed.\n");
			printf("Area: %i %i %i %i %i\n",Areas[0].x1,Areas[0].y1,Areas[0].x2,Areas[0].y2,Areas[0].bpp);
		}
		Osd->SetAreas(Areas,nAreas);
    } else if (d == 0) { //XXX full menu
    	nAreas=3;
     	SetArea(Areas[0],0,0,w,lineHeight, 2);
     	SetArea(Areas[1],0,lineHeight,w,(Setup.OSDheight - 3) * lineHeight, 2);
     	SetArea(Areas[2],0, (Setup.OSDheight - 2) * lineHeight, w,2 * lineHeight, 4);
		if (!Osd->CanHandleAreas(Areas,nAreas)==oeOk) {
			printf("CanHandleAreas(3) failed.\n");
			printf("Area: %i %i %i %i %i\n",Areas[0].x1,Areas[0].y1,Areas[0].x2,Areas[0].y2,Areas[0].bpp);
			printf("Area: %i %i %i %i %i\n",Areas[1].x1,Areas[1].y1,Areas[1].x2,Areas[1].y2,Areas[1].bpp);
			printf("Area: %i %i %i %i %i\n",Areas[2].x1,Areas[2].y1,Areas[2].x2,Areas[2].y2,Areas[2].bpp);
		}
		Osd->SetAreas(Areas,nAreas);

     	Osd->GetBitmap(1)->Index(clrBackground);
     	Osd->GetBitmap(1)->Index(clrCyan);
    	Osd->GetBitmap(1)->Index(clrWhite);
     	Osd->GetBitmap(1)->Index(clrBlack);
     	Osd->GetBitmap(1)->Reset();
    } else { //XXX progress display
		nAreas=1;
		SetArea(Areas[0],0,0, w, h, 4);
		if (!Osd->CanHandleAreas(Areas,nAreas)==oeOk) {
			printf("CanHandleAreas(4) failed.\n");
			printf("Area: %i %i %i %i %i\n",Areas[0].x1,Areas[0].y1,Areas[0].x2,Areas[0].y2,Areas[0].bpp);
		}
		Osd->SetAreas(Areas,nAreas);
    }

}

void cInterface::Write(int x, int y, const char *s, eDvbColor FgColor, eDvbColor BgColor) {
	if (!Osd) return;

	const cFont *Font=cFont::GetFont(fontOsd);

	Osd->DrawText(x,y,s,FgColor,BgColor,Font);
}

void cInterface::Flush(void) {
	if (!Osd) return;

	Osd->Flush();
}

void cInterface::Clear(void) {
	if (!Osd) return;

	Osd->DrawRectangle(0,0,Osd->Width()-1,Osd->Height()-1,clrBackground);
}

void cInterface::Close(void) {
	delete Osd;
	Osd=NULL;
}



#endif // VDRVERSNUM >= 10307

#if VDRVERSNUM >= 10318



char *readline(FILE *f)
{
  static char buffer[10240];
  if (fgets(buffer, sizeof(buffer), f) > 0) {
     int l = strlen(buffer) - 1;
     if (l >= 0 && buffer[l] == '\n')
        buffer[l] = 0;
     return buffer;
     }
  return NULL;
}

const char *AddDirectoryCompat(const char *DirName, const char *FileName)
{
  static char *buf = NULL;
  free(buf);
  asprintf(&buf, "%s/%s", DirName && *DirName ? DirName : ".", FileName);
  return buf;
}


#endif // VDRVERSNUM >= 10318
