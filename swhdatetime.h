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

#ifndef SWHDATETIME_H
#define SWHDATETIME_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <swephexp.h>

/** @brief Get current Julian day number, Gregorian calendar
 * @return Julian day number
 */
double swh_jdnow(void);

/** @brief Get Julian day number from a date and time
 *
 * A conveniance function that, compared to swe_julday, does not require
 * hour in decimal format, but accepts integers only.
 *
 * @attention Does not check date validity. See swe_date_conversion.
 *
 * @see swh_revjul()
 *
 * @param year Year (4 digits)
 * @param mon Month [1;12]
 * @param day Day [1;31]
 * @param hour Hour [0;23]
 * @param min Minutes [0;59]
 * @param sec Seconds [0;59]
 * @param flag Calendar type (SE_GREG_CAL|SE_JUL_CAL)
 * @return Julian day number
 */
#define swh_julday(year, mon, day, hour, min, sec, flag)\
        swe_julday(year, mon, day, ((hour)+((min)/60.0)+((sec)/3600.0)), flag)

/** @brief Reverse Julian day to date and time
 *
 * Similar to swe_revjul, but returns time with three integers instead
 * of one double (fractions of seconds may get lost.)
 *
 * @see swh_julday()
 *
 * @param jd Julian day
 * @param flag Calendar type (SE_GREG_CAL|SE_JUL_CAL)
 * @param dt Results, declared as int[6] (year, month, day, hour, min, sec)
 * @return 0
 */
int swh_revjul(double jd, int flag, int dt[6]);

/** @brief Get integers from datetime representation
 *
 * Datetimes are expected to roughly follow the ISO 8601 standard,
 * 'Year-Month-Day Hour:Minutes:Seconds'.
 *
 * All non-digits in the given string are ignored and any of them can be
 * used as separator, including spaces. A minus is evaluated only in
 * front of the year, as first char of the string.
 *
 * Optionaly, the time part (hour etc) can be omitted, in that case will
 * return zeros.
 *
 * @param coord datetime string
 * @param ret Returned integers declared as int[6]
 * @return 0 on success, or 1 if string is invalid
 */
int swh_dt2i(const char* dt, int ret[6]);

/** @bief Get integers from a time representation
 *
 * Similar to swh_dt2i, but just for time. Minutes and seconds are optional.
 *
 * @see swh_dt2i
 * @param t Time string
 * @param Returned integers, declared as int[3]
 * @return 0 on success, or 1 if string is invalid
 */
int swh_t2i(const char* t, int ret[3]);

/** @brief Get integers representing a duration between JD numbers
 *
 * @param jdstart Lowest JD number
 * @param jdend Highest JD number
 * @param ret Returned integers, declared as int[4] (days, hours, min, sec)
 */
void swh_jduration(
    double jdstart,
    double jdend,
    int ret[4]);

/** @brief Get an (iso 8601-style) string from given JD
 *
 * @param jd Julian day (ut)
 * @param cal SE_GREG_CAL or SE_JUL_CAL
 * @param ret Returned string, declared as char[64]
 * @return 0, or 1 on error
 */
int swh_jd2isostr(double jd, int cal, char ret[64]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHDATETIME_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
