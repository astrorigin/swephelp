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

#ifndef SWHASPECT_H
#define SWHASPECT_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Aspect matching - aspect in range [0;360[
 *
 * Check if the two given positions match the aspect within the given orb.
 *
 * This also calculates the difference between targeted aspect and objects
 * distance (so that pos0 + asp + diffret = pos2), and its speed (so that
 * a negative speed indicates an applying aspect). Returned factor simply
 * is that difference expressed in orb units (so that orb * facret = diffret).
 *
 * If you are not interested in that mumbo-jumbo, just set objects speeds to
 * zero, and consider only the value returned by the function.
 *
 * @param pos0 First object longitude, in degrees [0;360[
 * @param speed0 First object longitude speed, in degrees per day
 * @param pos1 Second object longitude, in degrees [0;360[
 * @param speed1 Second object longitude speed, in degrees per day
 * @param aspect Aspect targeted, in degrees [0;360[
 * @param orb Orb allowed, in degrees
 * @param diffret Difference between aspect and objects distance, in degrees
 * @param speedret Difference speed, in degrees per day
 * @param facret Difference expressed in orb units
 * @return 0 if aspect match within orb, else 1
 */
int swh_match_aspect(
    double pos0,
    double speed0,
    double pos1,
    double speed1,
    double aspect,
    double orb,
    double* diffret,
    double* speedret,
    double* facret);

/** @brief Aspect matching - aspect in range [0;180]
 *
 * Same as swh_match_aspect, but aspect in [0;180], instead of [0;360[
 *
 * @see swh_match_aspect()
 */
int swh_match_aspect2(
    double pos0,
    double speed0,
    double pos1,
    double speed1,
    double aspect,
    double orb,
    double* diffret,
    double* speedret,
    double* facret);

/** @brief Aspect matching - aspect in range [0;360[ and complex orb
 *
 * Same as swh_match_aspect, but with a different orb in case aspect is
 * applying, separating, or stable.
 *
 * @see swh_match_aspect()
 */
int swh_match_aspect3(
    double pos0,
    double speed0,
    double pos1,
    double speed1,
    double aspect,
    double app_orb,
    double sep_orb,
    double def_orb,
    double* diffret,
    double* speedret,
    double* facret);

/** @brief Aspect matching - aspect in range [0;180] and complex orb
 *
 * Same as swh_match_aspect2, but with a different orb in case aspect is
 * applying, separating, or stable.
 *
 * @see swh_match_aspect2()
 */
int swh_match_aspect4(
    double pos0,
    double speed0,
    double pos1,
    double speed1,
    double aspect,
    double app_orb,
    double sep_orb,
    double def_orb,
    double* diffret,
    double* speedret,
    double* facret);

/** @brief Calculate antiscion and contrantiscion
 *
 * @param pos Object positions and speeds, as returned by swe_calc functions
 * @param axis Degree of axis, eg. 90 for 0° cancer-capricorn
 * @param antisret Returned antiscion positions, declared as double[6]
 * @param contrantisret Returned contrantiscion positions, declared as double[6]
 */
void swh_antiscion(
    const double pos[6],
    const double axis,
    double antisret[6],
    double contrantisret[6]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHASPECT_H */
/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
