/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dataplaylist.c 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#include <stdlib.h>
#include <unistd.h>
#include "dataplaylist.h"
#include "playlist.h"
#include "i18n.h"
#include "vdrtools.h"
#include <vdr/keys.h>
#include <vdr/tools.h>
#include <vdr/videodir.h>

// -- cPlaylistMark -----------------------------------------------------------------

cPlaylistMark::cPlaylistMark(char *Filename)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistMark::cPlaylistMark Name=%s", plugin_name, Filename);
#endif
  filename = Filename ? strdup(Filename) : NULL;
}

cPlaylistMark::~cPlaylistMark(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistMark::~cPlaylistMark Name=%s", plugin_name, filename);
#endif
  FREENULL(filename);
}

// -- cPlaylistMarkCol -----------------------------------------------------------------

cPlaylistMarkCol PlaylistMark;

// -- cPlaylistRecord -----------------------------------------------------------------

cPlaylistRecord::cPlaylistRecord(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::cPlaylistRecord", plugin_name);
#endif
  SetName(NULL);
  SetDefaults();
  playlist = NULL;
}

cPlaylistRecord::cPlaylistRecord(cPlaylist *Playlist, char *Filename)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::cPlaylistRecord Filename=%s", plugin_name, Filename);
#endif
  playlist = Playlist;
  SetName(Filename);
  SetDefaults();
}

cPlaylistRecord::cPlaylistRecord(cPlaylist *Playlist, cRecording *Recording, cPlaylistRecord *Parent)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::cPlaylistRecord Parent=%s", plugin_name, Parent ? Parent->name : "(null)");
#endif
  playlist = Playlist;
  SetName(Recording->FileName());
  parent = Parent;
  SetDefaults(Recording);
}


cPlaylistRecord::~cPlaylistRecord(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::~cPlaylistRecord [%s]", plugin_name, name);
#endif
  FREENULL(filename);
  FREENULL(summary);
  FREENULL(title);
  FREENULL(path);
  parent = NULL; // no delete!
}

void cPlaylistRecord::SetName(const char *Filename)
{
  if (Filename)
  {
    char *buffer = strdup(Filename + strlen(VideoDirectory) + 1);

//suche record ..

    buffer = ExchangeChars(buffer, false);

    char *temp = strrchr(buffer, '~');
    if (temp)
    {
      *temp = 0;
      temp = strrchr(buffer, '~');
    }
    if (temp)
      strn0cpy(name, temp + 1, MaxFileName);
    else
      strn0cpy(name, buffer, MaxFileName);
    path = strdup(buffer);
    free(buffer);
    filename = strdup(Filename);
    isdir = filename[strlen(filename) - 1] == '/';
  } else
  {
    name[0] = 0;
    filename = NULL;
    path = NULL;
    isdir = false;
  }
  parent = NULL;
}

void cPlaylistRecord::SetDefaults(cRecording *Recording)
{
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    options[i] = parent ? NoYesDefault_defaultRecordDir : NoYesDefault_defaultPlaylist; // Option_playoption is not use
  isdel = false;
  isnew = false;
  isedited = false;
  start = 0;
  title = NULL;
  summary = NULL;
  if (!isdir && filename)
    if (!CopyFromRecording(Recording))
      ERROR(tr("Error while accessing recording!"));
}

bool cPlaylistRecord::CopyFromRecording(cRecording *Recording)
{
  cRecording *recording = Recording ? Recording : Recordings.GetByName(filename);
  if (recording)
  {
    isedited = recording->IsEdited();
    isnew = recording->IsNew();
    start = recording->start;
    title = strdup(recording->Title('\t', true, recording->HierarchyLevels()));
    char *temp = strrchr(title, '\t');
    if (temp)
      *temp = 0;
    summary = recording->Info()->Title() ? strdup(recording->Info()->Title()) : NULL;
    return true;
  }
  isdel = true;
  return false;
}

void cPlaylistRecord::CopyOptions(cPlaylistRecord *PlaylistRecord)
{
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    options[i] = PlaylistRecord->options[i];
}

