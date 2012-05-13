/*
 * Copyright (C) 2007,2008 Andreas Koch - the open7x0.org group
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "tools.h"

#define DUMMY_SIZE_OFFSET 0x74
#define DUMMY_LOADADDR_OFFSET 0x7c
#define BOOTLOADER_ENTRYPOINT_OFFSET 0x80
#define KERNEL_HEADER_SIZE 0x400
#define DUMMY_START 0x20c
#define DUMMY_LOAD_ADDR 0x80001000  

#define BOOTLOADER_ENRTY (0x9FC00000 + 64 * 1024 + 0x400)

#define HEAD_START 0x3EC
#define HEAD_SYS_TYPE 0x3EC
#define SYS_TYPE_M740 1
#define SYS_TYPE_M750 2
#define HEAD_KERNEL_LOAD_ADDR 0x3F0
#define HEAD_KERNEL_ENTRY  0x3F4
#define HEAD_KERNEL_COMPRESSED 0x3F8
#define HEAD_KERNEL_COMPRESSED_SIZE 0x3FC

uint8_t kernel_header_start[0x80] = {
   // Rest remains zero or should replaced any way
   'D',  'I',  'S',  'C',  'V',  'I',  'S',  'I',
   'O',  'N',  ' ',  ' ', 0x00, 0x00, 0x00, 0x00,

   'D',  'I',  'S',  'C',  'V',  'I',  'S',  'I',
   'O',  'N', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x81, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define SYSTEM_MAP_FORMAT_STR "%x %*c %255s\n"
#define KERNEL_LOADADDR_SYM "_ftext"
#define KERNEL_ENTRY_SYM "kernel_entry"

void usage(void)
{
  printf("gen_kernel_header <kernel lzmaed binary> <kernel system map> <dummy gz> <bootloader.bin> <outfile> -m740|-m750s\n");
  printf("\tgenerates image from kernel <kernel lzmaed binary>  \n"
         "\twith <kernel system map> and write it outfile\n");
}

void gen_image(uint32_t sys_type,
                uint32_t kernel_loadaddr,
                uint32_t kernel_entrypoint,
                const char *kernel_name,
                const char *dummy_name,
                const char *bootloader_name,
                const char *out_name)
{
  uint8_t kernel_header[KERNEL_HEADER_SIZE];
  FILE *f;
  FILE *out;
  
  struct stat st;
  if (stat(kernel_name,&st))
    die("Cannot get size of bziped kernel image %s", kernel_name);
  uint32_t kernel_size = (uint32_t) st.st_size;
  
  if (stat(dummy_name,&st))
    die("Cannot get size of dummy image %s", dummy_name);
  uint32_t dummy_size = (uint32_t) st.st_size;
  
  if (dummy_size > HEAD_START - DUMMY_START)
    die("Dummy too big");
  
  if (stat(bootloader_name,&st))
    die("Cannot get size of bootloader %s", bootloader_name);
  uint32_t bootloader_size = (uint32_t) st.st_size;

  memset(kernel_header, 0, KERNEL_HEADER_SIZE);
  memcpy(kernel_header, kernel_header_start, sizeof(kernel_header_start));

  f = fopen(dummy_name,"r");
  if (!f)
     die("Cannot open dummy image %s", dummy_name);
     
  if (fread(kernel_header + DUMMY_START, dummy_size, 1, f) != 1) 
     die("Cannot read dummy image %s", dummy_name);
     
  fclose(f);  
     
  uint32_t dummy_size_be = HOST2BE32(dummy_size);
  memcpy(kernel_header + DUMMY_SIZE_OFFSET, &dummy_size_be, 4);
  uint32_t tmp = HOST2BE32(DUMMY_LOAD_ADDR);
  memcpy(kernel_header + DUMMY_LOADADDR_OFFSET, &tmp, 4);
  tmp = HOST2BE32(BOOTLOADER_ENRTY);
  memcpy(kernel_header + BOOTLOADER_ENTRYPOINT_OFFSET, &tmp, 4);
  
  sys_type = HOST2BE32(sys_type);
  uint32_t kernel_start_be = HOST2BE32((BOOTLOADER_ENRTY + bootloader_size + 3) & ~3);
  uint32_t kernel_size_be = HOST2BE32(kernel_size);
  kernel_loadaddr = HOST2BE32(kernel_loadaddr);
  kernel_entrypoint = HOST2BE32(kernel_entrypoint);
  
  memcpy(kernel_header + HEAD_SYS_TYPE , &sys_type, 4);
  memcpy(kernel_header + HEAD_KERNEL_LOAD_ADDR , &kernel_loadaddr, 4);
  memcpy(kernel_header + HEAD_KERNEL_ENTRY, &kernel_entrypoint, 4);
  memcpy(kernel_header + HEAD_KERNEL_COMPRESSED, &kernel_start_be, 4);
  memcpy(kernel_header + HEAD_KERNEL_COMPRESSED_SIZE, &kernel_size_be, 4);
  
  out = fopen(out_name, "w");
  if (!out)
     die("Cannot output file %s", out_name);
     
  if (fwrite(kernel_header, KERNEL_HEADER_SIZE, 1,out) != 1)
    die("Cannot write header");

  uint8_t buf[4096];  
  f = fopen(bootloader_name,"r");
  if (!f)
     die("Cannot open bootloader image file %s", bootloader_name);
    
  while (bootloader_size > 0) {
    int r_size = 4096; 
    if (bootloader_size < 4096) {
      memset(buf,0,4096);
      r_size = bootloader_size;
    }
    
    if (fread(buf,r_size, 1, f) != 1)
      die("Cannot read bootloader image file %s", bootloader_name); 
      
    r_size = (r_size + 3) & ~3;
    if (fwrite(buf, r_size, 1,out) != 1)
       die("Cannot write bootloader");
    
    bootloader_size -= r_size;
  }   
  fclose(f);
  
  f = fopen(kernel_name,"r");
  if (!f)
     die("Cannot open kernel image file %s", kernel_name);
    
  while (kernel_size > 0) {
    int r_size = 4096; 
    if (kernel_size < 4096) {
      r_size = kernel_size;
    }
    
    if (fread(buf,r_size, 1, f) != 1)
      die("Cannot read kernel image file %s", kernel_name); 
      
    if (fwrite(buf, r_size, 1,out) != 1)
       die("Cannot write kernel");
    kernel_size -= r_size;
  }  
}

void parse_system_map(const char *map_filename, uint32_t *kernel_loadaddr,
                      uint32_t *kernel_entrypoint)
{
  FILE *map_file;
  char sym_name[256];
  uint32_t sym_addr;
  int loadaddr_found = 0;
  int entrypoint_found = 0;

  if (!(map_file = fopen(map_filename,"r")))
    die("Cannot open system map file %s", map_filename);

  while ((!loadaddr_found || !entrypoint_found)) {
    if (fscanf(map_file, SYSTEM_MAP_FORMAT_STR, &sym_addr, sym_name) != 2)
      die("Cannot parse system map");

    sym_name[255] = 0;
    if (!strcmp(sym_name, KERNEL_LOADADDR_SYM)) {
      loadaddr_found = 1;
      *kernel_loadaddr = sym_addr;
    } else if (!strcmp(sym_name, KERNEL_ENTRY_SYM)) {
      entrypoint_found = 1;
      *kernel_entrypoint = sym_addr;
    }
  }
  fclose(map_file);
}

int main(int argc, char **argv)
{
  uint32_t kernel_loadaddr;
  uint32_t kernel_entrypoint;
  uint32_t sys_type = 0;
  if (argc < 7) {
    usage();
    return 1;
  }
  
  if (strcmp(argv[6],"-m740") == 0) {
    sys_type = SYS_TYPE_M740;
  } else if (strcmp(argv[6],"-m750s") == 0) {
    sys_type = SYS_TYPE_M750;
  } else {
    usage();
    return 1;
  }
  parse_system_map(argv[2],&kernel_loadaddr, &kernel_entrypoint);
  gen_image(sys_type, kernel_loadaddr, kernel_entrypoint, argv[1], argv[3], argv[4], argv[5]);
  return 0;
}
