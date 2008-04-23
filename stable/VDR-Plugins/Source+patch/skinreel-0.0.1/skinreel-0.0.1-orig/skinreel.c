#include <vdr/skinclassic.h>
#include "skinreel.h"
#include <vdr/font.h>
#include <vdr/i18n.h>
#include <vdr/menu.h>
#include <vdr/osd.h>
#include <vdr/themes.h>

#define ScrollWidth  5

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Reel Skin-Plugin";
static const char *MAINMENUENTRY  = NULL;
#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
#include <vdr/plugin.h>
int SKINVERSION			  = 00001;
int display;
bool changing;
tColor ChangeColor;
#endif
tColor clrGray62 = 0xA0000000;

static cTheme Theme;

THEME_CLR(Theme, clrBackground,             clrGray62);
THEME_CLR(Theme, clrButtonRedFg,            clrWhite);
THEME_CLR(Theme, clrButtonRedBg,            clrRed);
THEME_CLR(Theme, clrButtonGreenFg,          clrBlack);
THEME_CLR(Theme, clrButtonGreenBg,          clrGreen);
THEME_CLR(Theme, clrButtonYellowFg,         clrBlack);
THEME_CLR(Theme, clrButtonYellowBg,         clrYellow);
THEME_CLR(Theme, clrButtonBlueFg,           clrWhite);
THEME_CLR(Theme, clrButtonBlueBg,           clrBlue);
THEME_CLR(Theme, clrMessageFrame,           clrYellow);
THEME_CLR(Theme, clrMessageStatusFg,        clrBlack);
THEME_CLR(Theme, clrMessageStatusBg,        clrCyan);
THEME_CLR(Theme, clrMessageInfoFg,          clrBlack);
THEME_CLR(Theme, clrMessageInfoBg,          clrGreen);
THEME_CLR(Theme, clrMessageWarningFg,       clrBlack);
THEME_CLR(Theme, clrMessageWarningBg,       clrYellow);
THEME_CLR(Theme, clrMessageErrorFg,         clrWhite);
THEME_CLR(Theme, clrMessageErrorBg,         clrRed);
THEME_CLR(Theme, clrVolumeFrame,            clrYellow);
THEME_CLR(Theme, clrVolumeSymbol,           clrBlack);
THEME_CLR(Theme, clrVolumeBarUpper,         0xFFBC8024);
THEME_CLR(Theme, clrVolumeBarLower,         0xFF248024);
THEME_CLR(Theme, clrChannelFrame,           clrYellow);
THEME_CLR(Theme, clrChannelName,            clrBlack);
THEME_CLR(Theme, clrChannelDate,            clrBlack);
THEME_CLR(Theme, clrChannelSymbolOn,        clrBlack);
THEME_CLR(Theme, clrChannelSymbolOff,       0xFFBC8024);
THEME_CLR(Theme, clrChannelSymbolRecFg,     clrWhite);
THEME_CLR(Theme, clrChannelSymbolRecBg,     clrRed);
THEME_CLR(Theme, clrChannelEpgTime,         clrBlack);
THEME_CLR(Theme, clrChannelEpgTitle,        clrCyan);
THEME_CLR(Theme, clrChannelEpgShortText,    clrYellow);
THEME_CLR(Theme, clrChannelTimebarSeen,     clrYellow);
THEME_CLR(Theme, clrChannelTimebarRest,     clrGray50);
THEME_CLR(Theme, clrMenuFrame,              clrYellow);
THEME_CLR(Theme, clrMenuTitle,              clrBlack);
THEME_CLR(Theme, clrMenuDate,               clrBlack);
THEME_CLR(Theme, clrMenuItemCurrentFg,      clrBlack);
THEME_CLR(Theme, clrMenuItemCurrentBg,      clrYellow);
THEME_CLR(Theme, clrMenuItemSelectable,     clrYellow);
THEME_CLR(Theme, clrMenuItemNonSelectable,  clrCyan);
THEME_CLR(Theme, clrMenuEventTime,          clrYellow);
THEME_CLR(Theme, clrMenuEventVpsFg,         clrBlack);
THEME_CLR(Theme, clrMenuEventVpsBg,         clrWhite);
THEME_CLR(Theme, clrMenuEventTitle,         clrCyan);
THEME_CLR(Theme, clrMenuEventShortText,     clrYellow);
THEME_CLR(Theme, clrMenuEventDescription,   clrCyan);
THEME_CLR(Theme, clrMenuScrollbarTotal,     clrYellow);
THEME_CLR(Theme, clrMenuScrollbarShown,     clrCyan);
THEME_CLR(Theme, clrMenuScrollbarArrow,     clrBlack);
THEME_CLR(Theme, clrMenuText,               clrCyan);
THEME_CLR(Theme, clrReplayFrame,            clrYellow);
THEME_CLR(Theme, clrReplayTitle,            clrBlack);
THEME_CLR(Theme, clrReplayMode,             clrBlack);
THEME_CLR(Theme, clrReplayCurrent,          clrBlack);
THEME_CLR(Theme, clrReplayTotal,            clrBlack);
THEME_CLR(Theme, clrReplayJump,             clrBlack);
THEME_CLR(Theme, clrReplayProgressSeen,     clrGreen);
THEME_CLR(Theme, clrReplayProgressRest,     clrWhite);
THEME_CLR(Theme, clrReplayProgressSelected, clrRed);
THEME_CLR(Theme, clrReplayProgressMark,     clrBlack);
THEME_CLR(Theme, clrReplayProgressCurrent,  clrRed);

/*THEME_CLR(Theme, clrBackground,             clrGray50);
THEME_CLR(Theme, clrButtonRedFg,            clrWhite);
THEME_CLR(Theme, clrButtonRedBg,            clrRed);
THEME_CLR(Theme, clrButtonGreenFg,          clrBlack);
THEME_CLR(Theme, clrButtonGreenBg,          clrGreen);
THEME_CLR(Theme, clrButtonYellowFg,         clrBlack);
THEME_CLR(Theme, clrButtonYellowBg,         clrYellow);
THEME_CLR(Theme, clrButtonBlueFg,           clrWhite);
THEME_CLR(Theme, clrButtonBlueBg,           clrBlue);
THEME_CLR(Theme, clrMessageStatusFg,        clrBlack);
THEME_CLR(Theme, clrMessageStatusBg,        clrCyan);
THEME_CLR(Theme, clrMessageInfoFg,          clrBlack);
THEME_CLR(Theme, clrMessageInfoBg,          clrGreen);
THEME_CLR(Theme, clrMessageWarningFg,       clrBlack);
THEME_CLR(Theme, clrMessageWarningBg,       clrYellow);
THEME_CLR(Theme, clrMessageErrorFg,         clrWhite);
THEME_CLR(Theme, clrMessageErrorBg,         clrRed);
THEME_CLR(Theme, clrVolumePrompt,           clrGreen);
THEME_CLR(Theme, clrVolumeBarUpper,         clrWhite);
THEME_CLR(Theme, clrVolumeBarLower,         clrGreen);
THEME_CLR(Theme, clrChannelName,            clrWhite);
THEME_CLR(Theme, clrChannelDate,            clrWhite);
THEME_CLR(Theme, clrChannelEpgTimeFg,       clrWhite);
THEME_CLR(Theme, clrChannelEpgTimeBg,       clrRed);
THEME_CLR(Theme, clrChannelEpgTitle,        clrCyan);
THEME_CLR(Theme, clrChannelEpgShortText,    clrYellow);
THEME_CLR(Theme, clrMenuTitleFg,            clrBlack);
THEME_CLR(Theme, clrMenuTitleBg,            clrCyan);
THEME_CLR(Theme, clrMenuDate,               clrBlack);
THEME_CLR(Theme, clrMenuItemCurrentFg,      clrBlack);
THEME_CLR(Theme, clrMenuItemCurrentBg,      clrCyan);
THEME_CLR(Theme, clrMenuItemSelectable,     clrWhite);
THEME_CLR(Theme, clrMenuItemNonSelectable,  clrCyan);
THEME_CLR(Theme, clrMenuEventTime,          clrWhite);
THEME_CLR(Theme, clrMenuEventVpsFg,         clrBlack);
THEME_CLR(Theme, clrMenuEventVpsBg,         clrWhite);
THEME_CLR(Theme, clrMenuEventTitle,         clrCyan);
THEME_CLR(Theme, clrMenuEventShortText,     clrWhite);
THEME_CLR(Theme, clrMenuEventDescription,   clrCyan);
THEME_CLR(Theme, clrMenuScrollbarTotal,     clrWhite);
THEME_CLR(Theme, clrMenuScrollbarShown,     clrCyan);
THEME_CLR(Theme, clrMenuText,               clrWhite);
THEME_CLR(Theme, clrReplayTitle,            clrWhite);
THEME_CLR(Theme, clrReplayCurrent,          clrWhite);
THEME_CLR(Theme, clrReplayTotal,            clrWhite);
THEME_CLR(Theme, clrReplayModeJump,         clrWhite);
THEME_CLR(Theme, clrReplayProgressSeen,     clrGreen);
THEME_CLR(Theme, clrReplayProgressRest,     clrWhite);
THEME_CLR(Theme, clrReplayProgressSelected, clrRed);
THEME_CLR(Theme, clrReplayProgressMark,     clrBlack);
THEME_CLR(Theme, clrReplayProgressCurrent,  clrRed);*/

