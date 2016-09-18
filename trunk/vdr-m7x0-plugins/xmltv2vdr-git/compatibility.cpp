#include "compatibility.h"

#if VDRVERSNUM < 10500

// --- UTF-8 support ---------------------------------------------------------

static uint SystemToUtf8[128] = { 0 };

int Utf8CharLen(const char *s)
{
  if (cCharSetConv::SystemCharacterTable())
     return 1;
#define MT(s, m, v) ((*(s) & (m)) == (v)) // Mask Test
  if (MT(s, 0xE0, 0xC0) && MT(s + 1, 0xC0, 0x80))
     return 2;
  if (MT(s, 0xF0, 0xE0) && MT(s + 1, 0xC0, 0x80) && MT(s + 2, 0xC0, 0x80))
     return 3;
  if (MT(s, 0xF8, 0xF0) && MT(s + 1, 0xC0, 0x80) && MT(s + 2, 0xC0, 0x80) && MT(s + 3, 0xC0, 0x80))
     return 4;
  return 1;
}

uint Utf8CharGet(const char *s, int Length)
{
  if (cCharSetConv::SystemCharacterTable())
     return (uchar)*s < 128 ? *s : SystemToUtf8[(uchar)*s - 128];
  if (!Length)
     Length = Utf8CharLen(s);
  switch (Length) {
    case 2: return ((*s & 0x1F) <<  6) |  (*(s + 1) & 0x3F);
    case 3: return ((*s & 0x0F) << 12) | ((*(s + 1) & 0x3F) <<  6) |  (*(s + 2) & 0x3F);
    case 4: return ((*s & 0x07) << 18) | ((*(s + 1) & 0x3F) << 12) | ((*(s + 2) & 0x3F) << 6) | (*(s + 3) & 0x3F);
    default: ;
    }
  return *s;
}

int Utf8CharSet(uint c, char *s)
{
  if (c < 0x80 || cCharSetConv::SystemCharacterTable()) {
     if (s)
        *s = c;
     return 1;
     }
  if (c < 0x800) {
     if (s) {
        *s++ = ((c >> 6) & 0x1F) | 0xC0;
        *s   = (c & 0x3F) | 0x80;
        }
     return 2;
     }
  if (c < 0x10000) {
     if (s) {
        *s++ = ((c >> 12) & 0x0F) | 0xE0;
        *s++ = ((c >>  6) & 0x3F) | 0x80;
        *s   = (c & 0x3F) | 0x80;
        }
     return 3;
     }
  if (c < 0x110000) {
     if (s) {
        *s++ = ((c >> 18) & 0x07) | 0xF0;
        *s++ = ((c >> 12) & 0x3F) | 0x80;
        *s++ = ((c >>  6) & 0x3F) | 0x80;
        *s   = (c & 0x3F) | 0x80;
        }
     return 4;
     }
  return 0; // can't convert to UTF-8
}

int Utf8SymChars(const char *s, int Symbols)
{
  if (cCharSetConv::SystemCharacterTable())
     return Symbols;
  int n = 0;
  while (*s && Symbols--) {
        int sl = Utf8CharLen(s);
        s += sl;
        n += sl;
        }
  return n;
}

int Utf8StrLen(const char *s)
{
  if (cCharSetConv::SystemCharacterTable())
     return strlen(s);
  int n = 0;
  while (*s) {
        s += Utf8CharLen(s);
        n++;
        }
  return n;
}

char *Utf8Strn0Cpy(char *Dest, const char *Src, int n)
{
  if (cCharSetConv::SystemCharacterTable())
     return strn0cpy(Dest, Src, n);
  char *d = Dest;
  while (*Src) {
        int sl = Utf8CharLen(Src);
        n -= sl;
        if (n > 0) {
           while (sl--)
                 *d++ = *Src++;
           }
        else
           break;
        }
  *d = 0;
  return Dest;
}

int Utf8ToArray(const char *s, uint *a, int Size)
{
  int n = 0;
  while (*s && --Size > 0) {
        if (cCharSetConv::SystemCharacterTable())
           *a++ = (uchar)(*s++);
        else {
           int sl = Utf8CharLen(s);
           *a++ = Utf8CharGet(s, sl);
           s += sl;
           }
        n++;
        }
  if (Size > 0)
     *a = 0;
  return n;
}

