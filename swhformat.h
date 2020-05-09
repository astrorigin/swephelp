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

/** @file swhformat.h
 * @brief swephelp formating functions
 */

#ifndef SWHFORMAT_H
#define SWHFORMAT_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Get degrees, sign number, minutes, seconds, from longitude position
 *
 * Split a longitude position to a more usual format.
 *
 * @param pos Longitude position [0;360[
 * @param ret Results, declared as int[4] (deg, sign, min, sec)
 * @return -
 */
void swh_degsplit(double pos, int *ret);

/** @brief Get string representation (english) of zodiacal sign
 * @param sign Sign number [0;11]
 * @param str Returned string, declared as char[12]
 * @return 0, or -1 if sign was invalid
 */
int swh_signtostr(int sign, char *str);

/** @brief Get house system name (english)
 * @param hsys House system char id [ABCDEFGHIiKLMNOPQRSTUVWXY]
 * @param str Returned string, declared as char[50]
 * @return 0, or -1 if house system is unknown
 */
int swh_house_system_name(char hsys, char *str);

/** @brief Get house system unique id
 * @param hsys House system char identifier (see swisseph docs)
 * @return Identifier, or -1 if house system invalid
 */
int swh_house_system_id(char hsys);

/** @brief Get house system char
 * @param hsys House system int identifier
 * @return iIdentifier, or -1 if house system invalid
 */
char swh_house_system_char(int hsys);

/** @brief Get sidereal mode flag
 *
 * Return the swisseph sidereal mode flag from the given sidereal mode index.
 *
 * @param sidmode
 * @return Flag, or -1 if invalid mode
 */
int swh_sidereal_mode_flag(int sidmode);

/** @brief Get sidereal mode unique identifier
 * @param sidmode Sidereal mode identifier
 * @return index [0:22]
 */
int swh_sidereal_mode_id(int sidmode);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHFORMAT_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
