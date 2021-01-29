#ifndef __xmltv2vdr_compatibility_h__
#define __xmltv2vdr_compatibility_h__

#if VDRVERSNUM < 10500

#include <vdr/tools.h>
#include <stdlib.h>
#include <iconv.h>

class cCharSetConv {
private:
  iconv_t cd;
  char *result;
  size_t length;
public:
  cCharSetConv(const char *FromCode = NULL, const char *ToCode = NULL);
     ///< Sets up a character set converter to convert from FromCode to ToCode.
     ///< If FromCode is NULL, "UTF-8" is used.
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
