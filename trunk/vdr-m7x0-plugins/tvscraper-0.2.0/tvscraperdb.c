#include <string>
#include <sstream>
#include <vector>
#include <sqlite3.h>
#include "tvscraperdb.h"

using namespace std;

cTVScraperDB::cTVScraperDB(void) {
    db = NULL;
    string memHD = "/dev/shm/";
    inMem = CheckDirExists(memHD.c_str());
    if (inMem) {
        stringstream sstrDbFileMem;
        sstrDbFileMem << memHD << "tvscraper.db";
        dbPathMem = sstrDbFileMem.str();
    }
    stringstream sstrDbFile;
    sstrDbFile << config.GetBaseDir() << "/tvscraper.db";
    dbPathPhys = sstrDbFile.str();
}

cTVScraperDB::~cTVScraperDB() {
    sqlite3_close(db);
}

vector<vector<string> > cTVScraperDB::Query(string query) {
    sqlite3_stmt *statement;
    vector<vector<string> > results;
    if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK) {
        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true) {
            result = sqlite3_step(statement);
            if(result == SQLITE_ROW) {
                vector<string> values;
                for(int col = 0; col < cols; col++) {
                    values.push_back((char*)sqlite3_column_text(statement, col));
                }
                results.push_back(values);
            } else {
                break;  
            }
        }
        sqlite3_finalize(statement);
    }
    string error = sqlite3_errmsg(db);
    if(error != "not an error") {
        esyslog("tvscraper: query failed: %s , error: %s", query.c_str(), error.c_str());
    }
    return results; 
}

vector<vector<string> > cTVScraperDB::QueryEscaped(string query, string where) {
    sqlite3_stmt *statement;
    vector<vector<string> > results;
    if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK) {
        sqlite3_bind_text(statement, 1, where.c_str(), -1, SQLITE_TRANSIENT);
        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true) {
            result = sqlite3_step(statement);
            if(result == SQLITE_ROW) {
                vector<string> values;
                for(int col = 0; col < cols; col++) {
                    values.push_back((char*)sqlite3_column_text(statement, col));
                }
                results.push_back(values);
            } else {
                break;  
            }
        }
        sqlite3_finalize(statement);
    }
    string error = sqlite3_errmsg(db);
    if(error != "not an error") {
        esyslog("tvscraper: query failed: %s , error: %s", query.c_str(), error.c_str());
    }
    return results; 
}

bool cTVScraperDB::Connect(void) {
    if (inMem) {
        if (sqlite3_open(dbPathMem.c_str(),&db)!=SQLITE_OK) {
            esyslog("tvscraper: failed to open or create %s", dbPathMem.c_str());
            return false;
        }
        esyslog("tvscraper: connecting to db %s", dbPathMem.c_str());
        int rc = LoadOrSaveDb(db, dbPathPhys.c_str(), false);
        if (rc != SQLITE_OK) {
            esyslog("tvscraper: error while loading data from %s, errorcode %d", dbPathPhys.c_str(), rc);
            return false;
        }
    } else {
        if (sqlite3_open(dbPathPhys.c_str(),&db)!=SQLITE_OK) {
            esyslog("tvscraper: failed to open or create %s", dbPathPhys.c_str());
            return false;
        }
        esyslog("tvscraper: connecting to db %s", dbPathPhys.c_str());
    }
    CreateTables();
    return true;
}

void cTVScraperDB::BackupToDisc(void) {
    if (inMem) {
        LoadOrSaveDb(db, dbPathPhys.c_str(), true);
    }
}

int cTVScraperDB::LoadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave) {
    int rc;                   /* Function return code */
    sqlite3 *pFile;           /* Database connection opened on zFilename */
    sqlite3_backup *pBackup;  /* Backup object used to copy data */
    sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
    sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

    rc = sqlite3_open(zFilename, &pFile);
    if( rc==SQLITE_OK ){
        pFrom = (isSave ? pInMemory : pFile);
        pTo   = (isSave ? pFile     : pInMemory);
        pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
        if( pBackup ){
            (void)sqlite3_backup_step(pBackup, -1);
            (void)sqlite3_backup_finish(pBackup);
        }
        rc = sqlite3_errcode(pTo);
    }

    (void)sqlite3_close(pFile);
    return rc;
}

