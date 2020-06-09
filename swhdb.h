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

#ifndef SWHDB_H
#define SWHDB_H

#ifdef __cplusplus
extern "C"
{
#endif

#define SWH_DB_VERSION_INT  20200603
#define SWH_DB_VERSION_STR  "20200603"

/** @brief Connect to astro database
 *
 * The environment variable SWH_DATA_PATH is checked for a valid string,
 * and will override the path argument given.
 *
 * @param path Path to database file, can be NULL if set in environment
 * @param check Check for correct database version (bool)
 * @param err Buffer for errors
 * @return 0, or 1 on error, or 2 if database needs upgrading
 */
int swh_db_connect(
    const char* path,
    int check,
    char err[512]);

/** @brief Close connection to astro database
 *
 * @return 0, or 1 on error
 */
int swh_db_close(void);

/** @brief Execute SQL statement on astro database
 *
 */
int swh_db_exec(
    const char* sql,
    int (*callback)(void* arg, int argc, char** argv, char** cols),
    void* arg,
    char err[512]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHDB_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