// #include "skinreel/a_top.xpm"
// #include "skinreel/b_top_x.xpm"
// #include "skinreel/c_top.xpm"
#include "skinreel/a_header.xpm"
#include "skinreel/b_header_x.xpm"
#include "skinreel/c_header.xpm"
// #include "skinreel/a_headertrenner.xpm"
// #include "skinreel/b_headertrenner_x.xpm"
// #include "skinreel/c_headertrenner.xpm"
// #include "skinreel/a_middle.xpm"
// #include "skinreel/c_middle.xpm"
// #include "skinreel/a_buttonstrenner.xpm"
// #include "skinreel/b_buttonstrenner_x.xpm"
// #include "skinreel/c_buttonstrenner.xpm"
#include "skinreel/a_buttons.xpm"
#include "skinreel/b_buttons_x.xpm"
#include "skinreel/c_buttons.xpm"
#include "skinreel/a_button_red.xpm"
#include "skinreel/b_button_red_x.xpm"
#include "skinreel/c_button_red.xpm"
#include "skinreel/a_button_green.xpm"
#include "skinreel/b_button_green_x.xpm"
#include "skinreel/c_button_green.xpm"
#include "skinreel/a_button_yellow.xpm"
#include "skinreel/b_button_yellow_x.xpm"
#include "skinreel/c_button_yellow.xpm"
#include "skinreel/a_button_blue.xpm"
#include "skinreel/b_button_blue_x.xpm"
#include "skinreel/c_button_blue.xpm"
#include "skinreel/a_button_grey.xpm"
#include "skinreel/b_button_grey_x.xpm"
#include "skinreel/c_button_grey.xpm"
// #include "skinreel/a_bottom.xpm"
// #include "skinreel/b_bottom_x.xpm"
// #include "skinreel/c_bottom.xpm"
#include "skinreel/tabline.xpm"
#include "skinreel/a_volume.xpm"
#include "skinreel/b_volumefull_x.xpm"
#include "skinreel/b_volumeempty_x.xpm"
#include "skinreel/c_volume.xpm"
#include "skinreel/a_textline.xpm"
#include "skinreel/b_textline_x.xpm"
#include "skinreel/c_textline.xpm"
#include "skinreel/teletext.xpm"
#include "skinreel/radio.xpm"
#include "skinreel/audio.xpm"
#include "skinreel/dolbydigital.xpm"
#include "skinreel/encrypted.xpm"
#include "skinreel/recording.xpm"

#include "skinreel/dc0_0.xpm"
#include "skinreel/dc0_1.xpm"
#include "skinreel/dc0_2.xpm"
#include "skinreel/dc1_0.xpm"
#include "skinreel/dc1_1.xpm"
#include "skinreel/dc1_2.xpm"
#include "skinreel/dc1_3.xpm"
#include "skinreel/dc1_4.xpm"
#include "skinreel/dc2_0.xpm"
#include "skinreel/dc2_1.xpm"
#include "skinreel/dc2_2.xpm"
#include "skinreel/dc2_3.xpm"
#include "skinreel/dc2_4.xpm"
#include "skinreel/dc2_5.xpm"

namespace
{
    void DrawBitmapTilesX(cOsd *osd, int x, int y, int x1, const cBitmap &bmp)
    {
        if (x < x1)
        {
/*	    for (int i = 0; i < MAXOSDAREAS; i++)
	    {
		cBitmap *bitmap = osd->GetBitmap(i);
		if (bitmap)
		{
    		    if (bitmap->Intersects(x, y, x + bmp.Width() - 1, y + bmp.Height() - 1))
		    {
		        if (bitmap->Covers(x, y, x + bmp.Width() - 1, y + bmp.Height() - 1))
        		{
		            bitmap->Reset();
		        }
		        x -= bitmap->X0();
		        y -= bitmap->Y0();*/
        		for (int ix = 0; ix < (x1 - x); ix++)
        		{
			    osd->DrawBitmap(x + ix, y, bmp);
        		}
/*		    }
		}
	    }*/
        }
    }

    void DrawBitmapTilesY(cOsd *osd, int x, int y, int y1, const cBitmap &bmp)
    {
        if (y < y1)
        {
/*	    for (int i = 0; i < MAXOSDAREAS; i++)
	    {
		cBitmap *bitmap = osd->GetBitmap(i);
		if (bitmap)
		{
    		    if (bitmap->Intersects(x, y, x + bmp.Width() - 1, y + bmp.Height() - 1))
		    {
		        if (bitmap->Covers(x, y, x + bmp.Width() - 1, y + bmp.Height() - 1))
        		{
		            bitmap->Reset();
		        }
		        x -= bitmap->X0();
		        y -= bitmap->Y0();*/
        		for (int iy = 0; iy < (y1 - y); iy++)
        		{
			    osd->DrawBitmap(x, y + y1, bmp);
        		}
/*		    }
		}
	    }*/
        }
    }
}

// --- cSkinReelDisplayChannel --------------------------------------------

#define Roundness   10
#define Gap          5

class cSkinReelDisplayChannel : public cSkinDisplayChannel {
private:
  cOsd *osd;
  int x0_0, x0_1, x0_2;
  int x1_0, x1_1, x1_2, x1_3, x1_4;
  int x2_0, x2_1, x2_2, x2_3, x2_4, x2_5;
  int y0_0;
  int x0, x1, x2, x3, x4, x5, x6, x7;
  int y0, y1, y2, y3, y4, y5, y6, y7;
  bool withInfo;
  int lineHeight;
  tColor frameColor;
  bool message;
  const cEvent *present;
  int lastSeen;
  static cBitmap bmDc0_0, bmDc0_1, bmDc0_2;
  static cBitmap bmDc1_0, bmDc1_1, bmDc1_2, bmDc1_3, bmDc1_4;
  static cBitmap bmDc2_0, bmDc2_1, bmDc2_2, bmDc2_3, bmDc2_4, bmDc2_5;
  static cBitmap bmTeletext, bmRadio, bmAudio, bmDolbyDigital, bmEncrypted, bmRecording;
public:
  cSkinReelDisplayChannel(bool WithInfo);
  virtual ~cSkinReelDisplayChannel();
  virtual void SetChannel(const cChannel *Channel, int Number);
  virtual void SetEvents(const cEvent *Present, const cEvent *Following);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush();
  };

