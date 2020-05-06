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

#include "swhsearch.h"

#include <swephexp.h>

/* @brief Half a second of time (expressed in day)
 * Used to set the precision of search functions.
 */
static double swh_precision = ((1.0/86400)/2);

#define ISPOSITIVE(x)   ((x) > 0 ? 1 : 0)

int swh_next_retro(int planet, double jdstart, double step, int backw,
    double dayspan, int flag, double *jdret, double *posret, char *err)
{
    int res, direct[3];
    double jdstop = 0; /* dummy assign */
    direct[2] = 0; /* dummy assign */
    /* set limit */
    dayspan = fabs(dayspan);
    if (dayspan)
    {
        jdstop = (backw) ? jdstart-dayspan : jdstart+dayspan;
        direct[2] = (backw) ? 0 : 1;
    }
    /* find step if 0 */
    if (step == 0)
    {
        step = swh_min_retro_time(planet, err);
        if (step < 0) { return -1; } /* never retro */
    } else { step = fabs(step); }
    /* find current direction */
    res = swe_calc_ut(jdstart, planet, flag, posret, err);
    if (res < 0) { return -1; }
    direct[1] = direct[0] = (posret[3] > 0) ? 1 : 0;
    /* step till direction changes */
    *jdret = jdstart;
    while (step > swh_precision)
    {
        *jdret = (backw) ? *jdret-step : *jdret+step;
        res = swe_calc_ut(*jdret, planet, flag, posret, err);
        if (res < 0) { return -1; }
        direct[0] = (posret[3] > 0) ? 1 : 0;
        if (direct[0] != direct[1]) /* invert step */
        {
            if (dayspan && backw == direct[2])
            {
                if (direct[2]) { if (*jdret > jdstop) return 1; }
                else { if (*jdret < jdstop) return 1; }
            }
            step = step/2;
            backw = (backw) ? 0 : 1;
            direct[1] = direct[0];
        }
        else if (dayspan && backw != direct[2])
        {
            if (direct[2]) { if (*jdret > jdstop) return 1; }
            else { if (*jdret < jdstop) return 1; }
        }
    }
    return 0;
}

int swh_next_aspect(int planet, double aspect, double fixedpt, double jdstart,
    double step, int backw, double dayspan, int flag, double *jdret,
    double *posret, char *err)
{
    int res, direct = 0; /* dummy assign */
    double jdstop = 0; /* dummy assign */
    dayspan = fabs(dayspan);
    if (dayspan)
    {
        jdstop = (backw) ? jdstart-dayspan : jdstart+dayspan;
        direct = (backw) ? 0 : 1;
    }
    if (step == 0)
    {
        step = swh_min_retro_time(planet, err);
        if (step < 0) { step = 10; } /* default step */
    } else { step = fabs(step); }
    fixedpt = swe_degnorm(fixedpt + aspect);
    *jdret = jdstart;
    while (step > swh_precision)
    {
        res = swh_go_past(planet, fixedpt, *jdret, step, backw, flag,
            jdret, posret, err);
        if (res < 0) { return -1; }
        if (dayspan && backw == direct)
        {
            if (direct) { if (*jdret > jdstop) return 1; }
            else { if (*jdret < jdstop) return 1; }
        }
        backw = (backw) ? 0 : 1;
        step = step/2;
    }
    return 0;
}

