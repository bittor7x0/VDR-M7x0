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
#define KERNEL_ARGS_COUNT 4

char *kernel_args_m740[] = {
  "vmlinux",
  "root=/dev/mtdblock1",
  "console=ttyS0,115200N8",
  "mtdparts=phys:6m(root),5888k(app),256k(prod),1536k(boot),-(rescue)"
};

char *kernel_args_m750[] = {
  "vmlinux",
  "root=/dev/mtdblock1",
  "console=ttyS0,115200N8",
  "mtdparts=phys:8m(root),3840k(app),256k(prod),1536k(boot),-(rescue)"
};

