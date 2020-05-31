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

#ifndef SWHTIMEZONE_H
#define SWHTIMEZONE_H

#ifdef __cplusplus
extern "C"
{
#endif

struct swh_tzabbr
{
    char* name;
    char* desc;
    char* iso;
    int   hours;
    int   minutes;
};

#define SWH_TZABBR_NUM  (203)

extern const struct swh_tzabbr swh_tzabbrlist[SWH_TZABBR_NUM+1];

/** @brief Find details about a timezone, given its abbreviation
 *
 * In some cases, up to 3 timezones bear the same abbreviation.
 *
 * Sources:
 * https://en.wikipedia.org/wiki/List_of_time_zone_abbreviations
 *
 * @param tz Abbreviated timezone name
 * @param ret Returned pointers to timezones found (3+NULL), read-only
 * @return 0 if found, else 1
 */
int swh_tzabbr_find(const char* tz, struct swh_tzabbr* ret[4]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHTIMEZONE_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