cBitmap cSkinReelDisplayChannel::bmDc0_0(dc0_0_xpm);
cBitmap cSkinReelDisplayChannel::bmDc0_1(dc0_1_xpm);
cBitmap cSkinReelDisplayChannel::bmDc0_2(dc0_2_xpm);

cBitmap cSkinReelDisplayChannel::bmDc1_0(dc1_0_xpm);
cBitmap cSkinReelDisplayChannel::bmDc1_1(dc1_1_xpm);
cBitmap cSkinReelDisplayChannel::bmDc1_2(dc1_2_xpm);
cBitmap cSkinReelDisplayChannel::bmDc1_3(dc1_3_xpm);
cBitmap cSkinReelDisplayChannel::bmDc1_4(dc1_4_xpm);

cBitmap cSkinReelDisplayChannel::bmDc2_0(dc2_0_xpm);
cBitmap cSkinReelDisplayChannel::bmDc2_1(dc2_1_xpm);
cBitmap cSkinReelDisplayChannel::bmDc2_2(dc2_2_xpm);
cBitmap cSkinReelDisplayChannel::bmDc2_3(dc2_3_xpm);
cBitmap cSkinReelDisplayChannel::bmDc2_4(dc2_4_xpm);
cBitmap cSkinReelDisplayChannel::bmDc2_5(dc2_5_xpm);

cBitmap cSkinReelDisplayChannel::bmTeletext(teletext_xpm);
cBitmap cSkinReelDisplayChannel::bmRadio(radio_xpm);
cBitmap cSkinReelDisplayChannel::bmAudio(audio_xpm);
cBitmap cSkinReelDisplayChannel::bmDolbyDigital(dolbydigital_xpm);
cBitmap cSkinReelDisplayChannel::bmEncrypted(encrypted_xpm);
cBitmap cSkinReelDisplayChannel::bmRecording(recording_xpm);
  
static cBitmap bmpATextLine(a_textline_xpm);
static cBitmap bmpBTextLine(b_textline_x_xpm);
static cBitmap bmpCTextLine(c_textline_xpm);

const int TIME_BAR_WIDTH = 200;

cSkinReelDisplayChannel::cSkinReelDisplayChannel(bool WithInfo)
{
    present = NULL;
    lastSeen = -1;
    const cFont *font = cFont::GetFont(fontOsd);
    withInfo = WithInfo;
    lineHeight = font->Height();
    frameColor = Theme.Color(clrChannelFrame);
    message = false;
    bmDc0_0.SetColor(0, 0);
    bmDc0_2.SetColor(0, 0);
    bmDc2_0.SetColor(0, 0);
    bmDc2_5.SetColor(0, 0);
    if (withInfo)
    {
        x0 = 0;
        x1 = x0 + font->Width("00:00");
        x2 = x1 + Roundness;
        x3 = x0 + font->Width("00:00") + bmDc1_0.Width() + bmDc1_2.Width();;
        x7 = Setup.OSDWidth;
        x6 = x7 - lineHeight / 2;
        x5 = x6 - lineHeight / 2;
        x4 = x5 - Gap;
        y0 = 0;
        y1 = y0 + bmDc0_0.Height(); //lineHeight;
        y2 = y1 + Roundness;
        y3 = y1 + 6;
        y4 = y1 + bmDc1_0.Height();
        y5 = y4 + Gap;
        y6 = y5 + Roundness;
        y7 = y4 + bmDc2_0.Height();
        // int yt = (y0 + y1) / 2;
        // int yb = (y6 + y7) / 2;
        osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + (Setup.ChannelInfoPos ? 0 : Setup.OSDHeight - y7));
        tArea Areas[] = { { 0, 0, x7 - 1, y7 - 1, 8 } };
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        x0_0 = x0;
        x0_1 = bmDc0_0.Width();
        x0_2 = x7 - bmDc0_2.Width();
        y0_0 = (bmDc0_0.Height() - lineHeight) / 2;
        osd->DrawRectangle(x0, y0, x7 - 1, y7 - 1, Theme.Color(clrBackground));
        osd->DrawBitmap(x0_0, y0, bmDc0_0);
        DrawBitmapTilesX(osd, x0_1, y0, x0_2, bmDc0_1);
        osd->DrawBitmap(x0_2, y0, bmDc0_2);
        x1_0 = x0;
        x1_1 = bmDc1_0.Width();
        x1_2 = x3 - bmDc1_2.Width();
        x1_3 = x3;
        x1_4 = x7 - bmDc1_4.Width();
        osd->DrawBitmap(x1_0, y1, bmDc1_0);
        DrawBitmapTilesX(osd, x1_1, y1, x1_2, bmDc1_1);
        osd->DrawBitmap(x1_2, y1, bmDc1_2);
        DrawBitmapTilesX(osd, x1_3, y1, x1_4, bmDc1_3);
        osd->DrawBitmap(x1_4, y1, bmDc1_4);
        x2_0 = x0;
        x2_1 = bmDc2_0.Width();
        x2_3 = x7 - TIME_BAR_WIDTH;
        x2_2 = x2_3 - bmDc2_2.Width();
        x2_4 = x2_3 + bmDc2_3.Width();
        x2_5 = x7 - bmDc2_5.Width();
        osd->DrawBitmap(x2_0, y4, bmDc2_0);
        DrawBitmapTilesX(osd, x2_1, y4, x2_2, bmDc2_1);
        osd->DrawBitmap(x2_2, y4, bmDc2_2);
        osd->DrawBitmap(x2_3, y4, bmDc2_3);
        DrawBitmapTilesX(osd, x2_4, y4, x2_5, bmDc2_4);
        osd->DrawBitmap(x2_5, y4, bmDc2_5);
        
        
        // osd->DrawRectangle(x0, y0, x1 - 1, y1 - 1, clrTransparent);
        // osd->DrawRectangle(x0, y6, x1 - 1, y7 - 1, clrTransparent);
        // osd->DrawRectangle(x6, y0, x7 - 1, yt - 1, clrTransparent);
        // osd->DrawRectangle(x6, yb, x7 - 1, y7 - 1, clrTransparent);
        // osd->DrawEllipse  (x0, y0, x1 - 1, y1 - 1, frameColor, 2);
        // osd->DrawRectangle(x1, y0, x4 - 1, y1 - 1, frameColor);
        // osd->DrawRectangle(x5, y0, x6 - 1, y1 - 1, frameColor);
        // osd->DrawEllipse  (x6, y0, x7 - 1, y1 - 1, frameColor, 5);
        // osd->DrawRectangle(x0, y1, x1 - 1, y2 - 1, frameColor);
        // osd->DrawEllipse  (x1, y1, x2 - 1, y2 - 1, frameColor, -2);
        // osd->DrawRectangle(x0, y3, x1 - 1, y4 - 1, frameColor);
        // osd->DrawRectangle(x0, y5, x1 - 1, y6 - 1, frameColor);
        // osd->DrawEllipse  (x1, y5, x2 - 1, y6 - 1, frameColor, -3);
        // osd->DrawEllipse  (x0, y6, x1 - 1, y7 - 1, frameColor, 3);
        // osd->DrawRectangle(x1, y6, x4 - 1, y7 - 1, frameColor);
        // osd->DrawRectangle(x5, y6, x6 - 1, y7 - 1, frameColor);
        // osd->DrawEllipse  (x6, y6, x7 - 1, y7 - 1, frameColor, 5);
    }
    else
    {
        x0 = 0;
        x1 = lineHeight / 2;
        x2 = lineHeight;
        x3 = x2 + Gap;
        x7 = Setup.OSDWidth;
        x6 = x7 - lineHeight / 2;
        x5 = x6 - lineHeight / 2;
        x4 = x5 - Gap;
        x0_0 = x0;
        x0_1 = bmDc0_0.Width();
        x0_2 = x7 - bmDc0_2.Width();
        y0_0 = (bmDc0_0.Height() - lineHeight) / 2;
        y0 = 0;
        y1 = bmDc0_0.Height();
        osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + (Setup.ChannelInfoPos ? 0 : Setup.OSDHeight - y1));
        tArea Areas[] = { { x0, y0, x7 - 1, y1 - 1, 8 } };
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        osd->DrawRectangle(x0, y0, x7 - 1, y1 - 1, Theme.Color(clrBackground));
        osd->DrawBitmap(x0_0, y0, bmDc0_0);
        DrawBitmapTilesX(osd, x0_1, y0, x0_2, bmDc0_1);
        osd->DrawBitmap(x0_2, y0, bmDc0_2);
        
        /*x0 = 0;
        x1 = lineHeight / 2;
        x2 = lineHeight;
        x3 = x2 + Gap;
        x7 = Setup.OSDWidth;
        x6 = x7 - lineHeight / 2;
        x5 = x6 - lineHeight / 2;
        x4 = x5 - Gap;
        y0 = 0;
        osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + (Setup.ChannelInfoPos ? 0 : Setup.OSDHeight - y1));
        tArea Areas[] = { { x0, y0, x7 - 1, y1 - 1, 8 } };
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        osd->DrawRectangle(x0, y0, x7 - 1, y1 - 1, clrTransparent);
        osd->DrawEllipse  (x0, y0, x1 - 1, y1 - 1, frameColor, 7);
        osd->DrawRectangle(x1, y0, x2 - 1, y1 - 1, frameColor);
        osd->DrawRectangle(x5, y0, x6 - 1, y1 - 1, frameColor);
        osd->DrawEllipse  (x6, y0, x7 - 1, y1 - 1, frameColor, 5);*/
    }
}

