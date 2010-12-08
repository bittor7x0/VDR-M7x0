/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 11.04.05 - 02.02.07, horchi
 */

//***************************************************************************
// Includes
//***************************************************************************

#include "locks.h"
#include "def.h"

//***************************************************************************
// Statics
//***************************************************************************

const char* cLockItem::searchModes[] = 
{
   "regular expression",
   "includes",
   "exact",
   "start with",
   0
};

//***************************************************************************
// Object
//***************************************************************************

cLockItem::cLockItem(int aActive)
{
   name = 0;
   title = 0;
   active = aActive;
   hasRange = yes;
   start = 0;
   end = 0;
   pattern = 0;
   searchMode = smExact;
}

cLockItem::cLockItem(const char* aName, int aActive, const char* aTitle)
{
   asprintf(&name, "%s", aName);
   asprintf(&pattern, "%s", aName);
   active = aActive;
   hasRange = yes;
   title = 0;
   start = DT::hhmm2Int("0000");
   end = DT::hhmm2Int("2359");
   searchMode = smExact;

   if (aTitle)
      asprintf(&title, "%s", aTitle);
}

cLockItem::~cLockItem()
{
   if (name) free(name);
   if (title) free(title);
   if (pattern) free(pattern);
}

//***************************************************************************
// Name
//***************************************************************************

void cLockItem::SetName(const char* aName)
{ 
   if (!aName) return;

   if (name) free(name);
   asprintf(&name, "%s", aName);
}

//***************************************************************************
// Pattern
//***************************************************************************

void cLockItem::SetPattern(const char* aPattern)
{ 
   if (pattern) free(pattern);
   asprintf(&pattern, "%s", aPattern);
}

//***************************************************************************
// Title
//***************************************************************************

void cLockItem::SetTitle(const char* aTitle)
{ 
   if (!aTitle) return;

   if (title) free(title);
   asprintf(&title, "%s", aTitle);
}

//***************************************************************************
// Parse
//***************************************************************************

bool cLockItem::Parse(char* line) 
{
   int fields;

   char* aName = 0;
   char* aActive = 0;
   char* aStart = 0;
   char* aEnd = 0;
   char* aPattern = 0;
   char* aSearchMode = 0;

   fields = sscanf(line, "%a[^:]:%a[^:]:%a[^:]:%a[^:]:%a[^:]:%a[^\n]", 
                   &aName, &aActive, &aStart, &aEnd, 
                   &aPattern, &aSearchMode);

   if (fields >= 4)
   {
      if (name) free(name);
      name = aName;
      active = strcmp(aActive, "yes") == 0;
      start = DT::hhmm2Int(aStart);
      end = DT::hhmm2Int(aEnd);
   }

   if (fields == 6)
   {
      if (pattern) free(pattern);
      pattern = aPattern;

      for (int i = 0; i < smCount; i++)
      {
         if (strcmp(searchModes[i], aSearchMode) == 0)
         {
            searchMode = i;
            break;
         }
      }
   }
   else if (fields == 4)
   {
      SetPattern(name);
      searchMode = smExact;
   }
   
   if (aActive) free(aActive);
   if (aStart)  free(aStart);
   if (aEnd)    free(aEnd);
   if (aSearchMode) free(aSearchMode);

   return fields == 4 || fields == 6;
}

//***************************************************************************
// Save
//***************************************************************************

bool cLockItem::Save(FILE* file)
{
   char aStart[sizeHHMM+TB];
   char aEnd[sizeHHMM+TB];

   DT::int2Hhmm(start, aStart);
   DT::int2Hhmm(end, aEnd);

   // Format: "<name>:<active>:<start>:<end>:<pattern>:<searchMode>"

   return fprintf(file, "%s:%s:%s:%s:%s:%s\n", 
                  name, 
                  active ? "yes" : "no", 
                  aStart, aEnd, pattern,
                  searchModes[searchMode]) > 0;
}

//***************************************************************************
// Locked
//***************************************************************************

int cLockItem::Locked(long startTime)
{
   long sec;
   char ct[sizeHHMM+TB];
   long theTime;

   if (startTime)
      theTime = startTime;
   else
      theTime = DT::lNow();        // get now in seconds

   // get seconds from today 00:00 in actual timezone

   tell(eloDebug, "Checking protection at '%s'", DT::int2Hhmm(theTime, ct));

   sec = DT::hhmm2Int(DT::int2Hhmm(theTime, ct));

   if (sec >= start && sec <= end && active)
      return yes;
   
   return no;
}

//***************************************************************************
// Match Pattern
//***************************************************************************

bool cLockItem::MatchPattern(const char* aName)
{
	if (searchMode == smRegexp)
		return rep(aName, pattern) == success;
   else if (searchMode == smExact)
      return strcmp(pattern, aName) == 0;
   else if (searchMode == smInclude)
      return strstr(aName, pattern) > 0;
   else if (searchMode == smStartWith)
      return strstr(aName, pattern) == aName;

   return no;
}

//***************************************************************************
// Has Name
//***************************************************************************

bool cLockItem::HasName(const char* aName)
{
   return strcmp(name, aName) == 0;
}

//***************************************************************************
// Class cLockItems
//***************************************************************************
//***************************************************************************
// Locked
//***************************************************************************

int cLockItems::Locked(const char* aName, long startTime)
{
   cLockItem* item;

   if (!aName)
      return no;

   for (item = First(); item; item = Next(item))
   {
      if (item->MatchPattern(aName))
         if (item->Locked(startTime))
            return yes;
   }

   return no;
}
