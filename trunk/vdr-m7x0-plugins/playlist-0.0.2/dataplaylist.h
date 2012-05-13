/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dataplaylist.h 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#ifndef __DATAPLAYLIST_H
#define __DATAPLAYLIST_H

#include <vdr/tools.h>
#include <vdr/interface.h>
#include "playlist.h"

class cPlaylistMark : public cListObject {
private:
  char *filename;
public:
  cPlaylistMark(char *Filename);
  virtual ~cPlaylistMark(void);
  char *Filename(void) { return filename; }
  };

class cPlaylistMarkCol : public cList<cPlaylistMark> {
private:
public:
  };

extern cPlaylistMarkCol PlaylistMark;

class cPlaylist;

class cPlaylistRecord : public cListObject {
  friend class cMenuRecordingEdit;
private:
  char   name[MaxFileName];
  char   *filename;
  char   *path;
  bool   isdir;
  int    isdel;
  bool   isnew;    // copy from cRecording
  time_t start;    // copy from cRecording
  char   *title;   // copy from cRecording
  bool   isedited; // copy from cRecording
  char   *summary; // copy from cRecording
  int    options[Option_max];
  cPlaylistRecord *parent;
  cPlaylist *playlist;
  void  SetName(const char *Filename);
  void  SetDefaults(cRecording *Recording = NULL);
  bool  CopyFromRecording(cRecording *Recording = NULL);
public:
  cPlaylistRecord(void);
  cPlaylistRecord(cPlaylist *Playlist, char *Filename);
  cPlaylistRecord(cPlaylist *Playlist, cRecording *Recording, cPlaylistRecord *Parent);
  virtual ~cPlaylistRecord(void);
  virtual bool operator< (const cListObject &ListObject);
  virtual bool operator!= (const cPlaylistRecord &PlaylistRecordObject);
  void CopyOptions(cPlaylistRecord *PlaylistRecord);
  bool Parse(const char *s);
  bool Save(FILE *f);
  bool DeleteRecord(void);
  int IndexRec(void);
  const char *Name(void)    { return &name[0]; }
  char *Filename(void)      { return filename; }
  bool IsNew(void)          { return isnew; }
  bool IsEdited(void)       { return isedited; }
  bool IsDir(void)          { return isdir; }
  bool IsDel(void)          { return isdel; }
  bool IsDirOrDel(void)     { return isdir || isdel; }
  time_t Start(void)        { return start; }
  const char *Title(void)   { return title; }
  bool Parent(void)         { return parent; }
  cPlaylistRecord *ParentObj(void) { return parent; }
  cPlaylist *Playlist(void) { return playlist; }
  void Playlist(cPlaylist *PlayList) { playlist = PlayList; }
  void SetDel(void)         { isdel = true; }
  void SetNew(bool Value)   { isnew = Value; }
  bool Option(eOptions Select);
  bool OptionInherited(eOptions Select) { return (Select < Option_confirmdeleterecord || Select >= Option_max) ? false : options[Select] > NoYesDefault_yes; }
  };

class cPlaylistRecordCol : public cConfig<cPlaylistRecord> {
private:
  bool change;
  cPlaylist *parent;
public:
  cPlaylistRecordCol(cPlaylist *Parent);
  virtual ~cPlaylistRecordCol(void);
  bool GetChange(void) { return change; }
  void SetChange(void) { change = true; }
  void DeleteFile();
  bool SaveWithFilename();
  bool SearchRecord(const char *Filename);
  };

class cPlaylistSortCol : public cList<cPlaylistRecord> {
private:
public:
  };

extern cPlaylistSortCol PlaylistSort;

class cPlaylist : public cListObject {
  friend class cMenuPlaylistEdit;
private:
  char   name[MaxFileName];
  char   *lastrecord;
  int    options[Option_max];
  int    recordcount;
  int    recordnew;

  cPlaylistRecordCol *recordcol;
public:
  cPlaylist(void);
  virtual ~cPlaylist(void);
  virtual bool operator!= (const cPlaylist &PlaylistObject);
  cPlaylistRecordCol *RecordCol2(void) { return recordcol; }
  bool Parse(const char *s);
  bool Save(FILE *f);
  bool SearchRecords(void);
  void CountRecords(void);
  cPlaylistRecord *First_PLR(void)                { return recordcol ? recordcol->First() : NULL; }
  cPlaylistRecord *Last_PLR(void)                 { return recordcol ? recordcol->Last() : NULL; }
  cPlaylistRecord *Next_PLR(cPlaylistRecord *PLR) { return recordcol ? recordcol->Next(PLR) : NULL; }
  cPlaylistRecord *Prev_PLR(cPlaylistRecord *PLR) { return recordcol ? recordcol->Prev(PLR) : NULL; }
  cPlaylistRecord *Firstnormal_PLR(void);
  cPlaylistRecord *Firstnew_PLR(void);
  cPlaylistRecord *Lastplayed_PLR(void);
  void SetChange(void)                            { if (recordcol) recordcol->SetChange(); }
  void SaveRecordCol(void)                        { if (recordcol && recordcol->GetChange() && (!storeplaylist.u || Interface->Confirm(tr("Question$Save Playlist?")))) recordcol->SaveWithFilename(); }
  void SetLastRecord(const char *LastRecord)      { FREENULL(lastrecord); lastrecord = strdup(LastRecord); }
  char *Name(void)                                { return &name[0]; }
  int RecordCount(bool Refesh = false)            { if (Refesh) CountRecords(); return recordcount; }
  int RecordNew(void)                             { return recordnew; }
  int Playoption(eOptions Select)                 { return Select == Option_playoption1 ? (options[Option_playoption1] == PlayOptions_defaultPlugin) ? PL_options[Option_playoption1].u : (options[Option_playoption1] < PlayOptions_defaultPlugin) ? options[Option_playoption1] : PL_options[Option_playoption1].d : Select == Option_playoption2 ? (options[Option_playoption2] == PlayOptions_defaultPlugin) ? PL_options[Option_playoption2].u : (options[Option_playoption2] < PlayOptions_defaultPlugin) ? options[Option_playoption2] : PL_options[Option_playoption2].d : PlayOptions_firstpos; }
  bool Option(eOptions Select)                    { return (Select < Option_confirmdeleterecord || Select >= Option_max) ? false : (options[Select] == NoYesDefault_defaultPlugin) ? PL_options[Select].u : (options[Select] < NoYesDefault_defaultPlugin) ? options[Select] : PL_options[Select].d; }
  bool OptionInherited(eOptions Select)           { return (Select < Option_confirmdeleterecord || Select >= Option_max) ? false : (Select == Option_playoption1 || Select == Option_playoption2) ? options[Select] == PlayOptions_defaultPlugin : options[Select] > NoYesDefault_yes; }
  };

//extern cPlaylist  *SelectPlaylist;
//extern int        Selectplayoption;
//extern int        Selectplayposition;
extern cPlaylistRecord *SelectPLR;
extern cPlaylistRecord *PlayedPLR;

class cPlaylistCol : public cConfig<cPlaylist> {
private:
public:
  };

extern cPlaylistCol PlaylistCol;

#endif //__DATAPLAYLIST_H