cSkinReelDisplayChannel::~cSkinReelDisplayChannel()
{
    delete osd;
}

void cSkinReelDisplayChannel::SetChannel(const cChannel *Channel, int Number)
{
    const u_int COLOR_SYMBOL_ON  = 0xFFFFFFFF;
    const u_int COLOR_SYMBOL_REC = 0xFFFF6060;
    const u_int COLOR_SYMBOL_OFF = 0xFF808080;
  // osd->DrawRectangle(x3, y0, x4 - 1, y1 - 1, frameColor);
    osd->DrawBitmap(x0_0, y0, bmDc0_0);
    DrawBitmapTilesX(osd, x0_1, y0, x0_2, bmDc0_1);
    osd->DrawBitmap(x0_2, y0, bmDc0_2);
  int x = x4 - 5;
  if (Channel && !Channel->GroupSep()) {
     int d = 3;
     bool rec = cRecordControls::Active();
     x -= bmRecording.Width() + d;
     osd->DrawBitmap(x, y0 + (y1 - y0 - bmRecording.Height()) / 2, bmRecording, rec ? COLOR_SYMBOL_REC : COLOR_SYMBOL_OFF, frameColor);
     x -= bmEncrypted.Width() + d;
     osd->DrawBitmap(x, y0 + (y1 - y0 - bmEncrypted.Height()) / 2, bmEncrypted, Channel->Ca() ? COLOR_SYMBOL_ON : COLOR_SYMBOL_OFF, frameColor);
     x -= bmDolbyDigital.Width() + d;
     osd->DrawBitmap(x, y0 + (y1 - y0 - bmDolbyDigital.Height()) / 2, bmDolbyDigital, Channel->Dpid(0) ? COLOR_SYMBOL_ON : COLOR_SYMBOL_OFF, frameColor);
     x -= bmAudio.Width() + d;
     osd->DrawBitmap(x, y0, bmAudio, Channel->Apid(1) ? COLOR_SYMBOL_ON : COLOR_SYMBOL_OFF, frameColor);
     if (Channel->Vpid()) {
        x -= bmTeletext.Width() + d;
        osd->DrawBitmap(x, y0 + (y1 - y0 - bmTeletext.Height()) / 2, bmTeletext, Channel->Tpid() ? COLOR_SYMBOL_ON : COLOR_SYMBOL_OFF, frameColor);
        }
     else if (Channel->Apid(0)) {
        x -= bmRadio.Width() + d;
        osd->DrawBitmap(x, y0 + (y1 - y0 - bmRadio.Height()) / 2, bmRadio, COLOR_SYMBOL_ON, frameColor);
        }
     }
  osd->DrawText(x0_1, y0_0, ChannelString(Channel, Number), 0xFFE0E0E0, 0, cFont::GetFont(fontOsd), x - x3 - 2);
}

void cSkinReelDisplayChannel::Flush()
{
  if (withInfo) {
     if (!message) {
        osd->DrawBitmap(x2_0, y4, bmDc2_0);
        DrawBitmapTilesX(osd, x2_1, y4, x2_2, bmDc2_1);
        osd->DrawBitmap(x2_2, y4, bmDc2_2);
        osd->DrawBitmap(x2_3, y4, bmDc2_3);
        DrawBitmapTilesX(osd, x2_4, y4, x2_5, bmDc2_4);
        osd->DrawBitmap(x2_5, y4, bmDc2_5);
        
        const cFont *font = cFont::GetFont(fontSml);
        cString date = DayDateTime();
        int w = font->Width(date);
        osd->DrawText(x4 - w - 2, y4 + (bmDc2_3.Height() - font->Height(date)) / 2 - 1, date, 0xFFE0E0E0, 0, font);
        cDevice *Device = cDevice::PrimaryDevice();
        const tTrackId *Track = Device->GetTrack(Device->GetCurrentAudioTrack());
        osd->DrawText(x2_1, y6, Track ? Track->description : "", 0xFFE0E0E0, 0, font, x4 - x3 - w - 4);
        }

     int seen = 0;
     if (present) {
        time_t t = time(NULL);
        if (t > present->StartTime())
           seen = min(y4 - y3 - 1, int((y4 - y3) * double(t - present->StartTime()) / present->Duration()));
        }
     if (seen != lastSeen) {
        int x = (x1_2 + x1_3 - ScrollWidth) / 2;
        osd->DrawRectangle(x, y3, x + ScrollWidth - 1, y4 - 1, 0xFF000000);
        if (seen)
        osd->DrawRectangle(x, y3, x + ScrollWidth - 1, y3 + seen, Theme.Color(clrChannelTimebarSeen));
        lastSeen = seen;
        }
     }
  osd->Flush();
}

void cSkinReelDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following)
{
  if (!withInfo)
     return;
  if (present != Present)
     lastSeen = -1;
  present = Present;
    osd->DrawBitmap(x1_0, y1, bmDc1_0);
    DrawBitmapTilesX(osd, x1_1, y1, x1_2, bmDc1_1);
    osd->DrawBitmap(x1_2, y1, bmDc1_2);
    DrawBitmapTilesX(osd, x1_3, y1, x1_4, bmDc1_3);
    osd->DrawBitmap(x1_4, y1, bmDc1_4);
  for (int i = 0; i < 2; i++) {
      const cEvent *e = !i ? Present : Following;
      if (e) {
         osd->DrawText(x1_1, y3 + 2 * i * lineHeight, e->GetTimeString(), 0xFFE0E0E0, 0, cFont::GetFont(fontOsd));
         osd->DrawText(x1_3, y3 + 2 * i * lineHeight, e->Title(), Theme.Color(clrChannelEpgTitle), 0, cFont::GetFont(fontOsd), x4 - x3 - 2);
         osd->DrawText(x1_3, y3 + (2 * i + 1) * lineHeight, e->ShortText(), Theme.Color(clrChannelEpgShortText), 0, cFont::GetFont(fontSml), x4 - x3 - 2);
         }
      }
}

void cSkinReelDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
  const cFont *font = cFont::GetFont(withInfo ? fontSml : fontOsd);
  if (Text) {
     int yt = withInfo ? y6 : y0;
     int yb = withInfo ? y7 : y1;
     osd->SaveRegion(x2, yt, x4 - 1, yb - 1);
     osd->DrawText(x2_1, yt, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), font, x4 - x3, 0, taCenter);
     message = true;
     }
  else {
     osd->RestoreRegion();
     message = false;
     }
}

