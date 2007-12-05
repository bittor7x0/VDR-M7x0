#ifndef _AFUZZY_H
#define _AFUZZY_H

#include <stdio.h>
// source from:
/*
  Leonid Boitsov 2002. (itman@narod.ru)
  C version of Stas Namin.
  This code is a GPL software and is distributed under GNU
  public licence without any warranty.
*/

typedef unsigned int Uint;

#define MaxPatSize (sizeof(Uint) * 8)

typedef struct
{
	Uint 		*R,
				*R1,
				*RP,
				*S,
				*RI;
	Uint 		*FilterS;

	int 		Map[256];
	int 		FilterMap[256];
	int			k;
	Uint		mask_ok;
	Uint		filter_ok;
	Uint		filter_shift;
	int			r_size;
	int			FilterSet;
} AFUZZY;

void afuzzy_init(const char *p, int kerr, int UseFilter, AFUZZY *fuzzy);
void afuzzy_free(AFUZZY *fuzzy);
int afuzzy_checkSUB(const char *t, AFUZZY *fuzzy);

#endif


