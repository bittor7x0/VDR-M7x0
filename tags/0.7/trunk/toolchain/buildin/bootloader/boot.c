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

#include "typedefs.h"
#include "bootdefs.h"
#include "cache.h"
#include "simple_serial.h"
#include "LzmaDecode.h"
#include "kernel_args.h"


static void __attribute__(( noreturn )) die(const char *text) 
{
  serial_put_string(text);
  serial_put_string("\n\n PANIC \n\n");
  while (1) ;
}

static void decompress_kernel(void)
{
  u8 *kernel_compressed_addr = (*((u8 **)HEAD_KERNEL_COMPRESSED));
  u32 kernel_compressed_size = *((u32 *)HEAD_KERNEL_COMPRESSED_SIZE);
  u8 *kernel_load_addr = (*((u8 **)HEAD_KERNEL_LOAD_ADDR));
  
  serial_put_string("Decompressing Kernel at 0x");
  serial_put_num_hex((u32) kernel_compressed_addr,8);
  serial_put_string(" Size 0x");
  serial_put_num_hex(kernel_compressed_size,8);
  serial_put_string(" to 0x");
  serial_put_num_hex((u32) kernel_load_addr,8);
  serial_put_string(" ...\n");
  
  CLzmaDecoderState state;
  if (LzmaDecodeProperties(&state.Properties, kernel_compressed_addr,
         LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) 
    die("Cannot decode lzma properties\n");

  CProb probs[LzmaGetNumProbs(&state.Properties)];
  state.Probs = probs;
  
  u32 kernel_uncompressed_size = 
    kernel_compressed_addr[LZMA_PROPERTIES_SIZE] | (kernel_compressed_addr[LZMA_PROPERTIES_SIZE + 1] << 8) | 
    (kernel_compressed_addr[LZMA_PROPERTIES_SIZE + 2] << 16) | (kernel_compressed_addr[LZMA_PROPERTIES_SIZE + 3] << 24) ;
    
  if (kernel_uncompressed_size > MAX_KERNEL_SIZE) {
    kernel_uncompressed_size = MAX_KERNEL_SIZE;
    serial_put_string("Uncompressed kernel size too big ... shorten\n");
  }
  
  u32 in_done = 0;
  u32 out_done = 0;
  int r = LzmaDecode(&state, kernel_compressed_addr + LZMA_PROPERTIES_SIZE + 8,
      kernel_compressed_size  - LZMA_PROPERTIES_SIZE - 8, &in_done,
      kernel_load_addr, kernel_uncompressed_size, &out_done);
  
  if (r != LZMA_RESULT_OK)
    die("Cannot decompress kernel");
  serial_put_string("Decompress completed\n");
  serial_put_string("0x");
  serial_put_num_hex(out_done,8);
  serial_put_string(" Bytes decompressed.\n");
}


void __attribute__ ((noreturn)) boot (void) 
{
  decompress_kernel();
  
  u32 kernel_entry_addr = *((u32 *)HEAD_KERNEL_ENTRY);
  u32 sys_type = *((u32 *)HEAD_SYS_TYPE);
  void (*kernel_entry)(int, char **, char **, int *);
  kernel_entry = (void (*)(int, char **, char **, int *))kernel_entry_addr;

  char **args;
  switch (sys_type) {
      case SYS_TYPE_M740:
        args = kernel_args_m740;
        break;
      case SYS_TYPE_M750:
        args = kernel_args_m750;
        break;  
      default:
        serial_put_string("ERROR: Unknown System Type '");
        serial_put_num_dec(sys_type,1,0);
        serial_put_string("' using m740\n");
        args = kernel_args_m740;
        break;
  }
    
  serial_put_string("Flushing caches ..\n");
  
  /* Flush caches for safty */
  flush_dcache();
  flush_icache();
  
  serial_put_string("Calling Kernel at 0x");
  serial_put_num_hex(kernel_entry_addr,8);
  serial_put_string(" ...\n\n");
  
  kernel_entry(KERNEL_ARGS_COUNT,args,NULL,NULL);  
    
  /* Never reached */
  die("\n\nERROR !!!! Kernel returned from start\n\n");
}
