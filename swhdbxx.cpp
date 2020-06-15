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

#include <cstring>

#include "swhdbxx.h"
#include "swhdbxx.hpp"

#include "swhdb.h"

swh::db::User::User(
    int idx,
    const char* name,
    const char* pswd,
    const char* mail,
    const char* info)
    :
    m_idx(idx),
    m_name(name),
    m_pswd(pswd),
    m_mail(mail),
    m_info(info)
{
}

void swhxx_db_user_new(void** o)
{
    *o = new swh::db::User();
}

void swhxx_db_user_dealloc(void** o)
{
    delete *(swh::db::User**)o;
    *o = NULL;
}

int swh::db::User::idx() const
{
    return m_idx;
}

int swhxx_db_user_get_idx(void* o)
{
    return ((swh::db::User*)o)->idx();
}

int swh::db::User::idx(int x)
{
    if (x < 0) {
        errorFormat("invalid idx (%d)", x);
        return 1;
    }
    m_idx = x;
    return 0;
}

int swhxx_db_user_set_idx(void* o, int x)
{
    return ((swh::db::User*)o)->idx(x);
}

const char* swh::db::User::name() const
{
    return m_name.c_str();
}

const char* swhxx_db_user_get_name(void* o)
{
    return ((swh::db::User*)o)->name();
}

int swh::db::User::name(const char* s)
{
    if (!s || !*s || strlen(s) > 255) {
        errorFormat("invalid name (%s)", s);
        return 1;
    }
    m_name = s;
    return 0;
}

int swhxx_db_user_set_name(void* o, const char* s)
{
    return ((swh::db::User*)o)->name(s);
}

const char* swh::db::User::pswd() const
{
    return m_pswd.c_str();
}

const char* swhxx_db_user_get_pswd(void* o)
{
    return ((swh::db::User*)o)->pswd();
}

int swh::db::User::pswd(const char* s)
{
    if (!s) {
        error("invalid pswd (NULL)");
        return 1;
    }
    m_pswd = s;
    return 0;
}

int swhxx_db_user_set_pswd(void* o, const char* s)
{
    return ((swh::db::User*)o)->pswd(s);
}

const char* swh::db::User::mail() const
{
    return m_mail.c_str();
}

const char* swhxx_db_user_get_mail(void* o)
{
    return ((swh::db::User*)o)->mail();
}

int swh::db::User::mail(const char* s)
{
    if (!s) {
        error("invalid mail (NULL)");
        return 1;
    }
    m_mail = s;
    return 0;
}

int swhxx_db_user_set_mail(void* o, const char* s)
{
    return ((swh::db::User*)o)->mail(s);
}

const char* swh::db::User::info() const
{
    return m_info.c_str();
}

const char* swhxx_db_user_get_info(void* o)
{
    return ((swh::db::User*)o)->info();
}

int swh::db::User::info(const char* s)
{
    if (!s) {
        error("invalid info (NULL)");
        return 1;
    }
    m_info = s;
    return 0;
}

int swhxx_db_user_set_info(void* o, const char* s)
{
    return ((swh::db::User*)o)->info(s);
}

int swh::db::User::drop()
{
    if (!m_idx || m_idx == 1) {
        errorFormat("cant drop user (%s)", m_name.c_str());
        return 1; // key error
    }
    char err[512];
    string sql = "delete from Users where _idx = " + to_string(m_idx) + ";";
    if (swh_db_exec(sql.c_str(), NULL, NULL, err)) {
        error(err);
        return 2; // sql error
    }
    m_idx = 0;
    return 0;
}

int swhxx_db_user_drop(void* o)
{
    return ((swh::db::User*)o)->drop();
}

int _swhxx_db_user_save_cb(void* arg, int argc, char** argv, char** cols)
{
    *(int*)arg = atoi(argv[0]);
    return 0;
}

int swh::db::User::save()
{
    char err[512];
    string sql;
    sql.reserve(512);
    string name = swh::replaceAll(m_name, "'", "''");
    string pswd = swh::replaceAll(m_pswd, "'", "''");
    string mail = swh::replaceAll(m_mail, "'", "''");
    string info = swh::replaceAll(m_info, "'", "''");
    if (!m_idx) {
        sql = "insert into Users (name, pswd, mail, info) values ('"
            + name + "', '" + pswd + "', '" + mail + "', '" + info + "');";
        if (swh_db_exec(sql.c_str(), NULL, NULL, err)) {
            error(err);
            return 1; // key error
        }
    }
    else {
        sql = "update Users set name = '" + name + "', pswd = '" + pswd
            + "', mail = '" + mail + "', info = '" + info + "' where _idx = "
            + to_string(m_idx) + ";";
        if (swh_db_exec(sql.c_str(), NULL, NULL, err)) {
            error(err);
            return 2; // bad state
        }
    }
    if (!m_idx) {
        sql = "select _idx from Users where name = '" + name + "';";
        if (swh_db_exec(sql.c_str(), &_swhxx_db_user_save_cb, &m_idx, err)) {
            error(err);
            return 2; // sql error
        }
    }
    return 0;
}

