/*
 * correctPesHeader.c: A corrector for broken pes headers (i.e. DVB-C recordings)
 *
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * Some parts are taken from vdr main sources.
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int read_next_packet_start(int fd, uint8_t *header_buffer)
{
  int r = read(fd,header_buffer,9);
  if (r < 0)
     return -1;
  if (r != 9)
     return 0;
  int skipped = 0;
  while (header_buffer[0] != 0 ||
         header_buffer[1] != 0 ||
         header_buffer[2] != 1 ||
         (header_buffer[3] < 0xC0 && header_buffer[3] != 0xBD) ||
         header_buffer[3] > 0xEF) {
        memmove(header_buffer, header_buffer + 1, 8);
        r = read(fd,header_buffer + 8,1);
        if (r < 0)
           return -1;
        if (r != 1)
           return 0;
        skipped++;
        }

  if (skipped)
     fprintf(stderr,"Skipped %d bytes while syncing on pes packet\n", skipped);

  return 1;
}

int is_header_to_fix(uint8_t *header_buffer)
{
  if (header_buffer[7] & 0x3F)
     return 1;

  return 0;
}

void fix_header(uint8_t *header_buffer)
{
  header_buffer[7] = header_buffer[7] & 0xC0;
  int pts_dts_flag = (header_buffer[7] >> 6);
  int skip_size = 0;
  if (pts_dts_flag == 1) {
     header_buffer[7] = 0;
     pts_dts_flag = 0;
     }

  if (pts_dts_flag & 2)
     skip_size += 5;

  if (pts_dts_flag & 1)
     skip_size += 5;

  int clear_size = header_buffer[9] - skip_size;
  if (clear_size < 0) {
     header_buffer[7] = 0;
     skip_size = 0;
     clear_size = header_buffer[9];
     }

  if (clear_size == 0)
     return;

  memset(header_buffer + 9 + skip_size, 0xFF, clear_size);
}

int read_header_rest(int fd, uint8_t *header_buffer)
{
  int r = read(fd, header_buffer + 9, header_buffer[8]);
  if (r < 0)
     return -1;
  if (r != header_buffer[8])
     return 0;
  return 1;
}

int write_header_back(int fd, uint8_t *header_buffer)
{
  int seek_back = header_buffer[8] + 2;
  if (lseek(fd, -seek_back, SEEK_CUR) < 0)
     return 0;

  int r = write(fd, header_buffer + 7, seek_back);
  return r - 2;
}

int seek_to_next(int fd, uint8_t *header_buffer, int skipped)
{
  skipped += 3;
  int seek_skip = ((header_buffer[4] << 8) | header_buffer[5]) - skipped;
  if (lseek(fd, seek_skip, SEEK_CUR) < 0)
     return 0;

  return 1;
}

void fix_file(const char *name)
{
  int fd = open(name, O_RDWR);
  if (fd < 0) {
     fprintf(stderr, "Cannot open file '%s'\n",name);
     return;
     }

  uint8_t header_buffer[255 + 9];
  int packet_count = 0;
  int packet_fix_count = 0;

  fprintf(stderr, "Start fixing file '%s'\n",name);
  int r;
  while ((r = read_next_packet_start(fd, header_buffer)) > 0) {
        int readed = 0;
        packet_count++;

        if (is_header_to_fix(header_buffer)) {
           if ((r = read_header_rest(fd, header_buffer)) <= 0)
              break;

           fix_header(header_buffer);
           readed = write_header_back(fd, header_buffer);
           if (readed < 0) {
              fprintf(stderr,"Cannot write to file '%s'\n", name);
              break;
              }
           packet_fix_count++;
           }

        if (!seek_to_next(fd, header_buffer, readed)) {
           fprintf(stderr,"Cannot seek in file '%s'\n", name);
           break;
           }
        }

  if (r < 0)
     fprintf(stderr,"Cannot read from file '%s'\n", name);

  fprintf(stderr, "Checked %d pes headers and %d fixed\n", packet_count, packet_fix_count);
}

void print_usage(void)
{
  fprintf(stderr,"correctPesHeader <rec-file> [<rec-file> ...]\n");
  fprintf(stderr,"Example: correctPesHeader 001.vdr 002.vdr\n");
}

int main (int argc, char **argv)
{
  if (argc < 2) {
     print_usage();
     return 1;
     }

  int i;
  for (i = 1; i < argc; i++) {
      fix_file(argv[i]);
      }

  return 0;
}