bool cPlaylistRecord::Parse(const char *s)
{
  static cPlaylistRecord *lastdir = NULL;
  char *namebuf = NULL;
  char *filenamebuf = NULL;
  int hasparent = false;
  int opt1, opt2, opt3;
  int fields = sscanf(s, "%a[^:]:%a[^:]:%d:%d:%d:%d:%d:%d:%d:%d:%d[:%d]", &namebuf,
                                                                          &filenamebuf,
                                                                          &hasparent,
                                                                          &isdel,
                                                                          &options[Option_confirmdeleterecord],
                                                                          &options[Option_confirmstartnewrecord],
                                                                          &options[Option_deletefromlist],
                                                                          &options[Option_deleterecord],
                                                                          &options[Option_jumpmark],
                                                                          &opt1, &opt2, &opt3);
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::Parse fields=%d line=%s*", plugin_name, fields, s);
#endif
  if (namebuf)
  {
    strn0cpy(name, namebuf, sizeof(name));
    free(namebuf);
  }
  FREENULL(filename);
  filename = (filenamebuf && !strcasecmp(filenamebuf, "@null@")) ? NULL : filenamebuf;
  strreplace(name, '|', ':');
  if (filename)
  {
    strreplace(filename, '|', ':');
    char *buffer = strdup(filename + strlen(VideoDirectory) + 1);
    buffer = ExchangeChars(buffer, false);
    char *temp = strrchr(buffer, '~');
    if (temp)
      *temp = 0;
    path = strdup(buffer);
    free(buffer);
    isdir = filename[strlen(filename) - 1] == '/';
    if (isdir)
      lastdir = this;
    else
    {
      if (lastdir && hasparent)
      {
        if (strstr(filename, lastdir->filename) == filename)
          parent = lastdir;
      }
      if (!CopyFromRecording())
        esyslog("%s: Error while accessing recording (deleted from playlist) %s", plugin_name, filename);
    }
  }
  if (fields == 12)
  {
    options[Option_playonlynew] = opt1;
    options[Option_searchnewrecord] = opt2;
    options[Option_searchrecordsub] = opt3;
  } else
  {
    options[Option_playonlynew] = isdir ? NoYesDefault_defaultPlaylist : NoYesDefault_defaultRecordDir;
    options[Option_searchnewrecord] = opt1;
    options[Option_searchrecordsub] = opt2;
  }
  return fields == 11 || fields == 12;
}

bool cPlaylistRecord::Save(FILE *f)
{
dsyslog("%s: cPlaylistRecord::Save", plugin_name);
  char *buffer = NULL;
  bool result = true;
  if (!isdel || parent)
  {
    strreplace(name, ':', '|');
    if (filename)
      strreplace(filename, ':', '|');
    asprintf(&buffer, "%s:%s:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\n",
                      name,
                      filename ? filename : "@null@",
                      parent ? true : false,
                      isdel,
                      options[Option_confirmdeleterecord],
                      options[Option_confirmstartnewrecord],
                      options[Option_deletefromlist],
                      options[Option_deleterecord],
                      options[Option_jumpmark],
                      options[Option_playonlynew],
                      options[Option_searchnewrecord],
                      options[Option_searchrecordsub]);
    
    strreplace(name, '|', ':');
    if (filename)
      strreplace(filename, '|', ':');
    result =  fputs(buffer, f) > 0;
    *(buffer + strlen(buffer) - 1) = 0;
    if (verbose.u)
      isyslog("%s: write line [%s]", plugin_name, buffer);
  }
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::Save buffer=%s result=%s", plugin_name, buffer, result ? "true" : "false");
#endif
  FREENULL(buffer);
  return result;
}

bool cPlaylistRecord::DeleteRecord(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecord::DeleteRecord Name=%s Filename=%s", plugin_name, name, filename);
#endif
  DeleteRecording(filename, false);
  if (playlist && playlist->RecordCol2())
  {
    isdel = true;
    playlist->RecordCol2()->SetChange();
  }
  return true;
}

