// viewer.c, brougs78

#include "viewer.h"


cOSDImageBitmap image;
extern bool showMenu;
int slideShowTime, slideShowDelta;


cOSDImageViewer::cOSDImageViewer(sFiles &pics) : cOsdObject()
{
	myPics = &pics;
	slideShow = false;
}

cOSDImageViewer::~cOSDImageViewer()
{
	delete osd;
}


void cOSDImageViewer::Show(void)
{
	OSDLeft = Setup.OSDLeft - OSDXOFFSET;
	OSDTop = Setup.OSDTop - OSDYOFFSET;
	OSDWidth = Setup.OSDWidth;
	OSDHeight = Setup.OSDHeight;
	
	
	osd = cOsdProvider::NewOsd(OSDXOFFSET, OSDYOFFSET);

	if (osd)
	{
		tArea areas [] = { {0, 0, OSDMAXWIDTH - 1, OSDMAXHEIGHT - 1, 8} };
		
		if (osd->CanHandleAreas(areas, sizeof(areas) / sizeof(tArea) ) == oeOk)
		{
			OSDImageSetup.have256colors = true;
			
			// initialize the graphics-varables
			wTxtArea = OSDWidth - SHADOWOFFSET;
			wPicArea = OSDWidth;
			hPicArea = OSDHeight - 3 * FONTSIZE;
			PicLeft = OSDLeft;
			PicTop = OSDTop + 3 * FONTSIZE / 2;
			
			// initialize the colors
			quantizeColors = (1 << 8) - 4; // We have to spare 4 colors
			myColors.txtFG = clrWhite;
			myColors.txtShadow = clrBlack;
			//myColors.txtBG = (clrWhite & 0x00FFFFFF) | (OSDImageSetup.textAlpha << 24);
			myColors.txtBG = 0x008488AA | (OSDImageSetup.textAlpha << 24);
			//myColors.picBG = (clrBlack & 0x00FFFFFF) | (OSDImageSetup.backgroundAlpha << 24);
			myColors.picBG = 0x0053587B | (OSDImageSetup.backgroundAlpha << 24);
	
#ifdef DEBUG
			printf("256-color-OSD available. Nice ;-)\n");
#endif

			osd->SetAreas(areas, sizeof(areas) / sizeof(tArea) );
		}
		else
		{
			int wPicAreaNew, hPicAreaNew;
			int maxPicMem;
			double ar;
			
			
			OSDImageSetup.have256colors = false;
			
			// correct the OSD-setup for byte-alignment
			OSDLeft = (OSDLeft >> 3) << 3;
			OSDWidth = (OSDWidth >> 3) << 3;
			
			// initialize the graphics-varables
			wTxtArea = OSDWidth;
			wPicArea = OSDWidth;
			hPicArea = OSDHeight - 3 * FONTSIZE;	
			
			// initialize the colors
			quantizeColors = (1 << 4) - 1; // We have to spare 1 color
			myColors.txtFG = clrWhite;
			myColors.txtBG = clrBlack;
			myColors.txtShadow = myColors.txtBG;
			myColors.picBG = clrTransparent;
			
			/*
			myColors.txtFG = clrWhite;
			myColors.txtShadow = clrTransparent;
			myColors.txtBG = clrBlack | (OSDImageSetup.textAlpha << 24);
			*/
			
			// calculate the max available memory for the picture-area
			// 2 x ( <total mem> - 2 x <info-line with 1 BIT> )
			maxPicMem = 2 * (MAXOSDMEM - 2 * (OSDWidth * FONTSIZE / 8) );
			// assume a 4:3-picture aspect-ratio
			ar = 4. / 3.;
			hPicAreaNew = (int)sqrt(maxPicMem / ar);
			wPicAreaNew = 4 * hPicAreaNew / 3;
			
			if (hPicAreaNew > hPicArea)
			{
				// Area to high -> make it wider
				hPicAreaNew = hPicArea;
				wPicAreaNew = maxPicMem / hPicAreaNew;
				
				if (wPicAreaNew > wPicArea)
				{
					// Area also to wide -> hPicArea x wPicArea is max.
					wPicAreaNew = wPicArea;
				}
			}
			else
			{
				// Area not to high, maybe to wide
				if (wPicAreaNew > wPicArea)
				{
					// Area to wide -> make it higher
					wPicAreaNew = wPicArea;
					hPicAreaNew = maxPicMem / wPicArea;
					
					if (hPicAreaNew > hPicArea)
					{
						// now it is to high -> hPicArea x wPicArea is max.
						hPicAreaNew = hPicArea;
					}
				}
			}			
			
			// assign new values and correct for byte-alignment
			wPicAreaNew = (wPicAreaNew >> 1) << 1;
			PicLeft = ( (OSDLeft + (wPicArea - wPicAreaNew) / 2) >> 1) << 1;
			PicTop = OSDTop + (hPicArea - hPicAreaNew) / 2 + 3 * FONTSIZE / 2;
			wPicArea = wPicAreaNew;
			hPicArea = hPicAreaNew;

#ifdef DEBUG
			printf("No 256-color-OSD available. We have to make some compromises.\n");
			printf("Size of picture area: %dx%d@4bpp (16colors)\n", wPicArea, hPicArea);
			printf("OSD-memory-usage: %d of %d Bytes\n", MAXOSDMEM - (maxPicMem - wPicArea * hPicArea), MAXOSDMEM);
#endif

			tArea areas [] = {
				{OSDLeft, OSDTop, OSDLeft + wTxtArea - 1, OSDTop + FONTSIZE - 1, 1},
				{PicLeft, PicTop, PicLeft + wPicArea - 1, PicTop + hPicArea - 1, 4}, 
				{OSDLeft, OSDTop + OSDHeight - FONTSIZE, OSDLeft + wTxtArea - 1, OSDTop + OSDHeight - 1, 1}
			};

			eOsdError status = osd->SetAreas(areas, sizeof(areas) / sizeof(tArea) );
			printf("Set areas: %d\n", status);
		}
			
		//osd->DrawRectangle(0, 0, OSDMAXWIDTH - 1, OSDMAXHEIGHT - 1, clrTransparent);
		osd->Flush();

		ViewBackground();
		ViewInfo(tr("Loading") );
		Load();
		ViewPicture();
		ViewInfo();
	}
	else
	{
		printf("osdimage: Error creating OSD!\n");
	}
}