int swhxx_db_user_save(void* o)
{
    return ((swh::db::User*)o)->save();
}

int _swhxx_db_user_cb(void* arg, int argc, char** argv, char** cols)
{
    swh::db::User* u = new swh::db::User(atoi(argv[0]), argv[1], argv[2],
                                         argv[3], argv[4]);
    *(swh::db::User**)arg = u;
    return 0;
}

int swh::db::User::root(swh::db::User** p, char err[512])
{
    return select(1, p, err);
}

int swhxx_db_user_root(void** o, char err[512])
{
    return swh::db::User::root((swh::db::User**) o, err);
}

int swh::db::User::select(int uidx, swh::db::User** p, char err[512])
{
    *p = NULL;
    if (uidx < 1) {
        snprintf(err, 512, "invalid idx (%d)", uidx);
        return 1; // key error
    }
    string sql = "select * from Users where _idx = " + to_string(uidx) + ";";
    if (swh_db_exec(sql.c_str(), &_swhxx_db_user_cb, p, err))
        return 2; // sql error
    return 0;
}

int swhxx_db_user_select_idx(int uidx, void** o, char err[512])
{
    return swh::db::User::select(uidx, (swh::db::User**) o, err);
}

int swh::db::User::select(const char* name, swh::db::User** p, char err[512])
{
    *p = NULL;
    if (!name || !*name || strlen(name) > 255) {
        snprintf(err, 512, "invalid name (%s)", name);
        return 1; // key error
    }
    string nam = swh::replaceAll(name, "'", "''");
    string sql = "select * from Users where name = '" + nam + "';";
    if (swh_db_exec(sql.c_str(), &_swhxx_db_user_cb, p, err))
        return 2; // sql error
    return 0;
}

int swhxx_db_user_select(const char* name, void** o, char err[512])
{
    return swh::db::User::select(name, (swh::db::User**) o, err);
}

swh::db::Data::Data(
    int idx,
    int uidx,
    const char* title,
    double jd,
    double lat,
    double lon,
    int alt,
    const char* datetime,
    const char* timezone,
    int isdst,
    const char* location,
    const char* country)
    :
    m_idx(idx),
    m_uidx(uidx),
    m_title(title),
    m_jd(jd),
    m_lat(lat),
    m_lon(lon),
    m_alt(alt),
    m_datetime(datetime),
    m_timezone(timezone),
    m_isdst(isdst),
    m_location(location),
    m_country(country)
{
}

void swhxx_db_data_new(void** o)
{
    *o = new swh::db::Data();
}

void swhxx_db_data_dealloc(void** o)
{
    delete *(swh::db::Data**)o;
    *o = NULL;
}

int swh::db::Data::idx() const
{
    return m_idx;
}

int swhxx_db_data_get_idx(void* o)
{
    return ((swh::db::Data*)o)->idx();
}

int swh::db::Data::idx(int x)
{
    if (x < 0) {
        errorFormat("invalid idx (%d)", x);
        return 1;
    }
    m_idx = x;
    return 0;
}

int swhxx_db_data_set_idx(void* o, int x)
{
    return ((swh::db::Data*)o)->idx(x);
}

int swh::db::Data::uidx() const
{
    return m_uidx;
}

int swhxx_db_data_get_uidx(void* o)
{
    return ((swh::db::Data*)o)->uidx();
}

int swh::db::Data::uidx(int x)
{
    if (x < 0) {
        errorFormat("invalid uidx (%d)", x);
        return 1;
    }
    m_uidx = x;
    return 0;
}

int swhxx_db_data_set_uidx(void* o, int x)
{
    return ((swh::db::Data*)o)->uidx(x);
}

const char* swh::db::Data::title() const
{
    return m_title.c_str();
}

const char* swhxx_db_data_get_title(void* o)
{
    return ((swh::db::Data*)o)->title();
}

int swh::db::Data::title(const char* title)
{
    if (!title || !*title || strlen(title) > 255) {
        errorFormat("invalid title (%s)", title);
        return 1;
    }
    m_title = title;
    return 0;
}

int swhxx_db_data_set_title(void* o, const char* title)
{
    return ((swh::db::Data*)o)->title(title);
}

double swh::db::Data::jd() const
{
    return m_jd;
}

double swhxx_db_data_get_jd(void* o)
{
    return ((swh::db::Data*)o)->jd();
}

