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

#include <assert.h>
#include <math.h>
#include <swephexp.h>

#define STEP    (0.5)

int swh_secsearch(
    double t1,
    int (*f)(double t, void* args, double* ret, char* err),
    void* fargs,
    double step,
    double stop,
    double* ret,
    char* err)
{
    const double tstart = t1;
    double tstop = 0;
    double t2 = 0;
    double f1 = 0;
    double f2 = 0;

    unsigned int i = 0;
    int x = 0;

    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;

    assert(f);
    assert(step);
    assert(ret);
    assert(err);

    if (stop)
        tstop = step > 0 ? t1 + fabs(stop) : t1 - fabs(stop);

    x = (*f)(t1, fargs, &f1, err);
    if (x)
        return 1;

    while (f1 * f2 >= 0
        || fabs(f2) > 90) /* trick swe_difdeg2n jumps */
    {
        t2 = t1;
        f2 = f1;
        t1 = tstart + (++i * step);

        if (stop && i > 1) {
            if (step > 0) {
                if (t1 > tstop)
                    return 2;
            } else {
                if (t1 < tstop)
                    return 2;
            }
        }

        x = (*f)(t1, fargs, &f1, err);
        if (x)
            return 1;
    }

    a = f1 - f2;
    b = t1 - t2;
    c = 0 - f2;
    d = (b * c) / a;

    *ret = d + t2;
    return 0;
}

typedef struct
{
    int planet;
    int flags;
} swh_next_retro_args_t;

int _swh_next_retro(double t, void* fargs, double* ret, char* err)
{
    const swh_next_retro_args_t* args = fargs;
    double res[6] = {0,0,0,0,0,0};

    int x = swe_calc_ut(t, args->planet, args->flags, res, err);
    if (x < 0)
        return x;
    *ret = res[3];
    return 0;
}

int swh_next_retro(
    int planet,
    double jdstart,
    int backw,
    double stop,
    int flags,
    double* jdret,
    double* posret,
    char* err)
{
    swh_next_retro_args_t args = {planet, flags};

    int x = swh_secsearch(jdstart, &_swh_next_retro, &args,
                          backw ? -STEP : STEP, stop, jdret, err);
    if (!x && posret) {
        int i = swe_calc_ut(*jdret, planet, flags, posret, err);
        if (i < 0)
            return 1;
    }
    return x;
}

typedef struct
{
    int planet;
    double aspect;
    double fixedpt;
    int flags;
} swh_next_aspect_args_t;

int _swh_next_aspect(double t, void* fargs, double* ret, char* err)
{
    const swh_next_aspect_args_t* args = fargs;
    double res[6] = {0,0,0,0,0,0};

    int x = swe_calc_ut(t, args->planet, args->flags, res, err);
    if (x < 0)
        return x;
    *ret = swe_difdeg2n(res[0] + args->aspect, args->fixedpt);
    return 0;
}

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
    char* err)
{
    swh_next_aspect_args_t args = {planet, swe_degnorm(aspect),
                                   swe_degnorm(fixedpt), flags};

    int x = swh_secsearch(jdstart, &_swh_next_aspect, &args,
                          backw ? -STEP: STEP, stop, jdret, err);
    if (!x && posret) {
        int i = swe_calc_ut(*jdret, planet, flags, posret, err);
        if (i < 0)
            return 1;
    }
    return x;
}

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
    char *err)
{
    int x1 = 0, x2 = 0;
    double jd1 = 0, jd2 = 0;
    const double aspnorm = swe_difdeg2n(aspect, 0);
    swh_next_aspect_args_t args = {planet, aspnorm,
                                   swe_degnorm(fixedpt), flags};

    x1 = swh_secsearch(jdstart, &_swh_next_aspect, &args,
                       backw ? -STEP : STEP, stop, &jd1, err);
    if (x1 == 1)
        return 1;
    if (aspnorm == 0 || aspnorm == -180) {
        if (!x1) {
            *jdret = jd1;
            if (posret) {
                int i = swe_calc_ut(jd1, planet, flags, posret, err);
                if (i < 0)
                    return 1;
            }
            return 0;
        }
        return 2;
    }
    args.aspect = swe_difdeg2n(0, aspect);
    x2 = swh_secsearch(jdstart, &_swh_next_aspect, &args,
                       backw ? -STEP : STEP, stop, &jd2, err);
    if (x2 == 1)
        return 1;
    if (x1 == 2 && x2 == 2)
        return 2;
    if (!x1 && !x2) {
        if (!backw)
            *jdret = jd1 < jd2 ? jd1 : jd2;
        else
            *jdret = jd1 > jd2 ? jd1 : jd2;
    }
    else
        *jdret = !x1 ? jd1 : jd2;
    if (posret) {
        x1 = swe_calc_ut(*jdret, planet, flags, posret, err);
        if (x1 < 0)
            return 1;
    }
    return 0;
}

