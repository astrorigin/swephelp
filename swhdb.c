/*
    Swephelp

    Copyright 2007-2020 Stanislas Marquis <stan@astrorigin.com>

    Swephelp is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    Swephelp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Swephelp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#define access _access
#define F_OK 00
#define R_OK 04
#define W_OK 02
#endif

#include <sqlite3.h>

#include "swhdb.h"

#ifdef _MSC_VER
#define TLS __declspec(thread)
#else
#define TLS __thread
#endif

static TLS sqlite3* _swh_db_cnx = NULL;

const char* _swh_db_creates_sql[] = {
"PRAGMA auto_vacuum = 1;",
"PRAGMA encoding = 'UTF-8';",
"PRAGMA foreign_keys = 1;",
"CREATE TABLE Meta"
"("
" version integer not null"
");",
"INSERT INTO Meta (version) values ("SWH_DB_VERSION_STR");",
"CREATE TABLE Users"
"("
" _idx integer primary key,"
" name varchar not null unique check (length(name) between 1 and 128),"
" pswd varchar not null default '',"
" mail varchar not null default '',"
" info varchar not null default ''"
");",
"INSERT INTO Users (_idx, name) values (1, 'root');",
"CREATE TABLE Data"
"("
" _idx integer primary key,"
" _uidx integer not null default 1,"
" title varchar not null check (length(title) between 1 and 128),"
" jd float not null,"
" latitude float not null default 0 check (latitude between -90 and 90),"
" longitude float not null default 0 check (longitude between -180 and 180),"
" altitude integer not null default 0 check (altitude between -10000 and 20000),"
" datetime varchar not null default '',"
" timezone varchar not null default '',"
" isdst integer not null default -1 check (isdst between -1 and 1),"
" location varchar not null default '',"
" country varchar not null default '',"
" foreign key (_uidx) references Users(_idx)"
");",
"CREATE TABLE Tags"
"("
" _idx integer primary key,"
" _uidx integer not null default 1,"
" name varchar not null unique check (length(name) between 1 and 128),"
" comment varchar default '',"
" foreign key (_uidx) references Users(_idx)"
");",
"CREATE TABLE DataTags"
"("
" _dataidx integer not null,"
" _tagidx integer not null,"
" primary key (_dataidx, _tagidx),"
" foreign key (_dataidx) references Data(_idx),"
" foreign key (_tagidx) references Tags(_idx)"
") without rowid;",
"CREATE TRIGGER TagsDeleteTrigger before delete on Tags for each row begin"
" delete from DataTags where _tagidx = old._idx;"
"end;",
"CREATE TABLE Notes"
"("
" _idx integer primary key,"
" _uidx integer not null default 1,"
" _dataidx integer not null,"
" _unixtime integer not null default (strftime('%s', 'now')),"
" note varchar not null check (note != ''),"
" foreign key (_uidx) references Users(_idx),"
" foreign key (_dataidx) references Data(_idx)"
");",
"CREATE TRIGGER DataDeleteTrigger before delete on Data for each row begin"
" delete from DataTags where _dataidx = old._idx;"
" delete from Notes where _dataidx = old._idx;"
"end;",
"CREATE TRIGGER UsersDeleteTrigger before delete on Users for each row begin"
" select raise (rollback, 'cant delete user root') where old._idx = 1;"
" delete from Tags where _uidx = old._idx;"
" delete from Notes where _uidx = old._idx;"
" delete from Data where _uidx = old._idx;"
"end;",
NULL
};

int swh_db_exec(
    const char* sql,
    int (*callback)(void* arg, int argc, char** argv, char** cols),
    void* arg,
    char err[512])
{
    int x;
    char* e = NULL;
    if (!_swh_db_cnx) {
        if (err)
            strcpy(err, "no database connection");
        else
            fprintf(stderr, "%s\n", "no database connection");
        return -1; // not connected
    }
#ifdef SWH_DB_TRACE
    printf("--> %s\n", sql);
#endif
    x = sqlite3_exec(_swh_db_cnx, sql, callback, arg, err ? &e : NULL);
    if (x != SQLITE_OK) {
        if (err) {
            memset(err, 0, 512);
            if (e) {
                strncpy(err, e, 511);
                sqlite3_free(e);
            }
            else
                snprintf(err, 511, "sqlite error code: (%d)", x);
        }
    }
    return x; // return sqlite code (>= 0)
}

int _swh_db_version_cb(void* arg, int argc, char** argv, char** cols)
{
    const int version = atoi(argv[0]);
    *(int*)arg = version != SWH_DB_VERSION_INT ? 1 : 0;
    return 0;
}

int swh_db_check_version(char err[512])
{
    int i = -1;
    if (swh_db_exec("select version from Meta order by version limit 1;",
                    &_swh_db_version_cb, &i, err))
        return 1; // sql error?!
    if (!i)
        return 0;
    if (i < 0) { // got no rows?!
        strcpy(err, "broken database");
        return 2;
    }
    memset(err, 0, 512);
    snprintf(err, 511, "database required version: %s", SWH_DB_VERSION_STR);
    return -1;
}

int swh_db_connect(const char* path,
    int check,
    char err[512])
{
    int i;
    char str[512];
    char* fpath = NULL;

    // close previous connection
    if (_swh_db_cnx && swh_db_close()) {
        strcpy(err, "error closing database connection");
        return 1;
    }
    // get db path
    if (!(fpath = getenv("SWH_DATA_PATH")) || !*fpath) {
        if (!path || !*path) {
            strcpy(err, "missing path to database");
            return 2;
        }
        fpath = (char*) path;
    }
    // check db file exists, if yes, check it is readable and writable
    i = access(fpath, F_OK) ? 0 : 1;
    if (i && access(fpath, R_OK|W_OK)) {
        memset(err, 0, 512);
        snprintf(err, 511, "bad permissions on database (%s)", fpath);
        return 3;
    }
    // prepare connection string
    memset(str, 0, 512);
    if (snprintf(str, 511, "file:%s?mode=rwc", fpath) < 0) {
        strcpy(err, "io error");
        return 4;
    }
    // initiate connection
    if (sqlite3_open(str, &_swh_db_cnx) != SQLITE_OK) {
        memset(err, 0, 512);
        snprintf(err, 511, "unable to connect to database (%s)", fpath);
        return 5;
    }
    // set busy timeout
    if (sqlite3_busy_timeout(_swh_db_cnx, 1000) != SQLITE_OK) {
        memset(err, 0, 512);
        snprintf(err, 511, "unable to set busy timeout on (%s)", fpath);
        return 6;
    }
    // install tables if new db
    if (!i) {
        char* p = (char*) _swh_db_creates_sql[0];
        do {
            if (swh_db_exec(p, NULL, NULL, err))
                return 7;
        } while ((p = (char*) _swh_db_creates_sql[++i]));
        return 0; // ok, no need to check version
    }
    if (!check)
        return 0;
    // check db version
    if ((i = swh_db_check_version(err)))
        return i < 0 ? -1 : 8;
    return 0;
}

int swh_db_close(void)
{
    if (!_swh_db_cnx)
        return 0;
    if (sqlite3_close(_swh_db_cnx) != SQLITE_OK)
        return 1;
    _swh_db_cnx = NULL;
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
