/*
 * $Id: event.h,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#ifndef VDR_TASTE_EVENT_H
#define VDR_TASTE_EVENT_H

#include <sys/types.h>
#include <regex.h>
							
#include <vdr/tools.h>
#include <vdr/config.h>

#define EVLINELENGTH 256

class cEventTaste : public cListObject {
  friend class cMenuSetupEditTaste;

private:
  char mPattern[EVLINELENGTH];
	int  mRegularExp;
	int  mIgnoreCase;
	bool mCompiled;

	regex_t mExpression;

public:
  cEventTaste(void);
  cEventTaste(const char *Pattern);
	cEventTaste(const cEventTaste &Src);
	~cEventTaste();

	cEventTaste &operator=(const cEventTaste &Src);

  bool Acceptable(const char *Event) const ;

  bool Parse(char *s);
	bool Compile(void);
  bool Save(FILE *f);

  const char *Pattern(void) const { return mPattern; }
};

class cEventsTaste : public cConfig<cEventTaste> {
public:
  bool Acceptable(const char *Event);
  cEventsTaste &operator=(const cEventsTaste &Source);
};

extern cEventsTaste EventsTaste;

#endif // VDR_TASTE_EVENT_H
