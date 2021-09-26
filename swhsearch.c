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

#include "swhsearch.h"

#define STEP    (0.5)
#define INCH    ((1.0/(24*60*60))*5)
#define PRECISE INCH

double swh_approx_retrotime(int pl)
{
    switch (pl) {
    case SE_SUN: return 0;
    case SE_MOON: return 0;
    case SE_MERCURY: return 18;
    case SE_VENUS: return 38;
    case SE_MARS: return 57;
    case SE_JUPITER: return 115;
    case SE_SATURN: return 130;
    case SE_URANUS: return 147;
    case SE_NEPTUNE: return 154;
    case SE_PLUTO: return 153;
    /*case SE_MEAN_NODE */
    /*case SE_TRUE_NODE */
    /*case SE_MEAN_APOG */
    /*case SE_OSCU_APOG */
    case SE_EARTH: return 0;
    case SE_CHIRON: return 124; /* 15 */
    case SE_PHOLUS: return 125; /* 16 */
    case SE_CERES: return 85; /* 17 */
    case SE_PALLAS: return 47; /* 18 */
    case SE_JUNO: return 68; /* 19 */
    case SE_VESTA: return 80; /* 20 */
    /*case SE_INTP_APOG */
    /*case SE_INTP_PERG */
    /* todo: asteroids? */
    default: return STEP;
    }
}

int swh_secsearch(
    double t1,
    int (*f)(double t, void* args, double* ret, char* err),
    void* fargs,
    double step,
    int (*nextep)(double step, void* args, double* t, char* err),
    double stop,
    int recursive,
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

        if (nextep) {
            ++i;
            x = (*nextep)(step, fargs, &t1, err);
            if (x)
                return 1;
        }
        else
            t1 = tstart + (++i * step);

        if (stop) {
            if (i > 1 && t2 == tstop)
                return 2;
            if (step > 0 ? t1 > tstop : t1 < tstop)
                t1 = tstop;
        }

        x = (*f)(t1, fargs, &f1, err);
        if (x)
            return 1;
    }

    if (recursive && fabs(step) > PRECISE)
        return swh_secsearch(t1, f, fargs, -step/2, NULL, 0, 1, ret, err);
    else {
        double a = f1 - f2;
        double b = t1 - t2;
        double c = 0 - f2;
        double d = (b * c) / a;
        *ret = d + t2;
        return 0;
    }
}

