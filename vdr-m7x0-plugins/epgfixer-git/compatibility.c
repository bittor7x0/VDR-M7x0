#include "compatibility.h"

#if VDRVERSNUM < 10500

// --- cCharSetConv ----------------------------------------------------------

cCharSetConv::cCharSetConv(const char *FromCode, const char *ToCode)
{
  if (!FromCode)
     FromCode = "UTF-8";
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

const char *cCharSetConv::Convert(const char *From, char *To, size_t ToLength)
{
  if (cd != (iconv_t)-1 && From && *From) {
     char *FromPtr = (char *)From;
     size_t FromLength = strlen(From);
     char *ToPtr = To;
     if (!ToPtr) {
        int NewLength = std::max(length, FromLength * 2); // some reserve to avoid later reallocations
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

#endif // VDRVERSNUM < 10500