int swh_next_aspect2(int planet, double aspect, double fixedpt, double jdstart,
    double step, int backw, double dayspan, int flag, double *jdret,
    double *posret, char *err)
{
    int res, res2;
    double jdtmp, postmp[6];
    aspect = fabs(swe_difdeg2n(0, aspect));
    if (aspect == 0 || aspect == 180)
    {
        return swh_next_aspect(planet, aspect, fixedpt, jdstart, step, backw,
            dayspan, flag, jdret, posret, err);
    }
    /* else try both */
    res = swh_next_aspect(planet, aspect, fixedpt, jdstart, step, backw,
        dayspan, flag, jdret, posret, err);
    if (res < 0) { return -1; }
    res2 = swh_next_aspect(planet, 0-aspect, fixedpt, jdstart, step, backw,
        dayspan, flag, &jdtmp, postmp, err);
    if (res2 < 0) { return -1; }
    if (res == 1 && res2 == 1) { return 1; } /* both reached time limit */
    else if (res != res2) /* one reached limit */
    {
        if (res2 == 0)
        {
            *jdret = jdtmp;
            for (res = 0; res < 6; res++) { posret[res] = postmp[res]; }
        }
    }
    else /* both found something */
    {
        if (backw)
        {
            if (jdtmp > *jdret)
            {
                *jdret = jdtmp;
                for (res = 0; res < 6; res++) { posret[res] = postmp[res]; }
            }
        }
        else
        {
            if (jdtmp < *jdret)
            {
                *jdret = jdtmp;
                for (res = 0; res < 6; res++) { posret[res] = postmp[res]; }
            }
        }
    }
    return 0;
}

int swh_next_aspect_with(int planet, double aspect, int other, char *star,
    double jdstart, double step, int backw, double dayspan, int flag,
    double *jdret, double *posret0, double *posret1, char *err)
{
    int res, starflag, direct = 0; /* dummy assign */
    double jdstop = 0, diff[4]; /* dummy assign */
    char starbuf[41];
    aspect = swe_degnorm(aspect);
    step = fabs(step);
    if (step == 0) { step = 10; }
    dayspan = fabs(dayspan);
    if (dayspan)
    {
        jdstop = (backw) ? jdstart-dayspan : jdstart+dayspan;
        direct = (backw) ? 0 : 1;
    }
    /* get planets positions */
    res = swe_calc_ut(jdstart, planet, flag, posret0, err);
    if (res < 0) { return -1; }
    if (strcmp("", star) != 0)
    {
        strcpy(starbuf, star); /* only first time */
        res = swe_fixstar_ut(starbuf, jdstart, flag, posret1, err);
        if (res < 0) { return -1; }
        starflag = 1;
    }
    else
    {
        res = swe_calc_ut(jdstart, other, flag, posret1, err);
        if (res < 0) { return -1; }
        starflag = 0;
    }
    /* compare */
    diff[1] = swe_difdeg2n(posret0[0], posret1[0] + aspect);
    diff[3] = swe_difdegn(posret0[0], posret1[0] + aspect);
    *jdret = jdstart;
    while (step > swh_precision)
    {
        *jdret = (backw) ? *jdret-step : *jdret+step;
        /* get planets positions */
        res = swe_calc_ut(*jdret, planet, flag, posret0, err);
        if (res < 0) { return -1; }
        if (starflag)
        {
            res = swe_fixstar_ut(starbuf, *jdret, flag, posret1, err);
            if (res < 0) { return -1; }
        }
        else
        {
            res = swe_calc_ut(*jdret, other, flag, posret1, err);
            if (res < 0) { return -1; }
        }
        /* compare */
        diff[0] = swe_difdeg2n(posret0[0], posret1[0] + aspect);
        diff[2] = swe_difdegn(posret0[0], posret1[0] + aspect);
        if (ISPOSITIVE(diff[1]) != ISPOSITIVE(diff[0])
            && fabs(diff[3]-diff[2]) > 180) /* invert step */
        {
            if (dayspan && backw == direct)
            {
                if (direct) { if (*jdret > jdstop) return 1; }
                else { if (*jdret < jdstop) return 1; }
            }
            backw = (backw) ? 0 : 1;
            step = step/2;
        }
        else if (dayspan && backw != direct)
        {
            if (direct) { if (*jdret > jdstop) return 1; }
            else { if (*jdret < jdstop) return 1; }
        }
        diff[1] = diff[0];
        diff[3] = diff[2];
    }
    return 0;
}

