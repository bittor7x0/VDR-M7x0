/*
 * tools.h: Various tools
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __TOOLS_H
#define __TOOLS_H

#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <math.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <endian.h>
#include <byteswap.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif

typedef unsigned char uchar;
#define uint64 uint64_t // for compatibility - TODO remove in version 1.5

extern int SysLogLevel;

bool getIaMode();
void setIaMode(bool mode);

//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
#define BE2HOST(a) (a)
#define HOST2BE(a) (a)
#define LE2HOST(a) swapBytes(a)
#define HOST2LE(a) swapBytes(a)
#else
#define LE2HOST(a) (a)
#define HOST2LE(a) (a)
#define BE2HOST(a) swapBytes(a)
#define HOST2BE(a) swapBytes(a)
#endif
#define putIntUnalignedBE putIntUnaligned  // Big Endian
#define getIntUnalignedBE getIntUnaligned
#define putIntUnaligned(a,b) put_unaligned((int32_t) (b), (int32_t *)(a))
#define getIntUnaligned(a) get_unaligned((int32_t *) (a))

// Syslog reactivated
#define esyslog(a...) void( (SysLogLevel > 0) ? syslog_with_tid(LOG_ERR,   a) : void() )
#define isyslog(a...) void( (SysLogLevel > 1) ? syslog_with_tid(LOG_INFO,  a) : void() )
#define dsyslog(a...) void( (SysLogLevel > 2) ? syslog_with_tid(LOG_DEBUG, a) : void() )

// Errno should survive this
#define LOG_ERROR       {  int __errno_save = errno; \
                           char __errorstr[256]; \
                           strerror_r(__errno_save,__errorstr,256); \
                           __errorstr[255]=0; \
                           esyslog("ERROR (%s,%d): %s", __FILE__, __LINE__,__errorstr);  \
                           errno = __errno_save; }

#define LOG_ERROR_STR(s) {  int __errno_save = errno; \
                           char __errorstr[256]; \
                           strerror_r(__errno_save,__errorstr,256); \
                           __errorstr[255]=0; \
                           esyslog("ERROR: %s: %s", s,__errorstr); \
                           errno = __errno_save; }



#if 0
#define LOG_ERROR         { esyslog("ERROR (%s,%d): %m", __FILE__, __LINE__); }
#define LOG_ERROR_STR(s)  { esyslog("ERROR: %s: %m", s); }
#endif
#define SECSINDAY  86400

#define KILOBYTE(n) ((n) * 1024)
#define MEGABYTE(n) ((n) * 1024LL * 1024LL)

#define MALLOC(type, size)  (type *)malloc(sizeof(type) * (size))

template<class T> inline void DELETENULL(T *&p) { T *q = p; p = NULL; delete q; }

#define CHECK(s) { if ((s) < 0) LOG_ERROR; } // used for 'ioctl()' calls
#define FATALERRNO (errno && errno != EAGAIN && errno != EINTR && errno != EBUSY)
// Getting EBUSY in many cases on m7x0 where this should not break things.

template<class T> inline T swapBytes(const T a) {
  switch(sizeof(T)) {
    case 8: return bswap_64(a);
    case 4: return bswap_32(a);
    case 2: return bswap_16(a);
    default: return a;
    }
  }

#if __cplusplus >= 201103L
// any gcc >= 4.8.1; we have swap, min, max
#include <algorithm> // std::min, std::max, (c++98: also swap)
#include <utility> // std::swap (since c++11)
using std::min;
using std::max;
using std::swap;
#else
// no c++11 and old compiler, let's include our own templates
template<class T> inline T min(const T a, const T b) { return a <= b ? a : b; }
template<class T> inline T max(const T a, const T b) { return a >= b ? a : b; }
template<class T> inline void swap(T &a, T &b) { T t = a; a = b; b = t; }
#endif

template<class T> inline int sgn(const T a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }

void syslog_with_tid(int priority, const char *format, ...) __attribute__ ((format (printf, 2, 3)));

//M7X0 END AK

#define BCDCHARTOINT(x) (10 * ((x & 0xF0) >> 4) + (x & 0xF))
int BCD2INT(int x);

// Unfortunately there are no platform independent macros for unaligned
// access, so we do it this way:

template<class T> inline T get_unaligned(const T *const p)
{
  struct s { T v; } __attribute__((packed));
  return ((s *)p)->v;
}

template<class T> inline void put_unaligned(/*unsigned int*/ const T i, T *const p)
{
  struct s { T v; } __attribute__((packed));
  ((s *)p)->v = i;
}

// Comparing doubles for equality is unsafe, but unfortunately we can't
// overwrite operator==(double, double), so this will have to do:

