#ifndef __epgfixer_compatibility_h__
#define __epgfixer_compatibility_h__

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

#endif // VDRVERSNUM < 10500

#endif // __epgfixer_compatibility_h__
