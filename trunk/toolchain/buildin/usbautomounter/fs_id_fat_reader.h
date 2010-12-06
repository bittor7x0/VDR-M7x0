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
 * $Id: fs_id_fat_reader.h 133 2007-07-16 13:52:26Z andreas $
 *
 */
#ifndef __FS_ID_FAT_READER_H
#define __FS_ID_FAT_READER 1
#include <stdint.h>
#include "partitions_reader.h"

static const uint8_t fs_id_fat_types[] = { 0x01, 0x04, 0x06, 0x0b, 0x0c, 0x0e, 0x00};
int fs_id_fat_reader(struct partition *part, int probe);

#endif
