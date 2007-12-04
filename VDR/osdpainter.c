/*
 * osdpainter.c: A tool for drawing on osd using vdr osd parts
 *
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * Some parts are taken from vdr main sources.
 *
 */

#include "dvbosd.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define FB_DEVICE "/dev/fb0"
#define HELPSHORTOPTION "h::"
#define HELPLONGOPTION "help"
#define HELPCHAR 'h'
#define ERROR(a...) void( void ( (fprintf(stderr,"ERROR: "), fprintf(stderr,a), fprintf(stderr,"\n")) ))
#define VERSIONSTRING "0.0.1"
#define PROGRAMNAME "osdpainter "
#define TITLESTR PROGRAMNAME "V. " VERSIONSTRING "\n"
#define HELPINDENT "   "
#define USAGESTR "usage: " PROGRAMNAME "[<common options>] <command> <command parameters>\n"

// --- cCommonGraphic --------------------------------------------------------
class cCommonGraphic {
private:
  static const char *optstring;
  static const struct option longopts[];
  int fd_fb;
protected:
  bool clearOnStart;
  bool clearOnStop;
  bool overlay;
  int width;
  int height;
  int x;
  int y;
  tColor bgColor;
  tColor fgColor;
  cOsd *osd;
  bool Init(void);
public:
  cCommonGraphic() {
    clearOnStart = false;
    clearOnStop = false;
    overlay = false;
    width = 720;
    height = 576;
    bgColor = 0x00000000;
    fgColor = 0xFFFFFFFF;
    x = 0;
    y = 0;
    osd = NULL;
    fd_fb = -1;
    }
  cCommonGraphic(cCommonGraphic &common) {
    clearOnStart = common.clearOnStart;
    clearOnStop = common.clearOnStop;
    overlay = common.overlay;
    x = common.x;
    y = common.y;
    width = common.width;
    height = common.height;
    bgColor = common.bgColor;
    fgColor = common.fgColor;
    osd = NULL;
    fd_fb = -1;
    }
  virtual ~cCommonGraphic() {
    delete osd;
    cOsdProvider::Shutdown();
    if (fd_fb >= 0) {
       close(fd_fb);
       }
    }
  virtual int ParseArg(int val, const char *arg);
  virtual const char *OptionString(void) const {
    return optstring;
    }
  virtual const struct option *LongOption(void) const {
    return longopts;
    }
  virtual const char *Help(void) const;
  virtual bool CheckParameter(void) {
    if (width + x > 720) {
       width = 720 - x;
       }
    if (height + y > 576) {
       height = 576 - y;
       }
    overlay = overlay && !clearOnStart;
    return true;
    };
  virtual bool Paint(void) { return false;}
  };

const char *cCommonGraphic::optstring = "-c:b:f:w:H:x:y:o" HELPSHORTOPTION;
const struct option cCommonGraphic::longopts[] = {
  { "clear", 1, NULL, 'c'},
  { "background-color", 1, NULL, 'b' },
  { "foreground-color", 1, NULL, 'f' },
  { "width", 1, NULL, 'w'},
  { "height", 1, NULL, 'H'},
  { "xoffset", 1, NULL, 'x'},
  { "yoffset", 1, NULL, 'y'},
  { "overlay", 0, NULL, 'o'},
  { HELPLONGOPTION, 2, NULL, HELPCHAR},
  { NULL, 0, NULL, 0}
};