typedef struct
{
    int planet;
    double aspect;
    int other;
    char* star;
    int flags;
    char* starbuf;
} swh_next_aspect_with_args_t;

int _swh_next_aspect_with(double t, void* fargs, double* ret, char* err)
{
    int x1 = 0, x2 = 0;
    swh_next_aspect_with_args_t* args = fargs;
    double res1[6] = {0,0,0,0,0,0};
    double res2[6] = {0,0,0,0,0,0};

    x1 = swe_calc_ut(t, args->planet, args->flags, res1, err);
    if (x1 < 0)
        return x1;
    if (args->star) {
        if (!args->starbuf) {
            args->starbuf = malloc((SE_MAX_STNAME*2)+1);
            if (!args->starbuf) {
                sprintf(err, "nomem");
                return 1;
            }
            memset(args->starbuf, 0, (SE_MAX_STNAME*2)+1);
            strncpy(args->starbuf, args->star, SE_MAX_STNAME*2);
        }
        x2 = swe_fixstar2_ut(args->starbuf, t, args->flags, res2, err);
    }
    else {
        x2 = swe_calc_ut(t, args->other, args->flags, res2, err);
    }
    if (x2 < 0)
        return x2;
    *ret = swe_difdeg2n(res1[0] + args->aspect, res2[0]);
    return 0;
}

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
    char* err)
{
    swh_next_aspect_with_args_t args = {planet, swe_degnorm(aspect),
                                        other, star, flags, NULL};

    int x = swh_secsearch(jdstart, &_swh_next_aspect_with, &args,
                          backw ? -STEP : STEP, stop, jdret, err);
    if (x) {
        if (args.starbuf)
            free(args.starbuf);
        return x;
    }
    if (posret1) {
        x = swe_calc_ut(*jdret, planet, flags, posret1, err);
        if (x < 0) {
            if (args.starbuf)
                free(args.starbuf);
            return 1;
        }
    }
    if (posret2) {
        if (star) {
            assert(args.starbuf);
            x = swe_fixstar2_ut(args.starbuf, *jdret, flags, posret2, err);
            if (x < 0) {
                free(args.starbuf);
                return 1;
            }
        }
        else {
            assert(!args.starbuf);
            x = swe_calc_ut(*jdret, other, flags, posret2, err);
            if (x < 0)
                return 1;
        }
    }
    if (args.starbuf)
        free(args.starbuf);
    return 0;
}

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
    char* err)
{
    int x1 = 0, x2 = 0;
    double jd1 = 0, jd2 = 0;
    const double aspnorm = swe_difdeg2n(aspect, 0);
    swh_next_aspect_with_args_t args = {planet, aspnorm, other,
                                        star, flags, NULL};

    x1 = swh_secsearch(jdstart, &_swh_next_aspect_with, &args,
                       backw ? -STEP : STEP, stop, &jd1, err);
    if (x1 == 1) {
        if (args.starbuf)
            free(args.starbuf);
        return 1;
    }
    if (aspnorm == 0 || aspnorm == -180) {
        if (x1) {
            assert(x1 == 2);
            if (args.starbuf)
                free(args.starbuf);
            return 2;
        }
        *jdret = jd1;
        if (posret1) {
            x1 = swe_calc_ut(jd1, planet, flags, posret1, err);
            if (x1 < 0) {
                if (args.starbuf)
                    free(args.starbuf);
                return 1;
            }
        }
        if (posret2) {
            if (star) {
                assert(args.starbuf);
                x1 = swe_fixstar2_ut(args.starbuf, jd1, flags, posret2, err);
                if (x1 < 0) {
                    free(args.starbuf);
                    return 1;
                }
            }
            else {
                assert(!args.starbuf);
                x1 = swe_calc_ut(jd1, other, flags, posret2, err);
                if (x1 < 0)
                    return 1;
            }
        }
        if (args.starbuf)
            free(args.starbuf);
        return 0;
    }
    args.aspect = swe_difdeg2n(0, aspect);
    x2 = swh_secsearch(jdstart, &_swh_next_aspect_with, &args,
                       backw ? -STEP : STEP, stop, &jd2, err);
    if (x2 == 1) {
        if (args.starbuf)
            free(args.starbuf);
        return 1;
    }
    if (x1 == 2 && x2 == 2) {
        if (args.starbuf)
            free(args.starbuf);
        return 2;
    }
    if (!x1 && !x2) {
        if (!backw)
            *jdret = jd1 < jd2 ? jd1 : jd2;
        else
            *jdret = jd1 > jd2 ? jd1 : jd2;
    }
    else
        *jdret = !x1 ? jd1 : jd2;
    if (posret1) {
        x1 = swe_calc_ut(*jdret, planet, flags, posret1, err);
        if (x1 < 0) {
            if (args.starbuf)
                free(args.starbuf);
            return 1;
        }
    }
    if (posret2) {
        if (star) {
            assert(args.starbuf);
            x1 = swe_fixstar2_ut(args.starbuf, jd1, flags, posret2, err);
            if (x1 < 0) {
                free(args.starbuf);
                return 1;
            }
        }
        else {
            assert(!args.starbuf);
            x1 = swe_calc_ut(jd1, other, flags, posret2, err);
            if (x1 < 0)
                return 1;
        }
    }
    if (args.starbuf)
        free(args.starbuf);
    return 0;
}