bool cTVScraperDB::CreateTables(void) {
    stringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS series (";
    sql << "series_id integer primary key, ";
    sql << "series_name nvarchar(255), ";
    sql << "series_overview text";
    sql << ");";
    
    sql << "CREATE TABLE IF NOT EXISTS series_actors (";
    sql << "actor_series_id integer, ";
    sql << "actor_name nvarchar(255), ";
    sql << "actor_role nvarchar(255), ";
    sql << "actor_thumbnail nvarchar(255)";
    sql << ");";
    sql << "CREATE INDEX IF NOT EXISTS idx1 on series_actors (actor_series_id); ";

    sql << "CREATE TABLE IF NOT EXISTS movies (";
    sql << "movie_id integer primary key, ";
    sql << "movie_title nvarchar(255), ";
    sql << "movie_original_title nvarchar(255), ";
    sql << "movie_overview text";
    sql << ");";
    
    sql << "CREATE TABLE IF NOT EXISTS actors (";
    sql << "actor_id integer primary key, ";
    sql << "actor_name nvarchar(255)";
    sql << ");";
    
    sql << "CREATE TABLE IF NOT EXISTS actor_movie (";
    sql << "actor_id integer, ";
    sql << "movie_id integer, ";
    sql << "actor_role nvarchar(255)";
    sql << ");";
    sql << "CREATE UNIQUE INDEX IF NOT EXISTS idx2 on actor_movie (actor_id, movie_id); ";

    sql << "CREATE TABLE IF NOT EXISTS event_movie (";
    sql << "event_id integer, ";
    sql << "valid_till integer, ";
    sql << "movie_id integer ";
    sql << ");";
    sql << "CREATE UNIQUE INDEX IF NOT EXISTS idx3 on event_movie (event_id, movie_id); ";

    sql << "CREATE TABLE IF NOT EXISTS event_series (";
    sql << "event_id integer, ";
    sql << "valid_till integer, ";
    sql << "series_id integer ";
    sql << ");";
    sql << "CREATE UNIQUE INDEX IF NOT EXISTS idx4 on event_series (event_id, series_id); ";

    sql << "CREATE TABLE IF NOT EXISTS recordings (";
    sql << "rec_event_id integer, ";
    sql << "series_id integer, ";
    sql << "movie_id integer ";
    sql << ");";
    sql << "CREATE UNIQUE INDEX IF NOT EXISTS idx5 on recordings (rec_event_id, series_id, movie_id); ";
    
    sql << "CREATE TABLE IF NOT EXISTS scrap_history (";
    sql << "channel_id nvarchar(255), ";
    sql << "newest_scrapped integer ";
    sql << ");";
    
    sql << "CREATE TABLE IF NOT EXISTS scrap_checker (";
    sql << "last_scrapped integer ";
    sql << ");";
    
    char *errmsg;
    if (sqlite3_exec(db,sql.str().c_str(),NULL,NULL,&errmsg)!=SQLITE_OK) {
        esyslog("tvscraper: createdb: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return false;
    }
    return true;
}

void cTVScraperDB::ClearOutdated(string movieDir) {
    //first check which movie images can be deleted
    time_t now = time(0);
    stringstream sql;
    sql << "select movie_id from event_movie where valid_till < " << now;
    vector<vector<string> > result = Query(sql.str());
    int numOutdated = result.size(); 
    if (numOutdated > 0) {
        for (int i=0; i < numOutdated; i++) {
            bool keepMovie = false;
            vector<string> row = result[i];
            if (row.size() > 0) {
                int movieID = atoi(row[0].c_str());
                if (movieID > 0) {
                    //are there other still valid events pointing to that movie?
                    keepMovie = CheckMovieOutdatedEvents(movieID);
                    if (!keepMovie) {
                        //are there recordings pointing to that movie?
                        keepMovie = CheckMovieOutdatedRecordings(movieID);
                    }
                    if (!keepMovie) {
                        DeleteMovie(movieID, movieDir);
                    }
                }
            }
        }
    }
    //delete all invalid events pointing to movies
    stringstream sql2;
    sql2 << "delete from event_movie where valid_till < " << now;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql2.str().c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    //delete all invalid events pointing to series, series will all be kept for later use
    stringstream sql3;
    sql3 << "delete from event_series where valid_till < " << now;
    sqlite3_stmt *stmt2;
    sqlite3_prepare_v2(db, sql3.str().c_str(), -1, &stmt2, NULL);
    sqlite3_step(stmt2);
    esyslog("tvscraper: Cleanup Done");
}

void cTVScraperDB::DeleteMovie(int movieID, string movieDir) {
    //delete images
    stringstream backdrop;
    backdrop << movieDir << "/" << movieID << "_backdrop.jpg";
    stringstream poster;
    poster << movieDir << "/" << movieID << "_poster.jpg";
    DeleteFile(backdrop.str());
    DeleteFile(poster.str());
    //delete cast of this movie
    stringstream sql;
    sql << "DELETE FROM actor_movie WHERE movie_id = " << movieID;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
}

bool cTVScraperDB::CheckMovieOutdatedEvents(int movieID) {
    time_t now = time(0);
    stringstream sql;
    sql << "select event_id from event_movie where movie_id = " << movieID << " and valid_till > " << now;
    int eventID = 0;
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            eventID = atoi(row[0].c_str());
        }
    }
    if (eventID > 0)
        return true;
    return false;
}

