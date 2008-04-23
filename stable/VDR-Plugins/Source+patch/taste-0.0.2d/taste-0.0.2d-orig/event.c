/*
 * $Id: event.c,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#include "event.h"

#include <ctype.h>

static char *duptolower(const char *s) {
	char *c = strdup(s);
	char *p = c;
	for (; *p; ++p)
		*p = tolower(*p);
	return c;
}

cEventsTaste EventsTaste;

cEventTaste::cEventTaste(void):
		mRegularExp(false),
		mIgnoreCase(false),
		mCompiled(false)
{
  strncpy(mPattern, tr("New Entry"), sizeof(mPattern));
}

cEventTaste::cEventTaste(const char *Pattern):
		mRegularExp(false),
		mIgnoreCase(false),
		mCompiled(false)
{
  strncpy(mPattern, Pattern, sizeof(mPattern));
}

cEventTaste::cEventTaste(const cEventTaste &Src)
{
	operator=(Src);
}

cEventTaste &cEventTaste::operator=(const cEventTaste &Src)
{
	printf("copy construct\n");
	strcpy(mPattern, Src.mPattern);
	mRegularExp = Src.mRegularExp;
	mIgnoreCase = Src.mIgnoreCase;
	mCompiled   = false;
	Compile();
	return *this;
}

cEventTaste::~cEventTaste()
{
	if (mRegularExp)
		regfree(&mExpression);
}

bool cEventTaste::Acceptable(const char *Event) const 
{
	if (mRegularExp)
		return regexec(&mExpression, Event, 0, NULL, 0) != 0;
	else if (mIgnoreCase) {
		char *ev = duptolower(Event);
		char *pa = duptolower(mPattern);
		printf("check for %s in %s\n", pa, ev);
		bool res = strstr(ev, pa) == NULL;
		free(ev); free(pa);
		return res;
	} else
		return strstr(Event, mPattern) == NULL;
}

bool cEventTaste::Parse(char *s) {
	char *patternbuf = NULL;
	int fields = sscanf(s, "%d:%d:%a[^\n]", &mRegularExp, &mIgnoreCase, &patternbuf);

	if (fields == 3) {
		strncpy(mPattern, skipspace(stripspace(patternbuf)), sizeof(mPattern));
		free(patternbuf);
	} else { // backward compatibility
		strncpy(mPattern, skipspace(stripspace(s)), sizeof(mPattern));
		mRegularExp = false;
		mIgnoreCase = false;
	}

  return Compile();
}

bool cEventTaste::Compile(void) {
	mCompiled = false;
	if (mRegularExp) {
		if (regcomp(&mExpression, mPattern, REG_EXTENDED | (mIgnoreCase ? REG_ICASE : 0)) != 0) {
			esyslog("ERROR: malformed regular expression: %s", mPattern);
			return false;
		} else
			mCompiled = true;
	}
	return true;
}

bool cEventTaste::Save(FILE *f) {
  return fprintf(f, "%d:%d:%s\n", mRegularExp, mIgnoreCase, mPattern) > 0;
}

bool cEventsTaste::Acceptable(const char *Event) {
  const cEventTaste *event = First();
  while (event != NULL) {
    if (!event->Acceptable(Event))
      return false;
    event = Next(event);
  }
  return true;
}

cEventsTaste &cEventsTaste::operator=(const cEventsTaste &Source) {
  cList<cEventTaste>::Clear();
  
  const cEventTaste *event = Source.First();
  while (event != NULL) {
    printf("transfering %p\n", event);
    Add(new cEventTaste(*event));
    event = Source.Next(event);
  }
  return *this;
}
