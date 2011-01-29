/*
 * Copyright (C) 2008 Andreas Koch - the open7x0.org group
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
 */
#ifndef _CACHE_H
#define _CACHE_H

#define DCACHE_SIZE (8*1024)
#define DCACHE_LSIZE (16)
#define ICACHE_SIZE (16*1024)
#define ICACHE_LSIZE (16)

#ifndef __ASSEMBLY__
/* Function pointers as flush function are directly excuted from flash,
 * which is in another segement.
 */
void _flush_icache(void);
void _flush_dcache(void);
extern void (*flush_icache)(void);
extern void (*flush_dcache)(void);
#endif
#endif