int cPlaylistRecord::IndexRec(void)
{
  if (!playlist)
    return -1;
  int index = 0;
  for (cPlaylistRecord *plr = playlist->First_PLR(); plr; plr = playlist->Next_PLR(plr))
  {
    if (!plr->isdir && !plr->isdel)
    {
      index++;
      if (plr == this)
        return index;
    }
  }
  return -2;
}

bool cPlaylistRecord::Option(eOptions Select)
{
  if (Select < Option_confirmdeleterecord || Select >= Option_max)
    return false;
  switch (options[Select])
  {
    case NoYesDefault_no:               return false;
    case NoYesDefault_yes:              return true;
    case NoYesDefault_defaultRecordDir: if (parent)
                                          return parent->Option(Select);
                                        // no break
    case NoYesDefault_defaultPlaylist:  if (playlist)
                                          return playlist->Option(Select);
                                        // no break
    case NoYesDefault_defaultPlugin:    return PL_options[Select].u;
    default:                            return PL_options[Select].d;
  }
}

bool cPlaylistRecord::operator< (const cListObject &ListObject)
{
  return start == ((cPlaylistRecord *)&ListObject)->start ? strcmp(name, ((cPlaylistRecord *)&ListObject)->name) < 0 : start < ((cPlaylistRecord *)&ListObject)->start;
}

bool cPlaylistRecord::operator!= (const cPlaylistRecord &PlaylistRecordObject)
{
  bool temp;
  if (strlen(PlaylistRecordObject.name))
    if (strlen(name))
      temp = strcmp(name, PlaylistRecordObject.name);
    else
      temp = true;
  else
    temp = strlen(name);
  if (temp)
    return true;
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    if (options[i] != PlaylistRecordObject.options[i])
      return true;
  return false;
}

// -- cPlaylistRecordCol ----------------------------------------------------------------

cPlaylistRecordCol::cPlaylistRecordCol(cPlaylist *Parent)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecordCol::cPlaylistRecordCol", plugin_name);
#endif
  change = false;
  parent = Parent;
}

cPlaylistRecordCol::~cPlaylistRecordCol(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecordCol::~cPlaylistRecordCol", plugin_name);
#endif
  parent = NULL;
}

void cPlaylistRecordCol::DeleteFile()
{
  if (!access(FileName(), F_OK))
    if (remove(FileName()) < 0)
      LOG_ERROR_STR(FileName());
}

bool cPlaylistRecordCol::SaveWithFilename()
{
#ifdef PL_Debug1
  dsyslog("%s: cPlaylistRecordCol::SaveWithFilename playlist=%s", plugin_name, parent ? parent->Name() : "(null)");
#endif
  DeleteFile();
  if (!parent || !*parent->Name())
    return false;
  char *tempname = NULL;
  asprintf(&tempname, "%s%s%s", "playlists.", parent->Name(), ".conf");
  const char *filenew = PluginPlaylist->ExpandPath(tempname);
  free(tempname);

  bool result = true;
  cSafeFile f(filenew);
  if (f.Open())
  {
    for(cPlaylistRecord *plr = First(); plr; plr = Next(plr))
    {
      if (!plr->Save((FILE *)f))
      {
        result = false;
        break;
      }
    }
    if (!f.Close())
      result = false;
  } else
    result = false;
  if (result)
//  {
//    Load(filenew);
    change = false;
//  }
  return result;
}

bool cPlaylistRecordCol::SearchRecord(const char *Filename)
{
  bool result = false;
  for (cPlaylistRecord *plr = First(); plr; plr = Next(plr))
  {
    if (strcasecmp(plr->Filename(), Filename) == 0)
    {
      result = true;
      break;
    }
  }
#ifdef PL_Debug2
  dsyslog("%s: cPlaylistRecordCol::SearchRecord Filename=%s found=%s", plugin_name, Filename, result ? "true" : "false");
#endif
  return result;
}


