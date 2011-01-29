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
 */

#ifndef __LIBOSD_I18N_H
#define __LIBOSD_I18N_H 1

/* Same number and same order as in vdr
 * on initialize checking for environment variable VDR_LANG
 * which value should be the language index.
 */
#define I18N_LANG_COUNT 21
#define tr(a) i18n_translate(a)

typedef const char *i18n_text_t[I18N_LANG_COUNT];

void i18n_init(int lang_index);
void i18n_deinit(void);
int i18n_register(const i18n_text_t *texts);
const char *i18n_encoding(void);
const char *i18n_translate(const char *str);

#endif
