/*
 * Fritz!Box plugin for VDR
 *
 * Copyright (C) 2007 Joachim Wilke <vdr@joachim-wilke.de>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

#include <vdr/config.h>
#include <vdr/osd.h>
#include <vdr/status.h>
#include "notifyosd.h"
#include "fritztools.h"

bool cNotifyOsd::open = false;

cNotifyOsd::cNotifyOsd(cFritzListener *listener) {
	
	font = cFont::GetFont(fontOsd);
	this->listener = listener;
	open = true;
	
	sCallInfo *callInfo = listener->GetCallInfo();
	// 1: In- or Outgoing call? + [medium]
	std::string tmpLine = callInfo->isOutgoing ? tr("Outgoing call") : tr("Incoming call");
	tmpLine += " [" + callInfo->medium;
	if (callInfo->medium.find("ISDN") != std::string::npos)
		tmpLine += " " + callInfo->localNumber;
	tmpLine += "]";
	lines.push_back(tmpLine);
	// 2: remote party (only if information available)
	tmpLine = callInfo->remoteName;
	if (tmpLine.size() > 0)
		lines.push_back(tmpLine);
	dsyslog("notifyosd.c: showing OSD with call information, %i lines", lines.size());
	cStatus::MsgOsdStatusMessage(cFritzTools::ComposeCallMessage(callInfo).c_str());
}

cNotifyOsd::~cNotifyOsd() {
	dsyslog("notifyosd.c: closing OSD");
	cStatus::MsgOsdStatusMessage(NULL);
	open = false;
	delete osd;
}

void cNotifyOsd::Show(void) {
	// osd parameters
	/* whole screen: (OsdLeft, OsdTop) - (OsdLeft+OsdWidth, OsdTop+OsdHeight)
	 * ------------------------------------------------------
	 * |                                                    |
	 * |                                                    |
	 * |                                                    |      border:
	 * |     -(border)-------------------------------       |  =>  (zoomed in)  noOsd
	 * |     |        ^                             |       |      ----------------------------
	 * |     |        ^ (2)                         |       |      |            ^ clrBackground
	 * |     | <(3)> Incoming call   [VoIP 0] <(3)> |       |      |            ^ (1)
	 * |     |        ^                             |       |      |    ----------------------- ...
	 * |     |        ^ (2)                         |       |      |    |       ^ clrForeground
	 * |     |      +491234567890                   |       |      |    |       ^ (1)
	 * |     |        ^                             |       |      |    |    ------------------
	 * |     |        ^ (2)                         |       |      |    |    |    clrBackground
	 * |     ----------------------------------------       |      |    |    |   
	 * |                       ^                            |          ...
	 * |                       ^ (4)                        |
	 * ------------------------------------------------------
	 */
	const tColor clrBackground = clrGray50;
	const tColor clrForeground = clrWhite;
	const size_t colorDepth     = 4;
	const size_t borderWidth    = 2;  // (1)   
	const size_t lineBuffer     = 10; // (2)
	const size_t cellspacing    = 20; // (3)
	const size_t verticalOffset = 40; // (4)
	const size_t numLines = lines.size();
	// osd dimensions
	int left, top, width, height;
	// determine osd width
	width = 0;
	for (size_t i=0; i < numLines; i++)
		if (font->Width(lines[i].c_str()) + 2*cellspacing > width)
			width = font->Width(lines[i].c_str()) + 2*cellspacing;
	if (width > cOsd::OsdWidth())
		width = cOsd::OsdWidth(); 
	height = (font->Height() + lineBuffer) * numLines + 4*borderWidth + lineBuffer;
	// assert alignment
	while (width  % (8/colorDepth) > 0) width++;
	while (height % (8/colorDepth) > 0) height++;
	// center osd
	left = cOsd::OsdLeft() + (cOsd::OsdWidth() - width) / 2; 
	top  = cOsd::OsdTop()  + cOsd::OsdHeight() - height - verticalOffset;

	// defines upper left corner of osd
	osd = cOsdProvider::NewOsd(left, top);
	// defines drawing area and color depth
	tArea Area = { 0, 0, width-1, height-1, colorDepth };
	osd->SetAreas(&Area, 1);
	// draw osd frame, 2px thick
	osd->DrawRectangle(0,             0,             width-1,               height-1,               clrBackground);
	osd->DrawRectangle(borderWidth,   borderWidth,   width-borderWidth-1,   height-borderWidth-1,   clrForeground);
	osd->DrawRectangle(2*borderWidth, 2*borderWidth, width-2*borderWidth-1, height-2*borderWidth-1, clrBackground);
	// draw text lines
	for (size_t i=0; i < numLines; i++)
		osd->DrawText(cellspacing, 2*borderWidth + lineBuffer + i*(font->Height() + lineBuffer), 
				      lines[i].c_str(), clrForeground, clrBackground, font);
	osd->Flush();
}

eOSState cNotifyOsd::ProcessKey(eKeys Key) {
	eOSState state = cOsdObject::ProcessKey(Key);
	if (state == osUnknown)
		switch (Key) {
		case kBack:
			state = osBack;
			break;
		case kNone:
			if (!listener->GetCallInfo())
				state = osBack;
		default:
			break;
		}
	return state;	
}