// -- cPlaylistSortCol ----------------------------------------------------------------

cPlaylistSortCol PlaylistSort;

// -- cPlaylist ----------------------------------------------------------------

cPlaylist::cPlaylist(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::cPlaylist", plugin_name);
#endif
  name[0] = 0;
  lastrecord = NULL;
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    options[i] = NoYesDefault_defaultPlugin;
  options[Option_playoption1] = PlayOptions_defaultPlugin;
  options[Option_playoption2] = PlayOptions_defaultPlugin;
  recordcount = 0;
  recordnew = 0;
  recordcol = new cPlaylistRecordCol(this);
}

cPlaylist::~cPlaylist(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::~cPlaylist [%s]", plugin_name, name);
#endif
  FREENULL(lastrecord);
  if (recordcol)
    DELETENULL(recordcol);
}

bool cPlaylist::Parse(const char *s)
{
  char *namebuf = NULL;
  char *lastbuf = NULL;
  int opt1, opt2, opt3, opt4;
  int fields = sscanf(s, "%a[^:]:%a[^:]:%d:%d:%d:%d:%d:%d:%d:%d[:%d][:%d]", &namebuf,
                                                                            &lastbuf,
                                                                            &options[Option_confirmdeleterecord],
                                                                            &options[Option_confirmstartnewrecord],
                                                                            &options[Option_deletefromlist],
                                                                            &options[Option_deleterecord],
                                                                            &options[Option_jumpmark],
                                                                            &options[Option_playoption1],
                                                                            &opt1, &opt2, &opt3, &opt4);
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::Parse fields=%d line=%s*", plugin_name, fields, s);
#endif
  if (namebuf)
  {
    strn0cpy(name, namebuf, sizeof(name));
    free(namebuf);
  }
  FREENULL(lastrecord);
  lastrecord = (lastbuf && !strcasecmp(lastbuf, "@null@")) ? NULL : lastbuf;
  strreplace(name, '|', ':');
  if (lastrecord)
    strreplace(lastrecord, '|', ':');
  if (fields == 12)
  {
    options[Option_playoption2] = opt1;
    options[Option_playonlynew] = opt2;
    options[Option_searchnewrecord] = opt3;
    options[Option_searchrecordsub] = opt4;
  } else
  {
    options[Option_playoption2] = PlayOptions_defaultPlugin;
    options[Option_playonlynew] = NoYesDefault_defaultPlugin;
    options[Option_searchnewrecord] = opt1;
    options[Option_searchrecordsub] = opt2;
  }
  return fields == 10 || fields == 12;
}

bool cPlaylist::Save(FILE *f)
{
  char *buffer = NULL;
  strreplace(name, ':', '|');
  if (lastrecord)
    strreplace(lastrecord, ':', '|');
  asprintf(&buffer, "%s:%s:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\n",
                    name,
                    lastrecord ? lastrecord : "@null@",
                    options[Option_confirmdeleterecord],
                    options[Option_confirmstartnewrecord],
                    options[Option_deletefromlist],
                    options[Option_deleterecord],
                    options[Option_jumpmark],
                    options[Option_playoption1],
                    options[Option_playoption2],
                    options[Option_playonlynew],
                    options[Option_searchnewrecord],
                    options[Option_searchrecordsub]);
  strreplace(name, '|', ':');
  if (lastrecord)
    strreplace(lastrecord, '|', ':');
  bool result = fputs(buffer, f) > 0;
  if (verbose.u)
    isyslog("%s: write line [%s]", plugin_name, buffer);
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::Save buffer=%s result=%s", plugin_name, buffer, result ? "true" : "false");
#endif
  free(buffer);
  return result;
}