int swh_next_aspect_with2(int planet, double aspect, int other, char *star,
    double jdstart, double step, int backw, double dayspan, int flag,
    double *jdret, double *posret0, double *posret1, char *err)
{
    int res, res2;
    double jdtmp, postmp0[6], postmp1[6];
    aspect = fabs(swe_difdeg2n(0, aspect));
    if (aspect == 0 || aspect == 180)
    {
        return swh_next_aspect_with(planet, aspect, other, star, jdstart, step,
            backw, dayspan, flag, jdret, posret0, posret1, err);
    }
    /* else try both */
    res = swh_next_aspect_with(planet, aspect, other, star, jdstart, step,
        backw, dayspan, flag, jdret, posret0, posret1, err);
    if (res < 0) { return -1; }
    res2 = swh_next_aspect_with(planet, 0-aspect, other, star, jdstart, step,
        backw, dayspan, flag, &jdtmp, postmp0, postmp1, err);
    if (res2 < 0) { return -1; }
    if (res == 1 && res2 == 1) { return 1; } /* both reached time limit */
    else if (res != res2) /* one reached limit */
    {
        if (res2 == 0)
        {
            *jdret = jdtmp;
            for (res = 0; res < 6; res++) { posret0[res] = postmp0[res]; }
            for (res = 0; res < 6; res++) { posret1[res] = postmp1[res]; }
        }
    }
    else /* both found something */
    {
        if (backw)
        {
            if (jdtmp > *jdret)
            {
                *jdret = jdtmp;
                for (res = 0; res < 6; res++) { posret0[res] = postmp0[res]; }
                for (res = 0; res < 6; res++) { posret1[res] = postmp1[res]; }
            }
        }
        else
        {
            if (jdtmp < *jdret)
            {
                *jdret = jdtmp;
                for (res = 0; res < 6; res++) { posret0[res] = postmp0[res]; }
                for (res = 0; res < 6; res++) { posret1[res] = postmp1[res]; }
            }
        }
    }
    return 0;
}

int swh_next_aspect_cusp(int planet, char *star, double aspect, int cusp,
    double jdstart, double lat, double lon, int hsys, double step, int backw,
    int flag, double *jdret, double *posret, double *cuspsret, double *ascmcret,
    char *err)
{
    int res, starflag;
    double diff[4];
    char starbuf[41];
    aspect = swe_degnorm(aspect);
    step = fabs(step);
    if (step == 0) { step = 0.2; }
    /* get start position */
    if (strcmp("", star) != 0)
    {
        strcpy(starbuf, star); /* only first time */
        res = swe_fixstar_ut(starbuf, jdstart, flag, posret, err);
        if (res < 0) { return -1; }
        starflag = 1;
    }
    else
    {
        res = swe_calc_ut(jdstart, planet, flag, posret, err);
        if (res < 0) { return -1; }
        starflag = 0;
    }
    res = swe_houses_ex(jdstart, flag, lat, lon, hsys, cuspsret, ascmcret);
    if (res < 0) { return -1; }
    /* compare */
    diff[1] = swe_difdeg2n(posret[0], cuspsret[cusp] + aspect);
    diff[3] = swe_difdegn(posret[0], cuspsret[cusp] + aspect);
    *jdret = jdstart;
    while (step > swh_precision)
    {
        *jdret = (backw) ? *jdret-step : *jdret+step;
        /* get positions */
        if (starflag)
        {
            res = swe_fixstar_ut(starbuf, *jdret, flag, posret, err);
            if (res < 0) { return -1; }
        }
        else
        {
            res = swe_calc_ut(*jdret, planet, flag, posret, err);
            if (res < 0) { return -1; }
        }
        res = swe_houses_ex(*jdret, flag, lat, lon, hsys, cuspsret, ascmcret);
        if (res < 0) { return -1; }
        /* compare */
        diff[0] = swe_difdeg2n(posret[0], cuspsret[cusp] + aspect);
        diff[2] = swe_difdegn(posret[0], cuspsret[cusp] + aspect);
        if (ISPOSITIVE(diff[1]) != ISPOSITIVE(diff[0])
            && fabs(diff[3]-diff[2]) > 180)
        {
            backw = (backw) ? 0 : 1;
            step = step/2;
        }
        diff[1] = diff[0];
        diff[3] = diff[2];
    }
    return 0;
}