// --- cSkinClassicDisplayReplay ---------------------------------------------

class cSkinClassicDisplayReplay : public cSkinDisplayReplay {
private:
  cOsd *osd;
  int x0, x1;
  int y0, y1, y2, y3;
  int lastCurrentWidth;
  bool message;
public:
  cSkinClassicDisplayReplay(bool ModeOnly);
  virtual ~cSkinClassicDisplayReplay();
  virtual void SetTitle(const char *Title);
  virtual void SetMode(bool Play, bool Forward, int Speed);
  virtual void SetProgress(int Current, int Total);
  virtual void SetCurrent(const char *Current);
  virtual void SetTotal(const char *Total);
  virtual void SetJump(const char *Jump);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

// --- cSkinReelDisplayMenu -----------------------------------------------

class cSkinReelDisplayMenu : public cSkinDisplayMenu {
private:
    cOsd *osd;
    int x0, x1;
    int y0, y1, y1_2, y2, y3, y4, y4_2, y5, osdWidth, osdHeight;
    int lineHeight;
//    static cBitmap bmpATop;
//    static cBitmap bmpBTop;
//    static cBitmap bmpCTop;
    static cBitmap bmpAHeader;
    static cBitmap bmpBHeader;
    static cBitmap bmpCHeader;
//     static cBitmap bmpAHeaderTrenner;
//     static cBitmap bmpBHeaderTrenner;
//     static cBitmap bmpCHeaderTrenner;
//     static cBitmap bmpAMiddle;
//     static cBitmap bmpCMiddle;
//     static cBitmap bmpAButtonsTrenner;
//     static cBitmap bmpBButtonsTrenner;
//     static cBitmap bmpCButtonsTrenner;
    static cBitmap bmpAButtons;
    static cBitmap bmpBButtons;
    static cBitmap bmpCButtons;
    static cBitmap bmpAButtonRed;
    static cBitmap bmpBButtonRed;
    static cBitmap bmpCButtonRed;
    static cBitmap bmpAButtonGreen;
    static cBitmap bmpBButtonGreen;
    static cBitmap bmpCButtonGreen;
    static cBitmap bmpAButtonYellow;
    static cBitmap bmpBButtonYellow;
    static cBitmap bmpCButtonYellow;
    static cBitmap bmpAButtonBlue;
    static cBitmap bmpBButtonBlue;
    static cBitmap bmpCButtonBlue;
    static cBitmap bmpAButtonGrey;
    static cBitmap bmpBButtonGrey;
    static cBitmap bmpCButtonGrey;
//    static cBitmap bmpABottom;
//    static cBitmap bmpBBottom;
//    static cBitmap bmpCBottom;
    static cBitmap bmpTabline;
    void SetScrollbar();
    void DrawBmpHorLine(int yPos,
                        const cBitmap &bmpA,
                        const cBitmap &bmpB,
                        const cBitmap &bmpC);
    void DrawButton(const char *text,
                    const cBitmap &bmpA,
                    const cBitmap &bmpB,
                    const cBitmap &bmpC,
                    tColor fgColor,
                    int &x, int w);
public:
    cSkinReelDisplayMenu();
    virtual ~cSkinReelDisplayMenu();
    virtual void Scroll(bool Up, bool Page);
    virtual int MaxItems();
    virtual void Clear();
    virtual void SetTitle(const char *Title);
    virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
    virtual void SetEvent(const cEvent *Event);
    virtual void SetRecording(const cRecording *Recording);
    virtual void SetText(const char *Text, bool FixedFont);
    virtual void Flush();
};

// --- cSkinReelDisplayVolume ---------------------------------------------

class cSkinReelDisplayVolume : public cSkinDisplayVolume {
private:
    cBitmap bmpAVolume;
    cBitmap bmpBVolumeFull;
    cBitmap bmpBVolumeEmpty;
    cBitmap bmpCVolume;
    cOsd *osd;
public:
    cSkinReelDisplayVolume();
    virtual ~cSkinReelDisplayVolume();
    virtual void SetVolume(int Current, int Total, bool Mute);
    virtual void Flush();
};

cSkinReelDisplayVolume::cSkinReelDisplayVolume()
:bmpAVolume(a_volume_xpm),
 bmpBVolumeFull(b_volumefull_x_xpm),
 bmpBVolumeEmpty(b_volumeempty_x_xpm),
 bmpCVolume(c_volume_xpm)
{
    bmpATextLine.SetColor(0, 0);
    bmpCTextLine.SetColor(0, 0);
    bmpAVolume.SetColor(0, 0);
    bmpCVolume.SetColor(0, 0);
    const cFont *font = cFont::GetFont(fontOsd);
    int lineHeight = font->Height();
    osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - lineHeight);
    tArea Areas[] = { { 0, 0, Setup.OSDWidth - 1, bmpBTextLine.Height() - 1, 8 } };
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
}

cSkinReelDisplayVolume::~cSkinReelDisplayVolume()
{
    delete osd;
}

void cSkinReelDisplayVolume::SetVolume(int Current, int Total, bool Mute)
{
    const cFont *font = cFont::GetFont(fontOsd);
    osd->DrawRectangle(0, 0, osd->Width() - 1, osd->Height() - 1, clrTransparent);
    if (Mute)
    {
        const char *Prompt = tr("Mute");
        const int promptWidth = font->Width(Prompt) + 6;
        const int promptHeight = font->Height(Prompt);
        const int x0Prompt = 0;
        const int x1Prompt = bmpATextLine.Width();
        const int x2Prompt = x1Prompt + promptWidth;
        
        osd->DrawBitmap(x0Prompt, 0, bmpATextLine);
        DrawBitmapTilesX(osd, x1Prompt, 0, x2Prompt, bmpBTextLine);
        osd->DrawBitmap(x2Prompt, 0, bmpCTextLine);
        osd->DrawText(x1Prompt, (bmpBTextLine.Height() - promptHeight) / 2, Prompt, 0xFFE0E0E0, clrBackground, font, promptWidth, promptHeight, taCenter);
    }
    else
    {
        const char *Prompt = tr("Volume ");
        const int promptWidth = font->Width(Prompt) + 6;
        const int promptHeight = font->Height(Prompt);
        const int x0Prompt = 0;
        const int x1Prompt = bmpATextLine.Width();
        const int x2Prompt = x1Prompt + promptWidth;
        const int x3Prompt = x2Prompt + bmpCTextLine.Width();
        const int x0Volume = x3Prompt + 4;
        const int x1_0Volume = x0Volume + bmpAVolume.Width();
        const int x2Volume = osd->Width() - bmpCVolume.Width();
        const int x1_1Volume = x1_0Volume + (x2Volume - x1_0Volume) * Current / Total;

        osd->DrawBitmap(x0Prompt, 0, bmpATextLine);
        DrawBitmapTilesX(osd, x1Prompt, 0, x2Prompt, bmpBTextLine);
        osd->DrawBitmap(x2Prompt, 0, bmpCTextLine);
        osd->DrawText(x1Prompt, (bmpBTextLine.Height() - promptHeight) / 2, Prompt, 0xFFE0E0E0, clrBackground, font, promptWidth, promptHeight, taCenter);

        const int yVolume = (bmpBTextLine.Height() - bmpBVolumeEmpty.Height()) / 2;
        osd->DrawBitmap(x0Volume, yVolume, bmpAVolume);
        DrawBitmapTilesX(osd, x1_0Volume, yVolume, x1_1Volume, bmpBVolumeFull);
        DrawBitmapTilesX(osd, x1_1Volume, yVolume, x2Volume, bmpBVolumeEmpty);
        osd->DrawBitmap(x2Volume, yVolume, bmpCVolume);
    }
}

void cSkinReelDisplayVolume::Flush(void)
{
    osd->Flush();
}

// --- cSkinClassicDisplayTracks ---------------------------------------------