bool cPlaylist::SearchRecords(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::SearchRecords start", plugin_name);
#endif
  bool modifyplaylist = false;
  for (cPlaylistRecord *searchdir = recordcol->First(); searchdir; searchdir = recordcol->Next(searchdir))
  {
    if (searchdir->IsDir() && searchdir->Option(Option_searchnewrecord))
    {
      PlaylistSort.Clear();
      cPlaylistRecord *plr = NULL;
      for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording))
      {
        if (strstr(recording->FileName(), searchdir->Filename()) == recording->FileName())
        {
          const char *next = NULL;
          if (!searchdir->Option(Option_searchrecordsub))
          {
            const char *test = recording->FileName() + strlen(searchdir->Filename());
            const char *temp = strchr(test, '/');
            if (temp)
              next = strchr(temp + 1, '/');
          }
          if ((!next || searchdir->Option(Option_searchrecordsub)) && !recordcol->SearchRecord(recording->FileName()))
          {
            plr = new cPlaylistRecord(this, recording, searchdir);
            PlaylistSort.Add(plr);
          }
        }
      }
      if (PlaylistSort.Count())
      {
        PlaylistSort.Sort();
        cPlaylistRecord *temp = NULL;
        cPlaylistRecord *lastrecord = NULL;
        for (cPlaylistRecord *testrecord = recordcol->Next(searchdir); PlaylistSort.Count() && testrecord && testrecord->ParentObj() == searchdir; testrecord = recordcol->Next(testrecord))
        {
          lastrecord = testrecord;
          temp = PlaylistSort.First();
          if (testrecord->Start() > temp->Start())
          {
            PlaylistSort.Del(temp, false);
            recordcol->Ins(temp, lastrecord);
            modifyplaylist = true;
            testrecord = temp; // if more then one recording to insert
          }
        }
        for (plr = PlaylistSort.Last(); plr; plr = temp)
        {
          temp = PlaylistSort.Prev(plr);
          PlaylistSort.Del(plr, false);
          recordcol->Add(plr, lastrecord);
          modifyplaylist = true;
        }
      }
    }
  }
  CountRecords();
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::SearchRecords modified=%s count=%d new=%d", plugin_name, modifyplaylist ? "true" : "false", recordcount, recordnew);
#endif
  return modifyplaylist;
}

void cPlaylist::CountRecords(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPlaylist::CountRecords", plugin_name);
#endif
  recordcount = 0;
  recordnew = 0;
  for (cPlaylistRecord *plr = recordcol->First(); plr; plr = recordcol->Next(plr))
  {
    if (!plr->IsDirOrDel())
    {
      recordcount++;
      if (plr->IsNew())
        recordnew++;
    }
  }
}

cPlaylistRecord *cPlaylist::Firstnormal_PLR(void)
{
  if (recordcol)
    for (cPlaylistRecord *plr = recordcol->First(); plr; plr = recordcol->Next(plr))
      if (!plr->IsDirOrDel())
        return plr;
  return NULL;
}

cPlaylistRecord *cPlaylist::Firstnew_PLR(void)
{
  if (recordcol)
    for (cPlaylistRecord *plr = recordcol->First(); plr; plr = recordcol->Next(plr))
      if (!plr->IsDirOrDel())
        if (plr->IsNew())
          return plr;
  return NULL;
}

cPlaylistRecord *cPlaylist::Lastplayed_PLR(void)
{
  if (recordcol && lastrecord)
    for (cPlaylistRecord *plr = recordcol->First(); plr; plr = recordcol->Next(plr))
      if (!plr->IsDirOrDel())
        if (strcmp(lastrecord, plr->Filename()) == 0)
          return plr;
  FREENULL(lastrecord);
  return NULL;
}

bool cPlaylist::operator!= (const cPlaylist &PlaylistObject)
{
  bool temp;
  if (strlen(PlaylistObject.name))
    if (strlen(name))
      temp = strcmp(name, PlaylistObject.name);
    else
      return true;
  else
    temp = strlen(name);
  if (temp)
    return true;
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    if (options[i] != PlaylistObject.options[i])
      return true;
  return false;
}

cPlaylistRecord *SelectPLR = NULL;
cPlaylistRecord *PlayedPLR = NULL;

// -- cPlaylistCol ----------------------------------------------------------------

cPlaylistCol PlaylistCol;