eOSState cOSDImageViewer::ProcessKey(eKeys key)
{
	//tArea areas;

	eOSState state = cOsdObject::ProcessKey(key);
	if (state == osUnknown)
	{
		switch (key & ~k_Repeat)
		{
			case kUp:
				if (myPics->curPic + 1 < myPics->Size() )
				{
					myPics->curPic++;
				}
				else
				{
					myPics->curPic = 0;
				}

				slideShow = false;
				ViewInfo(tr("Loading") );
				Load();
				break;
			case kDown:
				if (myPics->curPic > 0)
				{
					myPics->curPic--;
				}
				else
				{
					myPics->curPic = myPics->Size() - 1;
				}

				slideShow = false;
				ViewInfo(tr("Loading") );
				Load();
				break;
			case kLeft:
				if (myPics->curPic > BIGJUMP - 1)
				{
					myPics->curPic -= BIGJUMP;
				}
				else
				{
					myPics->curPic = 0;
				}

				slideShow = false;
				ViewInfo(tr("Loading") );
				Load();
				break;
			case kRight:
				if (myPics->curPic + BIGJUMP < myPics->Size() )
				{
					myPics->curPic += BIGJUMP;;
				}
				else
				{
					myPics->curPic = myPics->Size() - 1;
				}

				slideShow = false;
				ViewInfo(tr("Loading") );
				Load();
				break;
			case kRed:
				slideShow = false;
				ViewInfo(tr("Resetting rotation") );
				image.Rotate0();
				break;
			case kGreen:
				slideShow = false;
				ViewInfo(tr("Rotating picture ccw") );
				image.RotateCCW();
				break;
			case kYellow:
				slideShow = false;
				ViewInfo(tr("Rotating picture cw") );
				image.RotateCW();
				break;
			case kBlue:
				slideShow = !slideShow;
				slideShowTime = time_ms();
				break;
			case kBack:
				slideShow = false;
				showMenu = true;
				return osEnd;
			case kOk:
				slideShow = false;
				break;
			case kNone:
				if (slideShow)
				{
					slideShowDelta = (time_ms() - slideShowTime) / 1000; // [ms]->[s]

					if (slideShowDelta >= OSDImageSetup.slideShowDelay)
					{
						if (myPics->curPic + 1 < myPics->Size() )
						{
							myPics->curPic++;

							ViewInfo(tr("Loading") );
							Load();

							slideShowTime = time_ms();
							break;
						}
						else
						{
							slideShow = false;

							ViewInfo();
							return state;
							break;
						}
					}
					else
					{
						return state;
						break;
					}
				}
				else
				{
					return state;
					break;
				}
			default:
				return state;
		}

		ViewBackground();
		ViewPicture();
		ViewInfo();
	}

	return state;
}


