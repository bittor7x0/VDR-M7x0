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
#include "bootdefs.h"
#include "typedefs.h"
#include "cache.h"

/* Setup by linker see ld.script */
extern u32 _loader_start;
extern u32 _loader_end;
extern u32 _loader_mem_start;

extern u32 _bss_start;
extern u32 _bss_end;

extern u32 _ex_entry_start;
extern u32 _ex_entry_end;

void __attribute__ (( section(".init") )) init_loader(void) 
{
  u32 *load;
  u32 *mem;
  
  /* Copy loader to RAM */
  load = &_loader_start;
  mem = &_loader_mem_start;
  
  while (load < &_loader_end) {
    *mem++ = *load++;
  }
  
  /* Setup bss */
  mem = &_bss_start;
  while (mem < &_bss_end) {
    *mem++ = 0;
  }
  
  /* Setup exception handler 
   * for safty (just to print exceptions)
   */
  load = &_ex_entry_start;
  mem = (u32 *) KSEG0;

  while (load < &_ex_entry_end) {
    u32 w = load[0];
    mem[0] = w;
    mem[0x80 / 4] = w;
    mem[0x100 / 4] = w;
    mem[0x180 / 4] = w;
    mem++; load++;
  }
  
  /* Flush Caches */
  _flush_dcache();
  _flush_icache();
}


