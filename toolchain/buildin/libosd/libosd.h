/*
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * $Id: libosd.h 316 2008-02-19 19:36:28Z andreas $
 *
 */
#ifndef __LIBOSD_H
#define __LIBOSD_H 1

#include "font.h"
#include "osd.h"
#include "osd_progress_bar.h"
#include "i18n.h"

/*
 * Uses environment variables for init:
 * VDR_TV_MODE := FBAS/S-VIDEO (1 := SVIDEO else FBAS)
 * VDR_LANG :=  language index (same as OSDLanguage in setup.conf of vdr)
 */
int libosd_init(int clear);
void libosd_deinit(int tvoff,int clear);
#endif
