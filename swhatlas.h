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

#ifndef SWHATLAS_H
#define SWHATLAS_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Connect to atlas database
 *
 * The environment variable SWH_ATLAS_PATH is checked for a valid string,
 * and will override the path argument given.
 *
 * @param path Path to database file, can be NULL if set in environment
 * @return 0, or 1 on error
 */
int swh_atlas_connect(const char* path);

/** @brief Close connection to atlas database
 *
 * @return 0, or 1 on error
 */
int swh_atlas_close(void);

/** @brief Get all the contents of the countries table
 *
 * @param callback Callback function for each row
 * @param arg Argument passed to callback function
 * @param err Buffer for error messages
 * @return 0, or 1 on error
 */
int swh_atlas_countries_list(
    int (*callback)(void* arg, int argc, char** argv, char** cols),
    void* arg,
    char err[512]);

/** @brief Search for locations, in a country
 *
 * @param location Name searched, can be abbreviated (not empty)
 * @param country Country searched, can be ISO code or abbreviated (not empty)
 * @param callback Callback function for each row
 * @param arg Argument passed to callback function
 * @param err Buffer for error messages
 * @return 0, or 1 on error
 */
int swh_atlas_search(
    const char* location,
    const char* country,
    int (*callback)(void* arg, int argc, char** argv, char** cols),
    void* arg,
    char err[512]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHATLAS_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
