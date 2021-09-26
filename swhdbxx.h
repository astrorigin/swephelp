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

#ifndef SWHDBXX_H
#define SWHDBXX_H

#ifdef __cplusplus
extern "C" {
#endif

void swhxx_db_user_new(void** o);

void swhxx_db_user_dealloc(void** o);

unsigned long swhxx_db_user_get_idx(void* o);

int swhxx_db_user_set_idx(void* o, unsigned long x);

const char* swhxx_db_user_get_name(void* o);

int swhxx_db_user_set_name(void* o, const char* s);

const char* swhxx_db_user_get_pswd(void* o);

int swhxx_db_user_set_pswd(void* o, const char* s);

const char* swhxx_db_user_get_mail(void* o);

int swhxx_db_user_set_mail(void* o, const char* s);

const char* swhxx_db_user_get_info(void* o);

int swhxx_db_user_set_info(void* o, const char* s);

int swhxx_db_user_drop(void* o);

int _swhxx_db_user_save_cb(void* arg, int argc, char** argv, char** cols);

int swhxx_db_user_save(void* o);

int _swhxx_db_user_cb(void* arg, int argc, char** argv, char** cols);

int swhxx_db_user_root(void** o, char err[512]);

int swhxx_db_user_select_idx(unsigned long uidx, void** o, char err[512]);

int swhxx_db_user_select(const char* name, void** o, char err[512]);

void swhxx_db_data_new(void** o);

void swhxx_db_data_dealloc(void** o);

unsigned long swhxx_db_data_get_idx(void* o);

int swhxx_db_data_set_idx(void* o, unsigned long x);

unsigned long swhxx_db_data_get_useridx(void* o);

int swhxx_db_data_set_useridx(void* o, unsigned long x);

const char* swhxx_db_data_get_title(void* o);

int swhxx_db_data_set_title(void* o, const char* title);

double swhxx_db_data_get_jd(void* o);

int swhxx_db_data_set_jd(void* o, double jd);

double swhxx_db_data_get_latitude(void* o);

int swhxx_db_data_set_latitude(void* o, double lat);

double swhxx_db_data_get_longitude(void* o);

int swhxx_db_data_set_longitude(void* o, double lon);

long swhxx_db_data_get_altitude(void* o);

int swhxx_db_data_set_altitude(void* o, long alt);

const char* swhxx_db_data_get_datetime(void* o);

int swhxx_db_data_set_datetime(void* o, const char* dt);

const char* swhxx_db_data_get_timezone(void* o);

int swhxx_db_data_set_timezone(void* o, const char* tz);

long swhxx_db_data_get_isdst(void* o);

int swhxx_db_data_set_isdst(void* o, long i);

const char* swhxx_db_data_get_location(void* o);

int swhxx_db_data_set_location(void* o, const char* loc);

const char* swhxx_db_data_get_country(void* o);

int swhxx_db_data_set_country(void* o, const char* cty);

int swhxx_db_data_owner(void* o, void** p, char err[512]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWHDBXX_H */

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