int Utf8FromArray(const uint *a, char *s, int Size, int Max)
{
  int NumChars = 0;
  int NumSyms = 0;
  while (*a && NumChars < Size) {
        if (Max >= 0 && NumSyms++ >= Max)
           break;
        if (cCharSetConv::SystemCharacterTable()) {
           *s++ = *a++;
           NumChars++;
           }
        else {
           int sl = Utf8CharSet(*a);
           if (NumChars + sl <= Size) {
              Utf8CharSet(*a, s);
              a++;
              s += sl;
              NumChars += sl;
              }
           else
              break;
           }
        }
  if (NumChars < Size)
     *s = 0;
  return NumChars;
}

// --- cCharSetConv ----------------------------------------------------------

char *cCharSetConv::systemCharacterTable = NULL;

cCharSetConv::cCharSetConv(const char *FromCode, const char *ToCode)
{
  if (!FromCode)
     FromCode = systemCharacterTable ? systemCharacterTable : "UTF-8";
  if (!ToCode)
     ToCode = "UTF-8";
  cd = iconv_open(ToCode, FromCode);
  result = NULL;
  length = 0;
}

cCharSetConv::~cCharSetConv()
{
  free(result);
  if (cd != (iconv_t)-1)
     iconv_close(cd);
}

void cCharSetConv::SetSystemCharacterTable(const char *CharacterTable)
{
  free(systemCharacterTable);
  systemCharacterTable = NULL;
  if (!strcasestr(CharacterTable, "UTF-8")) {
     // Set up a map for the character values 128...255:
     char buf[129];
     for (int i = 0; i < 128; i++)
         buf[i] = i + 128;
     buf[128] = 0;
     cCharSetConv csc(CharacterTable);
     const char *s = csc.Convert(buf);
     int i = 0;
     while (*s) {
           int sl = Utf8CharLen(s);
           SystemToUtf8[i] = Utf8CharGet(s, sl);
           s += sl;
           i++;
           }
     systemCharacterTable = strdup(CharacterTable);
     }
}

const char *cCharSetConv::Convert(const char *From, char *To, size_t ToLength)
{
  if (cd != (iconv_t)-1 && From && *From) {
     char *FromPtr = (char *)From;
     size_t FromLength = strlen(From);
     char *ToPtr = To;
     if (!ToPtr) {
        int NewLength = max(length, FromLength * 2); // some reserve to avoid later reallocations
        if (char *NewBuffer = (char *)realloc(result, NewLength)) {
           length = NewLength;
           result = NewBuffer;
           }
        else {
           esyslog("ERROR: out of memory");
           return From;
           }
        ToPtr = result;
        ToLength = length;
        }
     else if (!ToLength)
        return From; // can't convert into a zero sized buffer
     ToLength--; // save space for terminating 0
     char *Converted = ToPtr;
     while (FromLength > 0) {
           if (iconv(cd, &FromPtr, &FromLength, &ToPtr, &ToLength) == size_t(-1)) {
              if (errno == E2BIG || errno == EILSEQ && ToLength < 1) {
                 if (To)
                    break; // caller provided a fixed size buffer, but it was too small
                 // The result buffer is too small, so increase it:
                 size_t d = ToPtr - result;
                 size_t r = length / 2;
                 int NewLength = length + r;
                 if (char *NewBuffer = (char *)realloc(result, NewLength)) {
                    length = NewLength;
                    Converted = result = NewBuffer;
                    }
                 else {
                    esyslog("ERROR: out of memory");
                    return From;
                    }
                 ToLength += r;
                 ToPtr = result + d;
                 }
              if (errno == EILSEQ) {
                 // A character can't be converted, so mark it with '?' and proceed:
                 FromPtr++;
                 FromLength--;
                 *ToPtr++ = '?';
                 ToLength--;
                 }
              else if (errno != E2BIG)
                 return From; // unknown error, return original string
              }
           }
     *ToPtr = 0;
     return Converted;
     }
  return From;
}

// --- cStringList -----------------------------------------------------------

cStringList::~cStringList()
{
  Clear();
}

int cStringList::Find(const char *s) const
{
  for (int i = 0; i < Size(); i++) {
      if (!strcmp(s, At(i)))
         return i;
      }
  return -1;
}

void cStringList::Clear(void)
{
  for (int i = 0; i < Size(); i++)
      free(At(i));
  cVector<char *>::Clear();
}

#endif // VDRVERSNUM < 10500

