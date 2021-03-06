/*
 * sources.h: Source handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __SOURCES_H
#define __SOURCES_H

#include "config.h"

#ifdef M750S

class cSource : public cListObject {
public:
  enum eSourceType {
    stNone  = 0x0000,
    stCable = 0x4000,
    stSat   = 0x8000,
    stTerr  = 0xC000,
    st_Mask = 0xC000,
    st_Neg  = 0x0800,
    st_Pos  = 0x07FF,
    };
private:
  int code;
  char *description;
public:
  cSource(void);
  ~cSource();
  int Code(void) const { return code; }
  const char *Description(void) const { return description; }
  bool Parse(const char *s);
  static cString ToString(int Code);
  static int FromString(const char *s);
  static int FromData(eSourceType SourceType, int Position = 0, bool East = false);
  static bool IsCable(int Code) { return (Code & st_Mask) == stCable; }
  static bool IsSat(int Code) { return (Code & st_Mask) == stSat; }
  static bool IsTerr(int Code) { return (Code & st_Mask) == stTerr; }
  };

class cSources : public cConfig<cSource> {
public:
  cSource *Get(int Code);
  };

extern cSources Sources;

#else
class cSource {
public:
  enum eSourceType {
    stNone  = 0x0000,
    stTerr  = 0xC000,
    st_Mask = 0xC000,
    st_Neg  = 0x0800,
    st_Pos  = 0x07FF,
    };
private:
  int code;
  char *description;
public:
  cSource(void);
  ~cSource();
  int Code(void) const { return code; }
  const char *Description(void) const { return description; }
  static cString ToString(int Code) { return "T"; }
  static int FromString(const char *s) { return stTerr; }
  static int FromData(eSourceType SourceType, int Position = 0, bool East = false) { return stTerr; }
  };


#endif // M750S

#endif //__SOURCES_H
