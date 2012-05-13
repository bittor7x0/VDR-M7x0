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
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "i18n.h"

/* Adapted from vdr */
static const i18n_text_t i18n_basic_defs[] = {
  // The first defs must be the first two letters as they are uses for LC_* variables
  { "en",
    "de",
    "sl",
    "it",
    "nl",
    "pt",
    "fr",
    "no",
    "fi",
    "pl",
    "es",
    "el",
    "sv",
    "ro",
    "hu",
    "ca",
    "ru",
    "cr",
    "et",
    "da",
    "cs",
  },
  // The character set needed for this language (this MUST be the second phrase!):
  { "iso-8859-15",
    "iso-8859-15",
    "iso-8859-2",
    "iso-8859-15",
    "iso-8859-15",
    "iso-8859-1",
    "iso-8859-1",
    "iso-8859-1",
    "iso-8859-15",
    "iso-8859-2",
    "iso-8859-15",
    "iso-8859-7",
    "iso-8859-1",
    "iso-8859-2",
    "iso-8859-2",
    "iso-8859-1",
    "iso-8859-5",
    "iso-8859-2",
    "iso-8859-13",
    "iso-8859-15",
    "iso-8859-2",
  },
  { NULL }
};

struct i18n_text_list {
	const i18n_text_t *texts;
	struct i18n_text_list *prev;
	struct i18n_text_list *next;
};

static struct {
	int lang_index;
	struct i18n_text_list *first_text;
	struct i18n_text_list *last_text;
} i18n_ctx;

void i18n_init(int lang_index)
{
	char *loc;
	i18n_ctx.first_text = NULL;
	i18n_ctx.last_text = NULL;

	if (lang_index < 0 || lang_index >= I18N_LANG_COUNT) {
		char *vdr_lang;
		vdr_lang = getenv("VDR_LANG");
		if (vdr_lang) {
			char *p;
			lang_index = strtol(vdr_lang, &p, 10);
			if (vdr_lang == p)
				lang_index = -1;
		}
	}

	if (lang_index >= 0 && lang_index < I18N_LANG_COUNT) {
		i18n_ctx.lang_index = lang_index;
		return;
	}

	setlocale(LC_ALL,"");
	loc = setlocale(LC_MESSAGES, NULL);
	if (loc) {
		int i;
		for (i = 0; i < I18N_LANG_COUNT; i++)
			if (!strncasecmp(loc, i18n_basic_defs[0][i], 2)) {
				i18n_ctx.lang_index = i;
				return;
			}
	}
	i18n_ctx.lang_index = 0;
}

void i18n_deinit(void)
{
	while (i18n_ctx.first_text) {
		struct i18n_text_list *ent;
		ent = i18n_ctx.first_text;
		i18n_ctx.first_text = ent->next;
		free(ent);
	}
	i18n_ctx.last_text = NULL;
}

int i18n_register(const i18n_text_t *texts)
{
	struct i18n_text_list *ent;
	ent = malloc(sizeof(*ent));

	if (!ent)
		return -1;

	ent->texts = texts;
	ent->next = NULL;
	if (i18n_ctx.last_text) {
		i18n_ctx.last_text->next = ent;
		ent->prev = i18n_ctx.last_text;
		i18n_ctx.last_text = ent;
	} else {
		ent->prev = NULL;
		i18n_ctx.first_text = ent;
		i18n_ctx.last_text = ent;
	}
	return 0;
}

const char *i18n_encoding(void)
{
	return i18n_basic_defs[1][i18n_ctx.lang_index];
}

const char *i18n_translate(const char *str)
{
	struct i18n_text_list *ent;

	ent = i18n_ctx.first_text;
	while (ent) {
		const i18n_text_t *texts;
		texts = ent->texts;
		while (**texts) {
			if (!strcmp(**texts,str)) {
				const char *trans;
				trans = (*texts)[i18n_ctx.lang_index];
				if (trans && *trans)
					return trans;
			}
			texts++;
		}
		ent = ent->next;
	}
	return str;
}