int swh::db::Data::jd(double jd)
{
    m_jd = jd;
    return 0;
}

int swhxx_db_data_set_jd(void* o, double jd)
{
    return ((swh::db::Data*)o)->jd(jd);
}

double swh::db::Data::lat() const
{
    return m_lat;
}

double swhxx_db_data_get_lat(void* o)
{
    return ((swh::db::Data*)o)->lat();
}

int swh::db::Data::lat(double d)
{
    if (d < -90 || d > 90) {
        errorFormat("invalid lat (%f)", d);
        return 1;
    }
    m_lat = d;
    return 0;
}

int swhxx_db_data_set_lat(void* o, double lat)
{
    return ((swh::db::Data*)o)->lat(lat);
}

double swh::db::Data::lon() const
{
    return m_lon;
}

double swhxx_db_data_get_lon(void* o)
{
    return ((swh::db::Data*)o)->lon();
}

int swh::db::Data::lon(double d)
{
    if (d < -180 || d > 180) {
        errorFormat("invalid lon (%f)", d);
        return 1;
    }
    m_lon = d;
    return 0;
}

int swhxx_db_data_set_lon(void* o, double lon)
{
    return ((swh::db::Data*)o)->lon(lon);
}

int swh::db::Data::alt() const
{
    return m_alt;
}

int swhxx_db_data_get_alt(void* o)
{
    return ((swh::db::Data*)o)->alt();
}

int swh::db::Data::alt(int i)
{
    if (i < -10000 || i > 20000) {
        errorFormat("invalid alt (%d)", i);
        return 1;
    }
    m_alt = i;
    return 0;
}

int swhxx_db_data_set_alt(void* o, int alt)
{
    return ((swh::db::Data*)o)->alt(alt);
}

const char* swh::db::Data::datetime() const
{
    return m_datetime.c_str();
}

const char* swhxx_db_data_get_datetime(void* o)
{
    return ((swh::db::Data*)o)->datetime();
}

int swh::db::Data::datetime(const char* s)
{
    if (!s || strlen(s) > 255) {
        errorFormat("invalid datetime (%s)", s);
        return 1;
    }
    m_datetime = s;
    return 0;
}

int swhxx_db_data_set_datetime(void* o, const char* dt)
{
    return ((swh::db::Data*)o)->datetime(dt);
}

const char* swh::db::Data::timezone() const
{
    return m_timezone.c_str();
}

const char* swhxx_db_data_get_timezone(void* o)
{
    return ((swh::db::Data*)o)->timezone();
}

int swh::db::Data::timezone(const char* s)
{
    if (!s || strlen(s) > 255) {
        errorFormat("invalid timezone (%s)", s);
        return 1;
    }
    m_timezone = s;
    return 0;
}

int swhxx_db_data_set_timezone(void* o, const char* tz)
{
    return ((swh::db::Data*)o)->timezone(tz);
}

int swh::db::Data::isdst() const
{
    return m_isdst;
}

int swhxx_db_data_get_isdst(void* o)
{
    return ((swh::db::Data*)o)->isdst();
}

int swh::db::Data::isdst(int i)
{
    if (i < -1 || i > 1) {
        errorFormat("invalid isdst (%d)", i);
        return 1;
    }
    m_isdst = i;
    return 0;
}

int swhxx_db_data_set_isdst(void* o, int i)
{
    return ((swh::db::Data*)o)->isdst(i);
}

const char* swh::db::Data::location() const
{
    return m_location.c_str();
}

const char* swhxx_db_data_get_location(void* o)
{
    return ((swh::db::Data*)o)->location();
}

int swh::db::Data::location(const char* s)
{
    if (!s || strlen(s) > 255) {
        errorFormat("invalid location (%s)", s);
        return 1;
    }
    m_location = s;
    return 0;
}

int swhxx_db_data_set_location(void* o, const char* loc)
{
    return ((swh::db::Data*)o)->location(loc);
}

const char* swh::db::Data::country() const
{
    return m_country.c_str();
}

const char* swhxx_db_data_get_country(void* o)
{
    return ((swh::db::Data*)o)->country();
}

int swh::db::Data::country(const char* s)
{
    if (!s || strlen(s) > 255) {
        errorFormat("invalid country (%s)", s);
        return 1;
    }
    m_country = s;
    return 0;
}

int swhxx_db_data_set_country(void* o, const char* cty)
{
    return ((swh::db::Data*)o)->country(cty);
}

int swh::db::Data::owner(swh::db::User** p, char err[512]) const
{
    return swh::db::User::select(m_uidx, p, err);
}

int swhxx_db_data_owner(void* o, void** p, char err[512])
{
    return ((swh::db::Data*)o)->owner((swh::db::User**) p, err);
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
