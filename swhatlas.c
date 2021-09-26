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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#include "swhatlas.h"

#ifdef _MSC_VER
#define TLS __declspec(thread)
#else
#define TLS __thread
#endif

TLS sqlite3* _swh_atlas_cnx = NULL;

int swh_atlas_connect(const char* path)
{
    int x;
    char p[512];
    char* env = NULL;

    if (_swh_atlas_cnx && swh_atlas_close())
        return 1;
    memset(p, 0, 512);
    if ((env = getenv("SWH_ATLAS_PATH")) && *env)
        x = snprintf(p, 511, "file:%s?mode=ro", env);
    else if (path && *path)
        x = snprintf(p, 511, "file:%s?mode=ro", path);
    else
        return 1;
    if (x < 0)
        return 1;
    x = sqlite3_open(p, &_swh_atlas_cnx);
    if (x != SQLITE_OK)
        return 1;
    return 0;
}

int swh_atlas_close(void)
{
    if (!_swh_atlas_cnx)
        return 0;
    if (sqlite3_close(_swh_atlas_cnx) != SQLITE_OK)
        return 1;
    _swh_atlas_cnx = NULL;
    return 0;
}

int swh_atlas_countries_list(
    int (*callback)(void* arg, int argc, char** argv, char** cols),
    void* arg,
    char err[512])
{
    const char* sql = "SELECT * FROM CountryInfo ORDER BY country;";
    char* e = NULL;
    int x;

    assert(callback);
    assert(err);
    if (!_swh_atlas_cnx) {
        strcpy(err, "not connected");
        return 1;
    }
    x = sqlite3_exec(_swh_atlas_cnx, sql, callback, arg, &e);
    if (x != SQLITE_OK) {
        memset(err, 0, 512);
        if (e) {
            snprintf(err, 511, "%s", e);
            sqlite3_free(e);
        }
        else
            snprintf(err, 511, "error (%d)", x);
        return 1;
    }
    return 0;
}

int swh_atlas_search(
    const char* location,
    const char* country,
    int (*callback)(void* arg, int argc, char** argv, char** cols),
    void* arg,
    char err[512])
{
    char* c;
    char* p;
    char* e = NULL;
    int x = 0;
    char loc[128];
    char ctry[128];
    char sql[800];

    assert(callback);
    assert(err);
    if (!_swh_atlas_cnx) {
        strcpy(err, "not connected");
        return 1;
    }
    if (!location || !*location) {
        strcpy(err, "missing argument: location");
        return 1;
    }
    if (!country || strlen(country) < 2) {
        strcpy(err, "missing argument: country");
        return 1;
    }
    loc[x++] = '%';
    for (c = (char*) location; *c && x < 125; ++c) {
        loc[x++] = *c == '\'' ? '?' : *c;
    }
    loc[x++] = '%';
    loc[x] = '\0';
    x = 0;
    for (c = (char*) country; *c && x < 126; ++c) {
        ctry[x++] = *c == '\'' ? '?' : *c;
    }
    ctry[x] = '\0';
    if (strlen(ctry) == 2) {
        ctry[0] = toupper(ctry[0]);
        ctry[1] = toupper(ctry[1]);
        p = "SELECT A.name, A.asciiname, A.alternatenames, B.iso, A.latitude,"
            " A.longitude, A.elevation, C.timezoneid"
            " FROM GeoNames as A, CountryInfo AS B, Timezones AS C"
            " WHERE B.iso = '%s' AND B._idx = A.country AND"
            " (A.name LIKE '%s' OR A.asciiname LIKE '%s' OR A.alternatenames"
            " LIKE '%s') AND A.timezone = C._idx ORDER BY A.name;";
    }
    else {
        ctry[x++] = '%';
        ctry[x] = '\0';
        p = "SELECT A.name, A.asciiname, A.alternatenames, B.iso, A.latitude,"
            " A.longitude, A.elevation, C.timezoneid"
            " FROM GeoNames as A, CountryInfo AS B, Timezones AS C"
            " WHERE B.country LIKE '%s' AND B._idx = A.country AND"
            " (A.name LIKE '%s' OR A.asciiname LIKE '%s' OR A.alternatenames"
            " LIKE '%s') ORDER BY A.name;";
    }
    memset(sql, 0, 800);
    x = snprintf(sql, 799, p, ctry, loc, loc, loc);
    if (x < 0) {
        strcpy(err, "io error");
        return 1;
    }
    x = sqlite3_exec(_swh_atlas_cnx, sql, callback, arg, &e);
    if (x != SQLITE_OK) {
        memset(err, 0, 512);
        if (e) {
            snprintf(err, 511, "%s", e);
            sqlite3_free(e);
        }
        else
            snprintf(err, 511, "error (%d)", x);
        return 1;
    }
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