int swh_next_aspect_cusp2(int planet, char *star, double aspect, int cusp,
    double jdstart, double lat, double lon, int hsys, double step, int backw,
    int flag, double *jdret, double *posret, double *cuspsret, double *ascmcret,
    char *err)
{
    int res, res2;
    double jdtmp, postmp[6], cuspstmp[37], ascmctmp[10];
    aspect = fabs(swe_difdeg2n(0, aspect));
    if (aspect == 0 || aspect == 180)
    {
        return swh_next_aspect_cusp(planet, star, aspect, cusp, jdstart, lat,
            lon, hsys, step, backw, flag, jdret, posret, cuspsret, ascmcret,
            err);
    }
    /* else try both */
    res = swh_next_aspect_cusp(planet, star, aspect, cusp, jdstart, lat,
            lon, hsys, step, backw, flag, jdret, posret, cuspsret, ascmcret,
            err);
    if (res < 0) { return -1; }
    res2 = swh_next_aspect_cusp(planet, star, aspect, cusp, jdstart, lat,
            lon, hsys, step, backw, flag, &jdtmp, postmp, cuspstmp, ascmctmp,
            err);
    if (res2 < 0) { return -1; }
    if (backw)
    {
        if (jdtmp > *jdret)
        {
            *jdret = jdtmp;
            for (res = 0; res < 6; res++) { posret[res] = postmp[res]; }
            for (res = 0; res < 37; res++) { cuspsret[res] = cuspstmp[res]; }
            for (res = 0; res < 10; res++) { ascmcret[res] = ascmctmp[res]; }
        }
    }
    else
    {
        if (jdtmp < *jdret)
        {
            *jdret = jdtmp;
            for (res = 0; res < 6; res++) { posret[res] = postmp[res]; }
            for (res = 0; res < 37; res++) { cuspsret[res] = cuspstmp[res]; }
            for (res = 0; res < 10; res++) { ascmcret[res] = ascmctmp[res]; }
        }
    }
    return 0;
}

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

int swh_years_diff(double jd1, double jd2, int flag, double *years, char *err)
{
    double pos1[6], pos2[6], dec;
    double corr = 2 * swh_precision;
    int res;
    res = swe_calc_ut(jd1, SE_SUN, flag, pos1, err);
    if (res < 0) { return -1; }
    res = swe_calc_ut(jd2, SE_SUN, flag, pos2, err);
    if (res < 0) { return -1; }
    *years = 0;
    if (jd1 < jd2) /* forward search */
    {
        dec = swe_difdegn(pos2[0], pos1[0]) / 360.0;
        while (1)
        {
            res = swh_next_aspect(SE_SUN, 0, pos1[0], jd1+corr, 120, 0, 0,
                flag, &jd1, pos2, err);
            if (res < 0) { return -1; }
            if (jd1+corr < jd2) { *years += 1; }
            else { break; }
        }
        *years += dec;
    }
    else if (jd1 > jd2) /* backward search */
    {
        dec = swe_difdegn(pos1[0], pos2[0]) / 360.0;
        while (1)
        {
            res = swh_next_aspect(SE_SUN, 0, pos1[0], jd1-corr, 120, 1, 0,
                flag, &jd1, pos2, err);
            if (res < 0) { return -1; }
            if (jd1-corr > jd2) { *years -= 1; }
            else { break; }
        }
        *years -= dec;
    }
    /* else equal julian days */
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
