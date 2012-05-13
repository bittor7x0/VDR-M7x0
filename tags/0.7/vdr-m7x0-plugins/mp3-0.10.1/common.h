/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ___COMMON_H
#define ___COMMON_H

#ifndef PLUGIN_NAME_I18N
#define PLUGIN_NAME_I18N "dummy"
#endif

#ifndef APIVERSNUM
#include <vdr/config.h>
#endif
#include "config.h"

#if APIVERSNUM >= 10313
#define SLEEP(x) cCondWait::SleepMs(x)
#else
#define SLEEP(x) usleep((x)*1000)
#endif

#if APIVERSNUM >= 10318
#include <vdr/tools.h>
static cTimeMs __time;
#define time_ms() ((int)__time.Elapsed())
#endif

#if APIVERSNUM >= 10338
#define BUTTON "Button$"
#else
#define BUTTON
#endif

#if !defined(NO_DEBUG) && defined(DEBUG)
#define d(x) { (x); }
#else
#define d(x) ;
#endif

#ifndef likely
#define likely(x) __builtin_expect((!!(x)),1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((!!(x)),0)
#endif

#endif //___COMMON_H
