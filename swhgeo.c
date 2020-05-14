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
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "swhgeo.h"
#include "swhwin.h"

int swh_geoc2d(const char* coord, double* ret)
{
    const char* pattern = "%d:%c:%d:%d%c";
    int i;
    int deg = 0, min = 0, sec = 0;
    char sign = 'x', rest = '\0';

    assert(coord);
    assert(ret);

    if (!*coord)
        return 1;
    i = sscanf(coord, pattern, &deg, &sign, &min, &sec, &rest);
    if (i == EOF || i < 2)
        return 1;
    if (rest)
        return 1;
    if (sec < 0 || sec > 59 || min < 0 || min > 59)
        return 1;
    sign = tolower(sign);
    switch (sign) {
    case 'n':
    case 's':
        if (deg < 0 || deg > 90)
            return 1;
        break;
    case 'e':
    case 'w':
        if (deg < 0 || deg > 180)
            return 1;
        break;
    default:
        return 1;
    }
    switch (sign) {
    case 'n':
    case 'e':
        *ret = deg + ((1.0/60)*min) + ((1.0/3600)*sec);
        break;
    default:
        *ret = -(deg + ((1.0/60)*min) + ((1.0/3600)*sec));
    }
    return 0;
}

int swh_geod2i(double coord, int *ret)
{
    coord = fabs(coord);
    ret[0] = (int) floor(coord);
    coord -= ret[0];
    ret[1] = (int) lround(coord * 60);
    coord -= ret[1] / 60.0;
    ret[2] = (int) lround(coord * 3600);
    if (ret[2] < 0) ret[2] = 0;
    return 0;
}

int swh_geod2c(double coord, int maxdeg, char *ret)
{
    int deg, dir, min, sec;
    if (coord < -(maxdeg) || coord > maxdeg)
        return -1;
    if (coord >= 0)
    {
        deg = (int) floor(coord);
        dir = 1;
    }
    else
    {
        deg = (int) fabs(ceil(coord));
        dir = 0;
    }
    coord = fabs(coord) - deg;
    min = (int) lround(coord * 60);
    coord -= min / 60.0;
    sec = (int) lround(coord * 3600);
    if (maxdeg == 90) /* latitude */
        sprintf(ret, "%.2d:%s:%.2d:%.2d", deg, dir ? "N" : "S", min, sec);
    else /* assuming longitude */
        sprintf(ret, "%.3d:%s:%.2d:%.2d", deg, dir ? "E" : "W", min, sec);
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
