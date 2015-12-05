/*
 * charset.c: Character set list item
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <string.h>
#include "charset.h"

#if APIVERSNUM < 10500
class cCharSetConv {
private:
  static char *systemCharacterTable;
public:
  cCharSetConv(const char *FromCode = NULL, const char *ToCode = NULL);
     ///< Sets up a character set converter to convert from FromCode to ToCode.
     ///< If FromCode is NULL, the previously set systemCharacterTable is used.
     ///< If ToCode is NULL, "UTF-8" is used.
  ~cCharSetConv();
  const char *Convert(const char *From, char *To = NULL, size_t ToLength = 0);
     ///< Converts the given Text from FromCode to ToCode (as set in the cosntructor).
     ///< If To is given, it is used to copy at most ToLength bytes of the result
     ///< (including the terminating 0) into that buffer. If To is not given,
     ///< the result is copied into a dynamically allocated buffer and is valid as
     ///< long as this object lives, or until the next call to Convert(). The
     ///< return value always points to the result if the conversion was successful
     ///< (even if a fixed size To buffer was given and the result didn't fit into
     ///< it). If the string could not be converted, the result points to the
     ///< original From string.
  static const char *SystemCharacterTable(void) { return systemCharacterTable; }
  };
#endif // APIVERSNUM < 10500

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
     cCharSetConv backconv(cCharSetConv::SystemCharacterTable(), origcharset ? origcharset : "iso6937");
     cString title(backconv.Convert(Event->Title()));
     cString shortText(backconv.Convert(Event->ShortText()));
     cString description(backconv.Convert(Event->Description()));
     cCharSetConv conv(realcharset, cCharSetConv::SystemCharacterTable());
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
