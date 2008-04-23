/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: i18n.h 96 2007-09-23 13:18:41Z tom $
 */

#ifndef VDR_SPIDER_I18N_H
#define VDR_SPIDER_I18N_H

#include "spider.h"
#include <vdr/i18n.h>
#include <vdr/config.h>


#if VDRVERSNUM < 10507

#define trNOOP(s) (s)

namespace SpiderPlugin
{

  extern const tI18nPhrase Phrases[];

} // namespace SpiderPlugin

#endif

#endif // VDR_SPIDER_I18N_H
