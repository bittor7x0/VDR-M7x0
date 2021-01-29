/*
 * charset.c: Character set list item
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <string.h>
#include "charset.h"
#include "compatibility.h"

/* Global instance */
cEpgfixerList<cCharSet, cEvent> EpgfixerCharSets;

cCharSet::cCharSet()
{
  origcharset = NULL;
  realcharset = NULL;
}

cCharSet::~cCharSet(void)
{
  free(origcharset);
  free(realcharset);
}

bool cCharSet::Apply(cEvent *Event)
{
  if (enabled && IsActive(Event->ChannelID())) {
#if APIVERSNUM >= 10503
     cCharSetConv backconv(cCharSetConv::SystemCharacterTable(), origcharset ? origcharset : "iso6937");
#else
     cCharSetConv backconv("ISO-8859-15", origcharset ? origcharset : "ISO-8859-1");
#endif
     cString title(backconv.Convert(Event->Title()));
     cString shortText(backconv.Convert(Event->ShortText()));
     cString description(backconv.Convert(Event->Description()));
#if APIVERSNUM >= 10503
     cCharSetConv conv(realcharset, cCharSetConv::SystemCharacterTable());
#else
     cCharSetConv conv(realcharset, "ISO-8859-15");
#endif
     Event->SetTitle(conv.Convert(title));
     Event->SetShortText(conv.Convert(shortText));
     Event->SetDescription(conv.Convert(description));
     }
  return false;
}

void cCharSet::SetFromString(char *s, bool Enabled)
{
  FREE(origcharset);
  FREE(realcharset);
  Free();
  cListItem::SetFromString(s, Enabled);
  if (enabled) {
     char *p = (s[0] == '!') ? s + 1 : s;
     char *r = strchr(p, ':');
     if (r) {
        *r = 0;
        numchannels = LoadChannelsFromString(p);
        p = r + 1;
        }
     r = strchr(p, '=');
     if (r) {
        *r = 0;
        origcharset = strdup(p);
        realcharset = strdup(r + 1);
        }
     else
        realcharset = strdup(p);
     }
}
