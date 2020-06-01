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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <swephexp.h>

#include "swhdatetime.h"
#include "swhwin.h"

double swh_jdnow(void)
{
    time_t t = time(NULL);
    struct tm tmp;
#ifdef WIN32
    errno_t e = gmtime_s(&tmp, &t);
    if (e) {
        fprintf(stderr, "swh_jdnow: gmtime_s: errno (%d)\n", e);
        return 0;
    }
#else
    gmtime_r(&t, &tmp);
#endif
    return swe_julday(tmp.tm_year+1900, tmp.tm_mon+1, tmp.tm_mday,
        (tmp.tm_hour+(tmp.tm_min/60.0)+(tmp.tm_sec/3600.0)), SE_GREG_CAL);
}

int swh_revjul(double jd, int flag, int dt[6])
{
    double t;
    assert(flag == SE_GREG_CAL || flag == SE_JUL_CAL);
    swe_revjul(jd, flag, &dt[0], &dt[1], &dt[2], &t);
    dt[3] = (int) floor(t);
    t -= dt[3];
    dt[4] = (int) floor(t * 60);
    t -= dt[4]/60.0;
    dt[5] = (int) lround(t * 3600);
    if (dt[5] == 60)
        dt[5] = 59;
    return 0;
}

int _swh_dtstrip(const char* dt, char* ret, const size_t maxlen)
{
    size_t i = 0;
    char* p = (char*) dt;

    if (*p == '-') {
        *ret++ = '-';
        ++p, ++i;
    }
    for (; *p; ++p) {
        if (++i == maxlen)
            return 1;
        if (!isdigit(*p)) {
            *ret++ = ' ';
            continue;
        }
        *ret++ = *p;
    }
    *ret = '\0';
    return 0;
}

int swh_dt2i(const char* dt, int ret[6])
{
    const char* pattern = "%d %d %d %d %d %d%c";
    int i;
    int ye = 0, mo = 1, da = 1, ho = 0, mi = 0, se = 0;
    char rest = '\0';
    char str[64];

    assert(dt);
    assert(ret);

    if (!*dt)
        return 1;
    if (_swh_dtstrip(dt, str, 64))
        return 1;
    i = sscanf(str, pattern, &ye, &mo, &da, &ho, &mi, &se, &rest);
    if (i == EOF || i < 3)
        return 1;
    if (rest)
        return 1;
    if (mo < 1 || mo > 12 || da < 1 || da > 31
        || ho < 0 || ho > 23 || mi < 0 || mi > 59 || se < 0 || se > 59)
        return 1;
    ret[0] = ye;
    ret[1] = mo;
    ret[2] = da;
    ret[3] = ho;
    ret[4] = mi;
    ret[5] = se;
    return 0;
}

int _swh_tstrip(const char* t, char* ret, const size_t maxlen)
{
    size_t i = 0;
    char* p = (char*) t;

    for (; *p; ++p) {
        if (++i == maxlen)
            return 1;
        if (!isdigit(*p)) {
            *ret++ = ' ';
            continue;
        }
        *ret++ = *p;
    }
    *ret = '\0';
    return 0;
}

int swh_t2i(const char* t, int ret[3])
{
    const char* pattern = "%d %d %d%c";
    int i;
    int ho = 0, mi = 0, se = 0;
    char rest = '\0';
    char str[32];

    assert(t);
    assert(ret);

    if (!*t)
        return 1;
    if (_swh_tstrip(t, str, 32))
        return 1;
    i = sscanf(str, pattern, &ho, &mi, &se, &rest);
    if (i == EOF || i < 1)
        return 1;
    if (rest)
        return 1;
    if (ho < 0 || ho > 23 || mi < 0 || mi > 59 || se < 0 || se > 59)
        return 1;
    ret[0] = ho;
    ret[1] = mi;
    ret[2] = se;
    return 0;
}

void swh_jduration(
    double jdstart,
    double jdend,
    int ret[4])
{
    double span = jdend >= jdstart ? jdend - jdstart : jdstart - jdend;
    ret[0] = (int) floor(span);
    span -= ret[0];
    ret[1] = (int) floor(span * 24.0);
    span -= ret[1] * (1/24.0);
    ret[2] = (int) floor(span * 1440.0);
    span -=  ret[2] * (1/1440.0);
    ret[3] = (int) floor(span * 86400.0);
}

int swh_jd2isostr(double jd, int cal, char ret[64])
{
    const char* fmt = "%Y-%m-%d %H:%M:%S UTC";
    struct tm tmp;
    int dt[6];

    assert(cal == SE_GREG_CAL || cal == SE_JUL_CAL);

    if (swh_revjul(jd, cal, dt))
        return 1;
    tmp.tm_year = dt[0] - 1900;
    tmp.tm_mon = dt[1] - 1;
    tmp.tm_mday = dt[2];
    tmp.tm_hour = dt[3];
    tmp.tm_min = dt[4];
    tmp.tm_sec = dt[5];
    if (!strftime(ret, 64, fmt, &tmp))
        return 1;
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