typedef struct
{
    int planet;
    double jdstart;
    int backw;
    double stop;
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

int swh_next_retro_possible(int planet, int flags)
{
    if (flags & SEFLG_HELCTR)
        return 0;
    switch (planet) {
    case SE_SUN:
    case SE_MOON:
    case SE_EARTH:
    /* others? */
        return 0;
    }
    return 1;
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
    int x;
    const double step = swh_approx_retrotime(planet) || STEP;
    swh_next_retro_args_t args = {planet, jdstart, backw, stop, flags};

    if (!swh_next_retro_possible(planet, flags)) {
        sprintf(err, "invalid argument");
        return 3;
    }

    x = swh_secsearch(jdstart, &_swh_next_retro, &args,
                      backw ? -step : step, NULL, stop, 1, jdret, err);
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
    double jdstart;
    int backw;
    double stop;
    int flags;
    int iretro;
    double tretro;
} swh_next_aspect_args_t;

int _swh_next_aspect_step(double step, void* fargs, double* t, char* err)
{
    swh_next_aspect_args_t* args = fargs;
    double next = 0;

    switch (args->iretro) {
    case -1:
        *t += step;
        return 0;
    case 1:
        if (*t != args->tretro)
            break;
        *t += step > 0 ? INCH : -INCH;
    case 0: {
        const double tstop = !args->stop ? 0 : args->backw ?
            args->jdstart - fabs(args->stop) : args->jdstart + fabs(args->stop);
        const double daysleft = !args->stop ? 0 :
            args->backw ? tstop + *t : tstop - *t;
        const int x = swh_next_retro(args->planet, *t, step < 0 ? 1 : 0,
                                     daysleft, args->flags, &args->tretro,
                                     NULL, err);
        switch (x) {
        case 1:
            return 1;
        case 2:
        case 3:
            args->iretro = -1;
            *t += step;
            return 0;
        default:
            assert(!x);
            args->iretro = 1;
        }
        break;
    }
    default:
        assert(0);
    }
    next = *t + step;
    if (step > 0 ? next > args->tretro : next < args->tretro)
        *t = args->tretro;
    else
        *t = next;
    return 0;
}

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
                                   swe_degnorm(fixedpt), jdstart,
                                   backw, stop, flags,
                                   0, 0};
    const double step = swh_approx_retrotime(planet) || STEP;
    int x = swh_secsearch(jdstart, &_swh_next_aspect, &args,
                          backw ? -step: step, &_swh_next_aspect_step,
                          stop, 1, jdret, err);
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
    char* err)
{
    int x1 = 0, x2 = 0;
    double jd1 = 0, jd2 = 0;
    const double aspnorm = swe_difdeg2n(aspect, 0);
    const double step = swh_approx_retrotime(planet) || STEP;
    swh_next_aspect_args_t args = {planet, aspnorm,
                                   swe_degnorm(fixedpt), jdstart,
                                   backw, stop, flags,
                                   0, 0};

    x1 = swh_secsearch(jdstart, &_swh_next_aspect, &args,
                       backw ? -step : step, &_swh_next_aspect_step,
                       stop, 1, &jd1, err);
    if (x1 == 1)
        return 1;
    if (aspnorm == 0 || aspnorm == -180) {
        if (!x1) {
            *jdret = jd1;
            if (posret) {
                x1 = swe_calc_ut(jd1, planet, flags, posret, err);
                if (x1 < 0)
                    return 1;
            }
            return 0;
        }
        return 2;
    }
    args.aspect = swe_difdeg2n(0, aspect);
    args.iretro = 0;
    args.tretro = 0;
    x2 = swh_secsearch(jdstart, &_swh_next_aspect, &args,
                       backw ? -step : step, &_swh_next_aspect_step,
                       stop, 1, &jd2, err);
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
    int x = 0;
    swh_next_aspect_with_args_t* args = fargs;
    double res1[6] = {0,0,0,0,0,0};
    double res2[6] = {0,0,0,0,0,0};

    x = swe_calc_ut(t, args->planet, args->flags, res1, err);
    if (x < 0)
        return x;
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
        x = swe_fixstar2_ut(args->starbuf, t, args->flags, res2, err);
    }
    else
        x = swe_calc_ut(t, args->other, args->flags, res2, err);
    if (x < 0)
        return x;
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
                          backw ? -STEP : STEP, NULL, stop, 1, jdret, err);
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
                       backw ? -STEP : STEP, NULL, stop, 1, &jd1, err);
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
                       backw ? -STEP : STEP, NULL, stop, 1, &jd2, err);
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
            x1 = swe_fixstar2_ut(args.starbuf, *jdret, flags, posret2, err);
            if (x1 < 0) {
                free(args.starbuf);
                return 1;
            }
        }
        else {
            assert(!args.starbuf);
            x1 = swe_calc_ut(*jdret, other, flags, posret2, err);
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
    else
        x = swe_calc_ut(t, args->planet, args->flags, res1, err);
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
                      backw ? -0.05 : 0.05, NULL, 0, 1, jdret, err);
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
                       backw ? -0.05 : 0.05, NULL, 0, 1, &jd1, err);
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
                       backw ? -0.05 : 0.05, NULL, 0, 1, &jd2, err);
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

int swh_years_diff(
    double jd1,
    double jd2,
    int flags,
    double* years,
    char* err)
{
    double pos1[6] = {0,0,0,0,0,0};
    double pos2[6] = {0,0,0,0,0,0};
    double dec;
    const double inch = INCH;
    int x;

    assert(years);
    assert(err);

    x = swe_calc_ut(jd1, SE_SUN, flags, pos1, err);
    if (x < 0)
        return x;
    x = swe_calc_ut(jd2, SE_SUN, flags, pos2, err);
    if (x < 0)
        return x;
    *years = 0;

    if (jd1 < jd2) { /* forward search */
        dec = swe_difdegn(pos2[0], pos1[0]) / 360.0;
        for (;;) {
            x = swh_next_aspect(SE_SUN, 0, pos1[0], jd1+inch, 0, 0,
                                flags, &jd1, NULL, err);
            if (x)
                return x;
            if (jd1+inch <= jd2)
                *years += 1;
            else
                break;
        }
        *years += dec;
    }
    else if (jd1 > jd2) { /* backwards search */
        dec = swe_difdegn(pos1[0], pos2[0]) / 360.0;
        for (;;) {
            x = swh_next_aspect(SE_SUN, 0, pos1[0], jd1-inch, 1, 0,
                                flags, &jd1, NULL, err);
            if (x)
                return x;
            if (jd1-inch > jd2)
                *years -= 1;
            else
                break;
        }
        *years -= dec;
    }
    /* else equal julian days */
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