bool cTVScraperDB::CheckMovieOutdatedRecordings(int movieID) {
    stringstream sql;
    sql << "select rec_event_id from recordings where movie_id = " << movieID;
    int eventID = 0;
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            eventID = atoi(row[0].c_str());
        }
    }
    if (eventID > 0)
        return true;
    return false;
}

void cTVScraperDB::InsertSeries(int seriesID, string name, string overview) {
    stringstream sql;
    sql << "INSERT INTO series (series_id, series_name, series_overview) ";
    sql << "VALUES (";
    sql << seriesID << ", ";
    sql << "?, ? ";
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, overview.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
}

void cTVScraperDB::InsertEventSeries(int eventID, time_t validTill, int seriesID) {
    stringstream sql;
    sql << "INSERT INTO event_series (event_id, valid_till, series_id) ";
    sql << "VALUES (";
    sql << eventID << ", " << validTill << ", " << seriesID;
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
}

void cTVScraperDB::InsertActor(int seriesID, string name, string role, string thumb) {
    stringstream sql;
    sql << "INSERT INTO series_actors (actor_series_id, actor_name, actor_role, actor_thumbnail) ";
    sql << "VALUES (";
    sql << seriesID << ", ";
    sql << "?, ? ,";
    sql << "'" << thumb << "'";
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
}

void cTVScraperDB::InsertMovie(int movieID, string title, string originalTitle, string overview) {
    stringstream sql;
    sql << "INSERT INTO movies (movie_id, movie_title, movie_original_title, movie_overview) ";
    sql << "VALUES (";
    sql << movieID << ", ";
    sql << "?, ?, ?";
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, originalTitle.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, overview.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
}

void cTVScraperDB::InsertEventMovie(int eventID, time_t validTill, int movieID) {
    stringstream sql;
    sql << "INSERT INTO event_movie (event_id, valid_till, movie_id) ";
    sql << "VALUES (";
    sql << eventID << ", " << validTill << ", " << movieID;
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
}