const char *cCommonGraphic::Help(void) const
{
  return "common options:\n"
         HELPINDENT "-b|--background-color <x>:\n"
         HELPINDENT HELPINDENT "x := 32 Bit background-color ARGB (default 0)\n\n"
         HELPINDENT "-c|--clear <n>:\n"
         HELPINDENT HELPINDENT "n := Clear Osd on\n"
         HELPINDENT HELPINDENT "0   Never (Default)\n"
         HELPINDENT HELPINDENT "1   On startup only\n"
         HELPINDENT HELPINDENT "2   On shutdown only\n"
         HELPINDENT HELPINDENT "3   On startup and shutdown\n\n"
         HELPINDENT "-f|--foreground-color <x>:\n"
         HELPINDENT HELPINDENT "x := 32 Bit foreground-color ARGB (default 0xFFFFFFFF)\n\n"
         HELPINDENT "-H|--height <x>:\n"
         HELPINDENT HELPINDENT "x := Height of osd bounding box (default 576)\n\n"
         HELPINDENT "-o|--overlay:\n"
         HELPINDENT HELPINDENT "Enable overlay mode\n\n"
         HELPINDENT "-w|--width <x>:\n"
         HELPINDENT HELPINDENT "x := Width of osd bounding box (default 720)\n\n"
         HELPINDENT "-x|--xoffset <x>:\n"
         HELPINDENT HELPINDENT "x := x offset of osd bound box. All other coordinates"
         "a relative\n"
         HELPINDENT HELPINDENT "     to this value (default 0)\n\n"
         HELPINDENT "-y|--yoffset <x>:\n"
         HELPINDENT HELPINDENT "x := y offset of osd bound box. All other coordinates"
         "a relative\n"
         HELPINDENT HELPINDENT "     to this value (default 0)\n\n";
}

