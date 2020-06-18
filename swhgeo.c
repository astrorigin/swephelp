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

int _swh_geocstrip(const char* coord, char* ret, const size_t maxlen)
{
    size_t i = 0;
    char* p = (char*) coord;

    for (; *p; ++p) {
        if (++i == maxlen)
            return 1;
        switch (*p) {
        case '\xc2': /* degree sign c2b0 */
            if (*++p != '\xb0')
                return 1;
        case '\'':
        case '"':
        case ',':
        case ':':
        case '/':
            *ret++ = ' ';
            continue;
        case 'N':
        case 'S':
        case 'E':
        case 'W':
            *ret++ = tolower(*p);
            continue;
        case 'n':
        case 's':
        case 'e':
        case 'w':
        case '-':
        case '.':
        case ' ':
            *ret++ = *p;
            continue;
        default:
            if (!isdigit(*p))
                return 1;
            *ret++ = *p;
        }
    }
    *ret = '\0';
    return 0;
}

int swh_geoc2d(const char* coord, double* ret)
{
    int i, ideg, imin;
    double ddeg, dmin, dsec;
    char sign, rest;
    char str[64];

    assert(coord);
    assert(ret);
    if (!*coord)
        return 1;
    /* remove decorations (°'",:/) */
    if (_swh_geocstrip(coord, str, 64))
        return 1;
    /* longest patterns first */
    i = sscanf(str, "%d %d %lf %c%c", /* DMSx */
               &ideg, &imin, &dsec, &sign, &rest);
    if (i == 4) {
        *ret = ideg + ((1.0/60)*imin) + ((1.0/3600)*dsec);
        ddeg = ideg;
        dmin = imin;
        goto checksign;
    }
    i = sscanf(str, "%d %c %d %lf%c", /* DxMS */
               &ideg, &sign, &imin, &dsec, &rest);
    if (i == 4) {
        *ret = ideg + ((1.0/60)*imin) + ((1.0/3600)*dsec);
        ddeg = ideg;
        dmin = imin;
        goto checksign;
    }
    i = sscanf(str, "%d %lf %c%c", /* DMx */
               &ideg, &dmin, &sign, &rest);
    if (i == 3) {
        *ret = ideg + ((1.0/60)*dmin);
        ddeg = ideg;
        dsec = 0;
        goto checksign;
    }
    i = sscanf(str, "%d %c %lf%c", /* DxM */
               &ideg, &sign, &dmin, &rest);
    if (i == 3) {
        *ret = ideg + ((1.0/60)*dmin);
        ddeg = ideg;
        dsec = 0;
        goto checksign;
    }
    i = sscanf(str, "%d %d %lf%c", /* DMS */
               &ideg, &imin, &dsec, &rest);
    if (i == 3) {
        *ret = ideg + ((1.0/60)*imin) + ((1.0/3600)*dsec);
        ddeg = ideg;
        dmin = imin;
        goto check;
    }
    i = sscanf(str, "%lf %c%c", /* Dx */
               &ddeg, &sign, &rest);
    if (i == 2) {
        *ret = ddeg;
        dmin = 0;
        dsec = 0;
        goto checksign;
    }
    i = sscanf(str, "%d %lf%c", /* DM */
               &ideg, &dmin, &rest);
    if (i == 2) {
        *ret = ideg + ((1.0/60)*dmin);
        ddeg = ideg;
        dsec = 0;
        goto check;
    }
    i = sscanf(str, "%lf%c", /* D */
               &ddeg, &rest);
    if (i == 1) {
        *ret = ddeg;
        dmin = 0;
        dsec = 0;
        goto check;
    }
    return 1;
  checksign:
    switch (sign) {
    case 'n':
    case 's':
        if (ddeg < 0 || ddeg > 90)
            return 1;
        break;
    default:
        if (ddeg < 0 || ddeg > 180)
            return 1;
    }
    switch (sign) {
    case 'n':
    case 'e':
        if (*ret < 0)
            return 1;
        break;
    default:
        if (*ret > 0)
            *ret = -(*ret);
    }
  check:
    if (ddeg < -180 || ddeg > 180
        || dmin < 0 || dmin >= 60 || dsec < 0 || dsec >= 60)
        return 1;
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