void cTVScraperDB::InsertMovieActor(int movieID, int actorID, string name, string role) {
    stringstream sql;
    sql << "INSERT INTO actors (actor_id, actor_name) ";
    sql << "VALUES (";
    sql << actorID << ", ?";
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);

    stringstream sql2;
    sql2 << "INSERT INTO actor_movie (actor_id, movie_id, actor_role) ";
    sql2 << "VALUES (";
    sql2 << actorID << ", " << movieID << ", ?";
    sql2 << ");";
    sqlite3_stmt *stmt2;
    sqlite3_prepare_v2(db, sql2.str().c_str(), -1, &stmt2, NULL);
    sqlite3_bind_text(stmt2, 1, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt2);
}

bool cTVScraperDB::MovieExists(int movieID) {
    stringstream sql;
    sql << "select count(movies.movie_id) as found from movies where movies.movie_id = " << movieID;
    vector<vector<string> > result = Query(sql.str());
    int found = 0;
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            found = atoi(row[0].c_str());
        }
    }
    if (found == 1)
        return true;
    return false;
}

bool cTVScraperDB::SeriesExists(int seriesID) {
    stringstream sql;
    sql << "select count(series.series_id) as found from series where series.series_id = " << seriesID;
    vector<vector<string> > result = Query(sql.str());
    int found = 0;
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            found = atoi(row[0].c_str());
        }
    }
    if (found == 1)
        return true;
    return false;
}

int cTVScraperDB::SearchMovie(string movieTitle) {
    string sql = "select movie_id from movies where movie_title=?";
    vector<vector<string> > result = QueryEscaped(sql, movieTitle);
    int movieID = 0;
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            movieID = atoi(row[0].c_str());
        }
    }
    return movieID;
}

int cTVScraperDB::SearchSeries(string seriesTitle) {
    string sql = "select series_id from series where series_name=?";
    vector<vector<string> > result = QueryEscaped(sql, seriesTitle);
    int seriesID = 0;
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            seriesID = atoi(row[0].c_str());
        }
    }
    return seriesID;
}

void cTVScraperDB::InsertRecording(int recEventID, int seriesID, int movieID) {
    stringstream sql;
    sql << "INSERT INTO recordings (rec_event_id, series_id, movie_id) ";
    sql << "VALUES (";
    sql << recEventID << ", " << seriesID << ", " << movieID;
    sql << ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
}

bool cTVScraperDB::SetRecordingSeries(int eventID) {
    stringstream sql;
    sql << "select series_id from event_series where event_id = " << eventID;
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            int seriesID = atoi(row[0].c_str());
            InsertRecording(eventID, seriesID, 0);
            return true;
        }
    }
    return false;
}

bool cTVScraperDB::SetRecordingMovie(int eventID) {
    stringstream sql;
    sql << "select movie_id from event_movie where event_id =" << eventID;
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            int movieID = atoi(row[0].c_str());
            InsertRecording(eventID, 0, movieID);
            return true;
        }
    }
    return false;
}

void cTVScraperDB::ClearRecordings(void) {
    stringstream sql;
    sql << "DELETE FROM recordings where 0 = 0";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
}

bool cTVScraperDB::CheckScrap(time_t timeStamp, string channelID) {
    bool doScrap = false;
    stringstream sql;
    sql << "select newest_scrapped from scrap_history where channel_id = '" << channelID.c_str() << "'";
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            char *ep;
            time_t newestScrapped = strtoul(row[0].c_str(), &ep, 10);
            if (newestScrapped < timeStamp) {
                doScrap = true;
                stringstream sql2;
                sql2 << "UPDATE scrap_history set newest_scrapped = " << timeStamp << " where channel_id='" << channelID.c_str() << "'";
                sqlite3_stmt *stmt;
                sqlite3_prepare_v2(db, sql2.str().c_str(), -1, &stmt, NULL);
                sqlite3_step(stmt);
            }
        }
    } else {
            doScrap = true;
            stringstream sql2;
            sql2 << "INSERT INTO scrap_history (channel_id, newest_scrapped) VALUES ('" << channelID.c_str() << "', " << timeStamp << ")";
            sqlite3_stmt *stmt;
            sqlite3_prepare_v2(db, sql2.str().c_str(), -1, &stmt, NULL);
            sqlite3_step(stmt);
    }
    return doScrap;
}

