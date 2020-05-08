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

#ifndef SWHMISC_H
#define SWHMISC_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Somewhat higher-level interface to swe_calc_ut/swe_fixstar2_ut
 *  @param tjdut Julian day UT
 *  @param planet Planet number
 *  @param star Star name, or NULL
 *  @param flags Swisseph calculation flags
 *  @param res Returned results, array for 6 double
 *  @param stnamret If star != NULL, buffer for returned star name
 *  @param err Buffer for errors
 *  @return Returned calculation flags
 *
 *  In case of star calculation (star != NULL), param stnamret must first be
 *  initialized with {'\0'}, and size (ST_MAX_STNAME*2)+1. If it is not empty,
 *  it wont be overwritten by the param star, and is used directly for
 *  a subsequent calculation.
 *
 *  @remark Made for pyswisseph.
 */
int swh_calc_ut(
    double tjdut,
    int planet,
    char* star,
    int flags,
    double* res,
    char* stnamret,
    char* err);

/** @brief Calculate Halbronn's "Saturn 4 Stars" index value
 *  @param jd Julian day UT
 *  @param flag Calculation flags, see Swisseph docs
 *  @param ret Array for 6 double (longitude of Sat, Ald, Reg, Ant, Fom, and index value)
 *  @param err Buffer for error, declared as char[256]
 *  @return 0 on success, else -1
 *
 *  This is an attempt at defining an "index value" based on Jacques Halbronn's
 *  theory, known as the "Saturn 4 stars" system, or "Astrocyclon", or "Central
 *  Astrology", etc.
 *
 *  The returned value (the index) represents, for a given time, the visual
 *  proximity of Saturn regarding its cycle along the path marked by the royal stars.
 *  The nearer of the stars Saturn is found, closer to 100 the index value is.
 *  Also, the index value is zero (0) exactly when saturn is at mid-point between
 *  the two nearest stars.
 */
int swh_saturn_4_stars( const double jd, const int flag, double* ret, char* err );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHMISC_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
