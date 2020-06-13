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
#include <math.h>
#include <swephexp.h>

#include "swhaspect.h"

int swh_match_aspect(
    double pos0,
    double speed0,
    double pos1,
    double speed1,
    double aspect,
    double orb,
    double* diffret,
    double* speedret,
    double* facret)
{
    const double diff = swe_difdegn(pos1, pos0);
    aspect = swe_degnorm(aspect);
    orb = fabs(orb);
    if (diff == aspect) {
        *speedret = speed0 > speed1 ? speed0 - speed1 :
            speed0 < speed1 ? speed1 - speed0 : 0;
        *diffret = 0;
        *facret = 0;
        return 0;
    }
    *diffret = diff - aspect;
    *speedret = *diffret > 0 ? speed1 - speed0 : speed0 - speed1;
    *facret = *diffret / orb;
    if (aspect - orb <= diff && diff <= aspect + orb)
        return 0;
    return 1;
}

int swh_match_aspect2(
    double pos0,
    double speed0,
    double pos1,
    double speed1,
    double aspect,
    double orb,
    double* diffret,
    double* speedret,
    double* facret)
{
    int x0, x1;
    double ret0[3], ret1[3];
    if (aspect < 0 || aspect > 180)
        aspect = swe_difdegn(0, aspect);
    x0 = swh_match_aspect(pos0, speed0, pos1, speed1, aspect, orb,
                          &ret0[0], &ret0[1], &ret0[2]);
    if (aspect == 0 || aspect == 180)
        goto first;
    x1 = swh_match_aspect(pos0, speed0, pos1, speed1, -(aspect), orb,
                          &ret1[0], &ret1[1], &ret1[2]);
    if (fabs(ret1[0]) < fabs(ret0[0]))
        goto second;
    else if (fabs(ret0[0]) < fabs(ret1[0]))
        goto first;
    else if (ret1[1] < ret0[1])
        goto second;
  first:
    *diffret = ret0[0];
    *speedret = ret0[1];
    *facret = ret0[2];
    return x0;
  second:
    *diffret = ret1[0];
    *speedret = ret1[1];
    *facret = ret1[2];
    return x1;
}

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
    double* facret)
{
    double* orb;
    const double diff = swe_difdegn(pos1, pos0);
    aspect = swe_degnorm(aspect);
    app_orb = fabs(app_orb);
    sep_orb = fabs(sep_orb);
    def_orb = fabs(def_orb);
    if (diff == aspect) {
        *speedret = speed0 > speed1 ? speed0 - speed1 :
            speed0 < speed1 ? speed1 - speed0 : 0;
        *diffret = 0;
        *facret = 0;
        return 0;
    }
    *diffret = diff - aspect;
    *speedret = *diffret > 0 ? speed1 - speed0 : speed0 - speed1;
    orb = *speedret < 0 ? &app_orb : *speedret > 0 ? &sep_orb : &def_orb;
    *facret = *diffret / *orb;
    if (aspect - *orb <= diff && diff <= aspect + *orb)
        return 0;
    return 1;
}

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
    double* facret)
{
    int x0, x1;
    double ret0[3], ret1[3];
    if (aspect < 0 || aspect > 180)
        aspect = swe_difdegn(0, aspect);
    x0 = swh_match_aspect3(pos0, speed0, pos1, speed1, aspect, app_orb,
                           sep_orb, def_orb, &ret0[0], &ret0[1], &ret0[2]);
    if (aspect == 0 || aspect == 180)
        goto first;
    x1 = swh_match_aspect3(pos0, speed0, pos1, speed1, -(aspect), app_orb,
                           sep_orb, def_orb, &ret1[0], &ret1[1], &ret1[2]);
    if (fabs(ret1[0]) < fabs(ret0[0]))
        goto second;
    else if (fabs(ret0[0]) < fabs(ret1[0]))
        goto first;
    else if (ret1[1] < ret0[1])
        goto second;
  first:
    *diffret = ret0[0];
    *speedret = ret0[1];
    *facret = ret0[2];
    return x0;
  second:
    *diffret = ret1[0];
    *speedret = ret1[1];
    *facret = ret1[2];
    return x1;
}

void swh_antiscion(
    const double pos[6],
    const double axis,
    double antisret[6],
    double contrantisret[6])
{
    assert(pos);
    assert(antisret);
    assert(contrantisret);
    antisret[0] = swe_degnorm(axis - swe_difdeg2n(pos[0], axis));
    antisret[1] = pos[1];
    antisret[2] = pos[2];
    antisret[3] = -(pos[3]);
    antisret[4] = pos[4];
    antisret[5] = pos[5];
    contrantisret[0] = swe_degnorm(antisret[0] + 180);
    contrantisret[1] = -(pos[1]);
    contrantisret[2] = pos[2];
    contrantisret[3] = -(pos[3]);
    contrantisret[4] = -(pos[4]);
    contrantisret[5] = pos[5];
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
