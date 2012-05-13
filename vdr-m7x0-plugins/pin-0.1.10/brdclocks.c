/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 25.01.06 - 02.02.07, horchi
 */

//***************************************************************************
// Includes
//***************************************************************************

#include "locks.h"
#include "def.h"

//***************************************************************************
// Object
//***************************************************************************

cLockedBroadcast::cLockedBroadcast()
   : cLockItem(yes)
{
   hasRange = no;
}

cLockedBroadcast::cLockedBroadcast(const char* aName)
   : cLockItem(aName, yes)
{
   hasRange = no;
}

cLockedBroadcast::~cLockedBroadcast()
{
   // nothicng yet ...
}

//***************************************************************************
// Parse
//***************************************************************************

bool cLockedBroadcast::Parse(char* line) 
{
   int fields;

   char* aName = 0;
   char* aPattern = 0;
   char* aSearchMode = 0;
   char* aActive = 0;

   fields = sscanf(line, "%a[^:]:%a[^:]:%a[^:]:%a[^\n]", 
                   &aName, &aPattern, &aSearchMode, &aActive);

   if (fields == 4)
   {
      if (name) free(name);
      name = aName;

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

      active = strcmp(aActive, "yes") == 0;
   }
   
   if (aSearchMode) free(aSearchMode);
   if (aActive) free(aActive);

   return fields == 4;
}

//***************************************************************************
// Save
//***************************************************************************

bool cLockedBroadcast::Save(FILE* file)
{
   // Format: "<name>:<pattern>:<searchMode>:<active>"

   return fprintf(file, "%s:%s:%s:%s\n", 
                  name,
                  pattern,
                  searchModes[searchMode],
                  active ? "yes" : "no") > 0;
}

//***************************************************************************
// Locked
//***************************************************************************

int cLockedBroadcast::Locked(long /*startTime*/)
{
   return active;
}

//***************************************************************************
// Class cLockedBroadcasts
//***************************************************************************
//***************************************************************************
// Locked
//***************************************************************************

int cLockedBroadcasts::Locked(const char* aName, long /*startTime*/)
{
   cLockedBroadcast* broadcast;

   if (!aName)
      return no;

   for (broadcast = First(); broadcast; broadcast = Next(broadcast))
   {
      if (broadcast->MatchPattern(aName))
         if (broadcast->Locked())
            return yes;
   }
   
   return no;
}