typedef struct
{
    int planet;
    char* star;
    double aspect;
    int cusp;
    double lat;
    double lon;
    int hsys;
    int flags;
    char* starbuf;
} swh_next_aspect_cusp_args_t;

int _swh_next_aspect_cusp(double t, void* fargs, double* ret, char* err)
{
    int x = 0;
    swh_next_aspect_cusp_args_t* args = fargs;
    double res1[6] = {0,0,0,0,0,0};
    double res2[37] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,0,0};
    double ascmc[10] = {0,0,0,0,0,0,0,0,0,0};

    if (args->star) {
        if (!args->starbuf) {
            args->starbuf = malloc((SE_MAX_STNAME*2)+1);
            if (!args->starbuf) {
                sprintf(err, "nomem");
                return 1;
            }
            memset(args->starbuf, 0, (SE_MAX_STNAME*2)+1);
            strncpy(args->starbuf, args->star, SE_MAX_STNAME*2);
        }
        x = swe_fixstar2_ut(args->starbuf, t, args->flags, res1, err);
    }
    else {
        x = swe_calc_ut(t, args->planet, args->flags, res1, err);
    }
    if (x < 0)
        return x;
    x = swe_houses_ex(t, args->flags, args->lat, args->lon,
                      args->hsys, res2, ascmc);
    if (x < 0)
        return x;
    *ret = swe_difdeg2n(res1[0] + args->aspect, res2[args->cusp]);
    return 0;
}

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
    char* err)
{
    int x = 0;
    swh_next_aspect_cusp_args_t args = {planet, star, swe_degnorm(aspect),
                                        cusp, lat, lon, hsys, flags, NULL};

    if (cusp < 1 || cusp > (hsys == 71 ? 36 : 12)) {
        assert(err);
        sprintf(err, "invalid cusp (%d)", cusp);
        return 1;
    }
    x = swh_secsearch(jdstart, &_swh_next_aspect_cusp, &args,
                      backw ? -0.05 : 0.05, 0, jdret, err);
    if (x) {
        if (args.starbuf)
            free(args.starbuf);
        return x;
    }
    if (posret) {
        if (star) {
            assert(args.starbuf);
            x = swe_fixstar2_ut(args.starbuf, *jdret, flags, posret, err);
            if (x < 0) {
                free(args.starbuf);
                return 1;
            }
        }
        else {
            assert(!args.starbuf);
            x = swe_calc_ut(*jdret, planet, flags, posret, err);
            if (x < 0) {
                return 1;
            }
        }
    }
    if (cuspsret && ascmcret) {
        x = swe_houses_ex(*jdret, flags, lat, lon, hsys, cuspsret, ascmcret);
        if (x < 0) {
            if (args.starbuf)
                free(args.starbuf);
            return 1;
        }
    }
    if (args.starbuf)
        free(args.starbuf);
    return 0;
}

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
    char* err)
{
    int x1 = 0, x2 = 0;
    double jd1 = 0, jd2 = 0;
    const double aspnorm = swe_difdeg2n(aspect, 0);
    swh_next_aspect_cusp_args_t args = {planet, star, aspnorm, cusp,
                                        lat, lon, hsys, flags, NULL};

    if (cusp < 1 || cusp > (hsys == 71 ? 36 : 12)) {
        assert(err);
        sprintf(err, "invalid cusp (%d)", cusp);
        return 1;
    }
    x1 = swh_secsearch(jdstart, &_swh_next_aspect_cusp, &args,
                       backw ? -0.05 : 0.05, 0, &jd1, err);
    if (x1 == 1) {
        if (args.starbuf)
            free(args.starbuf);
        return 1;
    }
    if (aspnorm == 0 || aspnorm == -180) {
        if (x1 == 2) {
            if (args.starbuf)
                free(args.starbuf);
            return 2;
        }
        assert(!x1);
        *jdret = jd1;
        if (posret) {
            if (star) {
                assert(args.starbuf);
                x1 = swe_fixstar2_ut(args.starbuf, jd1, flags, posret, err);
                if (x1 < 0) {
                    free(args.starbuf);
                    return 1;
                }
            }
            else {
                assert(!args.starbuf);
                x1 = swe_calc_ut(jd1, planet, flags, posret, err);
                if (x1 < 0)
                    return 1;
            }
        }
        if (cuspsret && ascmcret) {
            x1 = swe_houses_ex(jd1, flags, lat, lon, hsys, cuspsret, ascmcret);
            if (x1 < 0) {
                if (args.starbuf)
                    free(args.starbuf);
                return 1;
            }
        }
        if (args.starbuf)
            free(args.starbuf);
        return 0;
    }
    args.aspect = swe_difdeg2n(0, aspect);
    x2 = swh_secsearch(jdstart, &_swh_next_aspect_cusp, &args,
                       backw ? -0.05 : 0.05, 0, &jd2, err);
    if (x2 == 1) {
        if (args.starbuf)
            free(args.starbuf);
        return 1;
    }
    if (x1 == 2 && x2 == 2) {
        if (args.starbuf)
            free(args.starbuf);
        return 2;
    }
    if (!x1 && !x2) {
        if (!backw)
            *jdret = jd1 < jd2 ? jd1 : jd2;
        else
            *jdret = jd1 > jd2 ? jd1 : jd2;
    }
    else {
        *jdret = !x1 ? jd1 : jd2;
    }
    if (posret) {
        if (star) {
            assert(args.starbuf);
            x1 = swe_fixstar2_ut(args.starbuf, *jdret, flags, posret, err);
            if (x1 < 0) {
                free(args.starbuf);
                return 1;
            }
        }
        else {
            assert(!args.starbuf);
            x1 = swe_calc_ut(*jdret, planet, flags, posret, err);
            if (x1 < 0)
                return 1;
        }
    }
    if (cuspsret && ascmcret) {
        x1 = swe_houses_ex(*jdret, flags, lat, lon, hsys, cuspsret, ascmcret);
        if (x1 < 0) {
            if (args.starbuf)
                free(args.starbuf);
            return 1;
        }
    }
    if (args.starbuf)
        free(args.starbuf);
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
    double corr = 2 * ((1.0/86400)/2);
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
            res = swh_next_aspect(SE_SUN, 0, pos1[0], jd1+corr, 0, 0,
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
            res = swh_next_aspect(SE_SUN, 0, pos1[0], jd1-corr, 1, 0,
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
