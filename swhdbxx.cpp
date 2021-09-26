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

#include <sqlite3.h>

#include "swhdbxx.h"
#include "swhdbxx.hpp"

#include "swhdb.h"

swh::db::User::User()
    :
    m_idx(0),
    m_name("?"),
    m_pswd(""),
    m_mail(""),
    m_info("")
{
}

void swhxx_db_user_new(void** o)
{
    *o = new (std::nothrow) swh::db::User();
}

void swhxx_db_user_dealloc(void** o)
{
    delete *(swh::db::User**)o;
    *o = NULL;
}

unsigned long swh::db::User::idx() const
{
    return m_idx;
}

unsigned long swhxx_db_user_get_idx(void* o)
{
    return ((swh::db::User*)o)->idx();
}

int swh::db::User::idx(unsigned long x)
{
    m_idx = x;
    return 0;
}

int swhxx_db_user_set_idx(void* o, unsigned long x)
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

bool swh::db::User::nameIsValid(const char* s)
{
    if (!s || !*s || strlen(s) > 128)
        return false;
    return true;
}

int swh::db::User::name(const char* s)
{
    if (!nameIsValid(s)) {
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

bool swh::db::User::pswdIsValid(const char* s)
{
    if (!s)
        return false;
    return true;
}

int swh::db::User::pswd(const char* s)
{
    if (!pswdIsValid(s)) {
        error("invalid pswd");
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

bool swh::db::User::mailIsValid(const char* s)
{
    if (!s) // better todo
        return false;
    return true;
}

int swh::db::User::mail(const char* s)
{
    if (!mailIsValid(s)) {
        errorFormat("invalid mail (%s)", s);
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

bool swh::db::User::infoIsValid(const char* s)
{
    if (!s)
        return false;
    return true;
}

int swh::db::User::info(const char* s)
{
    if (!infoIsValid(s)) {
        errorFormat("invalid info (%s)", s);
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
    if (m_idx < 2) {
        errorFormat("cant drop user (%s) (%lu)", m_name.c_str(), m_idx);
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
    *(unsigned long*)arg = strtoul(argv[0], NULL, 10);
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
        // save
        sql = "insert into Users (name, pswd, mail, info) values ('"
            + name + "', '" + pswd + "', '" + mail + "', '" + info + "');";
        if (swh_db_exec(sql.c_str(), NULL, NULL, err)) {
            error(err);
            return 1; // key error (duplicate key)
        }
        // retrieve idx
        sql = "select _idx from Users where name = '" + name + "';";
        if (swh_db_exec(sql.c_str(), &_swhxx_db_user_save_cb, &m_idx, err)) {
            error(err);
            return 2; // sql error?
        }
        if (!m_idx) {
            errorFormat("cant retrieve idx of user (%s)", m_name.c_str());
            return 2; // wut?
        }
    }
    else {
        // update
        sql = "update Users set name = '" + name + "', pswd = '" + pswd
            + "', mail = '" + mail + "', info = '" + info + "' where _idx = "
            + to_string(m_idx) + ";";
        if (swh_db_exec(sql.c_str(), NULL, NULL, err)) {
            error(err);
            return 2; // wut?
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
    swh::db::User* u = new (std::nothrow) swh::db::User();
    if (!u)
        return 1;
    *(swh::db::User**)arg = u;
    if (u->idx(strtoul(argv[0], NULL, 10))
        || u->name(argv[1])
        || u->pswd(argv[2])
        || u->mail(argv[3])
        || u->info(argv[4]))
        return 1;
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

int swh::db::User::select(unsigned long uidx, swh::db::User** p, char err[512])
{
    int i;
    *p = NULL;
    if (!uidx) {
        memset(err, 0, 512);
        snprintf(err, 511, "invalid idx (%lu)", uidx);
        return 1; // key error
    }
    string sql = "select * from Users where _idx = " + to_string(uidx) + ";";
    if ((i = swh_db_exec(sql.c_str(), &_swhxx_db_user_cb, p, err))) {
        if (i == SQLITE_ABORT) {
            if (!p)
                return 4; // nomem
            return 3; // db corruption
        }
        return 2; // sql error
    }
    return 0;
}

int swhxx_db_user_select_idx(unsigned long uidx, void** o, char err[512])
{
    return swh::db::User::select(uidx, (swh::db::User**) o, err);
}

int swh::db::User::select(const char* name, swh::db::User** p, char err[512])
{
    int i;
    *p = NULL;
    if (!nameIsValid(name)) {
        snprintf(err, 512, "invalid name (%s)", name);
        return 1; // key error
    }
    string nam = swh::replaceAll(name, "'", "''");
    string sql = "select * from Users where name = '" + nam + "';";
    if ((i = swh_db_exec(sql.c_str(), &_swhxx_db_user_cb, p, err))) {
        if (i == SQLITE_ABORT) {
            if (!p)
                return 4; // nomem
            return 3; // db corruption
        }
        return 2; // sql error
    }
    return 0;
}

int swhxx_db_user_select(const char* name, void** o, char err[512])
{
    return swh::db::User::select(name, (swh::db::User**) o, err);
}

swh::db::Data::Data()
    :
    m_idx(0),
    m_useridx(1),
    m_title("?"),
    m_jd(0),
    m_latitude(0),
    m_longitude(0),
    m_altitude(0),
    m_datetime(""),
    m_timezone(""),
    m_isdst(-1),
    m_location(""),
    m_country("")
{
}

void swhxx_db_data_new(void** o)
{
    *o = new (std::nothrow) swh::db::Data();
}

void swhxx_db_data_dealloc(void** o)
{
    delete *(swh::db::Data**)o;
    *o = NULL;
}

unsigned long swh::db::Data::idx() const
{
    return m_idx;
}

unsigned long swhxx_db_data_get_idx(void* o)
{
    return ((swh::db::Data*)o)->idx();
}

int swh::db::Data::idx(unsigned long x)
{
    m_idx = x;
    return 0;
}

int swhxx_db_data_set_idx(void* o, unsigned long x)
{
    return ((swh::db::Data*)o)->idx(x);
}

unsigned long swh::db::Data::useridx() const
{
    return m_useridx;
}

unsigned long swhxx_db_data_get_useridx(void* o)
{
    return ((swh::db::Data*)o)->useridx();
}

int swh::db::Data::useridx(unsigned long x)
{
    m_useridx = x;
    return 0;
}

int swhxx_db_data_set_useridx(void* o, unsigned long x)
{
    return ((swh::db::Data*)o)->useridx(x);
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

double swh::db::Data::latitude() const
{
    return m_latitude;
}

double swhxx_db_data_get_latitude(void* o)
{
    return ((swh::db::Data*)o)->latitude();
}

int swh::db::Data::latitude(double d)
{
    if (d < -90 || d > 90) {
        errorFormat("invalid latitude (%f)", d);
        return 1;
    }
    m_latitude = d;
    return 0;
}

int swhxx_db_data_set_latitude(void* o, double lat)
{
    return ((swh::db::Data*)o)->latitude(lat);
}

double swh::db::Data::longitude() const
{
    return m_longitude;
}

double swhxx_db_data_get_longitude(void* o)
{
    return ((swh::db::Data*)o)->longitude();
}

int swh::db::Data::longitude(double d)
{
    if (d < -180 || d > 180) {
        errorFormat("invalid longitude (%f)", d);
        return 1;
    }
    m_longitude = d;
    return 0;
}

int swhxx_db_data_set_longitude(void* o, double lon)
{
    return ((swh::db::Data*)o)->longitude(lon);
}

long swh::db::Data::altitude() const
{
    return m_altitude;
}

long swhxx_db_data_get_altitude(void* o)
{
    return ((swh::db::Data*)o)->altitude();
}

int swh::db::Data::altitude(long i)
{
    if (i < -10000 || i > 20000) {
        errorFormat("invalid altitude (%ld)", i);
        return 1;
    }
    m_altitude = i;
    return 0;
}

int swhxx_db_data_set_altitude(void* o, long alt)
{
    return ((swh::db::Data*)o)->altitude(alt);
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

long swh::db::Data::isdst() const
{
    return m_isdst;
}

long swhxx_db_data_get_isdst(void* o)
{
    return ((swh::db::Data*)o)->isdst();
}

int swh::db::Data::isdst(long i)
{
    if (i < -1 || i > 1) {
        errorFormat("invalid isdst (%ld)", i);
        return 1;
    }
    m_isdst = i;
    return 0;
}

int swhxx_db_data_set_isdst(void* o, long i)
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
    return swh::db::User::select(m_useridx, p, err);
}

int swhxx_db_data_owner(void* o, void** p, char err[512])
{
    return ((swh::db::Data*)o)->owner((swh::db::User**) p, err);
}

/* vi: set fenc=utf-8 ff=unix et sw=4 ts=4 */