class cSkinClassicDisplayTracks : public cSkinDisplayTracks {
private:
  cOsd *osd;
  int x0, x1;
  int y0, y1, y2;
  int lineHeight;
  int currentIndex;
  void SetItem(const char *Text, int Index, bool Current);
public:
  cSkinClassicDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual ~cSkinClassicDisplayTracks();
  virtual void SetTrack(int Index, const char * const *Tracks);
  virtual void SetAudioChannel(int AudioChannel) {}
  virtual void Flush(void);
  };

// --- cSkinReelDisplayMessage --------------------------------------------

class cSkinReelDisplayMessage : public cSkinDisplayMessage {
private:
    cOsd *osd;
public:
    cSkinReelDisplayMessage();
    virtual ~cSkinReelDisplayMessage();
    virtual void SetMessage(eMessageType Type, const char *Text);
    virtual void Flush(void);
};


cSkinReelDisplayMessage::cSkinReelDisplayMessage()
{
    const cFont *font = cFont::GetFont(fontOsd);
    int lineHeight = font->Height();
    osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - lineHeight);
    tArea Areas[] = { { 0, 0, Setup.OSDWidth - 1, lineHeight - 1, 2 } };
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
}

cSkinReelDisplayMessage::~cSkinReelDisplayMessage()
{
    delete osd;
}

void cSkinReelDisplayMessage::SetMessage(eMessageType Type, const char *Text)
{
    const cFont *font = cFont::GetFont(fontOsd);
    osd->DrawText(0, 0, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), font, Setup.OSDWidth, 0, taCenter);
}

void cSkinReelDisplayMessage::Flush()
{
    osd->Flush();
}

// --- cSkinReelDisplayMenu --------------------------------------------

void cSkinReelDisplayMenu::DrawBmpHorLine(int yPos,
                                          const cBitmap &bmpA,
                                          const cBitmap &bmpB,
                                          const cBitmap &bmpC)
{
    osd->DrawBitmap(0, yPos, bmpA);
    const int xb = bmpA.Width();
    const int xc = osdWidth - bmpC.Width();
    DrawBitmapTilesX(osd, xb, yPos, xc, bmpB);
    osd->DrawBitmap(xc, yPos, bmpC);
}

