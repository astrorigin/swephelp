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
#ifdef WIN32 /* has not gmtime_r ? */
    struct tm *tmp = gmtime(&t);
    return swe_julday(tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday,
        (tmp->tm_hour+(tmp->tm_min/60.0)+(tmp->tm_sec/3600.0)), SE_GREG_CAL);
#else
    struct tm tmp;
    gmtime_r(&t, &tmp);
    return swe_julday(tmp.tm_year+1900, tmp.tm_mon+1, tmp.tm_mday,
        (tmp.tm_hour+(tmp.tm_min/60.0)+(tmp.tm_sec/3600.0)), SE_GREG_CAL);
#endif
}

int swh_revjul(double jd, int flag, int *dt)
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

int swh_dt2i(const char *dt, int *ret)
{
    char *ptr, buf[32];
#ifndef WIN32
    char *saveptr;
#define strtok(buf, chr) strtok_r(buf, chr, &saveptr)
#endif
    memset(buf, 0, sizeof(char) * 32);
    strncpy(buf, dt, 31);
    ptr = strtok(buf, "/");
    if (ptr == NULL || strspn(ptr, "-0123456789") != strlen(ptr))
        return -1;
    ret[0] = atoi(ptr); /* year */
    ptr = strtok(NULL, "/");
    if (ptr == NULL || strspn(ptr, "0123456789") != strlen(ptr))
        return -1;
    ret[1] = atoi(ptr); /* month */
    assert(ret[1] > 0 && ret[1] < 13);
    ptr = strtok(NULL, " ");
    if (ptr == NULL || strspn(ptr, "0123456789") != strlen(ptr))
        return -1;
    ret[2] = atoi(ptr); /* mday */
    assert(ret[2] > 0 && ret[2] < 32);
    ptr = strtok(NULL, ":");
    if (ptr == NULL || strspn(ptr, "0123456789") != strlen(ptr))
        return -1;
    ret[3] = atoi(ptr); /* hour */
    assert(ret[3] > -1 && ret[3] < 24);
    ptr = strtok(NULL, ":");
    if (ptr == NULL || strspn(ptr, "0123456789") != strlen(ptr))
        return -1;
    ret[4] = atoi(ptr); /* minutes */
    assert(ret[4] > -1 && ret[4] < 60);
    ptr = strtok(NULL, ":");
    if (ptr == NULL || strspn(ptr, "0123456789") != strlen(ptr))
        return -1;
    ret[5] = atoi(ptr); /* seconds */
    assert(ret[5] > -1 && ret[5] < 60);
#ifndef WIN32
#undef strtok
#endif
    return 0;
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 : */