bool cTVScraperDB::CheckStartScrapping(int minimumDistance) {
    bool startScrapping = false;
    time_t now = time(0);
    stringstream sql;
    sql << "select last_scrapped from scrap_checker";
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            char *ep;
            time_t last_scrapped = strtoul(row[0].c_str(), &ep, 10);
            int difference = (int)(now - last_scrapped);
            if (difference > minimumDistance) {
                startScrapping = true;
                stringstream sql2;
                sql2 << "delete from scrap_checker";
                sqlite3_stmt *stmt;
                sqlite3_prepare_v2(db, sql2.str().c_str(), -1, &stmt, NULL);
                sqlite3_step(stmt);
                stringstream sql3;
                sql3 << "INSERT INTO scrap_checker (last_scrapped) VALUES (" << now << ")";
                sqlite3_stmt *stmt2;
                sqlite3_prepare_v2(db, sql3.str().c_str(), -1, &stmt2, NULL);
                sqlite3_step(stmt2);
            }
        }
    } else {
            startScrapping = true;
            stringstream sql2;
            sql2 << "INSERT INTO scrap_checker (last_scrapped) VALUES (" << now << ")";
            sqlite3_stmt *stmt;
            sqlite3_prepare_v2(db, sql2.str().c_str(), -1, &stmt, NULL);
            sqlite3_step(stmt);
    }
    return startScrapping;
}

int cTVScraperDB::GetSeriesID(int eventID, bool isRecording) {
    stringstream sql;
    if (!isRecording)
        sql << "select series_id from event_series where event_id = " << eventID;
    else
        sql << "select series_id from recordings where rec_event_id = " << eventID;
    vector<vector<string> > result = Query(sql.str());
    int seriesID = 0;
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            seriesID = atoi(row[0].c_str());       
        }
    }
    return seriesID;
}

int cTVScraperDB::GetMovieID(int eventID, bool isRecording) {
    stringstream sql;
    if (!isRecording)
        sql << "select movie_id from event_movie where event_id = " << eventID;
    else
        sql << "select movie_id from recordings where rec_event_id = " << eventID;
    vector<vector<string> > result = Query(sql.str());
    int movieID = 0;
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            movieID = atoi(row[0].c_str());       
        }
    }
    return movieID;
}

vector<vector<string> > cTVScraperDB::GetActorsSeries(int seriesID) {
    stringstream sql;
    sql << "select actor_name, actor_role, actor_thumbnail ";
    sql << "from series_actors ";
    sql << "where actor_series_id = " << seriesID;
    vector<vector<string> > result = Query(sql.str());
    return result;
}

vector<vector<string> > cTVScraperDB::GetActorsMovie(int movieID) {
    stringstream sql;
    sql << "select actors.actor_id, actor_name, actor_role ";
    sql << "from actors, actor_movie ";
    sql << "where actor_movie.actor_id = actors.actor_id ";
    sql << "and actor_movie.movie_id = " << movieID;
    vector<vector<string> > result = Query(sql.str());
    return result;
}

string cTVScraperDB::GetDescriptionSeries(int seriesID) {
    string description = "";
    stringstream sql;
    sql << "select series_overview from series ";
    sql << "where series_id = " << seriesID;
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            description= row[0];       
        }
    }
    return description;
}

string cTVScraperDB::GetDescriptionMovie(int movieID) {
    string description = "";
    stringstream sql;
    sql << "select movie_overview from movies ";
    sql << "where movie_id = " << movieID;
    vector<vector<string> > result = Query(sql.str());
    if (result.size() > 0) {
        vector<vector<string> >::iterator it = result.begin();
        vector<string> row = *it;
        if (row.size() > 0) {
            description= row[0];       
        }
    }
    return description;
}
