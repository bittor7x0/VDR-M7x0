/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 11.04.05 - 02.02.07, horchi
 */

#ifndef VDR_PIN_LOCKS_H
#define VDR_PIN_LOCKS_H

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/tools.h>
#include <vdr/config.h>

#include "rep.h"
#include "def.h"

//***************************************************************************
// Lock Item
//***************************************************************************

class cLockItem : public cListObject
{     
   public:

      enum SearchMode
      {
         smUnknown = na,

         smRegexp,
         smInclude,
         smExact,
         smStartWith,

         smCount
      };
   
      cLockItem(int aActive = no);
      cLockItem(const char* aName, int aActive = yes, const char* aTitle = 0);
      virtual ~cLockItem();
      
      // interface

      virtual bool Parse(char* line);
      virtual bool Save(FILE* file);

      // tests

      virtual int Locked(long startTime = 0);
      bool MatchPattern(const char* aName);
      bool HasName(const char* aName);
      
      // gettings

      int GetActive()                  { return active; }
      const char* GetName()            { return name; }
      const char* GetTitle()           { return title; }
      long GetStart()                  { return start; }
      long GetEnd()                    { return end; }
      char* GetStart(char* ct)         { return DT::int2Hhmm(start, ct); }
      char* GetEnd(char* ct)           { return DT::int2Hhmm(end, ct); }
      int HasRange()                   { return hasRange; }
      const char* GetPattern()         { return pattern; }
      int SearchMode()                 { return searchMode; }

      // settings

      void SetActive(int aActive)      { active = aActive; }
      void SetName(const char* aName);
      void SetTitle(const char* aTitle);
      void SetStart(const char* ct)    { start = DT::hhmm2Int(ct); }
      void SetEnd(const char* ct)      { end = DT::hhmm2Int(ct); }
      void SetStart(long lt)           { start = lt; }
      void SetEnd(long lt)             { end = lt; }
      void SetRangeFlag(int flag)      { hasRange = flag; }
      void SetSearchMode(int aMode)    { searchMode = aMode; }
      void SetPattern(const char* aPattern);

      static const char* searchModes[smCount+1];

   protected:
   
      char* name;
      char* title;
      long start;         // in seconds
      long end;           // in seconds
      int active;
      int hasRange;
      char* pattern;
      int searchMode;
};

//***************************************************************************
// Locked Broadcast
//***************************************************************************

class cLockedBroadcast : public cLockItem
{     
   public:

      cLockedBroadcast();
      cLockedBroadcast(const char* aName);
      virtual ~cLockedBroadcast();

      // interface

      virtual bool Parse(char* line);
      virtual bool Save(FILE* file);

      // tests

      virtual int Locked(long startTime = 0);
};

//***************************************************************************
// Lock List Base
//***************************************************************************

template<class T> class cLockList : public cConfig<T>
{
   public:

      cLockList(int aType) : cConfig<T>::cConfig() { listType = aType; }

      virtual int Locked(const char* aName, long startTime = 0) = 0;
      virtual int GetListType() { return listType; }
      virtual void SetListType(int aType) { listType = aType; }

      virtual cLockItem* FindByName(const char* aName)
      {
         T* item;

         if (!aName)
            return 0;
   
         for (item = (T*)this->First(); item; item = (T*)this->Next(item))
         {
            if (item->HasName(aName))
               return item;
         }
   
         return 0;
      }

   protected:
      
      int listType;
};

//***************************************************************************
// Lock Item List
//***************************************************************************

class cLockItems : public cLockList<cLockItem>
{
   public:

      cLockItems(int aType = na) : cLockList<cLockItem>::cLockList(aType) {}

      virtual int Locked(const char* aName, long startTime = 0);
};

//***************************************************************************
// Lock Broadcasts List
//***************************************************************************

class cLockedBroadcasts : public cLockList<cLockedBroadcast>
{
   public:

      cLockedBroadcasts(int aType = na) : cLockList<cLockedBroadcast>::cLockList(aType) {}
   
      virtual int Locked(const char* aName, long startTime = 0);
};

//***************************************************************************
#endif // VDR_PIN_LOCKS_H