void cOSDImageViewer::Load(void)
{
#ifdef DEBUG
	int time = time_ms(), dt;
#endif


	// actually load the Image
	image.Load(myPics->CurPicPath() );

#ifdef DEBUG
	dt = time_ms() - time;
	printf("Viewing picture %d/%d: '%s'\n", myPics->curPic + 1, myPics->Size(), myPics->CurPicPath() );
	printf("\t- Loading: %dms\n", dt);
#endif
}


void cOSDImageViewer::ViewPicture(void)
{
	int wPic, hPic;
	int xOff, yOff, xText;

	
	cBitmap bmp(8, 8, 1);
	
	image.Render(bmp, wPicArea, hPicArea, quantizeColors, OSDImageSetup.quantizer, OSDImageSetup.dither, myColors, OSDImageSetup.picAlpha);
	wPic = image.Width();
	hPic = image.Height();
	xOff = PicLeft + (wPicArea - wPic) / 2;
	yOff = PicTop + (hPicArea - hPic) / 2;
	xText = OSDLeft;

	osd->DrawBitmap(xOff, yOff, bmp);
	osd->Flush();
}


void cOSDImageViewer::ViewBackground(void)
{
	char buf[256];

	// draw background
	osd->DrawRectangle(0, 0, OSDMAXWIDTH - 1, OSDMAXHEIGHT - 1, myColors.picBG);

	// draw top text-area and text
	osd->DrawRectangle(0, OSDTop, OSDMAXWIDTH - 1, OSDTop + FONTSIZE - 1, myColors.txtBG);

	snprintf(buf, sizeof(buf), " %s: '%s'", tr("Directory"), myPics->CurPath() );

	osd->DrawText(OSDLeft + SHADOWOFFSET, OSDTop, buf, myColors.txtShadow, clrTransparent, FONT, wTxtArea, FONTSIZE);
	osd->DrawText(OSDLeft, OSDTop, buf, myColors.txtFG, clrTransparent, FONT, wTxtArea, FONTSIZE);
}


void cOSDImageViewer::ViewInfo(const char *info)
{
	char buf[256];
	int x2Rec = OSDLeft + OSDWidth - 1;
	int y2Rec = OSDTop + OSDHeight - 1;
	int x1Rec = x2Rec - FONTSIZE * 2 + 1;
	int y1Rec = y2Rec - FONTSIZE + 1;
	int ds = (y2Rec - y1Rec) / 6;


	// draw bootom text-area and text
	osd->DrawRectangle(0, OSDTop + OSDHeight - FONTSIZE, OSDMAXWIDTH - 1, OSDTop + OSDHeight - 1, myColors.txtBG);

	snprintf(buf, sizeof(buf), " %d/%d: %s '%s'", myPics->curPic + 1, myPics->Size(), info, myPics->CurPicName() );

	osd->DrawText(OSDLeft + SHADOWOFFSET, OSDTop + OSDHeight - FONTSIZE, buf, myColors.txtShadow, clrTransparent, FONT, slideShow ? x1Rec - OSDLeft + 1 - SHADOWOFFSET: wTxtArea, FONTSIZE);
	osd->DrawText(OSDLeft, OSDTop + OSDHeight - FONTSIZE, buf, myColors.txtFG, clrTransparent, FONT, slideShow ? x1Rec - OSDLeft + 1 : wTxtArea, FONTSIZE);
	
	if (slideShow)
	{
		// draw play-icon with slideShowDelay to represent the slideshow-mode

		osd->DrawSlope(x1Rec + ds + SHADOWOFFSET, y1Rec + ds, x1Rec + 4 * ds, y2Rec - FONTSIZE / 2, myColors.txtShadow, 2);
		osd->DrawSlope(x1Rec + ds, y1Rec + ds, x1Rec + 4 * ds, y2Rec - FONTSIZE / 2, myColors.txtFG, 2);
		
		osd->DrawSlope(x1Rec + ds + SHADOWOFFSET, y1Rec + FONTSIZE / 2, x1Rec + 4 * ds, y2Rec - ds, myColors.txtShadow, 5);
		osd->DrawSlope(x1Rec + ds, y1Rec + FONTSIZE / 2, x1Rec + 4 * ds, y2Rec - ds, myColors.txtFG, 5);
		
		snprintf(buf, sizeof(buf), " %ds ", OSDImageSetup.slideShowDelay);

		osd->DrawText(x1Rec + SHADOWOFFSET, y1Rec, buf, myColors.txtShadow, clrTransparent, FONT, x2Rec - x1Rec - SHADOWOFFSET + 1, FONTSIZE, taRight);
		osd->DrawText(x1Rec, y1Rec, buf, myColors.txtFG, clrTransparent, FONT, x2Rec - x1Rec - SHADOWOFFSET + 1, FONTSIZE, taRight);
	}

	osd->Flush();
}


