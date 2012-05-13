/*
 * avards-services.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: avards.h,v 1.1 2007/11/22 09:01:05 amair Exp $
 */

#ifndef __AVARDS_SERVICES_H
#define __AVARDS_SERVICES_H

#if APIVERSNUM < 10504
#define AVARDS_MAXOSDSIZE_SERVICE_STRING_ID "avards-MaxOSDsize-v1.0"

struct avards_MaxOSDsize_v1_0 {
  int left, top, width, height;
};
#endif

#define AVARDS_CURRENT_WSSMODE_SERVICE_STRING_ID "avards-CurrentWSSMode-v1.0"

struct avards_CurrentWssMode_v1_0 {
  const char *ModeString;
};


#define AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID "avards-CurrentVideoFormat-v1.0"

struct avards_CurrentVideoFormat_v1_0 {
  const char *ModeString;
  int Mode;
  int Width;
  int Height;
};


#endif // __AVARDS_SERVICES_H
