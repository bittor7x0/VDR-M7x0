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

#ifndef ___I18N_H
#define ___I18N_H

#include <vdr/i18n.h>

extern const char *i18n_name;

#if APIVERSNUM < 10507
extern const tI18nPhrase Phrases[];
#endif

#undef tr
#define tr(s)  I18nTranslate(s, i18n_name)

#ifndef trNOOP
#define trNOOP(s) (s)
#endif
#ifndef trVDR
#define trVDR(s)  tr(s)
#endif

#endif //___I18N_H
