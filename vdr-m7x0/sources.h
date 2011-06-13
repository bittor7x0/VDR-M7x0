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

class cSource : public cListObject {
public:
  enum eSourceType {
    stNone  = 0x0000,
#ifdef M750S
    stCable = 0x4000,
    stSat   = 0x8000,
#endif
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
#ifdef M750S
  static bool IsCable(int Code) { return (Code & st_Mask) == stCable; }
  static bool IsSat(int Code) { return (Code & st_Mask) == stSat; }
#endif
  static bool IsTerr(int Code) { return (Code & st_Mask) == stTerr; }
  };

class cSources : public cConfig<cSource> {
public:
  cSource *Get(int Code);
  };

extern cSources Sources;

#endif //__SOURCES_H