inline bool DoubleEqual(double a, double b)
{
  return fabs(a - b) <= DBL_EPSILON;
}

class cString {
private:
  char *s;
public:
  cString(const char *S = NULL, bool TakePointer = false);
  cString(const char *S, const char *To); ///< Copies S up to To (exclusive). To must be a valid pointer into S. If To is NULL, everything is copied.
  cString(const cString &String);
  virtual ~cString();
  operator const char * () const { return s; } // for use in (const char *) context
  const char * operator*() const { return s; } // for use in (const void *) context (printf() etc.)
  cString &operator=(const cString &String);
  cString &CompactChars(char c); ///< Compact any sequence of characters 'c' to a single character, and strip all of them from the beginning and end of this string.
  static cString sprintf(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
  static cString sprintf(const char *fmt, va_list &ap);
  };

ssize_t safe_read(int filedes, void *buffer, size_t size);
ssize_t safe_write(int filedes, const void *buffer, size_t size);
int readchar(int filedes);
void writechar(int filedes, char c);
int WriteAllOrNothing(int fd, const uchar *Data, int Length, int TimeoutMs = 0, int RetryMs = 0);
    ///< Writes either all Data to the given file descriptor, or nothing at all.
    ///< If TimeoutMs is greater than 0, it will only retry for that long, otherwise
    ///< it will retry forever. RetryMs defines the time between two retries.
char *strcpyrealloc(char *dest, const char *src);
char *strn0cpy(char *dest, const char *src, size_t n);
char *strreplace(char *s, char c1, char c2);
char *strreplace(char *s, const char *s1, const char *s2); ///< re-allocates 's' and deletes the original string if necessary!
inline char *skipspace(const char *s)
{
  if ((uchar)*s > ' ') // most strings don't have any leading space, so handle this case as fast as possible
     return (char *)s;
  while (*s && (uchar)*s <= ' ') // avoiding isspace() here, because it is much slower
        s++;
  return (char *)s;
}
char *stripspace(char *s);
char *compactspace(char *s);
char *compactchars(char *s, char c); ///< removes all occurrences of 'c' from the beginning an end of 's' and replaces sequences of multiple 'c's with a single 'c'.
cString strescape(const char *s, const char *chars);
bool startswith(const char *s, const char *p);
bool endswith(const char *s, const char *p);
bool isempty(const char *s);
int numdigits(int n);
bool isnumber(const char *s);
cString itoa(int n);
cString AddDirectory(const char *DirName, const char *FileName);
int FreeDiskSpaceMB(const char *Directory, int *UsedMB = NULL);
bool DirectoryOk(const char *DirName, bool LogErrors = false);
bool MakeDirs(const char *FileName, bool IsDirectory = false);
bool RemoveFileOrDir(const char *FileName, bool FollowSymlinks = false);
bool RemoveEmptyDirectories(const char *DirName, bool RemoveThis = false);
int DirSizeMB(const char *DirName); ///< returns the total size of the files in the given directory, or -1 in case of an error
char *ReadLink(const char *FileName); ///< returns a new string allocated on the heap, which the caller must delete (or NULL in case of an error)
bool SpinUpDisk(const char *FileName);
void TouchFile(const char *FileName);
time_t LastModifiedTime(const char *FileName);
cString WeekDayName(int WeekDay);
cString WeekDayName(time_t t);
cString DayDateTime(time_t t = 0);
cString TimeToString(time_t t);
cString DateString(time_t t);
cString TimeString(time_t t);
#ifdef WITH_LIBJPEG
uchar *RgbToJpeg(uchar *Mem, int Width, int Height, int &Size, int Quality = 100);
    ///< Converts the given Memory to a JPEG image and returns a pointer
    ///< to the resulting image. Mem must point to a data block of exactly
    ///< (Width * Height) triplets of RGB image data bytes. Upon return, Size
    ///< will hold the number of bytes of the resulting JPEG data.
    ///< Quality can be in the range 0..100 and controls the quality of the
    ///< resulting image, where 100 is "best". The caller takes ownership of
    ///< the result and has to delete it once it is no longer needed.
    ///< The result may be NULL in case of an error.
#endif

class cBase64Encoder {
private:
  const uchar *data;
  int length;
  int maxResult;
  int i;
  char *result;
  static const char *b64;
public:
  cBase64Encoder(const uchar *Data, int Length, int MaxResult = 64);
      ///< Sets up a new base 64 encoder for the given Data, with the given Length.
      ///< Data will not be copied and must be valid as long as NextLine() will be
      ///< called. MaxResult defines the maximum number of characters in any
      ///< result line. The resulting lines may be shorter than MaxResult in case
      ///< its value is not a multiple of 4.
  ~cBase64Encoder();
  const char *NextLine(void);
      ///< Returns the next line of encoded data (terminated by '\0'), or NULL if
      ///< there is no more encoded data. The caller must call NextLine() and process
      ///< each returned line until NULL is returned, in order to get the entire
      ///< data encoded. The returned data is only valid until the next time NextLine()
      ///< is called, or until the object is destroyed.
  };

class cTimeMs {
private:
  uint64_t begin;
public:
  cTimeMs(void);
  static uint64_t Now(void);
  void Set(int Ms = 0);
      ///< Sets the timer. If Ms is 0, call Elapsed() to get the number of milliseconds
      ///< since the timer has been set. If Ms is greater than 0, TimedOut() returns
      ///< true as soon as Ms milliseconds have passed since calling Set(). If Ms is
      ///< negative, results are undefined.
      ///< Depending on the value of Ms, an object of cTimeMs can handle either
      ///< timeouts or elapsed times, not both at the same time.
  bool TimedOut(void) const;
  uint64_t Elapsed(void) const;
  };

class cReadLine {
private:
  size_t size;
  char *buffer;
public:
  cReadLine(void);
  ~cReadLine();
  char *Read(FILE *f);
  };

class cPoller {
private:
  enum { MaxPollFiles = 16 };
  pollfd pfd[MaxPollFiles];
  int numFileHandles;
public:
  cPoller(int FileHandle = -1, bool Out = false);
  bool Add(int FileHandle, bool Out);
  bool Poll(int TimeoutMs = 0);
  };

class cReadDir {
private:
  DIR *directory;
  struct dirent *result;
  union { // according to "The GNU C Library Reference Manual"
    struct dirent d;
    char b[offsetof(struct dirent, d_name) + NAME_MAX + 1];
    } u;
public:
  cReadDir(const char *Directory);
  ~cReadDir();
//M7X0 BEGIN AK
  void Close(void);
//M7X0 End AK
  bool Ok(void) { return directory != NULL; }
  struct dirent *Next(void);
  };

class cFile {
private:
  static bool files[];
  static int maxFiles;
  int f;
public:
  cFile(void);
  ~cFile();
  operator int () { return f; }
  bool Open(const char *FileName, int Flags, mode_t Mode = DEFFILEMODE);
  bool Open(int FileDes);
  void Close(void);
  bool IsOpen(void) { return f >= 0; }
  bool Ready(bool Wait = true);
  static bool AnyFileReady(int FileDes = -1, int TimeoutMs = 1000);
  static bool FileReady(int FileDes, int TimeoutMs = 1000);
  static bool FileReadyForWriting(int FileDes, int TimeoutMs = 1000);
  };

class cSafeFile {
private:
  FILE *f;
  char *fileName;
  char *tempName;
public:
  cSafeFile(const char *FileName);
  ~cSafeFile();
  operator FILE* () { return f; }
  bool Open(void);
  bool Close(bool ok = true);
  };

/// cUnbufferedFile is used for large files that are mainly written or read
/// in a streaming manner, and thus should not be cached.

class cUnbufferedFile {
private:
  int fd;
  off_t curpos;
#ifdef USE_FADVISE
  off_t cachedstart;
  off_t cachedend;
  off_t begin;
  off_t lastpos;
  off_t ahead;
  size_t readahead;
  size_t written;
  size_t totwritten;
  int FadviseDrop(off_t Offset, off_t Len);
#endif
//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
  bool directIOused;
  int blockSize;
  int FallBackFromDirectIO(void);
#endif
//M7X0 END AK
public:
  cUnbufferedFile(void);
  ~cUnbufferedFile();
  int Open(const char *FileName, int Flags, mode_t Mode = DEFFILEMODE);
  int Close(void);
#ifdef USE_FADVISE
  void SetReadAhead(size_t ra);
#endif
  off_t Seek(off_t Offset, int Whence);
  ssize_t Read(void *Data, size_t Size);
  ssize_t Write(const void *Data, size_t Size);
  int Truncate(off_t Length);
  static cUnbufferedFile *Create(const char *FileName, int Flags, mode_t Mode = DEFFILEMODE);
  };

class cLockFile {
private:
  char *fileName;
  int f;
public:
  cLockFile(const char *Directory);
  ~cLockFile();
  bool Lock(int WaitSeconds = 0);
  void Unlock(void);
  };

class cListObject {
private:
  cListObject *prev, *next;
  cListObject(const cListObject &ListObject) { abort(); } // no copy constructor!
  cListObject& operator= (const cListObject &ListObject) { abort(); return *this; } // no assignment operator!
public:
  cListObject(void);
  virtual ~cListObject();
  virtual int Compare(const cListObject &ListObject) const { return 0; }
      ///< Must return 0 if this object is equal to ListObject, a positive value
      ///< if it is "greater", and a negative value if it is "smaller".
  void Append(cListObject *Object);
  void Insert(cListObject *Object);
  void Unlink(void);
  int Index(void) const;
  cListObject *Prev(void) const { return prev; }
  cListObject *Next(void) const { return next; }
  };

class cListBase {
protected:
  cListObject *objects, *lastObject;
  cListBase(void);
  int count;
public:
  virtual ~cListBase();
  void Add(cListObject *Object, cListObject *After = NULL);
  void Ins(cListObject *Object, cListObject *Before = NULL);
  void Del(cListObject *Object, bool DeleteObject = true);
  virtual void Move(int From, int To);
  void Move(cListObject *From, cListObject *To);
  virtual void Clear(void);
  cListObject *Get(int Index) const;
  int Count(void) const { return count; }
  void Sort(void);
  };

template<class T> class cList : public cListBase {
public:
  T *Get(int Index) const { return (T *)cListBase::Get(Index); }
  T *First(void) const { return (T *)objects; }
  T *Last(void) const { return (T *)lastObject; }
  T *Prev(const T *object) const { return (T *)object->cListObject::Prev(); } // need to call cListObject's members to
  T *Next(const T *object) const { return (T *)object->cListObject::Next(); } // avoid ambiguities in case of a "list of lists"
  };

class cHashObject : public cListObject {
  friend class cHashBase;
private:
  unsigned int id;
  cListObject *object;
public:
  cHashObject(cListObject *Object, unsigned int Id) { object = Object; id = Id; }
  cListObject *Object(void) { return object; }
  };

class cHashBase {
private:
  cList<cHashObject> **hashTable;
  int size;
  unsigned int hashfn(unsigned int Id) const { return Id % size; }
protected:
  cHashBase(int Size);
public:
  virtual ~cHashBase();
  void Add(cListObject *Object, unsigned int Id);
  void Del(cListObject *Object, unsigned int Id);
  void Clear(void);
  cListObject *Get(unsigned int Id) const;
  cList<cHashObject> *GetList(unsigned int Id) const;
  };

#define HASHSIZE 512

template<class T> class cHash : public cHashBase {
public:
  cHash(int Size = HASHSIZE) : cHashBase(Size) {}
  T *Get(unsigned int Id) const { return (T *)cHashBase::Get(Id); }
};

template<class T> inline T constrain(T v, T l, T h) { return v < l ? l : v > h ? h : v; }

template<class T> class cVector {
  ///< cVector may only be used for *simple* types, like int or pointers - not for class objects that allocate additional memory!
private:
  mutable int allocated;
  mutable int size;
  mutable T *data;
  cVector(const cVector &Vector) {} // don't copy...
  cVector &operator=(const cVector &Vector) { return *this; } // ...or assign this!
  void Realloc(int Index) const
  {
    if (++Index > allocated) {
       data = (T *)realloc(data, Index * sizeof(T));
       if (!data) {
          esyslog("ERROR: out of memory - abort!");
          abort();
          }
       for (int i = allocated; i < Index; i++)
           data[i] = T(0);
       allocated = Index;
       }
  }
public:
  cVector(int Allocated = 10)
  {
    allocated = 0;
    size = 0;
    data = NULL;
    Realloc(Allocated);
  }
  virtual ~cVector() { free(data); }
  T& At(int Index) const
  {
    Realloc(Index);
    if (Index >= size)
       size = Index + 1;
    return data[Index];
  }
  const T& operator[](int Index) const
  {
    return At(Index);
  }
  T& operator[](int Index)
  {
    return At(Index);
  }
  int Size(void) const { return size; }
  virtual void Insert(T Data, int Before = 0)
  {
    if (Before < size) {
       Realloc(size);
       memmove(&data[Before + 1], &data[Before], (size - Before) * sizeof(T));
       size++;
       data[Before] = Data;
       }
    else
       Append(Data);
  }
  virtual void Append(T Data)
  {
    if (size >= allocated)
       Realloc(allocated * 3 / 2); // increase size by 50%
    data[size++] = Data;
  }
  virtual void Remove(int Index)
  {
    if (Index < size - 1)
       memmove(&data[Index], &data[Index + 1], (size - Index) * sizeof(T));
    size--;
  }
  virtual void Clear(void)
  {
    for (int i = 0; i < size; i++)
        data[i] = T(0);
    size = 0;
  }
  void Sort(__compar_fn_t Compare)
  {
    qsort(data, size, sizeof(T), Compare);
  }
  };

#endif //__TOOLS_H
