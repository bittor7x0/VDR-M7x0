/*
 * i18n.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */


#ifndef __SNAPSHOT_I18N_H
#define __SNAPSHOT_I18N_H

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/i18n.h>

#if APIVERSNUM < 10507
extern const tI18nPhrase Phrases[];
#define trNOOP(s) (s)
#endif

//***************************************************************************
#endif //__SNAPSHOT_I18N_H
