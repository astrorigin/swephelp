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

#ifndef SWHGEO_H
#define SWHGEO_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Get double from latitude or longitude string
 *
 * Possible string formats:
 *  - DMSx
 *  - DxMS
 *  - DMx
 *  - DxM
 *  - DMS
 *  - Dx
 *  - DM
 *  - D
 * Where D is degrees, M is minutes, S is seconds, x is a char in "NSEW"
 * (direction).
 * The last number given can be a floating point number.
 * If no direction is given, a negative degree value is accepted down to -180.
 * Decorations chars (like °"':/,) can serve as separators and are accepted
 * anywhere (ignored), as well as spaces. Dots (points) are not separators
 * but used for floating points numbers.
 *
 * @param coord Latitude or longitude string
 * @param ret Returned double
 * @return 0 on success, or 1 if string is invalid
 */
int swh_geoc2d(const char* coord, double* ret);

/** @brief Extract degrees, minutes and seconds from a geographical coordinate
 * @param coord Latitude or Longitude
 * @param ret Returned degrees, minutes, seconds, declared as int[3]
 * @return 0
 */
int swh_geod2i(double coord, int *ret);

/** @brief Get string from geographical coordinates
 *
 * Get formated string for a geographical coordinate, like
 * "{degrees}:{N|S|E|W}:{minutes}:{seconds}".
 *
 * @remarks Use the defined macros swh_geolat2c swh_geolon2c instead.
 *
 * @param coord longitude as double
 * @param ret Buffer for returned string, declared as char[12]
 * @param maxdeg 90 for latitude, 180 for longitude
 * @return 0 on success, or -1 if coord is invalid
 */
int swh_geod2c(double coord, int maxdeg, char *ret);

/** @brief Get string from geographical latitude
 *
 * Get formated string for a latitude, like
 * "{degrees}:{N|S}:{minutes}:{seconds}".
 *
 * @param coord latitude as double
 * @param ret Buffer for returned string, declared as char[11]
 * @return 0 on success, or -1 if latitude is invalid
 */
#define swh_geolat2c(coord, ret)        swh_geod2c((coord), 90, (ret))

/** @brief Get string from geographical longitude
 *
 * Get formated string for a longitude, like
 * "{degrees}:{E|W}:{minutes}:{seconds}".
 *
 * @param coord latitude as double
 * @param ret Buffer for returned string, declared as char[12]
 * @return 0 on success, or -1 if latitude is invalid
 */
#define swh_geolon2c(coord, ret)        swh_geod2c((coord), 180, (ret))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHGEO_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
