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

#ifndef SWHSEARCH_H
#define SWHSEARCH_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Find next direction changing of object
 *
 * This function tries to find when and where a planet in direct or
 * retrograde motion goes retrograde or direct (respectively).
 *
 * @remarks Some planets or conditions (flags) wont be treated and the
 * function will in those cases return 3.
 *
 * @remarks If stop is set to 0, the search is not limited in time.
 * Otherwise, the function may return 2 when time limit has been reached.
 * Flag must include SEFLG_SPEED, and SEFLG_NOGDEFL to avoid bad surprises;
 * alternatively use true positions.
 *
 * @param planet Planet number (SE_*, etc)
 * @param jdstart Julian day number, when search is starting
 * @param backw Search before jdstart [1], or after [0] (boolean)
 * @param stop Limit search to a certain time, expressed in days
 * @param flags Calculation flags, see swisseph docs
 * @param jdret Julian day number found
 * @param posret Planet's positions found
 * @param err Buffer for error, declared as char[256]
 * @return 0 on success, 1 on error, 2 if limit reached, 3 if argument is invalid
 */
int swh_next_retro(
    int planet,
    double jdstart,
    int backw,
    double stop,
    int flags,
    double* jdret,
    double* posret,
    char* err);

/** @brief Find next exact aspect to a fixed point
 *
 * Get Julian day number and positions when a celestial object makes a
 * longitudinal aspect to a fixed point expressed in longitude degrees.
 *
 * @remarks If stop is set to 0, the search is not limited in time.
 * Otherwise, the function may return 2 when time limit has been reached.
 *
 * @param planet Planet number (SE_*, etc)
 * @param aspect Aspect, in degrees [0;360[
 * @param fixedpt Fixed point targeted [0;360[
 * @param jdstart Julian day number, when search is starting
 * @param backw Search before jdstart [1], or after [0] (boolean)
 * @param stop Limit search to a certain time, expressed in days
 * @param flags Calculation flags, see swisseph docs
 * @param jdret Julian day number found
 * @param posret Planet's positions found
 * @param err Buffer for errors, declared as char[256]
 * @return 0 on success, 2 if time limit reached, 1 on error
 */
int swh_next_aspect(
    int planet,
    double aspect,
    double fixedpt,
    double jdstart,
    int backw,
    double stop,
    int flags,
    double* jdret,
    double* posret,
    char* err);

/** @brief Find next exact aspect to a fixed point
 *
 * Same as swh_next_aspect but with aspect in [0;180], instead of [0;360[.
 *
 * @see swh_next_aspect()
 */
int swh_next_aspect2(
    int planet,
    double aspect,
    double fixedpt,
    double jdstart,
    int backw,
    double stop,
    int flags,
    double* jdret,
    double* posret,
    char *err);

/** @brief Find next aspect between two moving objects
 *
 * Get Julian day number and positions when a celestial object makes a
 * longitudinal aspect to another moving object.
 *
 * @remarks If star != NULL, the other planet is ignored.
 * If stop is set to 0, the search is not limited in time.
 * Otherwise, the function may return 2 when time limit has been reached.
 *
 * @param planet Planet number (SE_*, etc)
 * @param aspect Aspect, in degrees [0;360[
 * @param other Other planet number
 * @param star Fixed star
 * @param jdstart Julian day number, when search is starting
 * @param backw Search before jdstart [1], or after [0] (boolean)
 * @param stop Limit search to a certain time, expressed in days
 * @param flags Calculation flags, see swisseph docs
 * @param jdret Julian day number found
 * @param posret1 Planet's positions found
 * @param posret2 Other planet (or star) positions found
 * @param err Buffer for errors, declared as char[256]
 * @return 0 on success, 2 if time limit reached, 1 on error
 */
int swh_next_aspect_with(
    int planet,
    double aspect,
    int other,
    char* star,
    double jdstart,
    int backw,
    double stop,
    int flags,
    double* jdret,
    double* posret1,
    double* posret2,
    char* err);

/** @brief Find next aspect between two moving objects
 *
 * Same as swh_next_aspect_with, but aspect in [0;180], instead of [0;360[.
 *
 * @see swh_next_aspect_with()
 */
int swh_next_aspect_with2(
    int planet,
    double aspect,
    int other,
    char* star,
    double jdstart,
    int backw,
    double stop,
    int flags,
    double* jdret,
    double* posret1,
    double* posret2,
    char* err);

/** @brief Find next aspect to a house cusp
 *
 * Get Julian day number and positions, and houses cusps, when a
 * celestial object makes longitudinal aspect to a house cusp.
 *
 * @remarks If star != NULL, the planet is ignored.
 *
 * @see For risings, settings, meridian transits, see swe_rise_trans.
 *
 * @param planet Planet number (SE_*, etc)
 * @param star Fixed star
 * @param aspect Aspect, in degrees [0;360[
 * @param cusp House cusp number [1;12], or [1;36] for Gauquelin sectors
 * @param jdstart Julian day number, when search is starting
 * @param lat Latitude, in degrees (north is positive)
 * @param lon Longitude, in degrees (east is positive)
 * @param hsys House system, see swisseph docs
 * @param backw Search before jdstart [1], or after [0] (boolean)
 * @param flags Calculation flags, see swisseph docs
 * @param jdret Julian day number found
 * @param posret Planet (or fixed star) positions found
 * @param cuspsret House cusps positions found
 * @param ascmcret Asc-Mc-etc found, see swisseph docs
 * @param err Buffer for errors, declared as char[256]
 * @return 0 on success, 1 on error
 */
int swh_next_aspect_cusp(
    int planet,
    char* star,
    double aspect,
    int cusp,
    double jdstart,
    double lat,
    double lon,
    int hsys,
    int backw,
    int flags,
    double* jdret,
    double* posret,
    double* cuspsret,
    double* ascmcret,
    char* err);

/** @brief Find next aspect to a house cusp
 *
 * Same as swh_next_aspect_cusp, but aspect in [0;180], instead of [0;360[.
 *
 * @see swh_next_aspect_cusp()
 */
int swh_next_aspect_cusp2(
    int planet,
    char* star,
    double aspect,
    int cusp,
    double jdstart,
    double lat,
    double lon,
    int hsys,
    int backw,
    int flags,
    double* jdret,
    double* posret,
    double* cuspsret,
    double* ascmcret,
    char* err);

/** @brief Get number of years difference between two julian days
 *
 * One exact "astrological" year can be considered as one solar return.
 * Then is it varying with the type of zodiac in use.
 *
 * @param jd1 First Julian day
 * @param jd2 Second Julian day
 * @param flags Calculation flags
 * @param years Result, declared as double
 * @param err Buffer for errors, declared as char[256]
 * @return 0 on success
 */
int swh_years_diff(double jd1,
    double jd2,
    int flags,
    double* years,
    char* err);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHSEARCH_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