// cBitmap cSkinReelDisplayMenu::bmpATop(a_top_xpm);
// cBitmap cSkinReelDisplayMenu::bmpBTop(b_top_x_xpm);
// cBitmap cSkinReelDisplayMenu::bmpCTop(c_top_xpm);
cBitmap cSkinReelDisplayMenu::bmpAHeader(a_header_xpm);
cBitmap cSkinReelDisplayMenu::bmpBHeader(b_header_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCHeader(c_header_xpm);
// cBitmap cSkinReelDisplayMenu::bmpAHeaderTrenner(a_headertrenner_xpm);
// cBitmap cSkinReelDisplayMenu::bmpBHeaderTrenner(b_headertrenner_x_xpm);
// cBitmap cSkinReelDisplayMenu::bmpCHeaderTrenner(c_headertrenner_xpm);
// cBitmap cSkinReelDisplayMenu::bmpAMiddle(a_middle_xpm);
// cBitmap cSkinReelDisplayMenu::bmpCMiddle(c_middle_xpm);
// cBitmap cSkinReelDisplayMenu::bmpAButtonsTrenner(a_buttonstrenner_xpm);
// cBitmap cSkinReelDisplayMenu::bmpBButtonsTrenner(b_buttonstrenner_x_xpm);
// cBitmap cSkinReelDisplayMenu::bmpCButtonsTrenner(c_buttonstrenner_xpm);
cBitmap cSkinReelDisplayMenu::bmpAButtons(a_buttons_xpm);
cBitmap cSkinReelDisplayMenu::bmpBButtons(b_buttons_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCButtons(c_buttons_xpm);
cBitmap cSkinReelDisplayMenu::bmpAButtonRed(a_button_red_xpm);
cBitmap cSkinReelDisplayMenu::bmpBButtonRed(b_button_red_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCButtonRed(c_button_red_xpm);
cBitmap cSkinReelDisplayMenu::bmpAButtonGreen(a_button_green_xpm);
cBitmap cSkinReelDisplayMenu::bmpBButtonGreen(b_button_green_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCButtonGreen(c_button_green_xpm);
cBitmap cSkinReelDisplayMenu::bmpAButtonYellow(a_button_yellow_xpm);
cBitmap cSkinReelDisplayMenu::bmpBButtonYellow(b_button_yellow_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCButtonYellow(c_button_yellow_xpm);
cBitmap cSkinReelDisplayMenu::bmpAButtonBlue(a_button_blue_xpm);
cBitmap cSkinReelDisplayMenu::bmpBButtonBlue(b_button_blue_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCButtonBlue(c_button_blue_xpm);
cBitmap cSkinReelDisplayMenu::bmpAButtonGrey(a_button_grey_xpm);
cBitmap cSkinReelDisplayMenu::bmpBButtonGrey(b_button_grey_x_xpm);
cBitmap cSkinReelDisplayMenu::bmpCButtonGrey(c_button_grey_xpm);
// cBitmap cSkinReelDisplayMenu::bmpABottom(a_bottom_xpm);
// cBitmap cSkinReelDisplayMenu::bmpBBottom(b_bottom_x_xpm);
// cBitmap cSkinReelDisplayMenu::bmpCBottom(c_bottom_xpm);
cBitmap cSkinReelDisplayMenu::bmpTabline(tabline_xpm);

cSkinReelDisplayMenu::cSkinReelDisplayMenu()
:   osdWidth(Setup.OSDWidth),
    osdHeight(Setup.OSDHeight)
{
    bmpAButtons.SetColor(0, 0);
    bmpCButtons.SetColor(0, 0);
    bmpAHeader.SetColor(0, 0);
    // bmpBTop.SetColor(0, clrTransparent);
    bmpCHeader.SetColor(0, 0);
    // bmpAMiddle.SetColor(0, clrGray50);
    // bmpCMiddle.SetColor(0, clrGray50);
    // bmpABottom.SetColor(0, clrTransparent);
    // bmpCBottom.SetColor(0, clrTransparent);
    const cFont *font = cFont::GetFont(fontOsd);
    lineHeight = font->Height();
    x0 = 0; // bmpAMiddle.Width();
    x1 = osdWidth; // - bmpCMiddle.Width();
    y0 = 0; // bmpBTop.Height();
    y1_2 = y0 + bmpBHeader.Height();
    y1 = y1_2; // + bmpBHeaderTrenner.Height();
    y2 = y1 + lineHeight;
    y5 = osdHeight; // - bmpBBottom.Height();
    y4_2 = y5 - bmpBButtonRed.Height();
    y4 = y4_2; // - bmpBButtonsTrenner.Height();
    y3 = y4 - lineHeight;
    osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
    tArea Areas[] = { { 0, 0, osdWidth - 1, /*y1 - 1, 8 },
                      { 0, y1, osdWidth - 1, y4_2 - 1, 8},
                      { 0, y4_2, osdWidth - 1, */osdHeight - 1, 8} };
    if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
    {
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    }
    osd->DrawRectangle(0, 0, osdWidth - 1, osdHeight - 1, clrTransparent);
    // Top
    // DrawBmpHorLine(0, bmpATop, bmpBTop, bmpCTop);
    // Header
    DrawBmpHorLine(y0, bmpAHeader, bmpBHeader, bmpCHeader);
    // Header trenner
    // DrawBmpHorLine(y1_2, bmpAHeaderTrenner, bmpBHeaderTrenner, bmpCHeaderTrenner);
    // A Middle
    // DrawBitmapTilesY(osd, 0, y1, y4, bmpAMiddle);
    // DrawBitmapTilesY(osd, x1, y1, y4, bmpCMiddle);
    // Bottonstrenner
    // DrawBmpHorLine(y4, bmpAButtonsTrenner, bmpBButtonsTrenner, bmpCButtonsTrenner);
    // Bottom
    // DrawBmpHorLine(y5, bmpABottom, bmpBBottom, bmpCBottom);
#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
    display = 2;
    changing = false;
#endif
}

cSkinReelDisplayMenu::~cSkinReelDisplayMenu()
{
#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
    display = 0;
    changing = false;
#endif
    delete osd;
}

void cSkinReelDisplayMenu::SetScrollbar()
{
    if (textScroller.CanScroll())
    {
        int yt = textScroller.Top();
        int yb = yt + textScroller.Height();
        int st = yt;
        int sb = yb;
        int tt = st + (sb - st) * textScroller.Offset() / textScroller.Total();
        int tb = tt + (sb - st) * textScroller.Shown() / textScroller.Total();
        int xl = x1 - ScrollWidth;
        osd->DrawRectangle(xl, st, x1 - 1, sb, Theme.Color(clrMenuScrollbarTotal));
        osd->DrawRectangle(xl, tt, x1 - 1, tb, Theme.Color(clrMenuScrollbarShown));
    }
}

void cSkinReelDisplayMenu::Scroll(bool Up, bool Page)
{
  cSkinDisplayMenu::Scroll(Up, Page);
  SetScrollbar();
}

int cSkinReelDisplayMenu::MaxItems(void)
{
  return (y3 - y2) / lineHeight;
}

void cSkinReelDisplayMenu::Clear()
{
    textScroller.Reset();
    osd->DrawRectangle(x0, y1, x1 - 1, y4 - 1, Theme.Color(clrBackground));
}

void cSkinReelDisplayMenu::SetTitle(const char *Title)
{
    const cFont *font = cFont::GetFont(fontOsd);
    const int y = y0 + (y1_2 - y0 - font->Height(Title)) / 2;
    DrawBmpHorLine(y0, bmpAHeader, bmpBHeader, bmpCHeader);
    osd->DrawText(bmpAHeader.Width() + 3, y, Title, 0xFFE0E0E0, 0, font, osdWidth - bmpAHeader.Width() - bmpCHeader.Width() - 3);
}

void cSkinReelDisplayMenu::DrawButton(const char *text,
                                      const cBitmap &bmpA,
                                      const cBitmap &bmpB,
                                      const cBitmap &bmpC,
                                      tColor fgColor,
                                      int &x, int w)
{
    const cFont *font = cFont::GetFont(fontOsd);
    osd->DrawBitmap(x, y4_2, bmpA);
    const int x1 = x;
    x += bmpA.Width();
    DrawBitmapTilesX(osd, x, y4_2, x + w, bmpB);
    x += w;
    osd->DrawBitmap(x, y4_2, bmpC);
    x += bmpC.Width();
    const int y = y4_2 + (y5 - y4_2 - font->Height(text)) / 2;
    osd->DrawText(x1, y - 1, text, fgColor, clrBackground, font, x - x1, 0, taCenter);
}

void cSkinReelDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
    if (Red || Green || Yellow || Blue)
    {
        int buttonCount = 0;
        int buttonsOverhead = bmpAButtons.Width() + bmpCButtons.Width();
        if (Red)
        {
            buttonsOverhead += bmpAButtonRed.Width() + bmpCButtonRed.Width();
            ++ buttonCount;
        }
        if (Green)
        {
            buttonsOverhead += bmpAButtonGreen.Width() + bmpCButtonGreen.Width();
            ++ buttonCount;
        }
        if (Yellow)
        {
            buttonsOverhead += bmpAButtonYellow.Width() + bmpCButtonYellow.Width();
            ++ buttonCount;
        }
        if (Blue)
        {
            buttonsOverhead += bmpAButtonBlue.Width() + bmpCButtonBlue.Width();
            ++ buttonCount;
        }
        const int buttonsBTotalWidth = osdWidth - buttonsOverhead;
        int buttonBWidth[4];
        int width = buttonsBTotalWidth;
        int n = 0;
        for (n = 0; n < buttonCount - 1; ++n)
        {
            const int bw = buttonsBTotalWidth / buttonCount;
            buttonBWidth[n] = bw;
            width -= bw;
        }
        buttonBWidth[n] = width;
        n = 0;
        osd->DrawBitmap(0, y4_2, bmpAButtons);
        int x = bmpAButtons.Width();
        if (Red)
        {
            DrawButton(Red, bmpAButtonRed, bmpBButtonRed, bmpCButtonRed, Theme.Color(clrButtonRedFg), x, buttonBWidth[n++]);
        }
        if (Green)
        {
            DrawButton(Green, bmpAButtonGreen, bmpBButtonGreen, bmpCButtonGreen, Theme.Color(clrButtonGreenFg), x, buttonBWidth[n++]);
        }
        if (Yellow)
        {
            DrawButton(Yellow, bmpAButtonYellow, bmpBButtonYellow, bmpCButtonYellow, Theme.Color(clrButtonYellowFg), x, buttonBWidth[n++]);
        }
        if (Blue)
        {
            DrawButton(Blue, bmpAButtonBlue, bmpBButtonBlue, bmpCButtonBlue, Theme.Color(clrButtonBlueFg), x, buttonBWidth[n++]);
        }
        osd->DrawBitmap(osdWidth - bmpCButtons.Width(), y4_2, bmpCButtons);
    }
    else
    {
        // No Buttons present
        DrawBmpHorLine(y4_2, bmpAButtons, bmpBButtons, bmpCButtons);
    }
}

void cSkinReelDisplayMenu::SetMessage(eMessageType Type, const char *Text)
{
  const cFont *font = cFont::GetFont(fontOsd);
  if (Text)
     osd->DrawText(x0, y3, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), font, x1 - x0, 0, taCenter);
  else
     osd->DrawRectangle(x0, y3, x1 - 1, y4 - 1, Theme.Color(clrBackground));
}

void cSkinReelDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable)
{
    int y = y2 + Index * lineHeight;
    tColor ColorFg, ColorBg;
    if (Current)
    {
        ColorFg = clrYellow; //Theme.Color(clrMenuItemCurrentFg);
        ColorBg = clrTransparent;
        DrawBitmapTilesX(osd, x0, y, x1, bmpTabline);
    }
    else {
        ColorFg = Theme.Color(Selectable ? clrMenuItemSelectable : clrMenuItemNonSelectable);
        ColorBg = Theme.Color(clrBackground);
        }
    const cFont *font = cFont::GetFont(fontOsd);
    for (int i = 0; i < MaxTabs; i++) {
        const char *s = GetTabbedText(Text, i);
        if (s) {
            int xt = x0 + Tab(i);
            osd->DrawText(xt, y, s, ColorFg, ColorBg, font, x1 - xt);
            }
        if (!Tab(i + 1))
            break;
        }
    SetEditableWidth(x1 - x0 - Tab(1));
}

void cSkinReelDisplayMenu::SetEvent(const cEvent *Event)
{
  if (!Event)
     return;
  const cFont *font = cFont::GetFont(fontOsd);
  int xl = x0 + 10;
  int y = y2;
  cTextScroller ts;
  char t[32];
  snprintf(t, sizeof(t), "%s  %s - %s", *Event->GetDateString(), *Event->GetTimeString(), *Event->GetEndTimeString());
  ts.Set(osd, xl, y, x1 - xl, y3 - y, t, font, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
  if (Event->Vps() && Event->Vps() != Event->StartTime()) {
     char *buffer;
     asprintf(&buffer, " VPS: %s", *Event->GetVpsString());
     const cFont *font = cFont::GetFont(fontSml);
     osd->DrawText(x1 - font->Width(buffer), y, buffer, Theme.Color(clrMenuEventVpsFg), Theme.Color(clrMenuEventVpsBg), font);
     free(buffer);
     }
  y += ts.Height();
  y += font->Height();
  ts.Set(osd, xl, y, x1 - xl, y3 - y, Event->Title(), font, Theme.Color(clrMenuEventTitle), Theme.Color(clrBackground));
  y += ts.Height();
  if (!isempty(Event->ShortText())) {
     const cFont *font = cFont::GetFont(fontSml);
     ts.Set(osd, xl, y, x1 - xl, y3 - y, Event->ShortText(), font, Theme.Color(clrMenuEventShortText), Theme.Color(clrBackground));
     y += ts.Height();
     }
  y += font->Height();
  if (!isempty(Event->Description())) {
     textScroller.Set(osd, xl, y, x1 - xl - 2 * ScrollWidth, y3 - y, Event->Description(), font, Theme.Color(clrMenuEventDescription), Theme.Color(clrBackground));
     SetScrollbar();
     }
}

void cSkinReelDisplayMenu::SetRecording(const cRecording *Recording)
{
  if (!Recording)
     return;
  const cRecordingInfo *Info = Recording->Info();
  const cFont *font = cFont::GetFont(fontOsd);
  int xl = x0 + 10;
  int y = y2;
  cTextScroller ts;
  char t[32];
  snprintf(t, sizeof(t), "%s  %s", *DateString(Recording->start), *TimeString(Recording->start));
  ts.Set(osd, xl, y, x1 - xl, y3 - y, t, font, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
  y += ts.Height();
  y += font->Height();
  const char *Title = Info->Title();
  if (isempty(Title))
     Title = Recording->Name();
  ts.Set(osd, xl, y, x1 - xl, y3 - y, Title, font, Theme.Color(clrMenuEventTitle), Theme.Color(clrBackground));
  y += ts.Height();
  if (!isempty(Info->ShortText())) {
     const cFont *font = cFont::GetFont(fontSml);
     ts.Set(osd, xl, y, x1 - xl, y3 - y, Info->ShortText(), font, Theme.Color(clrMenuEventShortText), Theme.Color(clrBackground));
     y += ts.Height();
     }
  y += font->Height();
  if (!isempty(Info->Description())) {
     textScroller.Set(osd, xl, y, x1 - xl - 2 * ScrollWidth, y3 - y, Info->Description(), font, Theme.Color(clrMenuEventDescription), Theme.Color(clrBackground));
     SetScrollbar();
     }
}

void cSkinReelDisplayMenu::SetText(const char *Text, bool FixedFont)
{
  const cFont *font = cFont::GetFont(FixedFont ? fontFix : fontOsd);
  textScroller.Set(osd, x0, y2, x1 - x0 - 2 * ScrollWidth, y3 - y2, Text, font, Theme.Color(clrMenuText), Theme.Color(clrBackground));
  SetScrollbar();
}

void cSkinReelDisplayMenu::Flush()
{
    cString date = DayDateTime();
    const cFont *font = cFont::GetFont(fontOsd);
    DrawBitmapTilesX(osd, osdWidth - font->Width(date) - bmpCHeader.Width() - 3, y0,
                     osdWidth - bmpCHeader.Width() - 3, bmpBHeader);
    const int y = y0 + (y1_2 - y0 - font->Height(date)) / 2;
    osd->DrawText(osdWidth - bmpCHeader.Width() - font->Width(date), y, date, 0xFFE0E0E0, 0, font);
#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
    if (changing) {
	tColor searchcolor = Theme.Color(clrBackground);
	int area = osd->GetBitmap(1) ? 1 : 0;
	int index = osd->GetBitmap(area)->Index(searchcolor);
	osd->GetBitmap(area)->SetColor(index, ChangeColor);
	osd->Flush();
	osd->GetBitmap(area)->SetColor(index, searchcolor);
	}
    else
#endif
    osd->Flush();
}

// --- cSkinReel ----------------------------------------------------------

cSkinReel::cSkinReel(void)
:cSkin("reel", &::Theme)//XXX naming problem???
{
}

const char *cSkinReel::Description()
{
  return tr("ReelBox VDR");
}

cSkinDisplayChannel *cSkinReel::DisplayChannel(bool WithInfo)
{
  return new cSkinReelDisplayChannel(WithInfo);
}

cSkinDisplayMenu *cSkinReel::DisplayMenu()
{
  return new cSkinReelDisplayMenu;
}

cSkinDisplayReplay *cSkinReel::DisplayReplay(bool ModeOnly)
{
  return new cSkinClassicDisplayReplay(ModeOnly);
}

cSkinDisplayVolume *cSkinReel::DisplayVolume()
{
  return new cSkinReelDisplayVolume;
}

cSkinDisplayTracks *cSkinReel::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
  return new cSkinClassicDisplayTracks(Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cSkinReel::DisplayMessage(void)
{
  return new cSkinReelDisplayMessage;
}

// --- cPluginSkinReel -----------------------------------------------------

class cPluginSkinReel : public cPlugin {
private:
#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
  struct skin_serviceData {
    char *servicename; 
    int version;
  };

  struct skinreel_serviceData {
    char *service;	//"Version"		"Change"		"Color" "DisplayChannel" "DisplayMenu" "DisplayMessages" "DisplayReplay" "DisplayTracks" "DisplayVolume" "<colorname>"
    int number;		//Version? -> Version!	!0->change 0->no change	0 -> Anzahl >0 ->Nummer d. Farbe 
    int error;		//	<Version?	->1 nicht möglich	-> 0 = kann geändert werden, -> 1 sollte nicht geändert werden, -> 2 = Fehler				  -> 1 = existiert nicht
    tColor color;	//			color to show ->	
    char *answer;	//Date						>0 -> Name der Farbe											  Name der Farbe
    int *flags;	//							0		1		2		3		4		5		
  };
#endif
public:
  cPluginSkinReel(void);
  virtual ~cPluginSkinReel();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
  bool Service(const char *Id, void *Data);
#endif
  };

cPluginSkinReel::cPluginSkinReel(void)
{
}

cPluginSkinReel::~cPluginSkinReel()
{
}

const char *cPluginSkinReel::CommandLineHelp(void)
{
  return NULL;
}

bool cPluginSkinReel::ProcessArgs(int argc, char *argv[])
{
  return true;
}

#if VDRVERSNUM >= 10330 || defined(PATCH_CUSTOMSERVICES)
bool cPluginSkinReel::Service(const char *Id, void *Data)
{
  if (strcmp(Id, "Skin-Service") == 0) {
    if (Data == NULL)
        return false;
    if (display != 2)
        return false;
    skin_serviceData* serviceData = (skin_serviceData*) Data;
    serviceData->servicename = strdup("SkinReel-Service");
    serviceData->version = SKINVERSION;
    return true;
    }
  if (strcmp(Id, "SkinReel-Service") == 0) {
    if (Data == NULL)
        return true;
    skinreel_serviceData* serviceData = (skinreel_serviceData*) Data;
    if (strcmp(serviceData->service, "Version") == 0) {
	if (serviceData->number > SKINVERSION)
	    serviceData->error = 1;
	else
	    serviceData->error = 0;
	serviceData->number = SKINVERSION;
	return true;
	}
    if (strcmp(serviceData->service, "Change") == 0) {
	if (display == 2) {
	    if (!serviceData->number)
		changing = false;
	    else {
    		ChangeColor = serviceData->color;
	        changing = true;
		}
	    serviceData->error = 0;
	    }
	else
	    serviceData->error = 1;
	return true;
	}
    }
  return false;
}
#endif

bool cPluginSkinReel::Initialize(void)
{
  return true;
}

bool cPluginSkinReel::Start(void)
{
  new cSkinReel;
  return true;
}

void cPluginSkinReel::Housekeeping(void)
{
}

cOsdObject *cPluginSkinReel::MainMenuAction(void)
{
  return NULL;
}

cMenuSetupPage *cPluginSkinReel::SetupMenu(void)
{
  return NULL;
}

bool cPluginSkinReel::SetupParse(const char *Name, const char *Value)
{
  return false;
}

VDRPLUGINCREATOR(cPluginSkinReel); // Don't touch this!
