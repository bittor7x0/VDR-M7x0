/*
 * i18n.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __ENIGMA_I18N_H
#define __ENIGMA_I18N_H

#include "common.h"

#include <vdr/i18n.h>

#if VDRVERSNUM < 10507
extern const tI18nPhrase Phrases[];
#define trNOOP(s) (s)
#endif

#if APIVERSNUM < 10509
#define trVDR(s) tr(s)
#endif

#endif // __ENIGMA_I18N_H
// vim:et:sw=2:ts=2:
