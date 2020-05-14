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

#include <math.h>
#include <swephexp.h>

#include "swhaspect.h"

int swh_match_aspect(double pos0, double speed0, double pos1, double speed1,
    double aspect, double orb, double *diffret, int *applic, double *factor)
{
    double diff = swe_difdegn(pos0, pos1);
    aspect = fabs(aspect);
    orb = fabs(orb);
    if (diff > aspect)
    {
        if (speed1 > speed0) { *applic = -1; }
        else if (speed1 < speed0) { *applic = 1; }
        else { *applic = 0; }
        *diffret = diff-aspect;
    }
    else if (diff < aspect)
    {
        if (speed1 > speed0) { *applic = 1; }
        else if (speed1 < speed0) { *applic = -1; }
        else { *applic = 0; }
        *diffret = aspect-diff;
    }
    else /* aspect is exact, cannot applic */
    {
        if (speed1 != speed0) { *applic = 1; }
        else { *applic = 0; }
        *diffret = 0;
        *factor = 0;
        return 0;
    }
    *factor = *diffret / orb;
    if (aspect-orb <= diff && diff <= aspect+orb) { return 0; } /* match */
    else { return -1; } /* no match */
}

int swh_match_aspect2(double pos0, double speed0, double pos1, double speed1,
    double aspect, double orb, double *diffret, int *applic, double *factor)
{
    double difdeg2n = swe_difdeg2n(pos0, pos1);
    double diff = fabs(difdeg2n);
    aspect = fabs(aspect);
    orb = fabs(orb);
    if (difdeg2n > 0)
    {
        if (diff > aspect)
        {
            if (speed1 > speed0) { *applic = -1; }
            else if (speed1 < speed0) { *applic = 1; }
            else { *applic = 0; }
            *diffret = diff-aspect;
        }
        else if (diff < aspect)
        {
            if (speed1 > speed0) { *applic = 1; }
            else if (speed1 < speed0) { *applic = -1; }
            else { *applic = 0; }
            *diffret = aspect-diff;
        }
        else /* aspect exact */
        {
            if (speed1 != speed0) { *applic = 1; }
            else { *applic = 0; }
            *diffret = 0;
            *factor = 0;
            return 0;
        }
    }
    else if (difdeg2n > -180)
    {
        if (diff > aspect)
        {
            if (speed1 > speed0) { *applic = 1; }
            else if (speed1 < speed0) { *applic = -1; }
            else { *applic = 0; }
            *diffret = diff-aspect;
        }
        else if (diff < aspect)
        {
            if (speed1 > speed0) { *applic = -1; }
            else if (speed1 < speed0) { *applic = 1; }
            else { *applic = 0; }
            *diffret = aspect-diff;
        }
        else /* aspect exact */
        {
            if (speed1 != speed0) { *applic = 1; }
            else { *applic = 0; }
            *diffret = 0;
            *factor = 0;
            return 0;
        }
    }
    else /* exact conjunction or opposition */
    {
        if (speed1 != speed0) { *applic = 1; }
        else { *applic = 0; }
        *diffret = 0;
        *factor = 0;
        return 0;
    }
    *factor = *diffret / orb;
    if (aspect-orb <= diff && diff <= aspect+orb) { return 0; } /* match */
    else { return -1; } /* no match */
}

int swh_match_aspect3(double pos0, double speed0, double pos1, double speed1,
    double aspect, double app_orb, double sep_orb, double def_orb,
    double *diffret, int *applic, double *factor)
{
    app_orb = fabs(app_orb);
    sep_orb = fabs(sep_orb);
    def_orb = fabs(def_orb);
    if (speed0 == speed1)
    { /* stable aspect */
        return swh_match_aspect(pos0, speed0, pos1, speed1, aspect,
            def_orb, diffret, applic, factor);
    }
    if (app_orb == sep_orb)
    {
        return swh_match_aspect(pos0, speed0, pos1, speed1, aspect, app_orb,
            diffret, applic, factor);
    }
    if (app_orb > sep_orb)
    {
        int i = swh_match_aspect(pos0, speed0, pos1, speed1, aspect, app_orb,
            diffret, applic, factor);
        if (i == 0)
        {
            if (*applic == -1)
            {
                return 0;
            }
            else if (*diffret <= sep_orb)
            {
                *factor = *diffret / sep_orb;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        int i = swh_match_aspect(pos0, speed0, pos1, speed1, aspect, sep_orb,
            diffret, applic, factor);
        if (i == 0)
        {
            if (*applic == 1)
            {
                return 0;
            }
            else if (*diffret <= app_orb)
            {
                *factor = *diffret / app_orb;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
}

int swh_match_aspect4(double pos0, double speed0, double pos1, double speed1,
    double aspect, double app_orb, double sep_orb, double def_orb,
    double *diffret, int *applic, double *factor)
{
    app_orb = fabs(app_orb);
    sep_orb = fabs(sep_orb);
    def_orb = fabs(def_orb);
    if (speed0 == speed1)
    { /* stable aspect */
        return swh_match_aspect2(pos0, speed0, pos1, speed1, aspect,
            def_orb, diffret, applic, factor);
    }
    if (app_orb == sep_orb)
    {
        return swh_match_aspect2(pos0, speed0, pos1, speed1, aspect, app_orb,
            diffret, applic, factor);
    }
    if (app_orb > sep_orb)
    {
        int i = swh_match_aspect2(pos0, speed0, pos1, speed1, aspect, app_orb,
            diffret, applic, factor);
        if (i == 0)
        {
            if (*applic == -1)
            {
                return 0;
            }
            else if (*diffret <= sep_orb)
            {
                *factor = *diffret / sep_orb;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        int i = swh_match_aspect2(pos0, speed0, pos1, speed1, aspect, sep_orb,
            diffret, applic, factor);
        if (i == 0)
        {
            if (*applic == 1)
            {
                return 0;
            }
            else if (*diffret <= app_orb)
            {
                *factor = *diffret / app_orb;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