int cCommonGraphic::ParseArg(int val, const char *arg)
{
  int r = 0;
  switch (val) {
    case 'c':
         if (isnumber(arg)) {
            switch (atoi(arg)) {
              case 0:
                   break;
              case 1:
                   clearOnStart = true;
                   break;
              case 2:
                   clearOnStop = true;
                   break;
              case 3:
                   clearOnStart = true;
                   clearOnStop = true;
                   break;
              default:
                  r = -1;
              }
            }
         else
            r = -1;
         break;
    case 'w':
         if (isnumber(arg)) {
            const int newwidth = atoi(arg);
            if (newwidth > 0 && newwidth <= 720)
               width = newwidth;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'H':
         if (isnumber(arg)) {
            const int newheight = atoi(arg);
            if (newheight > 0 && newheight <= 576)
               height = newheight;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'x':
         if (isnumber(arg)) {
            const int newx = atoi(arg);
            if (newx > 0 && newx <= 720)
               x = newx;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'y':
         if (isnumber(arg)) {
            const int newy = atoi(arg);
            if (newy > 0 && newy <= 576)
               y = newy;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'b': {
         char *endp;
         tColor newbgColor  = strtoul(arg, &endp, 0);
         if (arg != endp) {
            bgColor = newbgColor;
            }
         else
            r = -1;
         }
         break;
    case 'f': {
         char *endp;
         tColor newfgColor  = strtoul(arg, &endp, 0);
         if (arg != endp) {
            fgColor = newfgColor;
            }
         else
            r = -1;
         }
         break;
    case 'o':
         overlay = true;
         break;
    default:
         r = -1;
    }
  return r;
}

bool cCommonGraphic::Init(void)
{
  fd_fb = open(FB_DEVICE,O_RDWR);
  if (fd_fb < 0) {
     ERROR("Cannot open framebuffer!");
     return false;
     }
  new cDvbOsdProvider(fd_fb, clearOnStart, clearOnStop, overlay);
  osd = cOsdProvider::NewOsd(x, y);
  tArea area;
  area.x1 = 0;
  area.y1 = 0;
  area.x2 = width - 1;
  area.y2 = height - 1;
  area.bpp = 8;

  if (osd->SetAreas(&area, 1) != oeOk) {
     ERROR("Invalid bounding box!");
     return false;
     }

  return true;
}

// --- cTextGraphic ----------------------------------------------------------

class cTextGraphic : public cCommonGraphic {
private:
  static const char *optstring;
  static const struct option longopts[];
  char *text;
  int twidth;
  int theight;
  int tx;
  int ty;
  int alignment;
  bool multiline;
  eDvbFont fontType;
  eDvbCode encoding;
public:
  static cCommonGraphic *New(cCommonGraphic &common) { return new cTextGraphic(common); }
  cTextGraphic(cCommonGraphic &common) : cCommonGraphic(common) {
    text = NULL;
    twidth = 0;
    theight = 0;
    tx = 0;
    ty = 0;
    alignment = taDefault;
    fontType = fontOsd;
    encoding = code_iso8859_15;
    multiline = false;
    }
  virtual ~cTextGraphic() { free(text); }
  virtual const char *OptionString(void) const {
    return optstring;
    }
  virtual const struct option *LongOption(void) const {
    return longopts;
    }
  virtual int ParseArg(int val, const char *arg);
  virtual const char *Help(void) const;
  virtual bool CheckParameter(void) {
    if (!text) {
       ERROR("No text given!");
       return false;
       }
    return true;
    }
  virtual bool Paint(void);
  };

const char *cTextGraphic::optstring = "-a:f:e:w:H:x:y:m" HELPSHORTOPTION;
const struct option cTextGraphic::longopts[] = {
  { "alignment", 1, NULL, 'a'},
  { "font", 1, NULL, 'f' },
  { "encoding", 1, NULL, 'e' },
  { "width", 1, NULL, 'w'},
  { "height", 1, NULL, 'H'},
  { "xoffset", 1, NULL, 'x'},
  { "yoffset", 1, NULL, 'y'},
  { "multiline", 0, NULL, 'm'},
  { HELPLONGOPTION, 2, NULL, HELPCHAR},
  { NULL, 0, NULL, 0}
};

const char *cTextGraphic::Help(void) const
{
  return "command usage: drawtext [<drawtext options>] <text>\n\n"
         "Draws <text> on osd with foreground color (specified in common options).\n"
         "If background color is specified in common options and its not fully transparent\n"
         "The whole osd-bounding box gets filled with it before drawing text.\n\n"
         "drawtext options:\n"
         HELPINDENT "-a|--alignment <x>:\n"
         HELPINDENT HELPINDENT "x := Alignment of text in drawing area. Bitwise or'd of flags:\n"
         HELPINDENT HELPINDENT "0   centered (in both directions)\n"
         HELPINDENT HELPINDENT "1   left (default)\n"
         HELPINDENT HELPINDENT "2   right\n"
         HELPINDENT HELPINDENT "4   top (defautl)\n"
         HELPINDENT HELPINDENT "8   bottom\n\n"
         HELPINDENT "-e|--encoding <n>:\n"
         HELPINDENT HELPINDENT "Font encoding <n>:n"
         HELPINDENT HELPINDENT "0   ISO-8859-15 (default)\n\n"
         HELPINDENT "-f|--font <n>:\n"
         HELPINDENT HELPINDENT "Font <n>:\n"
         HELPINDENT HELPINDENT "0   normal font (default)\n"
         HELPINDENT HELPINDENT "1   fixed size font\n"
         HELPINDENT HELPINDENT "2   small font\n\n"
         HELPINDENT "-H|--height <x>:\n"
         HELPINDENT HELPINDENT "x := Height of text drawing area (default 0 )\n"
         HELPINDENT HELPINDENT "     0 means unbound\n\n"
         HELPINDENT "-m|--multiline:\n"
         HELPINDENT HELPINDENT "Enable multiline mode. Text gets automaticly wrapped, if\n"
         HELPINDENT HELPINDENT "exceeds width of text drawing area.\n\n"
         HELPINDENT "-w|--width <x>:\n"
         HELPINDENT HELPINDENT "x := Width of text drawing area(default 0 )\n"
         HELPINDENT HELPINDENT "     0 means unbound\n\n"
         HELPINDENT "-x|--xoffset <x>:\n"
         HELPINDENT HELPINDENT "x := x position of text area.\n\n"
         HELPINDENT "-y|--yoffset <x>:\n"
         HELPINDENT HELPINDENT "x := y position of text area.\n\n";
}

int cTextGraphic::ParseArg(int val, const char *arg)
{
  int r = 0;
  switch (val) {
    case 1:
         text = strdup(arg);
         break;
    case 'a': {
         char *endp;
         int newalign  = strtol(arg, &endp, 0);
         if (arg != endp) {
            alignment = newalign;
            }
         else {
            r = -1;
            }
         }
         break;
    case 'f':
         if (isnumber(arg)) {
            int newfont = atoi(arg);
            if (newfont >= 0 && newfont < eDvbFontSize)
               fontType = (eDvbFont) newfont;
            else
                 r = -1;
            }
         else
            r = -1;
         break;
    case 'e':
         if (isnumber(arg)) {
            int newenc = atoi(arg);
            if (newenc >= 0 && newenc < eDvbCodeSize)
               encoding = (eDvbCode) newenc;
            else
                 r = -1;
            }
         else
            r = -1;
         break;
    case 'w':
         if (isnumber(arg)) {
            const int newwidth = atoi(arg);
            if (newwidth > 0 && newwidth <= 720)
               twidth = newwidth;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'H':
         if (isnumber(arg)) {
            const int newheight = atoi(arg);
            if (newheight > 0 && newheight <= 576)
               theight = newheight;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'x':
         if (isnumber(arg)) {
            const int newx = atoi(arg);
            if (newx > 0 && newx <= 720)
               tx = newx;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'y':
         if (isnumber(arg)) {
            const int newy = atoi(arg);
            if (newy > 0 && newy <= 576)
               ty = newy;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'm':
         multiline = true;
         break;
    default:
         r = -1;
    }
  return r;
}

bool cTextGraphic::Paint(void) {
  overlay = overlay && !(bgColor & 0xFF000000);
  if (!Init())
     return false;

  if (!overlay) {
     tIndex bgIndex = osd->GetBitmap(0)->Index(bgColor);
     if (bgIndex)
        memset(osd->GetBitmap(0)->GetBuffer(), bgIndex, width *height);
     }
  if (!(bgColor & 0xFF000000) || overlay) {
     osd->GetBitmap(0)->Clean();
     }

  cFont::SetCode(encoding);
  const cFont *font = cFont::GetFont(fontType);
  if (!font) {
     ERROR("Cannot get font!");
     return false;
     }

  if (multiline) {
     int area_width = width - tx;
     int area_height = height - ty;
     if (twidth && twidth < area_width)
        area_width = twidth;
     if (theight && theight < area_height)
        area_height = theight;
     cTextWrapper textWrap(text, font, area_width);
     const int lineHeight = font->Height();
     const int lineCount = (lineHeight * textWrap.Lines() > area_height ?
                            area_height / lineHeight : textWrap.Lines());
     int y_pos;
     if (alignment & taTop) {
        y_pos = ty;
        }
     else if (alignment & taBottom) {
        y_pos = area_height - (lineCount * lineHeight);
        }
     else { // centered
        y_pos = (area_height - (lineCount * lineHeight)) >> 1;
        }
     for (int i = 0; i < lineCount; i++) {
         osd->DrawText(tx, y_pos, textWrap.GetLine(i), fgColor, clrTransparent,
                                               font, area_width, 0, alignment);
         y_pos += lineHeight;
         }
     }
  else {
     osd->DrawText(tx, ty, text, fgColor, clrTransparent, font, twidth,
                                                   theight, alignment);
     }
  osd->Flush();
  return true;
}

// --- cBitmapGraphic --------------------------------------------------------

class cBitmapGraphic : public cCommonGraphic {
private:
  static const char *optstring;
  static const struct option longopts[];
  char *filename;
  int bx;
  int by;
public:
  static cCommonGraphic *New(cCommonGraphic &common) { return new cBitmapGraphic(common); }
  cBitmapGraphic(cCommonGraphic &common) : cCommonGraphic(common) {
    filename = NULL;
    bx = 0;
    by = 0;
    }
  virtual ~cBitmapGraphic() { free(filename); }
  virtual const char *OptionString(void) const {
    return optstring;
    }
  virtual const struct option *LongOption(void) const {
    return longopts;
    }
  virtual int ParseArg(int val, const char *arg);
  virtual const char *Help(void) const;
  virtual bool CheckParameter(void) {
    if (!filename) {
       ERROR("No xpm file name given!");
       return false;
       }
    return true;
    }
  virtual bool Paint(void);
  };

const char *cBitmapGraphic::optstring = "-x:y:" HELPSHORTOPTION;
const struct option cBitmapGraphic::longopts[] = {
  { "xoffset", 1, NULL, 'x'},
  { "yoffset", 1, NULL, 'y'},
  { HELPLONGOPTION, 2, NULL, HELPCHAR},
  { NULL, 0, NULL, 0}
};

const char *cBitmapGraphic::Help(void) const
{
  return "command usage: drawbitmap [<drawbitmap options>] <xpm filename>\n\n"
         "Draws XPM bitmap in <xpm filename>  on osd.\n"
         "If background color is specified in common options and its not fully transparent\n"
         "The whole osd-bounding box gets filled with it before drawing.\n\n"
         "drawbitmap options:\n"
         HELPINDENT "-x|--xoffset <x>:\n"
         HELPINDENT HELPINDENT "x := x position of bitmap.\n\n"
         HELPINDENT "-y|--yoffset <x>:\n"
         HELPINDENT HELPINDENT "x := y position of bitmap.\n\n";
}

int cBitmapGraphic::ParseArg(int val, const char *arg)
{
  int r = 0;
  switch (val) {
    case 1:
         filename = strdup(arg);
         break;
    case 'x':
         if (isnumber(arg)) {
            const int newx = atoi(arg);
            if (newx > 0 && newx <= 720)
               bx = newx;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    case 'y':
         if (isnumber(arg)) {
            const int newy = atoi(arg);
            if (newy > 0 && newy <= 576)
               by = newy;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    default:
         r = -1;
    }
  return r;
}

bool cBitmapGraphic::Paint(void) {
  if (clearOnStart  && !(bgColor & 0xFF000000)) {
     dsyslog("DEBUG: cBitmapGraphic::Paint(): using direct draw");
     x += bx;
     y += by;
     width -= bx;
     height -= by;
     if (!Init())
        return false;
     int saved_width = width;
     int saved_height = height;
     cBitmap *bitmap = osd->GetBitmap(0);
     if (!bitmap->LoadXpm(filename)){
        ERROR("Cannot load bitmap %s",filename);
        return false;
        }
     saved_width = min(bitmap->Width() , saved_width) - 1;
     saved_height = min(bitmap->Height(), saved_height) - 1;
     bitmap->Clean();
     tIndex saved = *bitmap->Data(0,0);
     bitmap->SetIndex(0,0, 0xFF);
     bitmap->SetIndex(0,0, saved);
     saved = *bitmap->Data(saved_width,saved_height);
     bitmap->SetIndex(saved_width,saved_height, 0xFF);
     bitmap->SetIndex(saved_width,saved_height, saved);
     osd->Flush();
     return true;
     }

  overlay = overlay && !(bgColor & 0xFF000000);
  if (!Init())
     return false;

  if (!overlay) {
     tIndex bgIndex = osd->GetBitmap(0)->Index(bgColor);
     if (bgIndex)
        memset(osd->GetBitmap(0)->GetBuffer(), bgIndex, width *height);
     }

  if (!(bgColor & 0xFF000000) || overlay) {
     osd->GetBitmap(0)->Clean();
     }

  cBitmap bitmap(1,1,8);

  if (!bitmap.LoadXpm(filename)){
     ERROR("Cannot load bitmap %s",filename);
     return false;
     }



  osd->DrawBitmap(bx, by, bitmap, 0, 0, false, !(bitmap.Color(0) && 0xFF000000));
  osd->Flush();
  return true;
}

// --- cRectangle ------------------------------------------------------------
class cRectangleGraphic : public cCommonGraphic {
private:
  static const char *optstring;
  static const struct option longopts[];
public:
  static cCommonGraphic *New(cCommonGraphic &common) { return new cRectangleGraphic(common); }
  cRectangleGraphic(cCommonGraphic &common) : cCommonGraphic(common) {  }
  virtual const char *OptionString(void) const {
    return optstring;
    }
  virtual const struct option *LongOption(void) const {
    return longopts;
    }
  virtual int ParseArg(int val, const char *arg) { return -1; }
  virtual const char *Help(void) const;
  virtual bool CheckParameter(void) {  return true; }
  virtual bool Paint(void);
  };

const char *cRectangleGraphic::optstring = "-" HELPSHORTOPTION;
const struct option cRectangleGraphic::longopts[] = {
  { HELPLONGOPTION, 2, NULL, HELPCHAR},
  { NULL, 0, NULL, 0}
};

const char *cRectangleGraphic::Help(void) const
{
  return "command usage: drawrectangle\n\n"
         "Draws rectangle with foreground color on the whole osd-bound box as\n"
         "specified in common options\n\n";
}



bool cRectangleGraphic::Paint(void) {
  overlay = false;
  if (!Init())
     return false;

  tIndex fgIndex = osd->GetBitmap(0)->Index(fgColor);
  if (fgIndex)
     memset(osd->GetBitmap(0)->GetBuffer(), fgIndex, width *height);

  osd->Flush();
  return true;
}

// --- cSlopeGraphic --------------------------------------------------------

class cSlopeGraphic : public cCommonGraphic {
private:
  static const char *optstring;
  static const struct option longopts[];
  int type;

public:
  static cCommonGraphic *New(cCommonGraphic &common) { return new cSlopeGraphic(common); }
  cSlopeGraphic(cCommonGraphic &common) : cCommonGraphic(common) {
    type = 0;
    }
  virtual const char *OptionString(void) const {
    return optstring;
    }
  virtual const struct option *LongOption(void) const {
    return longopts;
    }
  virtual int ParseArg(int val, const char *arg);
  virtual const char *Help(void) const;
  virtual bool CheckParameter(void) { return true; }
  virtual bool Paint(void);
  };

const char *cSlopeGraphic::optstring = "-t:" HELPSHORTOPTION;
const struct option cSlopeGraphic::longopts[] = {
  { "type", 1, NULL, 't'},
  { HELPLONGOPTION, 2, NULL, HELPCHAR},
  { NULL, 0, NULL, 0}
};

const char *cSlopeGraphic::Help(void) const
{
  return "command usage: drawslope [<drawslope options>]\n\n"
         "Draws a slope with foreground color in the osd bounding box.\n"
         "If background color is specified in common options and its not fully transparent\n"
         "The whole osd-bounding box gets filled with it before drawing.\n\n"
         "drawslope options:\n"
         HELPINDENT "-t|--type <n>:\n"
         HELPINDENT HELPINDENT "n :=  Slope Direction\n"
         HELPINDENT HELPINDENT "0   horizontal, rising,  lower (default)\n"
         HELPINDENT HELPINDENT "1   horizontal, rising,  upper\n"
         HELPINDENT HELPINDENT "2   horizontal, falling, lower\n"
         HELPINDENT HELPINDENT "3   horizontal, falling, upper\n"
         HELPINDENT HELPINDENT "4   vertical,   rising,  lower\n"
         HELPINDENT HELPINDENT "5   vertical,   rising,  upper\n"
         HELPINDENT HELPINDENT "6   vertical,   falling, lower\n"
         HELPINDENT HELPINDENT "7   vertical,   falling, upper\n\n";
}

int cSlopeGraphic::ParseArg(int val, const char *arg)
{
  int r = 0;
  switch (val) {
    case 't':
         if (isnumber(arg)) {
            const int newtype = atoi(arg);
            if (newtype >= 0 && newtype <= 7)
               type = newtype;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    default:
         r = -1;
    }
  return r;
}

bool cSlopeGraphic::Paint(void) {
  overlay = overlay && !(bgColor & 0xFF000000);
  if (!Init())
     return false;

  if (!overlay) {
     tIndex bgIndex = osd->GetBitmap(0)->Index(bgColor);
     if (bgIndex)
        memset(osd->GetBitmap(0)->GetBuffer(), bgIndex, width * height);
     }

  if (!(bgColor & 0xFF000000) || overlay) {
     osd->GetBitmap(0)->Clean();
     }

  osd->DrawSlope(0, 0, width, height, fgColor, type);
  osd->Flush();
  return true;
}

// --- cEllipseGraphic --------------------------------------------------------

class cEllipseGraphic : public cCommonGraphic {
private:
  static const char *optstring;
  static const struct option longopts[];
  int type;

public:
  static cCommonGraphic *New(cCommonGraphic &common) { return new cEllipseGraphic(common); }
  cEllipseGraphic(cCommonGraphic &common) : cCommonGraphic(common) {
    type = 0;
    }
  virtual const char *OptionString(void) const {
    return optstring;
    }
  virtual const struct option *LongOption(void) const {
    return longopts;
    }
  virtual int ParseArg(int val, const char *arg);
  virtual const char *Help(void) const;
  virtual bool CheckParameter(void) { return true; }
  virtual bool Paint(void);
  };

const char *cEllipseGraphic::optstring = "-t:" HELPSHORTOPTION;
const struct option cEllipseGraphic::longopts[] = {
  { "type", 1, NULL, 't'},
  { HELPLONGOPTION, 2, NULL, HELPCHAR},
  { NULL, 0, NULL, 0}
};

const char *cEllipseGraphic::Help(void) const
{
  return "command usage: drawellipse [<drawellipse options>]\n\n"
         "Draws a ellipse with foreground color in the osd bounding box.\n"
         "If background color is specified in common options and its not fully transparent\n"
         "The whole osd-bounding box gets filled with it before drawing.\n\n"
         "drawellipse options:\n"
         HELPINDENT "-t|--type <n>:\n"
         HELPINDENT HELPINDENT "n :=  Ellipse drawing type (negative value draws inverted)\n"
         HELPINDENT HELPINDENT "0   draws entire ellipse (default)\n"
         HELPINDENT HELPINDENT "1   draws only the first quadrant\n"
         HELPINDENT HELPINDENT "2   draws only the second quadrant\n"
         HELPINDENT HELPINDENT "3   draws only the third quadrant\n"
         HELPINDENT HELPINDENT "4   draws only the fourth quadrant\n"
         HELPINDENT HELPINDENT "5   draws the right half\n"
         HELPINDENT HELPINDENT "6   draws the top half\n"
         HELPINDENT HELPINDENT "7   draws the left half\n"
         HELPINDENT HELPINDENT "8   draws the bottom half\n\n";
}

int cEllipseGraphic::ParseArg(int val, const char *arg)
{
  int r = 0;
  switch (val) {
    case 't':
         if (isnumber(arg) || (*arg == '-' && isnumber(arg +1))) {
            const int newtype = atoi(arg);
            if (newtype >= -4 && newtype <= 8)
               type = newtype;
            else
               r = -1;
            }
         else
            r = -1;
         break;
    default:
         r = -1;
    }
  return r;
}

bool cEllipseGraphic::Paint(void) {
  overlay = overlay && !(bgColor & 0xFF000000);
  if (!Init())
     return false;

  if (!overlay) {
     tIndex bgIndex = osd->GetBitmap(0)->Index(bgColor);
     if (bgIndex)
        memset(osd->GetBitmap(0)->GetBuffer(), bgIndex, width * height);
     }

  if (!(bgColor & 0xFF000000) || overlay) {
     osd->GetBitmap(0)->Clean();
     }

  osd->DrawEllipse(0, 0, width, height, fgColor, type);
  osd->Flush();
  return true;
}

// --- Main Part -------------------------------------------------------------
struct sCommands {
 const char *name;
 cCommonGraphic* (*init) (cCommonGraphic&);
};

static sCommands commands[] = {
  { "drawtext", cTextGraphic::New },
  { "drawbitmap", cBitmapGraphic::New },
  { "drawrectangle", cRectangleGraphic::New },
  { "drawslope", cSlopeGraphic::New },
  { "drawellipse", cEllipseGraphic::New },
  { NULL, NULL}
};


static void printHelp(cCommonGraphic *com, cCommonGraphic *gra, const char *arg)
{
  printf(TITLESTR);
  printf(USAGESTR);
  if (!arg) {
     if (!gra) {
        printf(com->Help());
        printf("Commands are: ");
        for (int i = 0; commands[i].name; i++) {
            printf("%s ", commands[i].name);
            }
        printf("\n\nUse --help all for help on all commands or --help <command> for help on <command>.\n");
        }
     else {
        printf(gra->Help());
        }
     return;
     }

  if (!strcasecmp("all",arg)) {
     printf(com->Help());
     for (int i = 0; commands[i].name; i++) {
         cCommonGraphic *cur = commands[i].init(*com);
         printf(cur->Help());
         delete cur;
         }
     return;
     }

  for (int i = 0; commands[i].name; i++) {
      if (!strcasecmp(commands[i].name,arg)) {
         cCommonGraphic *cur = commands[i].init(*com);;
         printf(cur->Help());
         delete cur;
         return;
         }
      }

  ERROR("Command %s unknown", arg);

}

int main (int argc, char **argv) {
  cCommonGraphic common;
  int val;
  while ((val = getopt_long(argc, argv, common.OptionString(),
                              common.LongOption(), NULL)) > 1) {
        switch (val) {
          case 'h':
               printHelp(&common,NULL,(!optarg && optind < argc &&
                 argv[optind][0] != '-' ? argv[optind] : optarg) );
               return 0;
          case '?':
               ERROR("Illegal option");
               return -1;
          case ':':
               ERROR("Missing parameter");
               return -1;
          default:
               if (common.ParseArg(val,optarg) < 0) {
                  ERROR("Invalid parameter");
                  return -1;
                  }
          }
        }

  if (!common.CheckParameter())
     return -1;

  if (val != 1) {
     ERROR("No command given!");
     return -1;
     }

  int cmd = -1;
  for (int i = 0; commands[i].name; i++)
      if (!strcasecmp(commands[i].name,optarg)) {
         cmd = i;
         break;
         }

  if (cmd == -1) {
     ERROR("Unknown command %s", optarg);
     return -1;
     }


  cCommonGraphic *commandGraphic = commands[cmd].init(common);
  while ((val = getopt_long(argc, argv, commandGraphic->OptionString(),
                              commandGraphic->LongOption(), NULL)) > 0) {
        switch (val) {
          case 'h':
               printHelp(&common,commandGraphic,(!optarg && optind < argc &&
                 argv[optind][0] != '-' ? argv[optind] : optarg));
               delete commandGraphic;
               return 0;
          case '?':
               ERROR("Illegal option");
               delete commandGraphic;
               return -1;
          case ':':
               ERROR("Missing parameter");
               delete commandGraphic;
               return -1;
          default:
               if (commandGraphic->ParseArg(val,optarg) < 0) {
                  ERROR("Invalid parameter");
                  delete commandGraphic;
                  return -1;
                  }
          }
        }

  if (!commandGraphic->CheckParameter()) {
     delete commandGraphic;
     return -1;
     }

  commandGraphic->Paint();
  delete commandGraphic;
  return 0;
}
