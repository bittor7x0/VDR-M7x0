#ifndef __xmltv2vdr_compatibility_h__
#define __xmltv2vdr_compatibility_h__

#if VDRVERSNUM < 10500

#include <vdr/tools.h>
#include <stdlib.h>
#include <iconv.h>

#define uint unsigned int
#define uchar unsigned char

// When handling strings that might contain UTF-8 characters, it may be necessary
// to process a "symbol" that consists of several actual character bytes. The
// following functions allow transparently accessing a "char *" string without
// having to worry about what character set is actually used.

int Utf8CharLen(const char *s);
    ///< Returns the number of character bytes at the beginning of the given
    ///< string that form a UTF-8 symbol.
uint Utf8CharGet(const char *s, int Length = 0);
    ///< Returns the UTF-8 symbol at the beginning of the given string.
    ///< Length can be given from a previous call to Utf8CharLen() to avoid calculating
    ///< it again. If no Length is given, Utf8CharLen() will be called.
int Utf8CharSet(uint c, char *s = NULL);
    ///< Converts the given UTF-8 symbol to a sequence of character bytes and copies
    ///< them to the given string. Returns the number of bytes written. If no string
    ///< is given, only the number of bytes is returned and nothing is copied.
int Utf8SymChars(const char *s, int Symbols);
    ///< Returns the number of character bytes at the beginning of the given
    ///< string that form at most the given number of UTF-8 symbols.
int Utf8StrLen(const char *s);
    ///< Returns the number of UTF-8 symbols formed by the given string of
    ///< character bytes.
char *Utf8Strn0Cpy(char *Dest, const char *Src, int n);
    ///< Copies at most n character bytes from Src to Dest, making sure that the
    ///< resulting copy ends with a complete UTF-8 symbol. The copy is guaranteed
    ///< to be zero terminated.
    ///< Returns a pointer to Dest.
int Utf8ToArray(const char *s, uint *a, int Size);
    ///< Converts the given character bytes (including the terminating 0) into an
    ///< array of UTF-8 symbols of the given Size. Returns the number of symbols
    ///< in the array (without the terminating 0).
int Utf8FromArray(const uint *a, char *s, int Size, int Max = -1);
    ///< Converts the given array of UTF-8 symbols (including the terminating 0)
    ///< into a sequence of character bytes of at most Size length. Returns the
    ///< number of character bytes written (without the terminating 0).
    ///< If Max is given, only that many symbols will be converted.
    ///< The resulting string is always zero-terminated if Size is big enough.

// When allocating buffer space, make sure we reserve enough space to hold
// a string in UTF-8 representation:

#define Utf8BufSize(s) ((s) * 4)

// The following macros automatically use the correct versions of the character
// class functions:

#define Utf8to(conv, c) (cCharSetConv::SystemCharacterTable() ? to##conv(c) : tow##conv(c))
#define Utf8is(ccls, c) (cCharSetConv::SystemCharacterTable() ? is##ccls(c) : isw##ccls(c))

class cCharSetConv {
private:
  iconv_t cd;
  char *result;
  size_t length;
  static char *systemCharacterTable;
public:
  cCharSetConv(const char *FromCode = NULL, const char *ToCode = NULL);
     ///< Sets up a character set converter to convert from FromCode to ToCode.
     ///< If FromCode is NULL, the previously set systemCharacterTable is used
     ///< (or "UTF-8" if no systemCharacterTable has been set).
     ///< If ToCode is NULL, "UTF-8" is used.
  ~cCharSetConv();
  const char *Convert(const char *From, char *To = NULL, size_t ToLength = 0);
     ///< Converts the given Text from FromCode to ToCode (as set in the constructor).
     ///< If To is given, it is used to copy at most ToLength bytes of the result
     ///< (including the terminating 0) into that buffer. If To is not given,
     ///< the result is copied into a dynamically allocated buffer and is valid as
     ///< long as this object lives, or until the next call to Convert(). The
     ///< return value always points to the result if the conversion was successful
     ///< (even if a fixed size To buffer was given and the result didn't fit into
     ///< it). If the string could not be converted, the result points to the
     ///< original From string.
  static const char *SystemCharacterTable(void) { return systemCharacterTable; }
  static void SetSystemCharacterTable(const char *CharacterTable);
  };

inline int CompareStrings(const void *a, const void *b)
{
  return strcmp(*(const char **)a, *(const char **)b);
}

inline int CompareStringsIgnoreCase(const void *a, const void *b)
{
  return strcasecmp(*(const char **)a, *(const char **)b);
}

class cStringList : public cVector<char *> {
public:
  cStringList(int Allocated = 10): cVector<char *>(Allocated) {}
  virtual ~cStringList();
  int Find(const char *s) const;
  void Sort(bool IgnoreCase = false)
  {
    if (IgnoreCase)
       cVector<char *>::Sort(CompareStringsIgnoreCase);
    else
       cVector<char *>::Sort(CompareStrings);
  }
  virtual void Clear(void);
  };

#endif // VDRVERSNUM < 10500

#endif // __xmltv2vdr_compatibility_h__
